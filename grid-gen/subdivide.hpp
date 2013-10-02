/*
 * subdivide.hpp
 *
 *  Created on: Sep 30, 2013
 *      Author: venkat
 */

#ifndef _GRID_GEN_SUBDIVIDE_HPP_
#define _GRID_GEN_SUBDIVIDE_HPP_

#include "defs.hpp"
namespace grid_gen {

  template<typename _GRID_TYPE, typename _GRID_CELL>
  class tSUB_DIVIDE {
     public:
      tSUB_DIVIDE(_GRID_TYPE *grid = NULL, _GRID_CELL *cell = NULL, eCOORD coord = GEOM_COORD_NONE)
          : m_grid(grid),
            m_cell(cell),
            m_coord(coord),
            m_box(cell->Box()) {
      }
      ~tSUB_DIVIDE() {

      }

     public:
      BOOL Do();

     private:
      VOID Init();
     private:
      _GRID_TYPE *m_grid;
      _GRID_CELL *m_cell;
      eCOORD m_coord;
      cBOX3 m_box;
      cCUT_CELL_CLAY m_clay;
  };

} /* namespace grid_gen */
#endif /* SUBDIVIDE_HPP_ */
