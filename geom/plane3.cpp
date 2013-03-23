#include "common_internal3d.hpp"

namespace geom {
cPLANE3::cPLANE3(const cPOINT3 &p,
			       const cPOINT3 &q,
			       const cPOINT3 &r)
{
  REAL rpx = p[0]-r[0];
  REAL rpy = p[1]-r[1];
  REAL rpz = p[2]-r[2];
  REAL rqx = q[0]-r[0];
  REAL rqy = q[1]-r[1];
  REAL rqz = q[2]-r[2];

  // Cross product rp * rq
  m_a = rpy*rqz - rqy*rpz;
  m_b = rpz*rqx - rqz*rpx;
  m_c = rpx*rqy - rqx*rpy;
  //it needs to be normalized
  REAL l = sqrt(m_a*m_a + m_b*m_b + m_c*m_c);
  if (l > cLIMITS::Epsilon()){
	  m_a /= l;
	  m_b /= l;
	  m_c /= l;
  }
  m_d = - m_a*r[0] - m_b*r[1] - m_c*r[2];
}

cPLANE3::cPLANE3(const cPOINT3 &point,
			       const cVECTOR3 &vec)
{
  REAL vectorLength = vec.Length();
  REAL x = vec[0]/vectorLength;
  REAL y = vec[1]/vectorLength;
  REAL z = vec[2]/vectorLength;

  m_a = x; m_b = y; m_c = z;

  m_d = -x*point[0] - y*point[1] - z*point[2];
}

cPOINT3
cPLANE3::Projection(const cPOINT3 &point) const
{
  // the equation of the plane is Ax+By+Cz+D=0
  // the normal direction is (A,B,C)
  // the projected point is p-lambda(A,B,C) where
  // A(x-lambda A) + B(y-lambda B) + C(z-lambda C) + D = 0

  REAL num = A()*point[0] + B()*point[1] + C()*point[2] + D();
  REAL den = A()*A() + B()*B() + C()*C();
  REAL lambda = num / den;

  return cPOINT3(point[0] - lambda * A(),
		 point[1] - lambda * B(),
		 point[2] - lambda * C());

}

eORIENTED_SIDE
cPLANE3::OrientedSide(const cPOINT3 &point) const
{
  REAL signedDistance = SignedDistance(point);
  if(fabs(signedDistance) <= cLIMITS::Tolerance())
    return ON_ORIENTED_BOUNDARY;

  if(signedDistance > 0.0)
    return ON_POSITIVE_SIDE;

  return ON_NEGATIVE_SIDE;
}

eRETURN_TYPE cPLANE3::SegmentIntersect(const cSEGMENT3 &segment,
		cPOINT3 *p_intersectionPoint) const
{
//  printf("vtx0 : "); segment.Vertex(0).Print();
//  printf("vtx1 : "); segment.Vertex(1).Print();
  REAL dist0 = SignedDistance(segment.Vertex(0));
  REAL dist1 = SignedDistance(segment.Vertex(1));
//  printf("dist0 = %lg; dist1 = %lg\n", dist0, dist1);

  if (fabs(dist0) <= cLIMITS::Tolerance() && fabs(dist1) <= cLIMITS::Tolerance())
    return RT_SEGMENT3;
  else if (fabs(dist0) <= cLIMITS::Tolerance()){
    if (p_intersectionPoint != NULL)
    	*p_intersectionPoint = segment.Vertex(0);
    return RT_ENDPOINT3;
  }
  else if (fabs(dist1) <= cLIMITS::Tolerance()){
    if (p_intersectionPoint != NULL)
      *p_intersectionPoint = segment.Vertex(1);
    return RT_ENDPOINT3;
  }

  else if (dist0 * dist1 > 0)
    return FAILURE;
  REAL t = dist0 / (dist0 - dist1);
//  printf("t = %lg\n", t);
  if (p_intersectionPoint != NULL)
    *p_intersectionPoint = segment.Evaluate(t);
//  printf("intersectionPoint"); p_intersectionPoint->Print();
  return RT_POINT3;
}

}
