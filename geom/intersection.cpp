#include "common_internal3d.hpp"
#include "do_intersect.hpp"
#include "intersection.hpp"

namespace geom{
eRETURN_TYPE Intersection(const cPLANE3 &plane,
			       const cLINE3 &line,
			       cPOINT3 &resultingPoint,
			       cLINE3 &resultingLine)
{


  cVECTOR3 planeNormal = plane.Normal();
  const cVECTOR3& lineDir = line.Direction();
  const cPOINT3& linePoint = line.Point();

  REAL dotProduct = lineDir * planeNormal;
  REAL signedDistance = plane.SignedDistance(linePoint);

  //Line is parallel to the plane.
  if(fabs(dotProduct) <= cLIMITS::Tolerance()) {
    if(fabs(signedDistance) <= cLIMITS::Tolerance()) { //Line is on the plane.
      resultingLine = line;
      return RT_LINE3;
    }
    else
      return FAILURE;
  }
  else {
    //Find the intersection point.
    REAL lambda = (REAL(-1.0) * signedDistance)/dotProduct;
    //resulting point = linePoint + lambda * lineDir;
    resultingPoint = linePoint + (lineDir * lambda);
    return RT_POINT3;
  }

  ASSERT(0); //Shouldn't get here.
  return FAILURE;
}


eRETURN_TYPE Intersection(const cPLANE3 &plane,
			       const cSEGMENT3 &segment,
			       cPOINT3 &resultingPoint,
			       cSEGMENT3 &resultingSegment)
{


  const cPOINT3 &source = segment.Source();
  const cPOINT3 &target = segment.Target();
  eORIENTED_SIDE sourceSide = plane.OrientedSide(source);
  eORIENTED_SIDE targetSide = plane.OrientedSide(target);
  switch (sourceSide) {
  case ON_ORIENTED_BOUNDARY:
    if (targetSide == ON_ORIENTED_BOUNDARY) {
      resultingSegment = segment;
      return RT_SEGMENT3;
    }
    else {
      resultingPoint = source;
      return RT_POINT3;
    }
  case ON_POSITIVE_SIDE:
    switch (targetSide) {
    case ON_POSITIVE_SIDE:
      return FAILURE;

    case ON_ORIENTED_BOUNDARY:
      resultingPoint = target;
      return RT_POINT3;

    case ON_NEGATIVE_SIDE:
      cLINE3 supportingLine = segment.SupportingLine();
      cLINE3 resultingLine;
      Intersection(plane, supportingLine, resultingPoint, resultingLine);
      return RT_POINT3;
    }
  case ON_NEGATIVE_SIDE:
    switch (targetSide) {
    case ON_ORIENTED_BOUNDARY:
      resultingPoint = target;
      return RT_POINT3;
    case ON_NEGATIVE_SIDE:
      return FAILURE;
    case ON_POSITIVE_SIDE:
      cLINE3 supportingLine = segment.SupportingLine();
      cLINE3 resultingLine;
      Intersection(plane, supportingLine, resultingPoint, resultingLine);
      return RT_POINT3;
    }
  }

  ASSERT(0); //Shouldn't reach here.

  return FAILURE;
}


eRETURN_TYPE Intersection(const cPLANE3 &plane,
			       const cRAY3 &ray,
			       cPOINT3 &resultingPoint,
			       cRAY3 &resultingRay)
{


  REAL vecDot = ray.ToVector() * plane.Normal();
  if(fabs(vecDot) <= cLIMITS::Tolerance()) {
    REAL distance = fabs(plane.SignedDistance(ray.Source()));
    if(distance <= cLIMITS::Tolerance()) {
      resultingRay = ray;
      return RT_RAY3;
    }

    return FAILURE;
  }
  else { //Intersection might be a point.
    cLINE3 rayLine = ray.SupportingLine(), intersectingLine;
    cPOINT3 raySource = ray.Source();

    eRETURN_TYPE returnType = Intersection(plane, rayLine,
							resultingPoint, intersectingLine);

    if(returnType != RT_POINT3) {
      ASSERT(returnType != RT_LINE3); //Should be taken care of by the above check.
      return FAILURE;
    }

    if(resultingPoint == ray.Source())
      return RT_POINT3;


    //Test might be a little slow.
    //Should be eventually replaced.
    cVECTOR3 resultVector = (resultingPoint - raySource);
    cVECTOR3 rayDirection = ray.ToVector();

    resultVector /= (resultVector.Length());
    rayDirection /= (rayDirection.Length());

    if(rayDirection == resultVector)
      return RT_POINT3;


    return FAILURE;
  }

  ASSERT(0); //should never get here.
  return FAILURE;
}


eRETURN_TYPE Intersection(const cLINE2 &line1,
			       const cLINE2 &line2,
			       cPOINT2 &resultPoint,
			       cLINE2 &resultLine)
{


  REAL denom = line1.A()*line2.B() - line2.A()*line1.B();

  if(fabs(denom) <= cLIMITS::Epsilon()) {
    cPOINT2 point(0.0, 0.0);

    REAL signedDist1 = line1.SignedDistance(point);
    REAL signedDist2 = line2.SignedDistance(point);

    //Check for coincident lines.
    if(fabs(signedDist1 - signedDist2) <=
       cLIMITS::Tolerance()) {
      resultLine = line1;
      return RT_LINE2;
    }
    else
      return FAILURE; //Lines are parallel.
  }

  //Intersection is a point;
  REAL commonFactor = 1.0/denom;
  REAL x = (line1.B()*line2.C() - line2.B()*line1.C())*commonFactor;
  REAL y = (line2.A()*line1.C() - line1.A()*line2.C())*commonFactor;

  resultPoint[0]=x; resultPoint[1]=y;
  return RT_POINT2;
}


eRETURN_TYPE Intersection(const cSEGMENT2 &segment1,
			       const cSEGMENT2 &segment2,
			       cSEGMENT2 &resultingSegment,
			       cPOINT2 &resultingPoint)
{


  cLINE2 line1 = segment1.SupportingLine();
  cLINE2 line2 = segment2.SupportingLine();

  REAL signedDistance1 = (line1.SignedDistance(segment2.Source()) *
			line1.SignedDistance(segment2.Target()));

  REAL signedDistance2 = (line2.SignedDistance(segment1.Source()) *
			line2.SignedDistance(segment1.Target()));

  if(signedDistance1 > 0 || signedDistance2 > 0)
    return FAILURE;

  //Either the segments are on the same line or intersect
  //at a point.
  cLINE2 resultingLine;
  eRETURN_TYPE retVal = Intersection(line1, line2,
						  resultingPoint, resultingLine);

  if(retVal == RT_POINT2)
    return RT_POINT2;

  const cPOINT2 *potentialEndPoints[4];
  potentialEndPoints[0] = &segment2.Source();
  potentialEndPoints[1] = &segment2.Target();
  potentialEndPoints[2] = &segment1.Source();
  potentialEndPoints[3] = &segment1.Target();

  REAL param[4];

  //Try to find the common segment.
  param[0] = segment1.Param(segment2.Source());
  param[1] = segment1.Param(segment2.Target());

  param[2] = segment2.Param(segment1.Source());
  param[3] = segment2.Param(segment1.Target());

  cPOINT2 endPoints[2];
  BOOL doIntersect = false;
  INT currIndex = 0;
  for(INT i = 0; i < 4; i++){
    if(param[i] >= 0.0 && param[i] <= 1.0) {
      doIntersect = true;
      endPoints[currIndex++] = *potentialEndPoints[i];
    }
  }

  if(doIntersect) {
    resultingSegment = cSEGMENT2(endPoints[0], endPoints[1]);

    if(resultingSegment.IsDegenerate()) {
      resultingPoint = resultingSegment.Source();
      return RT_POINT2;
    }

    return RT_SEGMENT2;
  }

  return FAILURE;
}


eRETURN_TYPE Intersection(const cRAY2 &ray,
			       const cSEGMENT2 &segment,
			       cSEGMENT2 &resultingSegment,
			       cPOINT2 &resultingPoint)
{

  cLINE2 rayLine = ray.SupportingLine();
  REAL signedDistance1 = rayLine.SignedDistance(segment.Source());
  REAL signedDistance2 = rayLine.SignedDistance(segment.Target());

  if(signedDistance1*signedDistance2 > 0)
    return FAILURE;

  cLINE2 segmentLine = segment.SupportingLine();

  cLINE2 resultLine;

  eRETURN_TYPE retVal = Intersection(rayLine, segmentLine,
					  resultingPoint, resultLine);

  ASSERT(retVal != FAILURE);
  if(retVal == FAILURE)
    return FAILURE;

  if(retVal == RT_LINE2) { //segment is collinear with ray.
    REAL param = segment.Param(ray.Source());

    if(param > 1.0)
      return FAILURE;

    if(param <= 0.0) {
      resultingSegment = segment;
      return RT_SEGMENT2;
    }

    //Segment and ray partially overlap.
    resultingSegment = cSEGMENT2(ray.Source(), segment.Target());

    if(resultingSegment.IsDegenerate()) { //Ray and segment overlap at tips.
      resultingPoint = ray.Source();
      return RT_POINT2;
    }

    return RT_SEGMENT2;
  }

  if(retVal == RT_POINT2) { //Ray and segment might still not intersect.
    REAL dot = (resultingPoint - ray.Source())*ray.ToVector(); //vec1*vec2;

    if(dot < 0) //Ray and segment don't intersect.
      return FAILURE;

    return RT_POINT2;
  }

  return FAILURE;
}

}
