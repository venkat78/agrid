/*
 * subdivide.cpp
 *
 *  Created on: Sep 30, 2013
 *      Author: venkat
 */

#include "subdivide.hpp"

namespace grid_gen {
  template<typename _GRID_TYPE, typename _GRID_CELL>
  VOID tSUB_DIVIDE<_GRID_TYPE, _GRID_CELL>::Init() {
    BoxToMesh(m_box, m_clay);
    for (INT i = 0; i < 8; i++) {
      INT p = vertex_enumeration[i][0];
      INT q = vertex_enumeration[i][1];
      INT r = vertex_enumeration[i][2];

      m_clay.Vertex(i)->Color(m_cell->VertexColor(p, q, r));
    }
  }

  template<typename _GRID_TYPE, typename _GRID_CELL>
  VOID tSUB_DIVIDE<_GRID_TYPE, _GRID_CELL>::Clip() {
    INT facetIndices[6] = {0, 1, 2, 3, 4, 5};
    REAL val = m_box.Center()[m_coord];

    for(INT i = 0; i < 6; i++)
      tMESH_CLIPPER<cCUT_CELL_CLAY>(m_clay).Clip(i, m_coord, val);
  }

  template<typename _GRID_TYPE, typename _GRID_CELL>
  BOOL tSUB_DIVIDE<_GRID_TYPE, _GRID_CELL>::Do() {
    Init();
    Clip();

  }
} /* namespace grid_gen */
