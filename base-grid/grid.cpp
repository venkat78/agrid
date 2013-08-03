/*
 * grid.cpp
 *
 *  Created on: May 27, 2013
 *      Author: venkat
 */

#include "grid.hpp"

namespace base_grid {

template<typename _GRID_ELT>
tGRID<_GRID_ELT>::tGRID(const cBOX3 &bounds, INT numCells[3]) {
  m_bounds = bounds;

  INT nCells = (REAL) (numCells[0] * numCells[1] * numCells[2]);

  DO_COORDS(coord)
  {
    m_numCells[coord] = numCells[coord];
    m_cellSize[coord] = bounds.Thickness(coord) / ((REAL) numCells[coord]);
  }
}

template<typename _GRID_ELT>
tGRID<_GRID_ELT>::tGRID(const cBOX3 &bounds, INT numCells) {
  INT numGridCells[3];
  for (INT i = 0; i < 3; i++)
    numGridCells[i] = numCells;
  tGRID(bounds, numGridCells);
}

template<typename _GRID_ELT>
iCELL_INDEX tGRID<_GRID_ELT>::CellIndex(const cPOINT3 &point) {
  UINT index[3];
  DO_COORDS(coord)
  {
    REAL u = ((point[coord] - m_bounds.Min()[coord]) / m_cellSize[coord]);
    index[coord] = (INT) u;
  }

  return iCELL_INDEX(index[0], index[1], index[2]);
}

//normal is used to resolve borderline cases
template<typename _GRID_ELT>
INT tGRID<_GRID_ELT>::ModifiedCellIndex(const cPOINT3 &point, cVECTOR3 normal,
                                        iCELL_INDEX indices[2]) {
  INT index[3];

  BOOL onGridPlane = false;
  eCOORD gridPlaneCoord;
  DO_COORDS(coord)
  {
    REAL u = ((point[coord] - m_bounds.Min()[coord]) / m_cellSize[coord]);
    index[coord] = (INT) (u + cLIMITS::Epsilon());
    if (fabs(u - index[coord]) <= cLIMITS::Epsilon()) {

      if (normal[coord] > 0.0) {
        index[coord]--;
      }
    }
  }

  INT numIndices = 1;

  indices[0] = iCELL_INDEX(index[0], index[1], index[2]);

  return numIndices;
}

template<typename _GRID_ELT>
iCELL_INDEX tGRID<_GRID_ELT>::CellIndex(REAL x, REAL y, REAL z) {
  return CellIndex(cPOINT3(x, y, z));
}

template<typename _GRID_ELT>
cBOX3 tGRID<_GRID_ELT>::CellBox(iCELL_INDEX cellIndex) {
  INT coordIndex[3];

  coordIndex[0] = cellIndex.x;
  coordIndex[1] = cellIndex.y;
  coordIndex[2] = cellIndex.z;

  cPOINT3 cellBoxMin, cellBoxMax;
  DO_COORDS(i)
  {
    REAL h = m_bounds.Thickness(i) / ((REAL) m_numCells[i]);
    cellBoxMin[i] = m_bounds.MinCoord(i) + coordIndex[i] * h;
    cellBoxMax[i] = cellBoxMin[i] + h;
  }

  cBOX3 cellBox(cellBoxMin, cellBoxMax);
  return cellBox;
}

template<typename _GRID_ELT>
REAL tGRID<_GRID_ELT>::CellUpperBound(REAL val, eCOORD coord) {
  REAL h = m_bounds.Thickness(coord) / ((REAL) m_numCells[coord]);
  INT index = (INT) ((val - m_bounds.MinCoord(coord)) / h) + 1;
  return m_bounds.MinCoord(coord) + (index * h);
}

template<typename _GRID_ELT>
REAL tGRID<_GRID_ELT>::CellLowerBound(REAL val, eCOORD coord) {
  REAL h = m_bounds.Thickness(coord) / ((REAL) m_numCells[coord]);
  INT index = (INT) ((val - m_bounds.MinCoord(coord)) / h);
  return m_bounds.MinCoord(coord) + (index * h);
}

}

