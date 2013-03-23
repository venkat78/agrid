#include "common_internal2d.hpp"
namespace geom {

//Point is assumed to lie on SupportingLine.
REAL cSEGMENT2::Param(const cPOINT2 &point) const {
    ASSERT(!IsDegenerate());
    ASSERT(SupportingLine().OrientedSide(point) == ON_ORIENTED_BOUNDARY);
    if(fabs(m_endPoints[1][0] - m_endPoints[0][0]) < cLIMITS::Epsilon())
      return (point[1] - m_endPoints[0][1])/(m_endPoints[1][1] - m_endPoints[0][1]);
    else
      return (point[0] - m_endPoints[0][0])/(m_endPoints[1][0] - m_endPoints[0][0]);
  }

//Point is assumed to lie on SupportingLine.
  BOOL cSEGMENT2::IsOnSegment(const cPOINT2 &point) const {
    ASSERT(!IsDegenerate());
    ASSERT(SupportingLine().OrientedSide(point) == ON_ORIENTED_BOUNDARY);
    REAL param = Param(point);
    ASSERT(!isnan(param));
    return (param < 0.0 || param > 1.0) ? false : true;
  }

  VOID cSEGMENT2::XRange(REAL range[2]) const {
      if(m_endPoints[0][0] < m_endPoints[1][0]) {
        range[0] = m_endPoints[0][0];
        range[1] = m_endPoints[1][0];
      }
      else {
        range[1] = m_endPoints[0][0];
        range[0] = m_endPoints[1][0];
      }
    }

  VOID cSEGMENT2::YRange(REAL range[2]) const {
      if(m_endPoints[0][1] < m_endPoints[1][1]) {
        range[0] = m_endPoints[0][1];
        range[1] = m_endPoints[1][1];
      }
      else {
        range[1] = m_endPoints[0][1];
        range[0] = m_endPoints[1][1];
      }
    }
}
