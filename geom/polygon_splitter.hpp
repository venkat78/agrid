/*
 * polygon_splitter.hpp
 *
 *  Created on: Dec 31, 2010
 */

#ifndef POLYGON_SPLITTER_HPP_
#define POLYGON_SPLITTER_HPP_


#include <list>

//a better but longer name would be polygon_split_finder
namespace geom {

  class cPOLYGON_SPLITTER {
  public:
    cPOLYGON_SPLITTER();

  public:
    VOID AddPoint(const cPOINT3 &point);
    REAL DeviationTolerance() const {return m_deviationTolerance;}
    VOID DeviationTolerance(REAL value) {m_deviationTolerance = value;}
    BOOL Split();
    VOID PrintPoints(FILE *stream = stdout) const;
    VOID PrintIndices(FILE *stream = stdout);
    BOOL Traverse();
    INT Index();
    VOID Debug(BOOL flag) {m_debug = flag;}

  private://methods
    VOID SplitLoop();
    VOID SplitLoopAtDiagonal(BOOL diagonalPointsAreCoincident = false);
    BOOL PrepareLoop();
    VOID FindDiagonal();
    std::list<INT>::iterator InsertIndexBefore(std::list<INT>::iterator position, INT index);
    std::list<INT>::iterator InsertIndexAfter(std::list<INT>::iterator position, INT index);
    std::list<INT>::iterator CircularNextIndexIterator(std::list<INT>::iterator current);
    std::list<INT>::iterator CircularPrevIndexIterator(std::list<INT>::iterator current);
    INT IndexPos(std::list<INT>::iterator pos); //debugging only
    BOOL Debug() {return m_debug;}
    BOOL PointsAreCollinear(INT index0, INT index1, INT index2);
    BOOL LoopContainsCoincidentVertices();

  private:  //fields
    INT m_currentLoopSize;
    BOOL n_noCoincidentVertices;
    cPLANE3 m_plane;
    REAL m_deviationMax;
    REAL m_deviationTolerance;
    std::list<INT>::iterator m_frontLoopIterator;
    std::list<INT>::iterator m_backLoopIterator;
    std::list<INT>::iterator m_diagonal0IndexIterator;
    std::list<INT>::iterator m_diagonal1IndexIterator;
    std::vector<cPOINT3> m_points;
    std::list<INT> m_indices;
    BOOL m_debug;
  };

} //namespace
#endif /* POLYGON_SPLITTER_HPP_ */

