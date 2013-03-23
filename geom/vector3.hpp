
#ifndef _GEOM_VECTOR3_H_
#define _GEOM_VECTOR3_H_

namespace geom {

class cVECTOR3
{

public:
  //Default constructor.
  cVECTOR3() : m_coords(0.0, 0.0, 0.0)
  {}

  cVECTOR3(const REAL x, const REAL y, const REAL z) : m_coords(x, y, z)
  {}

  //Builds NULL vector.
  cVECTOR3(const sNULL_VECTOR &nullVector) : m_coords(0.0, 0.0, 0.0)
  {}

  //Copy constructor.
  cVECTOR3(const cVECTOR3 &vector) : m_coords(vector.m_coords)
  {}

  REAL SquaredLength() const { return (*this) * (*this); }
  REAL Length() const { return sqrt(SquaredLength()); }
  cVECTOR3 Normalize() const { return (*this)/Length(); }

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

  const REAL& operator[] (INT i) const
  {
    return m_coords[i];
  }

  REAL operator*(const cVECTOR3 &vec) const
  {
    return (m_coords[0]*vec.m_coords[0] +
	    m_coords[1]*vec.m_coords[1] +
	    m_coords[2]*vec.m_coords[2]);
  }

  cVECTOR3 operator / (const REAL scale) const
  {
    ASSERT(scale != 0.0);

    return cVECTOR3(m_coords[0]/scale,
		       m_coords[1]/scale,
		       m_coords[2]/scale);
  }

  BOOL operator==( const cVECTOR3 &vec ) const
  {
	  //YG replaced epsilon by tolerance o 02.13.13 to avoid small inaccuracies in normal computation
    return (fabs(m_coords[0]-vec[0]) <= cLIMITS::Tolerance() &&
	    fabs(m_coords[1]-vec[1]) <= cLIMITS::Tolerance() &&
	    fabs(m_coords[2]-vec[2]) <= cLIMITS::Tolerance());
  }

  BOOL operator != (const cVECTOR3 &vec) const
  {
    return !(*this == vec);
  }

  cVECTOR3 operator * (const REAL scale) const
  {
    return cVECTOR3(m_coords[0] * scale,
		       m_coords[1] * scale,
		       m_coords[2] * scale);
  }

  cVECTOR3 operator + (const cVECTOR3& vec) const
  {
    return cVECTOR3(m_coords[0] + vec.m_coords[0],
		       m_coords[1] + vec.m_coords[1],
		       m_coords[2] + vec.m_coords[2]);
  }

  cVECTOR3 operator - () const
  {
    return cVECTOR3( -m_coords[0], -m_coords[1], -m_coords[2]);
  }

  cVECTOR3 operator - (const cVECTOR3& vec) const
  {
    return cVECTOR3(m_coords[0] - vec.m_coords[0],
		       m_coords[1] - vec.m_coords[1],
		       m_coords[2] - vec.m_coords[2]);
  }

  cVECTOR3& operator /=(const REAL scale)
  {
    m_coords[0] /= scale ;
    m_coords[1] /= scale ;
    m_coords[2] /= scale ;
    return *this ;
  }

  cVECTOR3& operator *=(const REAL scale)
  {
    m_coords[0] *= scale ;
    m_coords[1] *= scale ;
    m_coords[2] *= scale ;
    return *this ;
  }

  cVECTOR3& operator +=(const cVECTOR3& vec)
  {
    m_coords[0] += vec.m_coords[0] ;
    m_coords[1] += vec.m_coords[1] ;
    m_coords[2] += vec.m_coords[2] ;
    return *this ;
  }

  cVECTOR3& operator -=(const cVECTOR3& vec)
  {
    m_coords[0] -= vec.m_coords[0] ;
    m_coords[1] -= vec.m_coords[1] ;
    m_coords[2] -= vec.m_coords[2] ;
    return *this ;
  }

  cVECTOR3 PerpVector() const;

  VOID Print(FILE *stream = stdout) const
  {
    fprintf(stream, "Vector %lg %lg %lg \n", m_coords[0],
	    m_coords[1], m_coords[2]);
  }

  eCOORD IsOnCoordPlane()
  {
	  DO_COORDS(coord){
		  if (fabs(m_coords[coord]) <= cLIMITS::Tolerance())
			  return coord;
	  }
	  return GEOM_COORD_NONE;
  }
  eCOORD AbsMaxCoord() const ;
  eCOORD AbsMinCoord() const ;

  BOOL IsNull() const ;
protected:
  tTRIPLE<REAL> m_coords;
};

cVECTOR3 Cross(const cVECTOR3& vec0, const cVECTOR3& vec1);
VOID Normalize(cVECTOR3& vec);

} //namespace
#endif //_GEOM_VECTOR3_H_
