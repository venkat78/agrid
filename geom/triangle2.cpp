#include "common_internal2d.hpp"

namespace geom {

INT cTRIANGLE2::LongestSide() const
{
  INT longestSide = 0;
  REAL edge1SquaredLen = SquaredDistance(m_vertices[0], m_vertices[1]);

  for(INT i = 1; i < 3; i++) {
    REAL edgeSquaredLen = SquaredDistance(m_vertices[i], m_vertices[(i+1) % 3]);

    if(edge1SquaredLen < edgeSquaredLen) {
      edge1SquaredLen = edgeSquaredLen;
      longestSide = i;
    }
  }

  return longestSide;
}

BOOL cTRIANGLE2::IsPointInside(const cPOINT2 &point) const
{
  REAL area = Area(m_vertices[0], m_vertices[1], m_vertices[2]);

  for(INT i = 0; i < 3 ; i++) {
    REAL area1 = Area(m_vertices[i],
			      m_vertices[(i+1)%3],
			      point);

    if(fabs(area1) <= cLIMITS::Epsilon())
      area1 = 0.0;

    if(area * area1 < 0.0)
      return false;
  }

  return true;
}

}
