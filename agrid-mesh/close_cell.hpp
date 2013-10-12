/*
 * close_cell.hpp
 *
 *  Created on: Oct 7, 2013
 *      Author: venkat
 */

#ifndef _AGRID_MESH_CLOSE_CELL_HPP_
#define _AGRID_MESH_CLOSE_CELL_HPP_

#include "defs.hpp"
template<typename _CELL_TYPE>
BOOL CloseCell(_CELL_TYPE *cell, std::vector<iVERTEX> &cycleVertices) {
  //The orientation in the vector is reverse.
  //Close hole in the opposite order of vertices in cycleVertices.
  INT numCycleVertices = cycleVertices.size();

  cell->NewFacetStart();

  for (INT i = numCycleVertices - 1; i >= 0; i--)
    cell->NewFacetAddVertex(cycleVertices[i]);

  iFACET newIndex = cell->NewFacetClose();

  if (newIndex == INVALID_IFACET)
    return false;

  return true;
}

template<typename _HALF_EDGE>
VOID CollectCycleVertices(std::stack<_HALF_EDGE*> &boundaryHes, iVERTEX terminalVtx, BOOL checkTerminalVtx, std::vector<iFACET> &cycleVertices) {
  do {
    _HALF_EDGE *topHe = boundaryHes.top();
    cycleVertices.push_back(topHe->Tail()->Index());

    if (checkTerminalVtx && topHe->Tail()->Index() == terminalVtx) {
      boundaryHes.pop();
      break;
    }

    boundaryHes.pop();
  } while (!boundaryHes.empty());

}
template<typename _CELL_TYPE, typename _HALF_EDGE>
BOOL CloseHole(_CELL_TYPE *cell, _HALF_EDGE *initHe) {
  std::stack<_HALF_EDGE*> boundaryHes;
  _HALF_EDGE* firstHe = initHe;
  BOOL visited[256];
  memset(visited, 0, sizeof(BOOL) * 256);

  BOOL cyclesLeft = false;

  //Implementing cycle detection algorithm.
  do {
    _HALF_EDGE* loopHe = firstHe;

    BOOL foundCycle = false;
    iVERTEX cycleAtV = INVALID_IVERTEX;
    do {
      ASSERT(loopHe->IsBorder());
      iVERTEX v = loopHe->Tail()->Index();
      if (visited[v] == true) {
        foundCycle = true;
        cycleAtV = v;
        break;
      }

      visited[v] = true;
      boundaryHes.push(loopHe);
      loopHe = loopHe->Next();
    } while (loopHe != firstHe);

    std::vector<iVERTEX> cycleVertices;
    if (foundCycle) {
      CollectCycleVertices<_HALF_EDGE>(boundaryHes, cycleAtV, foundCycle, cycleVertices);
      if (!CloseCell<_CELL_TYPE>(cell,cycleVertices))
        return false;

      if (!boundaryHes.empty()) {
        cyclesLeft = true;
        firstHe = boundaryHes.top();
        memset(visited, 0, sizeof(BOOL) * 256);
        while (!boundaryHes.empty())
          boundaryHes.pop();
      } else
        cyclesLeft = false;
    }  //foundCycle.
    else {
      //Hole is a simple loop.
      CollectCycleVertices<_HALF_EDGE>(boundaryHes, INVALID_IVERTEX, foundCycle, cycleVertices);
      if (!CloseCell<_CELL_TYPE>(cell,cycleVertices))
        return false;

      cyclesLeft = false;
    }

  } while (cyclesLeft);

  return true;
}

template <typename _CELL_TYPE>
typename _CELL_TYPE::cHALF_EDGE* BoundaryEdge(_CELL_TYPE *cell) {
  typename _CELL_TYPE::half_edge_iterator currHalfEdge = cell->HalfEdgesBegin();
  typename _CELL_TYPE::half_edge_iterator lastHalfEdge = cell->HalfEdgesEnd();

  for(; currHalfEdge != lastHalfEdge; currHalfEdge++ ) {
    if(currHalfEdge->IsBorder())
    return currHalfEdge.operator->();
  }

  return NULL;
}

template<typename _CELL_TYPE>
BOOL Close(_CELL_TYPE *cell) {
  typedef typename _CELL_TYPE::cHALF_EDGE cHALF_EDGE;
  while (true) {
    //Find an open boundary and close it by a new facet.
    typename _CELL_TYPE::cHALF_EDGE *boundaryHalfEdge = BoundaryEdge<_CELL_TYPE>(cell);

    if (boundaryHalfEdge == NULL)
      break;

    if (!CloseHole<_CELL_TYPE, cHALF_EDGE>(cell, boundaryHalfEdge))
      return false;
  }

  return true;
}

#endif /* CLOSE_CELL_HPP_ */
