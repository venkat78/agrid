#include "common_internal3d.hpp"

namespace geom {

BOOL
cTRIANGLE3::IsPointInside(const cPOINT3 &point, BOOL checkForPointOnPlane) const

{
  cVECTOR3 triangleNormal = Normal();

  if(checkForPointOnPlane &&
     !cPLANE3(m_vertices[0], triangleNormal).HasOn(point))
    return false;

  eCOORD dropCoord = triangleNormal.AbsMaxCoord();

  cTRIANGLE2 projectedTriangle(Project(dropCoord));

  return projectedTriangle.IsPointInside(point.DropCoord(dropCoord));
}

REAL
cTRIANGLE3::PointDistanceSq(const cPOINT3 &lookupPoint) const
{
  BOOL pointInside;
  cPOINT3 closestPoint;
  return this->PointDistanceSq(lookupPoint, &pointInside,
                               closestPoint,
                               std::numeric_limits<REAL>::max());
}

//triangle is degenerate if one of its heights is less that tol
BOOL cTRIANGLE3::IsDegenerate() const
{
  INT edge_max = 2;
  REAL l_max = SquaredDistance(m_vertices[2], m_vertices[0]);
  for (int i=0; i < 2; i++){
	REAL l = SquaredDistance(m_vertices[i], m_vertices[i+1]);
	if (l > l_max){
		l_max = l;
		edge_max = i;
	}
  }
  INT edge_max_plus = edge_max + 1;
  if (edge_max_plus == 3)
	  edge_max_plus = 0;
  INT vertex_mid = 3 - edge_max - edge_max_plus; //third vertex
  cSEGMENT3 longestEdge(m_vertices[edge_max], m_vertices[edge_max_plus]);
  return longestEdge.HasOn(m_vertices[vertex_mid]);
}

REAL
cTRIANGLE3::PointDistanceSq(const cPOINT3 &lookupPoint,
                                       BOOL *isClosestPointInside,
                                       cPOINT3 &closestPointRet,
                                       const REAL currMaxDistanceSq) const
{
  cPOINT3 projectedPoint = SupportingPlane().Projection(lookupPoint);
  REAL squaredDist = SquaredDistance(lookupPoint, projectedPoint);

  if(squaredDist > currMaxDistanceSq)
    return std::numeric_limits<REAL>::max();

  *isClosestPointInside = IsPointInside(projectedPoint, false);
  if(*isClosestPointInside) {
    closestPointRet = projectedPoint;
    return squaredDist;
  }

  REAL closestVtxDist = std::numeric_limits<REAL>::max();
  INT closestVtx;
  for (INT i=0; i<3; i++) {
    REAL currDist = SquaredDistance(lookupPoint, Vertex(i));
    if (currDist < closestVtxDist) {
      closestVtxDist = currDist;
      closestVtx = i;
    }
  }

  eANGLE firstAngle = AngleType(Vertex((closestVtx+1)%3),
					     Vertex(closestVtx),
					     projectedPoint);
  eANGLE secondAngle = AngleType(Vertex((closestVtx+2)%3),
					      Vertex(closestVtx),
					      projectedPoint);

  if(firstAngle == OBTUSE_ANGLE && secondAngle == OBTUSE_ANGLE) {
    // Closest point is the closest vertex
    closestPointRet = Vertex(closestVtx);
    return closestVtxDist;
  }
  else if (firstAngle != OBTUSE_ANGLE && secondAngle == OBTUSE_ANGLE) {
    //Closest point is on the edge between closest vertex and closest vertex + 1
    closestPointRet =
      cLINE3(Vertex((closestVtx+1)%3), Vertex(closestVtx)).Projection(lookupPoint);
    return SquaredDistance(lookupPoint, closestPointRet);
  }
  else if (firstAngle == OBTUSE_ANGLE && secondAngle != OBTUSE_ANGLE) {
    //Closest point is on the edge between closest vertex and closest vertex + 2
    closestPointRet =
      cLINE3(Vertex((closestVtx+2)%3), Vertex(closestVtx)).Projection(lookupPoint);
    return SquaredDistance(lookupPoint, closestPointRet);
  }
  else {
    //closestPoint is on one of those two edges
    cPOINT3 closestPoint1 =
      cLINE3(Vertex((closestVtx+1)%3), Vertex(closestVtx)).Projection(lookupPoint);
    cPOINT3 closestPoint2 =
      cLINE3(Vertex((closestVtx+2)%3), Vertex(closestVtx)).Projection(lookupPoint);

    REAL squaredDist1 = SquaredDistance(lookupPoint, closestPoint1);
    REAL squaredDist2 = SquaredDistance(lookupPoint, closestPoint2);
    if(squaredDist1 < squaredDist2) {
      closestPointRet = closestPoint1;
      return squaredDist1;
    }
    else {
      closestPointRet = closestPoint2;
      return squaredDist2;
    }
  }
}

}
