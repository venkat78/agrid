
#ifndef _GEOM_LINE2_H_
#define _GEOM_LINE2_H_

namespace geom {
class cLINE2
{
public:
  cLINE2() : m_coeffs(REAL(0), REAL(0), REAL(0))
  {}

  cLINE2(const REAL &a, const REAL &b, const REAL &c);

  cLINE2(const cPOINT2 &p, const cPOINT2 &q){
	  MakeLineFromPoints(p[0], p[1],
	 		     q[0], q[1]);
  }

  const REAL& A() const { return m_coeffs[0]; }
  const REAL& B() const { return m_coeffs[1]; }
  const REAL& C() const { return m_coeffs[2]; }

  REAL SignedDistance(const cPOINT2 &point) const {
    return A()*point[0] + B()*point[1] + C();
  }

  BOOL MakeSignedDistancePositive(const cPOINT2 &point);
  //if the point-line distance is negative, negate the line coefficients
  eORIENTED_SIDE OrientedSide(const cPOINT2 &p) const;
  VOID MakeLineFromPoints(const REAL &px, const REAL &py,
			  const REAL &qx, const REAL &qy);

  cLINE2 Opposite() const
  {
    return cLINE2(REAL(-1)*A(), REAL(-1)*B(), REAL(-1)*C());
  }

  BOOL operator == ( const cLINE2 &line) const
  {
    return (EqualsInternal(line) ||
	    EqualsInternal(line.Opposite()));
  }

  BOOL operator != (const cLINE2 &line) const {
    return !(*this == line);
  }

private:
  BOOL EqualsInternal(const cLINE2 &line) const
  {
    return ((fabs(A() - line.A()) <= cLIMITS::Tolerance()) &&
	    (fabs(B() - line.B()) <= cLIMITS::Tolerance()) &&
	    (fabs(C() - line.C()) <= cLIMITS::Tolerance())) ? true : false;
  }

protected:
  tTRIPLE<REAL> m_coeffs;
};
}

#endif //_GEOM_LINE2_H_
