#ifndef _GRID_GEN_MANIFOLD_OBJ_HPP_
#define _GRID_GEN_MANIFOLD_OBJ_HPP_

#include "defs.hpp"

namespace grid_gen {
/*
 * Class stores the details of manifolds being processed.
 */
template<typename _GRID_TYPE, typename _MESH_TYPE>
class tMANIFOLD_OBJ : public tMANIFOLD_RECORD<_MESH_TYPE> {
 public:
  typedef tMANIFOLD_RECORD<_MESH_TYPE> cBASE;
  typedef typename _MESH_TYPE::cMANIFOLD cMANIFOLD;
 public:
  tMANIFOLD_OBJ()
      : cBASE(),
        m_grid() {
  }

  tMANIFOLD_OBJ(const cBOX3 &simVol, INT numCells[3], cMANIFOLD* manifold,
                _MESH_TYPE *mesh)
      : cBASE(manifold, mesh),
        m_grid(simVol, numCells) {
  }

  _GRID_TYPE *Grid() {
    return m_grid;
  }

 private:
  _GRID_TYPE m_grid;
};

}

#endif //GRID_GEN_MANIFOLD_OBJ_HPP_
