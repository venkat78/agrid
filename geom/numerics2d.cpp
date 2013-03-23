#include "common_internal2d.hpp"

namespace geom {

REAL Angle(const cPOINT2 &point1,
	       const cPOINT2 &point2,
	       const cPOINT2 &point3)
{
  cVECTOR2 vec1 = point1 - point2;
  cVECTOR2 vec2 = point3 - point2;

  REAL commonFactor = 1.0/sqrt((vec1*vec1) * (vec2*vec2));

  REAL sinTheta = Det2Val(vec2[0], vec2[1],
			      vec1[0], vec1[1]) * commonFactor;

  REAL cosTheta = (vec1*vec2)*commonFactor;

  if(fabs(sinTheta) <= cLIMITS::Epsilon() &&
     fabs(cosTheta) <= cLIMITS::Epsilon())
    return 0.0;

  REAL theta = atan2(sinTheta, cosTheta);
  if (theta < 0.0) // increase it by 2pi
    theta += 2.0 * M_PI;

  return theta;
}

REAL Area(const cPOINT2 &point1,
			const cPOINT2 &point2,
       const cPOINT2 &point3)
{
  cVECTOR2 vec1 = point2 - point1;
  cVECTOR2 vec2 = point3 - point2;

  return 0.5*Det2Val(vec1[0], vec1[1],
			   vec2[0], vec2[1]);
}

eORIENTATION CoplanarOrientation(const cPOINT2 &point1,
		const cPOINT2 &point2,
		const cPOINT2 &point3)
{

  REAL area = Det2Val(point3[0]-point2[0], point3[1]-point2[1],
			  point1[0]-point2[0], point1[1]-point2[1]);

  if(fabs(area) <= cLIMITS::Tolerance())
    return COLLINEAR;

  return Sign(area);
}

}
