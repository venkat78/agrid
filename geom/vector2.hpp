#ifndef _GEOM_VECTOR2_H_
#define _GEOM_VECTOR2_H_

namespace geom {

class cVECTOR2
{
public:
  //Default constructor.
  cVECTOR2() : m_coords(0.0, 0.0)
  {}

  cVECTOR2(const REAL x, const REAL y) : m_coords(x, y)
  {}

  //Builds NULL vector.
  cVECTOR2(const sNULL_VECTOR &nullVector) : m_coords(0.0, 0.0)
  {}

  //Copy constructor.
  cVECTOR2(const cVECTOR2 &vector) : m_coords(vector.m_coords)
  {}

  REAL SquaredLength() const { return (*this) * (*this); }
  REAL Length() const { return ::sqrt(SquaredLength()); }

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

  REAL operator*(const cVECTOR2 &vec) const
  {
    return (m_coords[0]*vec.m_coords[0] +
	    m_coords[1]*vec.m_coords[1]);
  }

  cVECTOR2 operator / (const REAL scale) const
  {
    ASSERT(scale != 0.0);

    return cVECTOR2(m_coords[0]/scale,
		       m_coords[1]/scale);
  }

  BOOL operator==( const cVECTOR2 vec ) const
  {
    return (fabs(m_coords[0]-vec[0]) <= cLIMITS::Epsilon() &&
	    fabs(m_coords[1]-vec[1]) <= cLIMITS::Epsilon());
  }

  cVECTOR2 operator * (const REAL scale) const
  {
    return cVECTOR2(m_coords[0] * scale,
		       m_coords[1] * scale);
  }

  cVECTOR2 operator + (const cVECTOR2& vec) const
  {
    return cVECTOR2(m_coords[0] + vec.m_coords[0],
		       m_coords[1] + vec.m_coords[1]);
  }

  cVECTOR2 operator - () const
  {
    return cVECTOR2( -m_coords[0], -m_coords[1]);
  }

  cVECTOR2& operator /=(const REAL scale)
  {
    m_coords[0] /= scale ;
    m_coords[1] /= scale ;
    return *this ;
  }

  cVECTOR2& operator *=(const REAL scale)
  {
    m_coords[0] *= scale ;
    m_coords[1] *= scale ;
    return *this ;
  }

  cVECTOR2& operator +=(const cVECTOR2& vec)
  {
    m_coords[0] += vec.m_coords[0] ;
    m_coords[1] += vec.m_coords[1] ;
    return *this ;
  }

  cVECTOR2& operator -=(const cVECTOR2& vec)
  {
    m_coords[0] -= vec.m_coords[0] ;
    m_coords[1] -= vec.m_coords[1] ;
    return *this ;
  }

  cVECTOR2& Normalize()
  {
    REAL scale = Length();
    if (scale == 0.0) {
      m_coords[0] = 0.0;
      m_coords[1] = 0.0;
    }
    else {
      m_coords[0] /= scale ;
      m_coords[1] /= scale ;
    }
    return *this;
  }

  VOID Print(FILE *stream = stdout) const
  {
    fprintf(stream, "Vector %lf %lf \n", m_coords[0],
	    m_coords[1]);
  }

protected:
  tPAIR<REAL> m_coords;
};

}
#endif //_GEOM_VECTOR2_H_
