
#ifndef _GEOM_TRANSFORM3_H_
#define _GEOM_TRANSFORM3_H_

namespace geom{

class cTRANSFORM3
{
public:
  cTRANSFORM3()
  {
    REAL nt0(0), nt1(1);
    t11 = nt1; t12 = nt0; t13 = nt0; t14 = nt0;
    t21 = nt0; t22 = nt1; t23 = nt0; t24 = nt0;
    t31 = nt0; t32 = nt0; t33 = nt1; t34 = nt0;
  }

  cTRANSFORM3(const sIDENTITY_TRANSFORM &identity)
  {
    REAL nt0(0), nt1(1);
    t11 = nt1; t12 = nt0; t13 = nt0; t14 = nt0;
    t21 = nt0; t22 = nt1; t23 = nt0; t24 = nt0;
    t31 = nt0; t32 = nt0; t33 = nt1; t34 = nt0;
  }

  cTRANSFORM3(const REAL& m11, const REAL& m12, const REAL& m13,
		 const REAL& m21, const REAL& m22, const REAL& m23,
		 const REAL& m31, const REAL& m32, const REAL& m33)
    : t11(m11), t12(m12), t13(m13), t14(REAL(0)),
      t21(m21), t22(m22), t23(m23), t24(REAL(0)),
      t31(m31), t32(m32), t33(m33), t34(REAL(0))
  {}

  cTRANSFORM3(const REAL& m11, const REAL& m12, const REAL& m13, const REAL& m14,
		 const REAL& m21, const REAL& m22, const REAL& m23, const REAL& m24,
		 const REAL& m31, const REAL& m32, const REAL& m33, const REAL& m34
		 )
    : t11(m11), t12(m12), t13(m13), t14(m14),
      t21(m21), t22(m22), t23(m23), t24(m24),
      t31(m31), t32(m32), t33(m33), t34(m34)
  {}

  cTRANSFORM3(const sTRANSLATION &translation,
		 const cVECTOR3 &v)
  {
    REAL nt0(0), nt1(1);
    t11 = nt1; t12 = nt0; t13 = nt0; t14 = nt0 + v.X();
    t21 = nt0; t22 = nt1; t23 = nt0; t24 = nt0 + v.Y();
    t31 = nt0; t32 = nt0; t33 = nt1; t34 = nt0 + v.Z();
  }

  cPOINT3 Transform(const cPOINT3& p) const // FIXME : construction
  {
    return cPOINT3(t11 * p.X() + t12 * p.Y() + t13 * p.Z() + t14,
		   t21 * p.X() + t22 * p.Y() + t23 * p.Z() + t24,
		   t31 * p.X() + t32 * p.Y() + t33 * p.Z() + t34);
  }

  // note that a vector is not translated
   cVECTOR3 Transform(const cVECTOR3& v) const // FIXME : construction
  {
    return cVECTOR3(t11 * v.X() + t12 * v.Y() + t13 * v.Z(),
                    t21 * v.X() + t22 * v.Y() + t23 * v.Z(),
                    t31 * v.X() + t32 * v.Y() + t33 * v.Z());
  }

  REAL Det2( const REAL& a00,  const REAL& a01,
	   const REAL& a10,  const REAL& a11) const
  {
    // First compute the det2x2
    REAL m01 = a00*a11 - a10*a01;
    return m01;
  }

  REAL Det3(const REAL& a00,  const REAL& a01,  const REAL& a02,
		  const REAL& a10,  const REAL& a11,  const REAL& a12,
		  const REAL& a20,  const REAL& a21,  const REAL& a22) const;

  cTRANSFORM3 Inverse() const;

  cTRANSFORM3 operator*(const cTRANSFORM3 &t) const
  {
    return cTRANSFORM3(t.t11*t11 + t.t12*t21 + t.t13*t31,
		      t.t11*t12 + t.t12*t22 + t.t13*t32,
		      t.t11*t13 + t.t12*t23 + t.t13*t33,
		      t.t11*t14 + t.t12*t24 + t.t13*t34 + t.t14,

		      t.t21*t11 + t.t22*t21 + t.t23*t31,
		      t.t21*t12 + t.t22*t22 + t.t23*t32,
		      t.t21*t13 + t.t22*t23 + t.t23*t33,
		      t.t21*t14 + t.t22*t24 + t.t23*t34 + t.t24,

		      t.t31*t11 + t.t32*t21 + t.t33*t31,
		      t.t31*t12 + t.t32*t22 + t.t33*t32,
		      t.t31*t13 + t.t32*t23 + t.t33*t33,
		      t.t31*t14 + t.t32*t24 + t.t33*t34 + t.t34);
  }

  REAL M(int i, int j) const ;
  VOID Print(FILE *stream = stdout) const {}


protected:
  REAL   t11, t12, t13, t14; // FIXME : Wouldn't this be better with an array ?
  REAL   t21, t22, t23, t24;
  REAL   t31, t32, t33, t34;
};

}

#endif //_GEOM_TRANSFORM3_H_
