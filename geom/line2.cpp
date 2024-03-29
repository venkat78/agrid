#include "common_internal2d.hpp"
namespace geom {

cLINE2::cLINE2(const REAL &a, const REAL &b, const REAL &c) : m_coeffs(a, b, c)
  {
    //Line a*x + b*y + c = 0 is the same as -a*x - b*y -c = 0
    //Trying to maintain a consistent representation by always
    //making a > 0.
    if(m_coeffs[0] < 0.0) {
      m_coeffs[0] *= -1.0;
      m_coeffs[1] *= -1.0;
      m_coeffs[2] *= -1.0;
    }
    else if(m_coeffs[0] == 0.0 && m_coeffs[1] < 0.0) {
      m_coeffs[1] *= -1.0;
      m_coeffs[2] *= -1.0;
    }
  }

VOID cLINE2::MakeLineFromPoints(const REAL& px, const REAL &py,
					   const REAL& qx, const REAL &qy)
{
  // The horizontal and vertical line get a special treatment
  // in order to make the intersection code robust for doubles
  if(py == qy){
    m_coeffs[0] = REAL(0) ;
    if(qx > px){
      m_coeffs[1] = REAL(1);
      m_coeffs[2] = -py;
    } else if(qx == px){
      m_coeffs[1] = REAL(0);
      m_coeffs[2] = REAL(0);
    }else{
      m_coeffs[1] = REAL(-1);
      m_coeffs[2] = py;
    }
  } else if(qx == px){
    m_coeffs[1] = REAL(0);
    if(qy > py){
      m_coeffs[0] = REAL(-1);
      m_coeffs[2] = px;
    } else if (qy == py){
      m_coeffs[0] = REAL(0);
      m_coeffs[2] = REAL(0);
    } else {
      m_coeffs[0] = REAL(1);
      m_coeffs[2] = -px;
    }
  } else {
    m_coeffs[0] = py - qy;
    m_coeffs[1] = qx - px;
    m_coeffs[2] = -px*m_coeffs[0] - py*m_coeffs[1];
  }


  if(m_coeffs[0] < 0.0) {
    //Line a*x + b*y + c = 0 is the same as -a*x - b*y -c = 0
    //Trying to maintain a consistent representation by always
    //making a > 0.
    m_coeffs[0] *= -1.0;
    m_coeffs[1] *= -1.0;
    m_coeffs[2] *= -1.0;
  }
  else if(m_coeffs[0] == 0.0 && m_coeffs[1] < 0.0) {
    m_coeffs[1] *= -1.0;
    m_coeffs[2] *= -1.0;
  }
}



eORIENTED_SIDE
cLINE2::OrientedSide(const cPOINT2 &point) const
{
  REAL signedDistance = SignedDistance(point);
  if(fabs(signedDistance) <= cLIMITS::Tolerance())
    return ON_ORIENTED_BOUNDARY;

  if(signedDistance > 0)
    return ON_POSITIVE_SIDE;

  return ON_NEGATIVE_SIDE;
}

//point is not on the line
BOOL cLINE2::MakeSignedDistancePositive(const cPOINT2 &point)
{
  if (SignedDistance(point) < 0.0){
	  m_coeffs[0] = -m_coeffs[0];
	  m_coeffs[1] = -m_coeffs[1];
	  m_coeffs[2] = -m_coeffs[2];
	  return false;
  }
  else
	  return true;
}

}
