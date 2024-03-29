#ifndef _GEOM_LINE3_H_
#define _GEOM_LINE3_H_

namespace geom {
class cLINE3
{

public:
  //Default constructor.
  cLINE3()
  {}

  cLINE3(const cPOINT3 &point1,
	     const cPOINT3 &point2) : m_origin(point1),
				     m_direction(point2 - point1)
  {}

  cPOINT3 Projection(const cPOINT3 &point) const {
	  REAL lambda = ((point - m_origin) * m_direction)/(m_direction * m_direction);
	  return m_origin + (m_direction * lambda);
  }

  BOOL HasOn(const cPOINT3 &point) const {
	  cPOINT3 projectionPoint = Projection(point);
	  return projectionPoint == point;
  }

  VOID Print(FILE *stream = stdout) {
    fprintf(stream, "Line Origin : %lf %lf %lf \n",
	    m_origin[0], m_origin[1], m_origin[2]);
    fprintf(stream, "Line Dir : %lf %lf %lf \n",
	    m_direction[0], m_direction[1], m_direction[2]);
  }

  const cPOINT3& Point() const { return m_origin; }
  const cVECTOR3& Direction() const { return m_direction; }
  cPOINT3 EndPoint() const { return m_origin + m_direction; }

  //!Calcuate the parameter of a point on line
  REAL Param(const cPOINT3 &pt) const {
    cVECTOR3 vec = pt - m_origin;
    REAL t = vec.Length()/m_direction.Length();
    REAL dot = vec*m_direction;
    t = dot > 0? t : -t;
    return t;
  }

protected:
  cPOINT3 m_origin;
  cVECTOR3 m_direction;
};

}

#endif //_GEOM_LINE3_H_
