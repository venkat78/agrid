#ifndef _GEOM_COMMON_INTERNAL3D_HPP_
#define _GEOM_COMMON_INTERNAL3D_HPP_

#include "common_internal2d.hpp"
#include "vector3.hpp"
#include "point3.hpp"
#include "point3i.hpp"
#include "numerics3d.hpp"
#include "plane3.hpp"
#include "line3.hpp"
#include "ray3.hpp"
#include "segment3.hpp"
#include "box3.hpp"
#include "box3i.hpp"
#include "triangle3.hpp"
#include "newell_normal_builder.hpp"

namespace geom {
  class cGEOM_3D_KERNEL {
  public:
    typedef cPOINT3 cPOINT;
    typedef cSEGMENT3 cSEGMENT;
    typedef cBOX3 cBOX;
    typedef cLINE3 cLINE;
    typedef cRAY3 cRAY;
    typedef cTRIANGLE3 cTRIANGLE;
  };
}
#endif /* COMMON_INTERNAL3D_HPP_ */
