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
  BOOL tSUB_DIVIDE<_GRID_TYPE, _GRID_CELL>::Do() {
    Init();


  }
} /* namespace grid_gen */
