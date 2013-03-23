#ifndef _GEOM_POINT3_H_
#define _GEOM_POINT3_H_


namespace geom {
class cPOINT3
{
public:

  cPOINT3() : m_coords(0.0, 0.0, 0.0)
  {}

  cPOINT3(const sORIGIN &origin) : m_coords(0.0, 0.0, 0.0)
  {}

  cPOINT3(const REAL x, const REAL y, const REAL z) : m_coords(x, y, z)
  {}

  cPOINT3(const cPOINT3& point) : m_coords(point.m_coords)
  {}

  BOOL operator == (const cPOINT3 &point) const
  {
    return (fabs(m_coords[0]-point[0]) <= cLIMITS::Tolerance() &&
	    fabs(m_coords[1]-point[1]) <= cLIMITS::Tolerance() &&
	    fabs(m_coords[2]-point[2]) <= cLIMITS::Tolerance());
  }

  BOOL operator != (const cPOINT3 &point) const {
    return !(*this == point);
  }

  VOID operator += (const cPOINT3& point) {
    m_coords[0] += point[0];
    m_coords[1] += point[1];
    m_coords[2] += point[2];
  }

  cPOINT3& operator += (const cVECTOR3& vec) {
    m_coords[0] += vec[0];
    m_coords[1] += vec[1];
    m_coords[2] += vec[2];
    return *this ;
  }

  cPOINT3& operator -= (const cVECTOR3& vec) {
    m_coords[0] -= vec[0];
    m_coords[1] -= vec[1];
    m_coords[2] -= vec[2];
    return *this ;
  }

  cPOINT3 operator + (const cPOINT3 &point) const
  {
    return cPOINT3(m_coords[0] + point[0],
                   m_coords[1] + point[1],
                   m_coords[2] + point[2]);

  }

  cPOINT3 operator + (const cVECTOR3 &vec) const
  {
    return cPOINT3(m_coords[0] + vec[0],
                      m_coords[1] + vec[1],
                      m_coords[2] + vec[2]);

  }

  cPOINT3 operator/(const REAL scale) const
  {
    ASSERT(scale != 0.0);
    return cPOINT3(m_coords[0]/scale,
		      m_coords[1]/scale,
		      m_coords[2]/scale);
  }

  cPOINT3 operator*(const REAL scale) const
  {
    return cPOINT3(m_coords[0]*scale,
                   m_coords[1]*scale,
                   m_coords[2]*scale);

  }

  cPOINT2 DropCoord(const eCOORD coord) const ;

  const REAL & X() const
  {
    return m_coords[0];
  }

  const REAL & Y() const
  {
    return m_coords[1];
  }

  const REAL & Z() const
  {
    return m_coords[2];
  }

  REAL& operator[] (INT i)
  {
    return m_coords[i];
  }

 const REAL & operator [] (INT i) const
  {
    return m_coords[i];
  }

  cVECTOR3 operator - (const cPOINT3 &point) const
  {
    return cVECTOR3( m_coords[0] - point.m_coords[0],
		     m_coords[1] - point.m_coords[1],
		     m_coords[2] - point.m_coords[2] );
  }

  VOID Print(FILE *stream = stdout) const
  {
    fprintf(stream, "Point %.12lf %.12lf %.12lf\n", m_coords[0], m_coords[1],
	    m_coords[2]);

  }

  cPOINT3& operator *= (const REAL scale) {
    m_coords[0] *= scale;
    m_coords[1] *= scale;
    m_coords[2] *= scale;
    return *this ;
  }

  cPOINT3 operator - (const cVECTOR3 &vec) const
  {
    return cPOINT3(m_coords[0] - vec[0],
		   m_coords[1] - vec[1],
		   m_coords[2] - vec[2]);

  }

protected:
  tTRIPLE<REAL> m_coords;
};

  /*
   * A very dumb way to compare points.
   */
  inline BOOL operator < (const cPOINT3 &point1, const cPOINT3 &point2)
  {
    if(fabs(point1[0] - point2[0]) < cLIMITS::Tolerance()) {

      if(fabs(point1[1] - point2[1]) < cLIMITS::Tolerance()) {

	if(fabs(point1[2] - point2[2]) < cLIMITS::Tolerance()) {
	  return false;
	}//fabs(point1[2] - point2[2]) < cLIMITS::Tolerance()

	else if(point1[2] < point2[2])
	  return true;
	else
	  return false;

      }//fabs(point1[1] - point2[1]) < cLIMITS::Tolerance()

      else if(point1[1] < point2[1])
	return true;
      else
	return false;

    } //fabs(point1[0] - point2[0]) < cLIMITS::Tolerance()
    else if(point1[0] < point2[0])
      return true;
    else
      return false;

    return true;
  }
}
#endif //_GEOM_POINT3_H_
