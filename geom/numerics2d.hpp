#ifndef _GEOM_NUMERICS2D_HPP_
#define _GEOM_NUMERICS2D_HPP_

namespace geom {
inline REAL SquaredDistance(const cPOINT2 &point1,
			  const cPOINT2 &point2)
{
  cVECTOR2 pointDiff = (point1 - point2);
  return pointDiff * pointDiff;
}

//------------------------------------------
// Computing 2D determinant.
//------------------------------------------

inline REAL Det2Val(const REAL det[2][2])
{
  return (det[0][0] * det[1][1]) - (det[0][1] * det[1][0]);
}


inline REAL Det2Val(const REAL a00, const REAL a01,
		    const REAL a10, const REAL a11)
{
  return a00*a11 - a01*a10;
}

REAL Area(const cPOINT2 &point1, const cPOINT2 &point2, const cPOINT2 &point3);
REAL Angle(const cPOINT2 &point1, const cPOINT2 &point2, const cPOINT2 &point3);

//-------------------------------------------
// Orientation of 3 points.
//-------------------------------------------
eORIENTATION CoplanarOrientation(const cPOINT2 &point1, const cPOINT2 &point2, const cPOINT2 &point3);

}


#endif /* GEOM_NUMERICS2D_HPP_ */
