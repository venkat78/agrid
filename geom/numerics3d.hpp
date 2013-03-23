#ifndef _GEOM_NUMERICS3D_HPP_
#define _GEOM_NUMERICS3D_HPP_

namespace geom {
inline REAL SquaredDistance(const cPOINT3 &point1,
		  const cPOINT3 &point2)
{
  cVECTOR3 pointDiff = (point1 - point2);
  return pointDiff * pointDiff;
}

inline cVECTOR3 Cross(const cVECTOR3 &vec1,
					  const cVECTOR3 &vec2)
{
  return cVECTOR3(vec1[1]*vec2[2] - vec1[2]*vec2[1],
			     vec1[2]*vec2[0] - vec1[0]*vec2[2],
			     vec1[0]*vec2[1] - vec1[1]*vec2[0]);
}

//----------------------------------------------------------
//Computes area formed by triangle Point1, Point2 and Point3
//-----------------------------------------------------------

inline REAL Area(const cPOINT3 &point1,
				const cPOINT3 &point2,
				const cPOINT3 &point3)
{
	cVECTOR3 areaVector = Cross(point2 - point1, point3 - point2);
	return 0.5*sqrt(areaVector*areaVector);
}


eANGLE AngleType(const cPOINT3 &point1, const cPOINT3 &point2, const cPOINT3 &point3);

REAL Angle(const cPOINT3 &point1, const cPOINT3 &point2, const cPOINT3 &point3);

eORIENTATION CoplanarOrientation(const REAL px, const REAL py,
					             const REAL qx, const REAL qy,
					             const REAL rx, const REAL ry);

eORIENTATION CoplanarOrientation(const cPOINT3 &point1, const cPOINT3 &point2, const cPOINT3 &point3);

//point1, point2, point3 are assumed to be collinear.
BOOL AreOrderedAlongLine(const cPOINT3 &p, const cPOINT3 &q, const cPOINT3 &r);

//points are assumed to be ordered
BOOL PointsCollinear(const cPOINT3 &p, const cPOINT3 &q, const cPOINT3 &r);

}

#endif /* _GEOM_NUMERICS3D_HPP_ */
