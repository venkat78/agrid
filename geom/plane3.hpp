
#ifndef _GEOM_PLANE3_H_
#define _GEOM_PLANE3_H_

namespace geom {

class cSEGMENT3;

class cPLANE3
{

public:
  //Default constructor.
  cPLANE3() : m_a(REAL(0)), m_b(REAL(0)),
		 m_c(REAL(0)), m_d(REAL(0))
  {}

  cPLANE3(const REAL &a,
	     const REAL &b,
	     const REAL &c,
	     const REAL &d) : m_a(a), m_b(b), m_c(c), m_d(d)
  {}

  cPLANE3(const cPOINT3 &p,
	     const cPOINT3 &q,
	     const cPOINT3 &r);

  cPLANE3(const cPOINT3 &point,
	     const cVECTOR3 &vec);

  cPLANE3(const cPLANE3 &plane) : m_a(plane.A()),
					m_b(plane.B()),
					m_c(plane.C()),
					m_d(plane.D())
  {}

  VOID Print(FILE *stream = stdout) const
  {
    fprintf(stream, "Plane : %lf %lf %lf %lf \n",
	    m_a, m_b, m_c, m_d);
  }

  cPOINT3 Projection(const cPOINT3& point) const ;

  cVECTOR3 Normal() const { return cVECTOR3(A(), B(), C()); }

  REAL SignedDistance(const cPOINT3& point) const
  {
    return (A()*point[0] +
	    B()*point[1] +
	    C()*point[2] +
	    D());
  }

  eRETURN_TYPE SegmentIntersect(const cSEGMENT3 &segment, cPOINT3 *p_intersectionPoint) const;
  BOOL HasOn(const cPOINT3 &point) const
  {
    return (fabs(SignedDistance(point)) <= cLIMITS::Tolerance() ?
    		true : false);
  }

  eORIENTED_SIDE OrientedSide(const cPOINT3 &point) const;

  REAL A() const { return m_a; }
  REAL B() const { return m_b; }
  REAL C() const { return m_c; }
  REAL D() const { return m_d; }

protected:
  REAL m_a, m_b, m_c, m_d;
};

}
#endif //_GEOM_PLANE3_H_
