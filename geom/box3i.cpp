/*
 * box3i.cpp
 *
 *  Created on: Feb 12, 2011
 */

//methods below are used for BOB (box inside box)
//the integer box is conceptually treated as volumetric grid, or collection of rect. cells,
//with vertices being integer points on the grid, and facets being cell faces.

#include "common_internal3d.hpp"
namespace geom {

  cBOX3i::cBOX3i(const cPOINT3i &p, const cPOINT3i &q)
  {
	m_cornerPoints[0]= p;
	m_cornerPoints[1] = q;
  }
  INT cBOX3i::NumCells() const
  {
	return XWidth() * YWidth() * ZWidth();
  }

  INT cBOX3i::NumVertices() const
  {
	return XWidth1() * YWidth1() * ZWidth1();
  }

  //number of facets with normal in the x direction, etc.
  INT cBOX3i::NumXFacets() const
  {
	return XWidth1() * YWidth() * ZWidth();
  }

  INT cBOX3i::NumYFacets() const
  {
	return XWidth() * YWidth1() * ZWidth();
  }

  INT cBOX3i::NumZFacets() const
  {
	return XWidth() * YWidth() * ZWidth1();
  }

  INT cBOX3i::NumFacets() const
  {
	return NumXFacets() + NumYFacets() + NumZFacets();
  }

  //the methods below provide conversion from 3d pos to index and vice versa
  //for cells, vertices, and facets

  cBOX3i::cCellPos cBOX3i::CellPosition(int index) const
  {
	cCellPos cellPos;
	CellPosition(index, cellPos);
	return cellPos;
  }

  VOID cBOX3i::CellPosition(int index, cCellPos &cellPos) const
  {
	cellPos[2] = ZMin() + index/(XWidth()*YWidth());
//	printf("Zmin = %d; XW = %d; YW = %d; cellZ = %d\n",
//			ZMin(), XWidth(), YWidth(), cellPos[2]);
	cellPos[1] = YMin() + index/XWidth() - YWidth()*(cellPos[2] - ZMin());
//	printf("YMin = %d; cellY = %d\n", YMin(), cellPos[1]);
	cellPos[0] = XMin() + index - XWidth()*(cellPos[1]-YMin() + YWidth()*(cellPos[2]-ZMin()));
//	printf("XMin() = %d; cellX = %d\n", XMin(), cellPos[0]);
	INT debugIndex = CellIndex(cellPos);
	ASSERT(debugIndex == index);
  }

  int cBOX3i::CellIndex(cBOX3i::cCellPos &cellPos) const
  {
	int index = cellPos[0] - XMin() +
		XWidth() * (cellPos[1] - YMin() +
				YWidth() * (cellPos[2] - ZMin()));
	return index;
  }

  BOOL cBOX3i::CellIsOnBorder(cCellPos &cellPos) const
  {
	DO_COORDS(coord){
	  if (cellPos[coord] == m_cornerPoints[0][coord] ||
			  cellPos[coord] == m_cornerPoints[1][coord] - 1)
		return true;
	}
	return false;
  }

  //indexation on the box boundary is dfined by this method
  INT cBOX3i::BorderCellIndex(const cCellPos &cellPos, eCOORD coord) const
  {
	INT index = 0;
//	printf("intBox: ");Print();
	DO_COORDS(c){
	  eCOORD c1 = CoordPlusOne(c);
	  eCOORD c2 = CoordPlusTwo(c);
	  if (c < coord)
		index += 2*Thickness(c1)*Thickness(c2);
	  else {
		if (cellPos[c] == MaxCoord(c))
		  index += Thickness(c1)*Thickness(c2);
		else
		  ASSERT(cellPos[c] == MinCoord(c));
		return index + (cellPos[c1] - MinCoord(c1))+
		Thickness(c1)*(cellPos[c2] - MinCoord(c2));
	  }
	}
	return -1;
  }

  //given the index, computes the cellPos and the coordinate
  eCOORD cBOX3i::BorderCellPos(INT index, cCellPos &cellPos) const
  {
	if (index < 0)
	  return GEOM_COORD_NONE;
	DO_COORDS(coord){
	  BOOL thisCoord = false;
	  eCOORD c1 = CoordPlusOne(coord);
	  eCOORD c2 = CoordPlusTwo(coord);
	  INT area = Thickness(c1)*Thickness(c2);
	  if (index < area){ //lower side of the box
		thisCoord = true;
			cellPos[coord] = MinCoord(coord);
	  }
	  index -= area;
	  if (index < area) {//upper side of the box
		thisCoord = true;
		cellPos[coord] = MaxCoord(coord)-1;
	  }
	  if (thisCoord){
		cellPos[c2] = MinCoord(c2) + index/Thickness(c1);
		cellPos[c1] = MinCoord(c1) + index-Thickness(c1)*(cellPos[c2] - MinCoord(c2));
		return coord;
	  }
	  index -= area;
	}
	return GEOM_COORD_NONE;
  }

  cBOX3i::cVertexPos cBOX3i::VertexPosition(INT index)
  {
	cVertexPos vertexPos;
	VertexPosition(index, vertexPos);
	return vertexPos;
  }

  VOID cBOX3i::VertexPosition(INT index, cVertexPos &vertexPos)
  {
	vertexPos[2] = ZMin() + index/(XWidth1()*YWidth1());
	vertexPos[1] = YMin() + index/XWidth1() - YWidth1()*(vertexPos[2] - ZMin());
	vertexPos[0] = XMin() + index -
		XWidth1()*(vertexPos[1]-YMin() + YWidth1()*(vertexPos[2]-ZMin()));
	//#ifdef DEBUG
	INT debugIndex = VertexIndex(vertexPos);
	ASSERT(debugIndex == index);
//#endif //DEBUG
  }

  INT cBOX3i::VertexIndex(cBOX3i::cVertexPos &vertexPos) const
  {
	int index = vertexPos[0] - XMin() +
		XWidth1() * (vertexPos[1] - YMin() + YWidth1() * (vertexPos[2] - ZMin()));
	return index;
  }

  cBOX3i::cFacetPos cBOX3i::FacetPosition(int index) const
  {
	cFacetPos facetPos;
	FacetPosition(index, facetPos);
	return facetPos;
  }

  VOID cBOX3i::FacetPosition(int index, cFacetPos &facetPos) const
  {
	if (index < 0)
	  facetPos.Direction(GEOM_COORD_NONE);
	else if (index < NumXFacets()){
	  facetPos.Direction(GEOM_X);
	  facetPos[0] = XMin() + index /(YWidth()*ZWidth());
	  int j = index - (facetPos[0] - XMin())*YWidth()*ZWidth();
	  facetPos[2] = ZMin() + j / YWidth();
	  facetPos[1] = YMin() - YWidth() * (facetPos[2] - ZMin()) + j;
	}
	else if (index < NumYFacets()){
	  index -= NumXFacets();
	  facetPos.Direction(GEOM_Y);
	  facetPos[1] = YMin() + index /(ZWidth()*XWidth());
	  int j = index - (facetPos[1] - YMin())*ZWidth()*XWidth();
	  facetPos[0] = XMin() + j / YWidth();
	  facetPos[2] = ZMin() - ZWidth() * (facetPos[0] - XMin()) + j;
	}
	else if (index < NumZFacets()){
	  index -= (NumXFacets() + NumYFacets());
	  facetPos.Direction(GEOM_Z);
	  facetPos[2] = ZMin() + index /(XWidth()*YWidth());
	  int j = index - (facetPos[2] - ZMin())*XWidth()*YWidth();
	  facetPos[1] = YMin() + j / ZWidth();
	  facetPos[0] = XMin() - XWidth() * (facetPos[1] - YMin()) + j;
	}
	else
	  facetPos.Direction(GEOM_COORD_NONE);
  }

  int cBOX3i::FacetIndex(cBOX3i::cFacetPos &facetPos) const
  {
	int index = -1;
	if (facetPos.Direction() == GEOM_X)
	  index = (facetPos[0] - XMin())*YWidth()*ZWidth() +
		facetPos[1] - YMin() +
				YWidth() * (facetPos[2] - ZMin());
	else if (facetPos.Direction() == GEOM_Y)
	  index = (facetPos[1] - YMin())*ZWidth()*XWidth() +
		facetPos[2] - ZMin() +
				ZWidth() * (facetPos[0] - XMin());
	else if (facetPos.Direction() == GEOM_Z)
	  index = (facetPos[2] - ZMin())*XWidth()*YWidth() +
		facetPos[0] - XMin() +
				XWidth() * (facetPos[1] - YMin());
	return index;
  }

  //the point is allowed to be located on the min side, but needs to be smaller that the max.
  BOOL cBOX3i::ContainsCell(const cCellPos& cellPos) const
  {
	DO_COORDS(coord){
	  if (cellPos[coord] < MinCoord(coord))
		return false;
	  if (cellPos[coord] >= MaxCoord(coord))
		return false;
	}
	return true;
  }
} //namespace
