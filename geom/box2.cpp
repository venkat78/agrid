#include "common_internal2d.hpp"
namespace geom {

cBOX2::cBOX2(const cPOINT2 &p,
		const cPOINT2 &q)
{
  REAL minx, maxx, miny, maxy;

  if (p.X() < q.X()) { minx = p.X(); maxx = q.X(); }
  else               { minx = q.X(); maxx = p.X(); }
  if (p.Y() < q.Y()) { miny = p.Y(); maxy = q.Y(); }
  else               { miny = q.Y(); maxy = p.Y(); }

  m_cornerPoints[0][0] = minx; m_cornerPoints[0][1] = miny;
  m_cornerPoints[1][0] = maxx; m_cornerPoints[1][1] = maxy;
}

cPOINT2 cBOX2::Vertex(INT i) const
{
  switch (i%4) {
  case 0: return Min();
  case 1: return cPOINT2(XMax(), YMin());
  case 2: return Max();
  default: return cPOINT2(XMin(), YMax());
  }
}

eBOUNDED_SIDE
cBOX2::BoundedSide(const cPOINT2 &p) const
{
  const cPOINT2 &maxPoint = Max(), &minPoint = Min();

  if((p[0] > minPoint[0] && p[0] < maxPoint[0]) &&
     (p[1] > minPoint[1] && p[1] < maxPoint[1]))
    return ON_BOUNDED_SIDE;
  else if((p[0] == minPoint[0] || p[0] == maxPoint[0]) ||
	  (p[1] == minPoint[1] || p[1] == maxPoint[1]))
    return ON_BOUNDARY;
  else
    return ON_UNBOUNDED_SIDE;
}


VOID cBOX2::operator+=(const cPOINT2 &point)
{
  cPOINT2 &boxMin = Min();
  cPOINT2 &boxMax = Max();

  for(int i=0; i<2; i++) {
    //Updating min point of the box.
    if(boxMin[i] > point[i])
      boxMin[i] = point[i];

    if(boxMax[i] < point[i])
      boxMax[i] = point[i];
  }
}
}


