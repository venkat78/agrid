#include "common_internal3d.hpp"
namespace geom {

//default constructor - empty box
cBOX3::cBOX3()
{
  REAL minValue = cLIMITS::Max();
  REAL maxValue = -1.0 * cLIMITS::Max();

  m_cornerPoints[0][0] = m_cornerPoints[0][1] = m_cornerPoints[0][2] = minValue;
  m_cornerPoints[1][0] = m_cornerPoints[1][1] = m_cornerPoints[1][2] = maxValue;
}

cBOX3::cBOX3(const cPOINT3 &p)
{
  m_cornerPoints[0] = m_cornerPoints[1] = p;
}

BOOL cBOX3::TwoPointsOnSameFace(const cPOINT3 &p1, const cPOINT3 &p2) const
{
  DO_COORDS(coord){
	if (fabs(p1[coord] - p2[coord]) <= cLIMITS::Tolerance()){
	  if (fabs(p1[coord] - m_cornerPoints[0][coord]) <= cLIMITS::Tolerance() ||
		   fabs(p1[coord] - m_cornerPoints[1][coord]) <= cLIMITS::Tolerance())
		  return true;
	}
  }
  return false;
}

cBOX3::cBOX3(const cPOINT3 &p,
			 const cPOINT3 &q)
{
  REAL minx, maxx, miny, maxy, minz, maxz;

  if (p.X() < q.X()) { minx = p.X(); maxx = q.X(); }
  else               { minx = q.X(); maxx = p.X(); }
  if (p.Y() < q.Y()) { miny = p.Y(); maxy = q.Y(); }
  else               { miny = q.Y(); maxy = p.Y(); }
  if (p.Z() < q.Z()) { minz = p.Z(); maxz = q.Z(); }
  else               { minz = q.Z(); maxz = p.Z(); }

  m_cornerPoints[0][0] = minx; m_cornerPoints[0][1] = miny; m_cornerPoints[0][2] = minz;
  m_cornerPoints[1][0] = maxx; m_cornerPoints[1][1] = maxy; m_cornerPoints[1][2] = maxz;
}

cBOX2 cBOX3::DropCoord(const eCOORD coord) const
{
  cPOINT2 p = Min().DropCoord(coord);
  cPOINT2 q = Max().DropCoord(coord);
  return cBOX2(p,q);
}

cPLANE3
cBOX3::BoundingPlane(INT i) const
{
  ASSERT(0 <= i && i <= 5);
  if(i == 0) //x-min plane.
    return cPLANE3(-1.0, 0.0, 0.0, XMin());
  else if(i == 1) //y-min plane.
    return cPLANE3(0.0, -1.0, 0.0, YMin());
  else if(i == 2)//z-min plane.
    return cPLANE3(0.0, 0.0, -1.0, ZMin());
  else if(i == 3)//x-max plane.
    return cPLANE3(1.0, 0.0, 0.0, (-1.0*XMax()));
  else if(i == 4)//y-max plane.
    return cPLANE3(0.0, 1.0, 0.0, (-1.0*YMax()));
  else//z-max plane.
    return cPLANE3(0.0, 0.0, 1.0, (-1.0*ZMax()));
}

//Bujji, do we need the next two methods? And if so, what are the rules
//for indexing box diagonals and edges?
cSEGMENT3
cBOX3::Edge(INT i) const
{
  ASSERT(i >= 0 && i < 12);

  if(i == 0)
    return cSEGMENT3(CornerVertex(0), CornerVertex(1));
  else if(i == 1)
    return cSEGMENT3(CornerVertex(0), CornerVertex(2));
  else if(i == 2)
    return cSEGMENT3(CornerVertex(0), CornerVertex(4));
  else if(i == 3)
    return cSEGMENT3(CornerVertex(1), CornerVertex(3));
  else if(i == 4)
    return cSEGMENT3(CornerVertex(1), CornerVertex(5));
  else if(i == 5)
    return cSEGMENT3(CornerVertex(2), CornerVertex(3));
  else if(i == 6)
    return cSEGMENT3(CornerVertex(2), CornerVertex(6));
  else if(i == 7)
    return cSEGMENT3(CornerVertex(4), CornerVertex(5));
  else if(i == 8)
    return cSEGMENT3(CornerVertex(4), CornerVertex(6));
  else if(i == 9)
    return cSEGMENT3(CornerVertex(7), CornerVertex(6));
  else if(i == 10)
    return cSEGMENT3(CornerVertex(7), CornerVertex(5));
  else
    return cSEGMENT3(CornerVertex(7), CornerVertex(3));
}


//this method returns an edge defined by the cell closure rules
//
cSEGMENT3
cBOX3::EdgeOrdered(INT i) const
{
  ASSERT(i >= 0 && i < 12);

  if(i == 0)
    return cSEGMENT3(CornerVertex(0), CornerVertex(4));
  else if(i == 1)
    return cSEGMENT3(CornerVertex(1), CornerVertex(5));
  else if(i == 2)
    return cSEGMENT3(CornerVertex(2), CornerVertex(6));
  else if(i == 3)
    return cSEGMENT3(CornerVertex(3), CornerVertex(7));
  else if(i == 4)
    return cSEGMENT3(CornerVertex(0), CornerVertex(2));
  else if(i == 5)
    return cSEGMENT3(CornerVertex(4), CornerVertex(6));
  else if(i == 6)
    return cSEGMENT3(CornerVertex(1), CornerVertex(3));
  else if(i == 7)
    return cSEGMENT3(CornerVertex(5), CornerVertex(7));
  else if(i == 8)
    return cSEGMENT3(CornerVertex(0), CornerVertex(1));
  else if(i == 9)
    return cSEGMENT3(CornerVertex(2), CornerVertex(3));
  else if(i == 10)
    return cSEGMENT3(CornerVertex(4), CornerVertex(5));
  else
    return cSEGMENT3(CornerVertex(6), CornerVertex(7));
}

/*
 * This method doesn't work.
 */
cSEGMENT3
cBOX3::Diagonal(INT i) const
{
  ASSERT(i >= 0 && i < 4);

  if(i == 0)
    return cSEGMENT3(Vertex(0), Vertex(7));
  else if(i == 1)
    return cSEGMENT3(Vertex(1), Vertex(4));
  else if(i == 2)
    return cSEGMENT3(Vertex(2), Vertex(5));
  else
    return cSEGMENT3(Vertex(3), Vertex(6));
}

REAL
cBOX3::PointDistSquared(const cPOINT3 &point) const
{
  REAL coord_dist;
  REAL totalDistSq = 0.0;
  const cPOINT3& minPoint = m_cornerPoints[0];
  const cPOINT3& maxPoint = m_cornerPoints[1];

  for (INT i=0; i<3; i++) {
    if (point[i] < minPoint[i]) {
      coord_dist = minPoint[i] - point[i];
        totalDistSq += coord_dist*coord_dist;
    }
    else if (point[i] > maxPoint[i]) {
      coord_dist = point[i] - maxPoint[i];
      totalDistSq += coord_dist*coord_dist;
    }
  }

  return totalDistSq;
}

BOOL cBOX3::ContainsPoint(const cPOINT3 &point) const
{
  const cPOINT3& minPoint = m_cornerPoints[0];
  const cPOINT3& maxPoint = m_cornerPoints[1];

  for (INT i=0; i<3; i++) {
    if (point[i] < minPoint[i] ||
	point[i] > maxPoint[i])
      return false;
  }

  return true;
}

BOOL cBOX3::ContainsBox(const cBOX3& box) const
{
  return ContainsPoint(box.Min()) && ContainsPoint(box.Max());
}

//grow the box size in each dimension by the given factor, Say, if factor = 2,
//the box size doubkesm nd if it is 1, the box is not changed
cBOX3 &cBOX3::Grow(REAL factor) {
	DO_COORDS(i){
		REAL u = Min()[i];
		REAL v = Max()[i];
		REAL h = 0.5*(factor-1.0)*(v-u);
		Min()[i] -= h;
		Max()[i] += h;
	}
	return *this;
}

BOOL cBOX3::Intersects(const cRAY3& ray,
				       REAL tolerance) const

{

  REAL t_min = 0.0;
  REAL t_max =  cLIMITS::Max();

  cPOINT3 rayOrigin = ray.Source();
  cVECTOR3 rayDirection = ray.ToVector();

  rayDirection/=rayDirection.Length();

  const cPOINT3& minPoint = m_cornerPoints[0];
  const cPOINT3& maxPoint = m_cornerPoints[1];

  DO_COORDS(i) {
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
      return false;
  }

  return true;
}

//YG changed the indexation:
//the standard rule is : x changes the most, z changes the least
//Bujji, please make sure that nothing is broken.
cPOINT3 cBOX3::Vertex(INT i) const
{
  switch (i%8) {
  case 0: return Min();
  case 1: return cPOINT3(Max().X(), Min().Y(), Min().Z());
  case 2: return cPOINT3(Min().X(), Max().Y(), Min().Z());
  case 3: return cPOINT3(Max().X(), Max().Y(), Min().Z());
  case 4: return cPOINT3(Min().X(), Min().Y(), Max().Z());
  case 5: return cPOINT3(Max().X(), Min().Y(), Max().Z());
  case 6: return cPOINT3(Min().X(), Max().Y(), Max().Z());
  case 7: return  Max();
  default: return Max();
  }
}

//YG wrote it to keep indexation in sync with agrid_utils
cPOINT3 cBOX3::CornerVertex(INT index) const
{
  switch (index%8) {
  case 0: return Min();
  case 1: return cPOINT3(Min().X(), Min().Y(), Max().Z());
  case 2: return cPOINT3(Min().X(), Max().Y(), Min().Z());
  case 3: return cPOINT3(Min().X(), Max().Y(), Max().Z());
  case 4: return cPOINT3(Max().X(), Min().Y(), Min().Z());
  case 5: return cPOINT3(Max().X(), Min().Y(), Max().Z());
  case 6: return cPOINT3(Max().X(), Max().Y(), Min().Z());
  case 7: return  Max();
  default: return Max();
  }
}

//opposite to the previous method. If a point is a box
//vertex (within tol), return the index of this vertex
INT cBOX3::VertexPointIndex(const cPOINT3& point) const
{
  INT index = 0;
  DO_COORDS(i) {
	if (fabs(point[i] - Max()[i]) <= cLIMITS::Tolerance())
	  index += (1>>i);
	else if (fabs(point[i] - Min()[i]) <= cLIMITS::Tolerance());
	else
	  return -1;
  }
  return index;
}

//returns the given box face coordinate
REAL cBOX3::FaceCoord(INT face_index)
{
  ASSERT(face_index >= 0 && face_index < 6);
  INT face_coord = face_index/2;
  INT face_position = face_index- 2*face_coord;
  return m_cornerPoints[face_position][face_coord];
}

REAL cBOX3::FaceCoord(eCOORD face_coord, eBOX_POSITION face_position)
{
  return m_cornerPoints[(INT)face_position][(INT)face_coord];
}

VOID cBOX3::operator+=(const cPOINT3 &point)
{
  cPOINT3 &boxMin = Min();
  cPOINT3 &boxMax = Max();

  for(int i=0; i<3; i++) {
  //Updating min point of the box.
    if(boxMin[i] > point[i])
      boxMin[i] = point[i];

    if(boxMax[i] < point[i])
      boxMax[i] = point[i];
  }
}

eBOUNDED_SIDE
cBOX3::BoundedSide(const cPOINT3 &p) const
{
  const cPOINT3 &maxPoint = Max(), &minPoint = Min();

  for(INT i = 0; i < 3; i++) {
    if(minPoint[i] - p[i] > cLIMITS::Tolerance() ||
       p[i] - maxPoint[i] > cLIMITS::Tolerance())
      //    if (p[i] < minPoint[i] || p[i] > maxPoint[i])
      return ON_UNBOUNDED_SIDE; //outside box
  }

  if(p[0] - minPoint[0] > cLIMITS::Tolerance() && maxPoint[0] - p[0] > cLIMITS::Tolerance() &&
     p[1] - minPoint[1] > cLIMITS::Tolerance() && maxPoint[1] - p[1] > cLIMITS::Tolerance() &&
     p[2] - minPoint[2] > cLIMITS::Tolerance() && maxPoint[2] - p[2] > cLIMITS::Tolerance())
    return ON_BOUNDED_SIDE;

//   if((p[0] > minPoint[0] && p[0] < maxPoint[0]) &&
//      (p[1] > minPoint[1] && p[1] < maxPoint[1]) &&
//      (p[2] > minPoint[2] && p[2] < maxPoint[2]))
//     return ON_BOUNDED_SIDE;  //inside box

  return ON_BOUNDARY; //on box boundary (on face or edge)
}

INT cBOX3::IsOnEdge(const cPOINT3 &p) const
{
  if(!HasOnBoundary(p))
    return -1;

  for(INT i = 0; i < 12 ; i++) {
    if(Edge(i).HasOn(p))
    return i;
  }

  return -1;
}

cVECTOR3 cBOX3::PositionVector(const cPOINT3 &p) const
{
  cVECTOR3 pos(0.0, 0.0, 0.0);
  const cPOINT3 &maxPoint = Max(), &minPoint = Min();

  if(fabs(p[0] - minPoint[0]) < cLIMITS::Tolerance())
    pos[0]=-1.0;
  else if(fabs(p[0] - maxPoint[0]) < cLIMITS::Tolerance())
    pos[0] = 1.0;

  if(fabs(p[1] - minPoint[1]) < cLIMITS::Tolerance())
    pos[1]=-1.0;
  else if(fabs(p[1] - maxPoint[1]) < cLIMITS::Tolerance())
    pos[1] = 1.0;

  if(fabs(p[2] - minPoint[2]) < cLIMITS::Tolerance())
    pos[2]=-1.0;
  else if(fabs(p[2] - maxPoint[2]) < cLIMITS::Tolerance())
    pos[2] = 1.0;

  return pos;
}

cBOX3 cBOX3::GrowBy(INT percentage) const
{
  REAL relativeTol[3];
  DO_COORDS(coord) {
    relativeTol[coord] = Thickness(coord)*percentage*0.005;

    if(fabs(relativeTol[coord]) < cLIMITS::Tolerance())
      relativeTol[coord] = cLIMITS::Tolerance() * 100;
  }

  return cBOX3(m_cornerPoints[0][0] - relativeTol[0],
               m_cornerPoints[0][1] - relativeTol[1],
               m_cornerPoints[0][2] - relativeTol[2],
               m_cornerPoints[1][0] + relativeTol[0],
               m_cornerPoints[1][1] + relativeTol[1],
               m_cornerPoints[1][2] + relativeTol[2]);
}

//if all six of the flags are false, false is returned.
//this methid is used in a loop for traversing points, like facet vertices
BOOL cBOX3::PointIsOnBoundary(cPOINT3 &point, BOOL pointOnBoundary[3][2]) const
{
  BOOL rv = false;
  DO_COORDS(coord){
	if (pointOnBoundary[coord][0]){
	  if (fabs(point[coord] - MinCoord(coord)) > cLIMITS::Tolerance())
		pointOnBoundary[coord][0] = false;
	}
	if (pointOnBoundary[coord][0])
	  rv = true;

	if (pointOnBoundary[coord][1]){
	  if (fabs(point[coord] - MaxCoord(coord)) > cLIMITS::Tolerance())
		pointOnBoundary[coord][1] = false;
	}
	if (pointOnBoundary[coord][1])
	  rv = true;
  }
  return rv;
}

}//namespace

