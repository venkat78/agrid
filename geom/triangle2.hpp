#ifndef _GEOM_TRIANGLE2_H_
#define _GEOM_TRIANGLE2_H_

namespace geom {

class cTRIANGLE2
{
public:
  cTRIANGLE2()
  {}

  cTRIANGLE2(const cPOINT2 &p, const cPOINT2 &q, const cPOINT2 &r)
    : m_vertices(p, q, r)
  {}

  cBOX2 ComputeBbox() const {
    cBOX2 box(m_vertices[0], m_vertices[1]);
    box += m_vertices[2];
    return box;
  }

  const cPOINT2& Vertex(INT i) const { return m_vertices[i]; }
  cPOINT2& Vertex(INT i) { return m_vertices[i]; }

  const cPOINT2& operator[] (INT i) const { return m_vertices[i]; }
  cPOINT2& operator[] (INT i)  { return m_vertices[i]; }

  VOID Print(FILE *stream = stdout) const
  {
    for(INT i = 0; i < 3; i++) {
      fprintf(stream, "Triangle vtx[%d] %lf %lf \n",
	      i, m_vertices[i][0], m_vertices[i][1]);
    }
  }

  cSEGMENT2 Segment(INT i) const
  {
    ASSERT(0<=i && i<=2);
    return cSEGMENT2(Vertex(i), Vertex((i+1)%3));
  }

  cSEGMENT2 Edge(INT i) const { return Segment(i); }

  BOOL IsPointInside(const cPOINT2 &point) const;

  //Need to check.
  BOOL IsPointOnEdge(const cPOINT2 &point) const {
    for(INT i=0; i< 3; i++)
      if(Edge(i).IsOnSegment(point))
	return true;

    return false;
  }

private:
  INT LongestSide() const ;
  REAL SignedArea() const { return Area(m_vertices[0], m_vertices[1], m_vertices[2]); }

protected:
  tTRIPLE<cPOINT2> m_vertices;
};

}

#endif //_GEOM_TRIANGLE2_H_
