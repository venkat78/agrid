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
  INT tGRID<_GRID_ELT>::ModifiedCellIndex(const cPOINT3 &point, cVECTOR3 normal, iCELL_INDEX indices[2]) {
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

  template<typename _GRID_ELT>
  eCELL_COLOR tGRID<_GRID_ELT>::CellColor(iCELL_INDEX index) {
    cGRID_CELL_COLORS::iterator pos = m_cellColors.find(index);

    if (pos == m_cellColors.end())
      return UNKNOWN;

    return pos->second;
  }

  template<typename _GRID_ELT>
  eCELL_COLOR tGRID<_GRID_ELT>::VertexColor(iGRID_VERTEX index) {
    cGRID_VERTEX_COLORS::iterator pos = m_vertexColors.find(index);

    if (pos == m_vertexColors.end())
      return UNKNOWN;

    return pos->second;
  }

  template<typename _GRID_ELT>
  VOID tGRID<_GRID_ELT>::CollectWhiteAndBlackVertices() {
    //Mark white vertices.
    cGRID_CELL_COLORS::iterator currPos = m_cellColors.begin();
    cGRID_CELL_COLORS::iterator lastPos = m_cellColors.end();

    for (; currPos != lastPos; currPos++) {
      if (currPos->second == WHITE) {
        iCELL_INDEX cellIndex = currPos->first;

        for (INT i = 0; i < 2; i++) {
          for (INT j = 0; j < 2; j++) {
            for (INT k = 0; k < 2; k++) {
              iGRID_VERTEX vIndex(cellIndex.x + i, cellIndex.y + j, cellIndex.z + k);
              m_vertexColors[vIndex] = WHITE;
            }
          }
        }
      }
    }

    //Mark black vertices.
    currPos = m_cellColors.begin();
    lastPos = m_cellColors.end();

    for (; currPos != lastPos; currPos++) {
      if (currPos->second == WHITE) {
        iCELL_INDEX cellIndex = currPos->first;

        for (INT i = 0; i < 2; i++) {
          for (INT j = 0; j < 2; j++) {
            for (INT k = 0; k < 2; k++) {
              iGRID_VERTEX vIndex(cellIndex.x + i, cellIndex.y + j, cellIndex.z + k);
              m_vertexColors[vIndex] = WHITE;
            }
          }
        }
      }
    }

  }

  template<typename _GRID_ELT>
  VOID tGRID<_GRID_ELT>::AddToFront(eCELL_COLOR sourceCellColor, iCELL_INDEX index, std::vector<iCELL_INDEX> &front,
                                    cGRID_CELL_MARKS &alreadyVisited) {
    if (!IsValid(index))
      return;

    cGRID_CELL_COLORS::iterator indexColor = m_cellColors.find(index);
    if (sourceCellColor == GRAY) {
      if (indexColor != m_cellColors.end() && indexColor->second == GRAY) {
        cGRID_CELL_MARKS::iterator pos = alreadyVisited.find(index);

        if (pos == alreadyVisited.end()) {
          alreadyVisited[index] = true;
          front.push_back(index);
        }

        return;
      }
    } else {
      cGRID_CELL_MARKS::iterator pos = alreadyVisited.find(index);
      if (pos == alreadyVisited.end()) {
        alreadyVisited[index] = true;
        front.push_back(index);

        if (indexColor == m_cellColors.end())
          CellColor(index, WHITE);
      }
    }
  }

  template<typename _GRID_ELT>
  VOID tGRID<_GRID_ELT>::AdvancingFront(std::vector<iCELL_INDEX> &front, cGRID_CELL_MARKS &alreadyVisited) {
    //Z-min, max cells.
    INT i = 0, j = 0;
    for (; i < NumCells(GEOM_X); i++) {
      for (; j < NumCells(GEOM_Y); j++) {
        AddToFront(WHITE, iCELL_INDEX(i, j, 0), front, alreadyVisited);
        AddToFront(WHITE, iCELL_INDEX(i, j, NumCells(GEOM_Z) - 1), front, alreadyVisited);
      }
    }

    //Y-min, max cells.
    i = 0, j = 0;
    for (; i < NumCells(GEOM_X); i++) {
      for (; j < NumCells(GEOM_Z); j++) {
        AddToFront(WHITE, iCELL_INDEX(i, 0, j), front, alreadyVisited);
        AddToFront(WHITE, iCELL_INDEX(i, NumCells(GEOM_Y) - 1, j), front, alreadyVisited);
      }
    }

    //X-min, max cells
    i = 0, j = 0;
    for (; i < NumCells(GEOM_Y); i++) {
      for (; j < NumCells(GEOM_Z); j++) {
        AddToFront(WHITE, iCELL_INDEX(0, i, j), front, alreadyVisited);
        AddToFront(WHITE, iCELL_INDEX(NumCells(GEOM_X) - 1, i, j), front, alreadyVisited);
      }
    }
  }

  template<typename _GRID_ELT>
  VOID tGRID<_GRID_ELT>::MarkBlackCells(cGRID_CELL_MARKS &visitedCells) {
    for (INT i = 0; i < m_numCells[0]; i++) {
      for (INT j = 0; j < m_numCells[1]; j++) {
        for (INT k = 0; k < m_numCells[2]; k++) {
          iCELL_INDEX index(i, j, k);
          cGRID_CELL_MARKS::iterator pos = visitedCells.find(iCELL_INDEX(i, j, k));
          if (pos == visitedCells.end()) {
            CellColor(index, BLACK);
          }
        }
      }
    }
  }

  template<typename _GRID_ELT>
  VOID tGRID<_GRID_ELT>::FlipColors() {
    typename cGRID_CELL_COLORS::iterator curr = m_cellColors.begin();
    typename cGRID_CELL_COLORS::iterator last = m_cellColors.end();

    for (; curr != last; curr++) {
      if (curr->second == BLACK) {
        m_cellColors[curr->first] = WHITE;
      } else if (curr->second == WHITE) {
        m_cellColors[curr->first] = BLACK;
      }
    }
  }

  inline VOID CoarseNeighbors(sINDEX index, sINDEX neighbors[6]) {
    neighbors[0] = iCELL_INDEX(index.x - 1, index.y, index.z);
    neighbors[1] = iCELL_INDEX(index.x + 1, index.y, index.z);
    neighbors[2] = iCELL_INDEX(index.x, index.y - 1, index.z);
    neighbors[3] = iCELL_INDEX(index.x, index.y + 1, index.z);
    neighbors[4] = iCELL_INDEX(index.x, index.y, index.z - 1);
    neighbors[5] = iCELL_INDEX(index.x, index.y, index.z + 1);
  }
  /*
   *  1. Before calling this method all gray cells need to be marked.
   *  2. The outer layer of cells are assumed to be WHITE.
   *  3. Keep walking until all white cells are visited.
   */
  template<typename _GRID_ELT>
  VOID tGRID<_GRID_ELT>::FloodFill() {
    std::vector<iCELL_INDEX> front;
    cGRID_CELL_MARKS alreadyVisited;

    AdvancingFront(front, alreadyVisited);

    INT oldFrontSize, newFrontSize;
    oldFrontSize = newFrontSize = front.size();

    do {
      INT i = 0;
      oldFrontSize = front.size();
      while (i < oldFrontSize) {
        iCELL_INDEX cellIndex = front[i];
        cGRID_CELL_COLORS::iterator cellColorItr = m_cellColors.find(cellIndex);
        eCELL_COLOR cellColor = cellColorItr != m_cellColors.end() ? cellColorItr->second : UNKNOWN;

        sINDEX neighbors[6];
        CoarseNeighbors(cellIndex, neighbors);

        for (INT j = 0; j < 6; j++)
          AddToFront(cellColor, neighbors[j], front, alreadyVisited);

        i++;
      }

      newFrontSize = front.size();

    } while (newFrontSize > oldFrontSize);

    MarkBlackCells(alreadyVisited);
    CreateBlackAndWhiteGridCells();
//  FlipColors();

    CollectWhiteAndBlackVertices();
  }

  template<typename _GRID_ELT>
  VOID tGRID<_GRID_ELT>::CreateBlackAndWhiteGridCells() {
    typename cGRID_CELL_COLORS::iterator curr = m_cellColors.begin();
    typename cGRID_CELL_COLORS::iterator last = m_cellColors.end();

    for (; curr != last; curr++) {
      if (curr->second != GRAY) {
        _GRID_ELT* elt = AddCoarseElement(curr->first);
        elt->Color(curr->second);
      }
    }
  }
}

