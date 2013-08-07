#ifndef _BASE_GRID_GRID_HPP_
#define _BASE_GRID_GRID_HPP_

#include "defs.hpp"
#include "grid_object_factory.hpp"
#include "grid_cell.hpp"

namespace base_grid {

template<typename _GRID_ELT>
class tGRID {
 public:
  /*
   * Constructors.
   */
  tGRID() {
    m_numCells[0] = m_numCells[1] = m_numCells[2];
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
    return m_factory.NewElement(index);
  }

  /*
   * Geometric properties of coarse level grid.
   */
  cBOX3 CellBox(sINDEX index);

  iCELL_INDEX CellIndex(const cPOINT3 &point);

  INT ModifiedCellIndex(const cPOINT3 &point, cVECTOR3 normal,
                        iCELL_INDEX indices[2]);

  iCELL_INDEX CellIndex(REAL x, REAL y, REAL z);

  REAL CellUpperBound(REAL val, eCOORD coord);

  REAL CellLowerBound(REAL val, eCOORD coord);

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
    REAL min01 = (
        (m_cellSize[0] < m_cellSize[1]) ? m_cellSize[0] : m_cellSize[1]);
    return ((min01 < m_cellSize[2]) ? min01 : m_cellSize[2]);
  }

  REAL CellLengthMax() {
    REAL max01 = (
        (m_cellSize[0] > m_cellSize[1]) ? m_cellSize[0] : m_cellSize[1]);
    return ((max01 > m_cellSize[2]) ? max01 : m_cellSize[2]);
  }

 private:
  tGRID_OBJECT_FACTORY<_GRID_ELT> m_factory;
  INT m_numCells[3];
  REAL m_cellSize[3];

  cBOX3 m_bounds;

};
}

#endif //_RECTILINEAR_GRID_HPP_
