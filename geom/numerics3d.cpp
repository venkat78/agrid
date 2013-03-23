#include "common_internal3d.hpp"

namespace geom {
eANGLE AngleType(const cPOINT3 &point1,
			     const cPOINT3 &point2,
			     const cPOINT3 &point3)
{
  cVECTOR3 vec1 = point1 - point2;
  cVECTOR3 vec2 = point3 - point2;

  return (eANGLE) Sign(vec1*vec2);
}

REAL Angle(const cPOINT3 &point1,
		   const cPOINT3 &point2,
		   const cPOINT3 &point3)
{
  cVECTOR3 vec1 = point1 - point2;
  cVECTOR3 vec2 = point3 - point2;


  REAL vec1Len = sqrt(vec1*vec1);
  REAL vec2Len = sqrt(vec2*vec2);

  REAL vec1DotVec2 = vec1*vec2;
  ASSERT(vec1Len != 0.0 || vec2Len != 0.0);

  return std::acos(vec1DotVec2/(vec1Len*vec2Len));
}

eORIENTATION CoplanarOrientation(const REAL px, const REAL py,
					             const REAL qx, const REAL qy,
					             const REAL rx, const REAL ry)
{
  REAL area = Det2Val(rx-qx, ry-qy,
			  px-qx, py-qy);

  if(fabs(area) <= cLIMITS::Tolerance())
    return COLLINEAR;

  return Sign(area);
}

eORIENTATION CoplanarOrientation(const cPOINT3 &point1,
		const cPOINT3 &point2,
		const cPOINT3 &point3)
{

  eORIENTATION orientation = CoplanarOrientation(point1[0], point1[1],
								  point2[0], point2[1],
								  point3[0], point3[1]);

  if(orientation != COLLINEAR)
    return orientation;

  orientation = CoplanarOrientation(point1[1], point1[2],
						  point2[1], point2[2],
						  point3[1], point3[2]);

  if(orientation != COLLINEAR)
    return orientation;

  return CoplanarOrientation(point1[2], point1[0],
					   point2[2], point2[0],
					   point3[2], point3[0]);
}

BOOL AreOrderedAlongLine(const cPOINT3 &p,
                         const cPOINT3 &q,
                         const cPOINT3 &r)
{
  cVECTOR3 pr = r - p;
  eCOORD maxCoord = pr.AbsMaxCoord();

  cVECTOR3 pq = q - p;

  REAL prLen = pr[maxCoord];
  REAL pqLen = pq[maxCoord];

  REAL absPRLen = fabs(prLen);
  REAL absPQLen = fabs(pqLen);

  if(absPRLen <= cLIMITS::Tolerance()) {
    if(absPQLen <= cLIMITS::Tolerance())
      return true; //p, q, r are coincident.
    else
      return false; //p and r are coincident.
  }

  REAL lambda = pqLen/prLen;

  if(fabs(lambda) <= cLIMITS::Tolerance())
    lambda = 0.0;

  if(lambda >= 0.0 && lambda <= 1.0)
    return true;

  return false;
}

BOOL PointsCollinear(const cPOINT3 &p, const cPOINT3 &q, const cPOINT3 &r)
{
	cSEGMENT3 segment(p, r);
	cPOINT3 projectionPoint = segment.PointProjectionPoint(q);
	return (projectionPoint == q);
}

} //namespace
