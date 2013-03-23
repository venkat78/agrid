#include "common_internal3d.hpp"
#include "transform3.hpp"

namespace geom {

REAL cTRANSFORM3::Det3(const REAL& a00,  const REAL& a01,  const REAL& a02,
	  const REAL& a10,  const REAL& a11,  const REAL& a12,
	  const REAL& a20,  const REAL& a21,  const REAL& a22) const
 {
   // First compute the det2x2
   REAL m01 = a00*a11 - a10*a01;
   REAL m02 = a00*a21 - a20*a01;
   REAL m12 = a10*a21 - a20*a11;
   // Now compute the minors of rank 3
   REAL m012 = m01*a22 - m02*a12 + m12*a02;
   return m012;
 }

REAL cTRANSFORM3::M(int i, int j) const
  {
    switch (i)
      {
      case 0: switch (j)
	{
	case 0: return t11;
	case 1: return t12;
	case 2: return t13;
	case 3: return t14;
	}
      case 1: switch (j)
	{
	case 0: return t21;
	case 1: return t22;
	case 2: return t23;
	case 3: return t24;
	}
      case 2: switch (j)
	{
	case 0: return t31;
	case 1: return t32;
	case 2: return t33;
	case 3: return t34;
	}
      case 3: switch (j)
	{
	case 0: return REAL(0);
	case 1: return REAL(0);
	case 2: return REAL(0);
	case 3: return REAL(1);
	}
      }
    return REAL(0);
  }

  cTRANSFORM3  cTRANSFORM3::Inverse() const // FIXME : construction
  {
    REAL w = 1.0/Det3(t11, t12, t13, t21, t22, t23, t31, t32, t33 );
    return cTRANSFORM3(Det2( t22, t23, t32, t33)*w,         // i 11
  			-Det2( t12, t13, t32, t33)*w,         // i 12
  			Det2( t12, t13, t22, t23)*w,         // i 13
  			-Det3( t12, t13, t14, t22, t23, t24, t32, t33, t34 )*w,

  			-Det2( t21, t23, t31, t33)*w,         // i 21
  			Det2( t11, t13, t31, t33)*w,         // i 22
  			-Det2( t11, t13, t21, t23)*w,         // i 23
  			Det3( t11, t13, t14, t21, t23, t24, t31, t33, t34 )*w,

  			Det2( t21, t22, t31, t32)*w,         // i 31
  			-Det2( t11, t12, t31, t32)*w,         // i 32
  			Det2( t11, t12, t21, t22)*w,         // i 33
  			-Det3( t11, t12, t14, t21, t22, t24, t31, t32, t34 )*w);

    //Det3( t11, t12, t13, t21, t22, t23, t31, t32, t33 ));
  }

}
