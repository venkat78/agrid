#include "common_internal3d.hpp"
#include "do_intersect.hpp"
#include "intersection.hpp"
#include "pcube.hpp"

namespace geom {
inline eRETURN_TYPE DoIntersect(const cLINE2 &line,
					    const cSEGMENT2 &segment)
{
  REAL signedDistance1 = line.SignedDistance(segment.Source());
  REAL signedDistance2 = line.SignedDistance(segment.Target());

  return signedDistance1 * signedDistance2 > 0.0 ? FAILURE : SUCCESS;
}


BOOL LiangBarskyClipTest(REAL p, REAL q, REAL *u1, REAL *u2)
{
  REAL r;
  BOOL retval = true;

  if (p < 0.0)
  {
    r = q / p;
    if (r > *u2)
      retval = false;
    else if (r > *u1)
      *u1 = r;

  }
  else if (p > 0.0)
  {
    r = q / p;
    if (r < * u1)
      retval = false;
    else if (r < *u2)
      *u2 = r;
  }
  else
    /* p = 0, so line is parallel to this clipping edge */
    if(q < 0.0)
      /* Line is outside clipping edge */
      retval = false;

  return retval;

}


eRETURN_TYPE DoIntersect(const cBOX2 &box,
				     const cSEGMENT2 &segment)
{

  const cPOINT2& minPoint = box.Min();
  const cPOINT2& maxPoint = box.Max();

  const cPOINT2& source = segment.Source();
  const cPOINT2& target = segment.Target();

  REAL left = minPoint.X();
  REAL right = maxPoint.X();
  REAL bottom = minPoint.Y();
  REAL top = maxPoint.Y();

  //Doing trivial rejection.
  if((source[0] < left && target[0] < left) ||
     (source[0] > right && target[0] > right) ||
     (source[1] < bottom && target[1] < bottom) ||
     (source[1] > top && target[1] > top))
    return FAILURE;

  //Liang Barsky algorithm
  REAL u1 = 0.0, u2 = 1.0, dx = target.X() - source.X(), dy;

  if(LiangBarskyClipTest (-dx, source.X() - left, &u1, &u2))
    if(LiangBarskyClipTest (dx, right - source.X(), &u1, &u2)) {
      dy = target.Y() - source.Y();
      if(LiangBarskyClipTest (-dy, source.Y() - bottom, &u1, &u2))
        if(LiangBarskyClipTest (dy, top - source.Y(), &u1, &u2))
	  return SUCCESS;
    }

  return FAILURE;
}



eRETURN_TYPE DoIntersect(const cBOX2 &box,
				     const cRAY2 &ray)
{


  REAL t_max =  cLIMITS::Max();
  REAL t_min = cLIMITS::Min();
  REAL tolerance = cLIMITS::Tolerance();

  cPOINT2 rayOrigin = ray.Source();
  cVECTOR2 rayDirection = ray.ToVector();

  rayDirection/=rayDirection.Length();

  const cPOINT2& minPoint = box.Min();
  const cPOINT2& maxPoint = box.Max();

  for(int i = 0; i < 2; i++) {
    REAL divI = 1/rayDirection[i];
    if (divI >= 0.0){
      REAL t = (minPoint[i] - rayOrigin[i] - tolerance) * divI;
      if (t > t_min)
	t_min = t;

      t = (maxPoint[i] - rayOrigin[i] + tolerance) * divI;
      if (t < t_max)
	t_max = t;
    }
    else{
      REAL t = (maxPoint[i] - rayOrigin[i] + tolerance) * divI;
      if (t > t_min)
	t_min = t;
      t = (minPoint[i] - rayOrigin[i] - tolerance) * divI;
      if (t < t_max)
	t_max = t;
    }

    if (t_min > t_max)

      return FAILURE;
  }

  return SUCCESS;
}


eRETURN_TYPE DoIntersect(const cSEGMENT2 &segment1,
				     const cSEGMENT2 &segment2)
{


  if(DoIntersect(segment1.ComputeBbox(),
				  segment2.ComputeBbox()) == FAILURE)
    return FAILURE;

  REAL distance[4];

  //segment1
  cLINE2 segment2Line = segment2.SupportingLine();

  distance[0] = segment2Line.SignedDistance(segment1.Source());

  if(fabs(distance[0]) < cLIMITS::Tolerance())
    distance[0] = 0.0;

  distance[1] = segment2Line.SignedDistance(segment1.Target());

  if(fabs(distance[1]) < cLIMITS::Tolerance())
    distance[1] = 0.0;

  REAL seg1Distance = distance[0]*distance[1];
  if(seg1Distance > 0.0)
    return FAILURE;

  //segment2
  cLINE2 segment1Line = segment1.SupportingLine();

  distance[2] = segment1Line.SignedDistance(segment2.Source());

  if(fabs(distance[2]) < cLIMITS::Tolerance())
    distance[2] = 0.0;

  distance[3] = segment1Line.SignedDistance(segment2.Target());

  if(fabs(distance[3]) < cLIMITS::Tolerance())
    distance[3] = 0.0;

  REAL seg2Distance = distance[2]*distance[3];
  if(seg2Distance > 0.0)
    return FAILURE;

  if(seg1Distance < 0.0 && seg2Distance < 0.0)
    return SUCCESS;

  //Deciding degenerate cases.
  if(distance[0] == 0.0)
    if(segment2.IsOnSegment(segment1.Source()))
      return SUCCESS;

  if(distance[1] == 0.0)
    if(segment2.IsOnSegment(segment1.Target()))
      return SUCCESS;

  if(distance[2] == 0.0)
    if(segment1.IsOnSegment(segment2.Source()))
      return SUCCESS;

  if(distance[3] == 0.0)
    if(segment1.IsOnSegment(segment2.Target()))
      return SUCCESS;

  return FAILURE;

}


eRETURN_TYPE DoIntersect(const cTRIANGLE2 &triangle,
					    const cSEGMENT2 &segment)
{
  if(DoIntersect(triangle.ComputeBbox(),
				  segment.ComputeBbox()) == FAILURE)
    return FAILURE;

  if(triangle.IsPointInside(segment.Source()))
    return SUCCESS;

  if(triangle.IsPointInside(segment.Target()))
    return SUCCESS;

  for(INT i = 0; i < 3 ; i++){
    if(DoIntersect(triangle.Edge(i), segment) == SUCCESS)
      return SUCCESS;
  }

  return FAILURE;
}



eRETURN_TYPE DoIntersect(const cTRIANGLE3 &triangle,
				     const cSEGMENT3 &lseg, BOOL doDebug)
{

  if(DoIntersect(triangle.ComputeBbox(),
				  lseg.ComputeBbox()) == FAILURE)
    return FAILURE;

  cPLANE3 supportingPlane = triangle.SupportingPlane();
  cSEGMENT3 resultingSegment;
  cPOINT3 resultingPoint;
  if (doDebug){
	  printf("supportingPlane: ");
	  supportingPlane.Print();
  }
  if(DoIntersect(supportingPlane, lseg) != SUCCESS)
    return FAILURE;

  eRETURN_TYPE result = Intersection(supportingPlane, lseg,
						  resultingPoint, resultingSegment);


  eCOORD maxCoord = supportingPlane.Normal().AbsMaxCoord();

  cTRIANGLE2 transformedTriangle(triangle.Vertex(0).DropCoord(maxCoord),
				 triangle.Vertex(1).DropCoord(maxCoord),
				 triangle.Vertex(2).DropCoord(maxCoord));

  if(result == RT_POINT3) { //Intersection is a point.
    cPOINT2 transformedPoint = resultingPoint.DropCoord(maxCoord);
    if(transformedTriangle.IsPointInside(transformedPoint))
      return SUCCESS;
    else
      return FAILURE;
  }
  else {
    //Intersection is a line segment.
    //The segment is on the triangle plane.
    //Check for actual intersection.
    //Transform the problem to 2-D problem and try to compute the result.
    cSEGMENT2 transformedSegment(lseg.Source().DropCoord(maxCoord),
						  lseg.Target().DropCoord(maxCoord));

    if(DoIntersect(transformedTriangle,
				    transformedSegment) == SUCCESS)
      return SUCCESS;

    return FAILURE;
  }

  ASSERT(0); //Should be decided even before it gets here.

  return FAILURE;
}


eRETURN_TYPE DoIntersect(const cPLANE3 &plane,
				     const cSEGMENT3 &segment)
{

  REAL endPoint1Dist = plane.SignedDistance(segment.Vertex(0));
  REAL endPoint2Dist = plane.SignedDistance(segment.Vertex(1));

  if( fabs(endPoint1Dist) < cLIMITS::Tolerance() ||
      fabs(endPoint2Dist) < cLIMITS::Tolerance())
    return SUCCESS;
  
  if(endPoint1Dist * endPoint2Dist > 0)
    return FAILURE;

  return SUCCESS;
}

eRETURN_TYPE DoIntersect(const cBOX3 &box,
			 const cSEGMENT3 &segment)
{

  cPOINT3 startPoint = segment.Vertex(0);
  cPOINT3 endPoint = segment.Vertex(1);

  cPOINT3 boxMinBound = box.Min();
  cPOINT3 boxMaxBound = box.Max();

  REAL paramRange[2];

  paramRange[0] = 0.0;
  paramRange[1] = 1.0;


  DO_COORDS(iCoord) {
    REAL maxBound = boxMaxBound[iCoord];
    REAL minBound = boxMinBound[iCoord];

    REAL alphaFactor = endPoint[iCoord] - startPoint[iCoord];

    if( fabs(alphaFactor) > cLIMITS::Tolerance()) {
      REAL tMax = (maxBound - startPoint[iCoord])/alphaFactor;
      REAL tMin = (minBound - startPoint[iCoord])/alphaFactor;

      if( tMax < tMin ) {
	REAL tSwap = tMin;
	tMin = tMax;
	tMax = tSwap;
      }

      if( paramRange[0] < tMin )
	paramRange[0] = tMin;

      if( paramRange[1] > tMax )
	paramRange[1] = tMax;

      if( paramRange[0] >= paramRange[1] )
	return FAILURE;
    }
  }

  return SUCCESS;
}


eRETURN_TYPE DoIntersect(const cTRIANGLE3 &triangle1,
			 const cTRIANGLE3 &triangle2)
{
  for(INT i = 0; i < 3; i ++)
    if( DoIntersect(triangle2, triangle1.Segment(i)) == SUCCESS )
        return SUCCESS;

  for(INT i = 0; i < 3; i ++)
    if( DoIntersect(triangle1, triangle2.Segment(i)) == SUCCESS )
      return SUCCESS;

  return FAILURE;
}

eRETURN_TYPE DoIntersect(const cBOX3 &box1,
			 const cBOX3 &box2)
{
  DO_COORDS(coord) {
    if(box1.MaxCoord(coord) < (box2.MinCoord(coord) -
			       cLIMITS::Tolerance()))
      return FAILURE;
    if(box2.MaxCoord(coord) < (box1.MinCoord(coord) -
			       cLIMITS::Tolerance()))
      return FAILURE;
  }


  return SUCCESS;
}


eRETURN_TYPE DoIntersect(const cBOX2 &box1,
			 const cBOX2 &box2)
{

  if(box1.XMax() < ( box2.XMin() -
		     cLIMITS::Tolerance()))
    return FAILURE;

  if(box2.XMax() < ( box1.XMin() -
		     cLIMITS::Tolerance()))
    return FAILURE;

  if(box1.YMax() < ( box2.YMin() -
		     cLIMITS::Tolerance()))
    return FAILURE;

  if(box2.YMax() < ( box1.YMin() -
		     cLIMITS::Tolerance()))
    return FAILURE;

  return SUCCESS;

}


eRETURN_TYPE DoIntersect(const cBOX3 &box,
			 const cTRIANGLE3 &triangle)
{
  cVECTOR3 translationVector(cPOINT3(0.0, 0.0, 0.0) -
				box.Min());

  cVECTOR3 triNormal = triangle.Normal();

  cTRIANGLE3 newTriangle(triangle[0], triangle[1], triangle[2]);
  newTriangle.Shift(translationVector);

  DO_COORDS(coord) {
	//scaling it to match unit cube.
    REAL scalingFactor = 1.0/box.Thickness(coord);
    for(INT i = 0; i < 3; i++) {
      newTriangle[i][coord] *= scalingFactor;
    }
  }

  newTriangle.Shift(cVECTOR3(-0.5, -0.5, -0.5));

  REAL vertices[3][3];
  REAL normal[3];

  for(INT i = 0 ; i < 3 ; i++) {
    normal[i] = triNormal[i];
    DO_COORDS(coord1) {
      vertices[i][coord1] = newTriangle[i][coord1];
    }
  }

  if (fast_polygon_intersects_cube(3, vertices, normal, 0, 0))
    return SUCCESS;

  return FAILURE;
}

}
