#ifndef _GEOM_RAY2_H_
#define _GEOM_RAY2_H_

namespace geom {
class cRAY2
{

public:
  //Default constructor.
  cRAY2() : m_endPoints(cPOINT2(ORIGIN),
			   cPOINT2(ORIGIN))
  {}

  cRAY2(const cPOINT2 &p,
	   const cPOINT2 &q) : m_endPoints(p, q)
  {}

  cRAY2(const cPOINT2 &point,
	   const cVECTOR2 &v) : m_endPoints(point, point + v)
  {}

  cRAY2(const cRAY2 &ray) : m_endPoints(ray.m_endPoints)
  {}

  cLINE2 SupportingLine() const { return cLINE2(m_endPoints[0], m_endPoints[1]); }
  cVECTOR2 ToVector() const { return m_endPoints[1] - m_endPoints[0]; }

  cPOINT2 Source() const { return m_endPoints[0]; }

  VOID Print(FILE *stream = stdout) const
  {
    fprintf(stream, "Ray origin %lf %lf %lf \n",
	    m_endPoints[0][0], m_endPoints[0][1], m_endPoints[0][2]);
    fprintf(stream, "Ray second point %lf %lf %lf \n",
	    m_endPoints[1][0], m_endPoints[1][1], m_endPoints[1][2]);
  }

protected:
  tPAIR<cPOINT2> m_endPoints;

};

}
#endif //_GEOM_RAY2_H_
