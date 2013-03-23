/*
 * polygon_splitter.cpp
 *
 *  Created on: Dec 31, 2010
 */

#include "common_internal3d.hpp"
#include "polygon_splitter.hpp"

namespace geom {

  const INT END_LOOP_INDEX = -1;
  cPOLYGON_SPLITTER::cPOLYGON_SPLITTER()
  {
    m_deviationMax = 0.0;
    m_deviationTolerance = cLIMITS::Tolerance();
    m_frontLoopIterator = m_indices.begin();
    m_backLoopIterator = m_indices.begin();
    n_noCoincidentVertices = false; //unless proven
    m_debug = false;
  }

  VOID cPOLYGON_SPLITTER::AddPoint(const cPOINT3 &point)
  {
    INT point_index = m_points.size();
    m_points.push_back(point);
    m_indices.push_back(point_index);
  }

  BOOL cPOLYGON_SPLITTER::Split()
  {
	m_frontLoopIterator = m_indices.begin();
//	m_backLoopIterator = m_indices.begin();
    //find diagonal and split the current loop
    while (m_frontLoopIterator != m_indices.end()){
      SplitLoop();
    }
    m_frontLoopIterator = m_indices.begin();

    return true;
  }

  BOOL cPOLYGON_SPLITTER::Traverse()
  {
    return m_frontLoopIterator != m_indices.end();
  }

  INT cPOLYGON_SPLITTER::Index()
  {
    INT index = *m_frontLoopIterator;
    m_frontLoopIterator++;
    return index;
  }

  VOID cPOLYGON_SPLITTER::PrintPoints(FILE *stream) const
  {
    fprintf(stream, "n points = %d\n", (INT)m_points.size());
    for (INT i=0; i < (INT)m_points.size(); i++){
      fprintf(stream, "%d:\tp: %lg %lg %lg\n",
          i, m_points[i][0], m_points[i][1], m_points[i][2]);
    }
  }

  VOID cPOLYGON_SPLITTER::PrintIndices(FILE *stream)
  {
    std::list<INT>::iterator loopIterator = m_indices.begin();
    fprintf(stream, "front %d:%d;\t", IndexPos(m_frontLoopIterator), *m_frontLoopIterator);
    fprintf(stream, "back %d:%d;\t\t", IndexPos(m_backLoopIterator), *m_backLoopIterator);
    INT i = 0;
    for (; loopIterator != m_indices.end(); loopIterator++, i++)
      fprintf(stream, "%d:%d; ", i, *loopIterator);
    fprintf(stream, "\n");
  }

  VOID cPOLYGON_SPLITTER::SplitLoop()
  {
	if (Debug()){
	  PrintPoints();
	  PrintIndices();
	}

	if (LoopContainsCoincidentVertices()){
	  SplitLoopAtDiagonal(true);
	  if (Debug())
		PrintIndices();
	}

	else if (!PrepareLoop()){ //loop needs to be split
      FindDiagonal();
      SplitLoopAtDiagonal();
      if (Debug())
    	PrintIndices();
    }
  }

  //if a loop contains coincident vertices, the diagonals are set to those
  //no sorting is done but
  BOOL cPOLYGON_SPLITTER::LoopContainsCoincidentVertices()
  {
	if (n_noCoincidentVertices)
	  return false;

	std::list<INT>::iterator loop0Iterator = m_frontLoopIterator;
	for (; loop0Iterator != m_indices.end(); loop0Iterator++){
	  INT index0 = (*loop0Iterator);
	  if (index0 < 0)
		break;
      std::list<INT>::iterator loop1Iterator = loop0Iterator;
	  loop1Iterator++;
	  for (; loop1Iterator != m_indices.end(); loop1Iterator++){
		INT index1 = (*loop1Iterator);
		if (index1 < 0)
		  break;
		if (m_points[index0] == m_points[index1]){
		  m_diagonal0IndexIterator = loop0Iterator;
		  m_diagonal1IndexIterator = loop1Iterator;
		  m_backLoopIterator = loop1Iterator;
		  std::list<INT>::iterator next = m_backLoopIterator++;
		  while (*next >= 0 &&  next != m_indices.end()){
			m_backLoopIterator = next;
			next++;
		  }
		  return true;
		}
	  }
	}
	if (loop0Iterator == m_indices.end()) //only if the end of the indices is reached there are no
		//multiple pairs of coincident vertices
	  n_noCoincidentVertices = true;
	return false;
  }

  //returns next index iterator, and if the current is the last one, the first iterator on the indices list
  std::list<INT>::iterator cPOLYGON_SPLITTER::CircularNextIndexIterator(std::list<INT>::iterator current)
  {
    std::list<INT>::iterator nextIterator = current;
    nextIterator++;
    if (nextIterator == m_indices.end() || *nextIterator == -1)
      nextIterator = m_frontLoopIterator;
    return nextIterator;
  }

  std::list<INT>::iterator cPOLYGON_SPLITTER::CircularPrevIndexIterator(std::list<INT>::iterator current)
  {
    std::list<INT>::iterator prevIterator = current;
    if (current == m_frontLoopIterator || *current == -1)
      prevIterator = m_backLoopIterator;
    else
      prevIterator--;
    return prevIterator;
  }

  //computes the loop's newell plane and the loop points min and max deviations from the plane
  //if the loop is planar, the m_frontLoopIterator is advanced through the loop end, and true is returned.
  //Otherwise, the return value is false
  BOOL  cPOLYGON_SPLITTER::PrepareLoop()
  {
    m_currentLoopSize = 0;
    cNEWELL_NORMAL_BUILDER normal_builder;

    std::list<INT>::iterator loopIterator = m_frontLoopIterator;
    for (; loopIterator != m_indices.end(); loopIterator++){
      INT index = (*loopIterator);
      if (index < 0)
       break;
      m_backLoopIterator = loopIterator;
      normal_builder.AddPoint(m_points[index]);
      m_currentLoopSize++;
    }
    if (m_currentLoopSize == 3){
      while (*loopIterator == END_LOOP_INDEX && loopIterator != m_indices.end())
    	m_frontLoopIterator = ++loopIterator;
      return true;
    }
    normal_builder.Complete();

    //now compute the max deviation - only for 4 points the plane passes through the mean point
    loopIterator = m_frontLoopIterator;
    REAL devnMin = cLIMITS::Max();
    REAL devnMax = -cLIMITS::Max();
    m_plane = normal_builder.Plane();

    for (; loopIterator != m_indices.end(); loopIterator++){
      INT index = (*loopIterator);
      if (index < 0)
       break;
      REAL d = m_plane.SignedDistance(m_points[index]);
      if (d < devnMin)
    	devnMin = d;
      if (d > devnMax)
    	devnMax = d;
    }

    REAL deviation = 0.5*(fabs(devnMax) + fabs(devnMin));

    if (fabs(deviation) < m_deviationTolerance){ //this loop is planar
      std::list<INT>::iterator it = m_backLoopIterator;
      it++;
      if ((it == m_indices.end()) || (*it != END_LOOP_INDEX))
    	it = InsertIndexAfter(m_backLoopIterator, END_LOOP_INDEX);
      while (*loopIterator == END_LOOP_INDEX && loopIterator != m_indices.end())
    	m_frontLoopIterator = ++loopIterator;
//      PrintIndices();
      return true;
    }
    else
      return false;
  }

  BOOL cPOLYGON_SPLITTER::PointsAreCollinear(INT index0, INT index1, INT index2)
  {
	return PointsCollinear(m_points[index0], m_points[index1], m_points[index2]);
  }

  //this method repeats the mas dist computation between the points and the plane
  //in PrepareLoop. the code can be optimized.
  VOID cPOLYGON_SPLITTER::FindDiagonal()
  {
	//find the furstest point wrt the Newell's plane on its positive side
	REAL distMax = -cLIMITS::Max();
	std::list<INT>::iterator maxDistIterator = m_frontLoopIterator;
    std::list<INT>::iterator loopIterator = m_frontLoopIterator;

    for (; loopIterator != m_indices.end(); loopIterator++){
      INT index = (*loopIterator);
      if (index < 0)
       break;
      REAL d = m_plane.SignedDistance(m_points[index]);
      if (d > distMax){
    	  distMax = d;
    	  maxDistIterator = loopIterator;
      }
    }

	m_diagonal0IndexIterator = maxDistIterator;
    if (m_currentLoopSize == 4){
    	m_diagonal1IndexIterator =
    		CircularNextIndexIterator(CircularNextIndexIterator(maxDistIterator));
    }
    else {
      std::list<INT>::iterator nextIterator = CircularNextIndexIterator(maxDistIterator);
      std::list<INT>::iterator beginIterator = CircularNextIndexIterator(nextIterator);
      while (PointsAreCollinear(*maxDistIterator, *nextIterator, *beginIterator))
    	  beginIterator = CircularNextIndexIterator(beginIterator);
      std::list<INT>::iterator prevIterator = CircularPrevIndexIterator(maxDistIterator);
      std::list<INT>::iterator endIterator = CircularPrevIndexIterator(prevIterator);
      while (PointsAreCollinear(*maxDistIterator, *prevIterator, *endIterator))
    	  endIterator = CircularPrevIndexIterator(endIterator);
      endIterator = CircularNextIndexIterator(endIterator);
      distMax = -cLIMITS::Max();
      loopIterator = beginIterator;
      do {
      	INT index = (*loopIterator);
      	REAL d = m_plane.SignedDistance(m_points[index]);
//      	if (Debug())
  //    	  printf("index %d: d %lg\n", index, d);
      	if (d  > distMax){
        	  distMax = d;
        	  m_diagonal1IndexIterator = loopIterator;
      	}
      	loopIterator = CircularNextIndexIterator(loopIterator);
      } while (loopIterator != endIterator);
    }
  }

  INT cPOLYGON_SPLITTER::IndexPos(std::list<INT>::iterator pos) //debugging only
  {
    std::list<INT>::iterator loopIterator = m_indices.begin();
    INT count = 0;
    for (; loopIterator != m_indices.end(); loopIterator++, count++){
      if (loopIterator == pos)
    	return count;
    }
    return -1;
  }

  VOID cPOLYGON_SPLITTER::SplitLoopAtDiagonal(BOOL diagonalPointsAreCoincident)
  {
    INT diagonal0Index = *m_diagonal0IndexIterator;
    INT diagonal1Index = *m_diagonal1IndexIterator;
    if (Debug()){
      printf("diagonal0 %d; diagonal1 %d\n", diagonal0Index, diagonal1Index);
      PrintIndices();
    }
    std::list<INT>::iterator it;
    if (diagonalPointsAreCoincident)
      it = InsertIndexBefore(m_diagonal0IndexIterator, END_LOOP_INDEX);
    else
      it = InsertIndexBefore(InsertIndexBefore(m_diagonal0IndexIterator, END_LOOP_INDEX),
    		  diagonal0Index);
    if (Debug())
    	PrintIndices();
    if (m_diagonal0IndexIterator == m_frontLoopIterator)
      m_frontLoopIterator = it;
    if (Debug())
    	PrintIndices();
    if (diagonalPointsAreCoincident)
      it = InsertIndexBefore(m_diagonal1IndexIterator, END_LOOP_INDEX);
    else
      it = InsertIndexBefore(InsertIndexBefore(m_diagonal1IndexIterator, END_LOOP_INDEX),
    		  diagonal1Index);
    if (Debug())
    	PrintIndices();
    if (m_diagonal1IndexIterator == m_frontLoopIterator)
      m_frontLoopIterator = it;
    if (Debug())
    	PrintIndices();

    //the first loop is split between the beginning and the end of the loop
    //move the indices from the front to the back of the current loop

    while (true){
      INT firstIndex = *m_frontLoopIterator;
      std::list<INT>::iterator tempBack = m_backLoopIterator;
      tempBack++;
      if (firstIndex >= 0 || (*tempBack) >= 0){
    	InsertIndexAfter(m_backLoopIterator, firstIndex);
    	m_backLoopIterator++;
        if (Debug())
        	PrintIndices();
      }
      std::list<INT>::iterator tempFront = m_frontLoopIterator;
      m_frontLoopIterator++;
      m_indices.erase(tempFront);
      if (Debug())
      	PrintIndices();
      if (firstIndex < 0)
        break;
    }
    if (m_indices.back() >= 0)
      m_indices.push_back(END_LOOP_INDEX);
    if (Debug())
    	PrintIndices();
  }

  //the std::list insert inserts a new element prior to the given position.
  //but this one inserts after the position
  std::list<INT>::iterator cPOLYGON_SPLITTER::InsertIndexAfter(std::list<INT>::iterator position,
                                          INT index)
  {
	position++;
	return m_indices.insert(position, index);
  }

  std::list<INT>::iterator cPOLYGON_SPLITTER::InsertIndexBefore(std::list<INT>::iterator position,
                                          INT index)
  {
	return m_indices.insert(position, index);
  }
}//namespace
