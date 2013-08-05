#ifndef _GRID_GEN_GRID_GENERATOR_HPP_
#define _GRID_GEN_GRID_GENERATOR_HPP_

#include "defs.hpp"
#include "model_defn_to_grid_defn.hpp"

namespace grid_gen {

template<typename _MODEL_TYPE>
class tGRID_GENERATOR {
  typedef typename tDEFINE_MODEL_GRID<_MODEL_TYPE>::cGRID cGRID;
  typedef typename tDEFINE_MODEL_GRID<_MODEL_TYPE>::cGRID_CELL cGRID_CELL;

 public:
  tGRID_GENERATOR() {
  }
  tGRID_GENERATOR(const cBOX3 &simVol, INT numCells[3])
      : m_grid(simVol, numCells) {
  }

 private:
  cGRID m_grid;
};

}

#endif //_GRID_GEN_GRID_GENERATOR_HPP_
