#ifndef _MODEL_DEFN_TO_GRID_DEFN_HPP_
#define _MODEL_DEFN_TO_GRID_DEFN_HPP_

#include "defs.hpp"
#include "manifold_obj.hpp"

namespace grid_gen {

template<typename _MODEL_TYPE>
struct tDEFINE_MODEL_GRID {
  typedef typename _MODEL_TYPE::cMESH cMESH;
  typedef tGRID_CELL<cMESH> cGRID_CELL;
  typedef tGRID<cGRID_CELL> cGRID;
  typedef tMANIFOLD_OBJ<cGRID, cMESH> cMANIFOLD_OBJ;
};

}
#endif //_MODEL_DEFN_TO_GRID_DEFN_HPP_;
