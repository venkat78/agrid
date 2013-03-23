
#ifndef _GEOM_TRIANGLE3_H_
#define _GEOM_TRIANGLE3_H_

#include <limits>

namespace geom {

class cTRIANGLE3
{

public:
  cTRIANGLE3()
  {}

  cTRIANGLE3(const cPOINT3 &p, const cPOINT3 &q, const cPOINT3 &r)
    : m_vertices(p, q, r)
  {}

  VOID Shift(const cVECTOR3 &vec) {
    m_vertices[0] = m_vertices[0] + vec;
    m_vertices[1] = m_vertices[1] + vec;
    m_vertices[2] = m_vertices[2] + vec;
  }

  cBOX3 ComputeBbox() const {
    cBOX3 box(m_vertices[0], m_vertices[1]);
    box += m_vertices[2];
    return box;
  }

  const cPOINT3& Vertex(INT i) const { return m_vertices[i]; }
  cPOINT3& Vertex(INT i) { return m_vertices[i]; }

  const cPOINT3& operator[] (INT i) const { return m_vertices[i]; }
  cPOINT3& operator[] (INT i)  { return m_vertices[i]; }

  cPLANE3 SupportingPlane() const { return cPLANE3(m_vertices[0],
						   Cross(Edge(0), Edge(1))); }

  cVECTOR3 Normal() const {
    cVECTOR3 normalVector = Cross(Edge(0), Edge(1));

    REAL len = normalVector.Length();
    if(fabs(len) <= cLIMITS::Tolerance())
      return cVECTOR3();

    return normalVector / len;
  }

  BOOL IsPointInside(const cPOINT3 &point,
			BOOL checkForPointOnPlane = true) const ;

  REAL PointDistanceSq(const cPOINT3 &lookupPoint,
                     BOOL *isClosestPointInside,
                     cPOINT3 &closestPointRet,
                     const REAL currMaxDistanceSq) const ;

  REAL PointDistanceSq(const cPOINT3 &lookupPoint) const ;

  BOOL IsDegenerate() const;

  VOID Print(FILE *stream = stdout) const
  {
    for(INT i = 0; i < 3; i++){
      fprintf(stream, "Triangle vtx[%d] %.12lf %.12lf %.12lf \n",
	      i, m_vertices[i][0], m_vertices[i][1], m_vertices[i][2]);
    }
  }

  cSEGMENT3 Segment(INT i) const
  {
    ASSERT(0<=i && i<=2);
    return cSEGMENT3(Vertex(i), Vertex((i+1)%3));
  }

  cVECTOR3 Edge(INT i) const {
    return m_vertices[(i+1)%3] - m_vertices[i];
  }

  //Projects triangles onto coordinate planes.
  cTRIANGLE2 Project(eCOORD dropCoord) const {
    return cTRIANGLE2(m_vertices[0].DropCoord(dropCoord),
		      m_vertices[1].DropCoord(dropCoord),
		      m_vertices[2].DropCoord(dropCoord));
  }

  BOOL IsVertex(const cPOINT3 &point) const
  {
    for(INT i = 0; i<3; i++)
      if(m_vertices[i] == point)
	return true;

    return false;
  }

protected:
  tTRIPLE<cPOINT3> m_vertices;
};

}
#endif //_GEOM_TRIANGLE3_H_
