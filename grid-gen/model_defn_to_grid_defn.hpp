#ifndef _MODEL_DEFN_TO_GRID_DEFN_HPP_
#define _MODEL_DEFN_TO_GRID_DEFN_HPP_

#include "defs.hpp"

namespace grid_gen {

template<typename _MODEL_TYPE>
struct tDEFINE_MODEL_GRID {
  typedef typename _MODEL_TYPE::cMESH cMESH;
  typedef tGRID_CELL<cMESH> cGRID_CELL;
  typedef tGRID<cGRID_CELL> cGRID;
};

}
#endif //_MODEL_DEFN_TO_GRID_DEFN_HPP_;
