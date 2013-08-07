#ifndef _GRID_GEN_GRID_GENERATOR_HPP_
#define _GRID_GEN_GRID_GENERATOR_HPP_

#include "defs.hpp"
#include "model_defn_to_grid_defn.hpp"

namespace grid_gen {

template<typename _MODEL_TYPE>
class tGRID_GENERATOR {
  typedef typename tDEFINE_MODEL_GRID<_MODEL_TYPE>::cGRID cGRID;
  typedef typename tDEFINE_MODEL_GRID<_MODEL_TYPE>::cGRID_CELL cGRID_CELL;
  typedef typename _MODEL_TYPE::cMESH cMESH;
  typedef typename cMESH::cMANIFOLD cMANIFOLD;
  typedef tMANIFOLD_RECORD<cMESH> cMANIFOLD_RECORD;

 public:
  tGRID_GENERATOR() {
  }
  tGRID_GENERATOR(const cBOX3 &simVol, INT numCells[3])
      : m_grid(simVol, numCells) {
  }

  /*
   *  Registers model in the grid generator.
   */
  VOID Register(_MODEL_TYPE *model);

 private:

  /*
   *  1. Registers one manifold at a time.
   *  2. Clips each facet to coarse cell size.
   */
  VOID Register(cMESH *mesh);

  /*
   * Slices the facets of a mesh, so that they are confined
   * to coarse cells of m_grid.
   */
  VOID Slice(cMESH *mesh);

  /*
   * Register facets of the manifold in the coarse level
   * grid.
   */
  VOID RegisterManifold(cMANIFOLD *manifold, cMESH *mesh);

 private:
  /*
   * manifolds recorded in the grid.
   */
  tTABLE<cMANIFOLD_RECORD> m_manifolds;
  cGRID m_grid;
};

}

#endif //_GRID_GEN_GRID_GENERATOR_HPP_
