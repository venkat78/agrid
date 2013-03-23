#ifndef _GEOM_COMMON_INTERNAL2D_HPP_
#define _GEOM_COMMON_INTERNAL2D_HPP_

#include "geom_defs.hpp"
#include "vector2.hpp"
#include "point2.hpp"
#include "numerics2d.hpp"
#include "box2.hpp"
#include "line2.hpp"
#include "ray2.hpp"
#include "segment2.hpp"
#include "triangle2.hpp"

namespace geom {
  class cGEOM_2D_KERNEL {
  public:
    typedef cPOINT2 cPOINT;
    typedef cSEGMENT2 cSEGMENT;
    typedef cBOX2 cBOX;
    typedef cLINE2 cLINE;
    typedef cRAY2 cRAY;
    typedef cTRIANGLE2 cTRIANGLE;
  };
}
#endif /* COMMON_INTERNAL2D_HPP_ */
