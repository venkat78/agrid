
#ifndef _GEOM_POINT2_H_
#define _GEOM_POINT2_H_

#include "geom_defs.hpp"
#include "vector2.hpp"

namespace geom {
class cPOINT2
{

public:
  cPOINT2() : m_coords(REAL(0), REAL(0))
  {}

  cPOINT2(const REAL &x, const REAL &y) : m_coords(x, y)
  {}

  cPOINT2(const sORIGIN &origin) : m_coords(0.0, 0.0)
  {}

  cPOINT2(const cPOINT2& point) : m_coords(point.m_coords)
  {}

  BOOL operator == (const cPOINT2 &point) const
  {
    return (fabs(m_coords[0]-point[0]) <= cLIMITS::Epsilon() &&
	    fabs(m_coords[1]-point[1]) <= cLIMITS::Epsilon());
  }

  BOOL operator != (const cPOINT2 &point) const
  {
    return !(*this == point);
  }

  const REAL & X() const
  {
    return m_coords[0];
  }

  const REAL & Y() const
  {
    return m_coords[1];
  }

  REAL& operator[] (INT i)
  {
    return m_coords[i];
  }

  const REAL& operator[] (INT i) const
  {
    return m_coords[i];
  }

  cPOINT2 operator + (const cVECTOR2 &vec) const
  {
    return cPOINT2(m_coords[0] + vec[0],
                      m_coords[1] + vec[1]);
  }

  cPOINT2 operator + (const cPOINT2 &point) const
  {
    return cPOINT2(m_coords[0] + point[0],
		      m_coords[1] + point[1]);
  }

  cPOINT2 operator*(const REAL scale) const
  {
    return cPOINT2(m_coords[0]*scale,
		      m_coords[1]*scale);

  }

  cVECTOR2 operator - (const cPOINT2 &point) const
  {
    return cVECTOR2( m_coords[0] - point.m_coords[0],
		     m_coords[1] - point.m_coords[1]);
  }

  VOID Print(FILE *stream = stdout) const
  {
    fprintf(stream, "Point %lf %lf \n", m_coords[0],
	    m_coords[1]);
  }

protected:
  tPAIR<REAL> m_coords;
};

}
#endif //_GEOM_POINT2_H_
