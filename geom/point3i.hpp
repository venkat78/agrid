
#ifndef _GEOM_POINT3i_H_
#define _GEOM_POINT3i_H_

namespace geom {

class cPOINT3i
{
public:
  cPOINT3i() : m_coords(0, 0, 0)
  {}

  cPOINT3i(const sORIGIN &origin) : m_coords(0, 0, 0)
  {}

  cPOINT3i(const INT x, const INT y, const INT z) : m_coords(x, y, z)
  {}

  cPOINT3i(const INT i[3]) : m_coords(i[0], i[1], i[2])
  {}

  cPOINT3i(const cPOINT3i& point) : m_coords(point.m_coords)
  {}

  VOID Print(FILE *stream = stdout) const
  {
    fprintf(stream, "%d \t %d \t %d\n", m_coords[0], m_coords[1], m_coords[2]);
  }

  BOOL operator == (const cPOINT3i &point)
  {
    return (m_coords[0]==point[0] &&
	    m_coords[1]==point[1] &&
	    m_coords[2]==point[2]);
  }

  cPOINT3i& operator += (const cPOINT3i& point) {
    m_coords[0] += point[0];
    m_coords[1] += point[1];
    m_coords[2] += point[2];
    return *this ;
  }

  cPOINT3i operator + (const cPOINT3i &point) const
  {
    return cPOINT3i(m_coords[0] + point[0],
		      m_coords[1] + point[1],
		      m_coords[2] + point[2]);

  }

  cPOINT3i& operator *= (const INT scale) {
    m_coords[0] *= scale;
    m_coords[1] *= scale;
    m_coords[2] *= scale;
    return *this ;
  }

  cPOINT3i operator*(const INT scale) const
  {
    return cPOINT3i(m_coords[0]*scale,
		      m_coords[1]*scale,
		      m_coords[2]*scale);

  }

  const INT & X() const
  {
    return m_coords[0];
  }

  const INT & Y() const
  {
    return m_coords[1];
  }

  const INT & Z() const
  {
    return m_coords[2];
  }

  const INT& operator[] (int i) const
  {
    return m_coords[i];
  }

  INT& operator[] (int i)
  {
    return m_coords[i];
  }

protected:
  tTRIPLE<INT> m_coords;
};

}
#endif //_GEOM_POINT3i_H_
