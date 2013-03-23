
#ifndef _GEOM_RAY3_H_
#define _GEOM_RAY3_H_

namespace geom {

class cRAY3
{

public:
  //Default constructor.
  cRAY3() : m_endPoints(cPOINT3(ORIGIN),
			   cPOINT3(ORIGIN))
  {}

  cRAY3(const cPOINT3 &p,
	   const cPOINT3 &q) : m_endPoints(p, q)
  {}

  cRAY3(const cPOINT3 &point,
	   const cVECTOR3 &v) : m_endPoints(point, point + v)
  {}

  cRAY3(const cRAY3 &ray) : m_endPoints(ray.m_endPoints)
  {}

  cLINE3 SupportingLine() const { return cLINE3(m_endPoints[0], m_endPoints[1]); }
  cVECTOR3 ToVector() const { return m_endPoints[1] - m_endPoints[0]; }

  cPOINT3 Source() const { return m_endPoints[0]; }

  VOID Print(FILE *stream = stdout) const
  {
    fprintf(stream, "Ray origin %lf %lf %lf \n",
	    m_endPoints[0][0], m_endPoints[0][1], m_endPoints[0][2]);
    fprintf(stream, "Ray second point %lf %lf %lf \n",
	    m_endPoints[1][0], m_endPoints[1][1], m_endPoints[1][2]);
  }

protected:
  tPAIR<cPOINT3> m_endPoints;

};

}
#endif //_GEOM_RAY3_H_
