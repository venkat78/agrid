#include "geom_defs.hpp"
#include "vector3.hpp"
namespace geom {

REAL cLIMITS::max = DBL_MAX;
REAL cLIMITS::epsilon = DBL_EPSILON;
REAL cLIMITS::tolerance = 1e-10;
REAL cLIMITS::min = DBL_MIN;

eCOORD cVECTOR3::AbsMaxCoord() const {
    eCOORD maxCoord = GEOM_X;

    DO_COORDS(coord) {
      if(fabs(m_coords[coord]) > fabs(m_coords[maxCoord]))
	maxCoord = coord;
    }

    return maxCoord;
  }

eCOORD cVECTOR3::AbsMinCoord() const {
    eCOORD minCoord = GEOM_X;

    DO_COORDS(coord) {
      if(fabs(m_coords[coord]) < fabs(m_coords[minCoord]))
        minCoord = coord;
    }

    return minCoord;
  }

BOOL cVECTOR3::IsNull() const {
    if(fabs(m_coords[0]) < cLIMITS::Tolerance() &&
       fabs(m_coords[1]) < cLIMITS::Tolerance() &&
       fabs(m_coords[2]) < cLIMITS::Tolerance())
      return true;

    return false;
  }

cVECTOR3 Cross(const cVECTOR3& vec0, const cVECTOR3& vec1)
{
  cVECTOR3 result;
  result[0] = vec0[1]*vec1[2] - vec0[2]*vec1[1];
  result[1] = vec0[2]*vec1[0] - vec0[0]*vec1[2];
  result[2] = vec0[0]*vec1[1] - vec0[1]*vec1[0];
  return result;
}

//return a norma;ized vector perpendicular to the given one
cVECTOR3 cVECTOR3::PerpVector() const
{
  cVECTOR3 perpVector;
  eCOORD minCoord = AbsMinCoord();
  perpVector[minCoord] = 0.0;
  perpVector[CoordPlusOne(minCoord)] = m_coords[CoordPlusTwo(minCoord)];
  perpVector[CoordPlusTwo(minCoord)] = -m_coords[CoordPlusOne(minCoord)];
  perpVector = perpVector.Normalize();
  return perpVector;
}

VOID Normalize(cVECTOR3& vec)
{
  REAL length = vec.Length();
  vec /= length;
}

}//namespace
