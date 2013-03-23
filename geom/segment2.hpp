
#ifndef _GEOM_SEGMENT2_H_
#define _GEOM_SEGMENT2_H_

namespace geom {

class cSEGMENT2
{

public:
  cSEGMENT2()
  {}

  cSEGMENT2(const cSEGMENT2 &segment) : m_endPoints(segment.m_endPoints)
  {}

  cSEGMENT2(const cPOINT2 &point1,
	       const cPOINT2 &point2) : m_endPoints(point1, point2)
  {}

  cBOX2 ComputeBbox() const {
    return cBOX2(m_endPoints[0], m_endPoints[1]);
  }

  cPOINT2 Vertex(INT i) const
  {
    return m_endPoints[i];
  }

  VOID Print(FILE *stream = stdout) const
  {
    fprintf(stream, "Segment ep1 %lf %lf \n",
	    m_endPoints[0][0], m_endPoints[0][1]);
    fprintf(stream, "Segment ep2 %lf %lf \n",
	    m_endPoints[1][0], m_endPoints[1][1]);
  }

  const cPOINT2& Source() const { return m_endPoints[0]; }
  const cPOINT2& Target() const { return m_endPoints[1]; }

  cLINE2 SupportingLine() const { return cLINE2(m_endPoints[0], m_endPoints[1]); }

  BOOL IsDegenerate() const {
    return m_endPoints[0] == m_endPoints[1] ? true : false;
  }

  //Point is assumed to lie on SupportingLine.
  REAL Param(const cPOINT2 &point) const ;

  //Point is assumed to lie on SupportingLine.
  BOOL IsOnSegment(const cPOINT2 &point) const ;

  VOID XRange(REAL range[2]) const ;

  VOID YRange(REAL range[2]) const ;

protected:
  tPAIR<cPOINT2> m_endPoints;
};

}
#endif //_GEOM_SEGMENT2_H_

