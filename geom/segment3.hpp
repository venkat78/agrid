
#ifndef _GEOM_SEGMENT3_H_
#define _GEOM_SEGMENT3_H_

namespace geom {
class cBOX3;

class cSEGMENT3
{


public:
  cSEGMENT3()
  {}

  cSEGMENT3(const cSEGMENT3 &segment) : m_endPoints(segment.m_endPoints)
  {}

  cSEGMENT3(const cPOINT3 &point1,
	    const cPOINT3 &point2) : m_endPoints(point1, point2)
  {}

  cBOX3 ComputeBbox() const ;//{
//    return cBOX3(m_endPoints[0], m_endPoints[1]);
//  }

  VOID Invert() {
    cPOINT3 p = m_endPoints[0];
    m_endPoints[0] = m_endPoints[1];
    m_endPoints[1] = p;
  }

  BOOL HasOn(const cPOINT3 &point) const ;

  /*
   * Returns index of the endpoint if point is an end.
   * Returns -1 if point is not an endpoint.
   */
  INT IsEndPoint(const cPOINT3 &point) const {
    if(point == Source())
      return 0;

    if(point == Target())
      return 1;

    return -1;
  }

  cPOINT3 MidPoint() const {
    return (m_endPoints[0] + m_endPoints[1])*0.5;
  }

  cPOINT3 Vertex(INT i) const
  {
    return m_endPoints[i];
  }

  VOID Print(FILE *stream = stdout) const
  {
    fprintf(stream, "Segment ep1 %.12lf %.12lf %.12lf \n",
	    m_endPoints[0][0], m_endPoints[0][1], m_endPoints[0][2]);
    fprintf(stream, "Segment ep2 %.12lf %.12lf %.12lf \n",
	    m_endPoints[1][0], m_endPoints[1][1], m_endPoints[1][2]);
  }

  REAL Length() {
    return Vector().Length();
  }

  VOID Print1(FILE *stream = stdout) const
  {
    fprintf(stream, "%lf %lf %lf; ",
  	    m_endPoints[0][0], m_endPoints[0][1], m_endPoints[0][2]);
    fprintf(stream, "%lf %lf %lf;\n",
  	    m_endPoints[1][0], m_endPoints[1][1], m_endPoints[1][2]);
  }

  const cPOINT3& Source() const { return m_endPoints[0]; }
  const cPOINT3& Target() const { return m_endPoints[1]; }
  cVECTOR3 Vector() const { return m_endPoints[1] - m_endPoints[0];}
  cLINE3 SupportingLine() const { return cLINE3(m_endPoints[0], m_endPoints[1]); }
  cPOINT3 Evaluate(REAL param) const { return (Source()*(1.0-param)) + (Target()*param); }
  REAL PointProjectionParam(const cPOINT3 &point);
  cPOINT3 PointProjectionPoint(const cPOINT3 &point);
  BOOL SegmentDistanceParams(const cSEGMENT3 &seg, REAL & param1, REAL & param2) const;

  /*
   *  Assumes point is on the segment.
   */
  REAL Param(const cPOINT3 &point) const {
    return cLINE3(Vertex(0), Vertex(1)).Param(point);
  }

protected:
  tPAIR<cPOINT3> m_endPoints;
};

}
#endif //_GEOM_SEGMENT3_H_
