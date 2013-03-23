#include "common_internal3d.hpp"

namespace geom {
cPOINT2 cPOINT3::DropCoord(const eCOORD coord) const {
    if(coord == GEOM_X)
      return cPOINT2(m_coords[1], m_coords[2]);
    else if(coord == GEOM_Y)
      return cPOINT2(m_coords[2], m_coords[0]);
    else
      return cPOINT2(m_coords[0], m_coords[1]);
  }
}
