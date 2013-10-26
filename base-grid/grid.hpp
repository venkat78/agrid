#ifndef _BASE_GRID_GRID_HPP_
#define _BASE_GRID_GRID_HPP_

#include "defs.hpp"
#include "grid_object_factory.hpp"
#include "grid_cell.hpp"
#include <cstdio>

namespace base_grid {
  struct sGRID_STORAGE_POLICY {
    static const INT numObjectsInPage = 512;
    static const INT logOfObjectsInPage = 9;
    static const INT numInitialPages = 512;
  };

  template<typename _GRID_ELT, typename _GRID_STORAGE_POLICY = sGRID_STORAGE_POLICY>
  class tGRID {
     public:
      /*
       * Constructors.
       */
      tGRID() {
        m_numCells[0] = m_numCells[1] = m_numCells[2] = 0;
      }

      tGRID(const cBOX3 &bounds, INT numCells[3]);
      tGRID(const cBOX3 &bounds, INT numCells);

      /*
       * Adding coarse elements of a grid.
       */
      _GRID_ELT *GetElement(INT index) {
        return m_factory.GetElement(index);
      }

      _GRID_ELT *CoarseElement(sINDEX index) {
        return m_factory.CoarseElement(index);
      }

      _GRID_ELT* AddCoarseElement(sINDEX index) {
        _GRID_ELT* elt = m_factory.NewElement(index);
        elt->Box(CellBox(index));
        return elt;
      }

      _GRID_ELT* AddCell()
      {
        return m_factory.NewElement();
      }

      /*
       * Geometric properties of coarse level grid.
       */
      cBOX3 CellBox(sINDEX index);

      iCELL_INDEX CellIndex(const cPOINT3 &point);

      INT ModifiedCellIndex(const cPOINT3 &point, cVECTOR3 normal, iCELL_INDEX indices[2]);

      iCELL_INDEX CellIndex(REAL x, REAL y, REAL z);

      REAL CellUpperBound(REAL val, eCOORD coord);

      REAL CellLowerBound(REAL val, eCOORD coord);

      INT NumCells(INT i) {
        return m_numCells[i];
      }

      INT NumCells() {
        return m_numCells[0] * m_numCells[1] * m_numCells[2];
      }

      /*
       * Vertex related methods
       */
      INT NumVertices() {
        return (m_numCells[0] + 1) * (m_numCells[1] + 1) * (m_numCells[2] + 1);
      }

      cPOINT3 VertexPoint(INT indices[3]) {
        cPOINT3 &min = m_bounds.Min();

        return cPOINT3(min[0] + (indices[0] * m_cellSize[0]), min[1] + (indices[1] * m_cellSize[1]), min[2] + (indices[2] * m_cellSize[2]));
      }

      cPOINT3 VertexPoint(iGRID_VERTEX index) {
        INT indices[3];
        indices[0] = index.x;
        indices[1] = index.y;
        indices[2] = index.z;
        return VertexPoint(indices);
      }

      VOID VertexIndex(const cPOINT3 &point, INT indices[3]) {
        cPOINT3 &min = m_bounds.Min();

        for (INT i = 0; i < 3; i++) {
          indices[i] = (INT) ((point[i] - min[i]) / m_cellSize[i]);
        }
      }

      //Assumes point is a grid vertex.
      iGRID_VERTEX VertexIndex(const cPOINT3 &point) {
        INT indices[3];
        VertexIndex(point, indices);
        return iGRID_VERTEX(indices[0], indices[1], indices[2]);
      }

      /*
       * Cell length related methods.
       */
      REAL CellLength(INT i) {
        return m_cellSize[i];
      }
      REAL CellLengthX() {
        return m_cellSize[0];
      }
      REAL CellLengthY() {
        return m_cellSize[1];
      }
      REAL CellLengthZ() {
        return m_cellSize[2];
      }
      REAL CellLengthMin() {
        REAL min01 = ((m_cellSize[0] < m_cellSize[1]) ? m_cellSize[0] : m_cellSize[1]);
        return ((min01 < m_cellSize[2]) ? min01 : m_cellSize[2]);
      }

      REAL CellLengthMax() {
        REAL max01 = ((m_cellSize[0] > m_cellSize[1]) ? m_cellSize[0] : m_cellSize[1]);
        return ((max01 > m_cellSize[2]) ? max01 : m_cellSize[2]);
      }

      VOID CellColor(iCELL_INDEX index, eCELL_COLOR color) {
        m_cellColors[index] = color;
      }

      eCELL_COLOR CellColor(iCELL_INDEX index);

      eCELL_COLOR VertexColor(iGRID_VERTEX index);
     public:
      //Methods related to walking.
      VOID FloodFill();

      VOID ClearColorMarkers() {
        m_cellColors.clear();
        m_vertexColors.clear();
      }

     private:
      VOID CollectWhiteAndBlackVertices();
      VOID AdvancingFront(std::vector<iCELL_INDEX> &, cGRID_CELL_MARKS &);
      VOID AddToFront(eCELL_COLOR, iCELL_INDEX, std::vector<iCELL_INDEX> &, cGRID_CELL_MARKS &);
      VOID MarkBlackCells(cGRID_CELL_MARKS &visitedCells);
      VOID FlipColors();
      VOID CreateBlackAndWhiteGridCells();
      BOOL IsValid(iCELL_INDEX index) {
        return ((0 <= index.x && index.x < m_numCells[0]) && (0 <= index.y && index.y < m_numCells[1]) && (0 <= index.z && index.z < m_numCells[2]));
      }

     private:
      tGRID_OBJECT_FACTORY<_GRID_ELT, _GRID_STORAGE_POLICY> m_factory;
      INT m_numCells[3];
      REAL m_cellSize[3];

      cBOX3 m_bounds;

      //First gray cells should always be marked.
      //Then white cells will be marked.
      cGRID_CELL_COLORS m_cellColors;

      //This is to hold colors of white vertices.
      cGRID_VERTEX_COLORS m_vertexColors;

  };
}

#endif //_RECTILINEAR_GRID_HPP_
