#ifndef GEOM_DEFS_HPP_
#define GEOM_DEFS_HPP_

#include "utils.hpp"
using namespace utils;

#include <cstdio>
#include <cfloat>
#include <cmath>

namespace geom {


//---------------------------------
// enumeration for GEOM coordinates.
//---------------------------------
enum eCOORD {GEOM_X=0, GEOM_Y, GEOM_Z, GEOM_COORD_NONE};

inline eCOORD thirdCoord(eCOORD coord0, eCOORD coord1)
{return (eCOORD)(3-(INT)coord0-(INT)coord1);}
inline eCOORD CoordPlusOne(eCOORD coord)
{return coord < GEOM_Z ? (eCOORD)(coord+1) : (eCOORD)(coord-2);}
inline eCOORD CoordPlusTwo(eCOORD coord)
{return coord < GEOM_Y ? (eCOORD)(coord+2) : (eCOORD)(coord-1);}

inline char coordChar(eCOORD coord) {return coord == GEOM_X ? 'x' : coord == GEOM_Y ? 'y' : coord == GEOM_Z ? 'z' : '-';}
//---------------------------------
// enumeration for SIGN.
//---------------------------------
enum eSIGN { NEGATIVE=-1, ZERO=0, POSITIVE=1 };

typedef eSIGN eORIENTATION;

#define COLLINEAR ZERO

#define DO_COORDS(coord)		\
for(eCOORD coord = GEOM_X; ((INT)coord) < 3; coord = (eCOORD)(((INT)coord) + 1))

//---------------------------------
// Defining Origin for BG.
//---------------------------------
class sORIGIN {};
extern sORIGIN ORIGIN;

//---------------------------------
// Defining Null vector for BG.
//---------------------------------
class sNULL_VECTOR {};
extern sNULL_VECTOR NULL_VECTOR;

//----------------------------------------
// Defining Identity transformation for BG.
// Inherited from CGAL.
//----------------------------------------
class sIDENTITY_TRANSFORM {};
extern sIDENTITY_TRANSFORM IDENTITY_TRANSFORM;

class sTRANSLATION {};
extern sTRANSLATION TRANSLATION;

class sSCALING {};
extern sSCALING SCALING;

class sROTATION {};
extern sROTATION ROTATION;

//---------------------------------------
// Enumeration for angle types in BG.
//---------------------------------------
enum eANGLE {
  OBTUSE_ANGLE=-1,
  RIGHT_ANGLE=0,
  ACUTE_ANGLE=1
};

//------------------------------------------
// Enumeration for Oriented side of plane.
//------------------------------------------
enum eORIENTED_SIDE {
  ON_NEGATIVE_SIDE=-1,
  ON_ORIENTED_BOUNDARY=0,
  ON_POSITIVE_SIDE=1
};

//------------------------------------------
// Enumeration for Bounding boxes.
//------------------------------------------
enum eBOUNDED_SIDE {
  ON_UNBOUNDED_SIDE=-1,
  ON_BOUNDARY=0,
  ON_BOUNDED_SIDE=1
};

//------------------------------------------
// Enumeration for position relative to a box.
//More positions can be added if needed, like left_of_bounary,
//inside_of_boundary, right_of_boundary
//------------------------------------------
enum eBOX_POSITION {
	POSITION_LEFT_BOUNDARY=0,
	POSITION_RIGHT_BOUNDARY,
	POSITION_NOT_ON_BOUNDARY,
	POSITION_UNKNOWN};

enum eBOX_FACE{
	BOX_FACE_NONE = -1,
	BOX_XMIN = 0,
	BOX_XMAX = 1,
	BOX_YMIN = 2,
	BOX_YMAX = 3,
	BOX_ZMIN = 4,
	BOX_ZMAX = 5};
//-------------------------------------------
// A general enum definition for return type
// to be used in GEOM.
//-------------------------------------------
enum eRETURN_TYPE {
  FAILURE=-1,
  SUCCESS,
  RT_POINT3,
  RT_SEGMENT3,
  RT_RAY3,
  RT_LINE3,
  RT_SEGMENT2,
  RT_POINT2,
  RT_LINE2,
  RT_ENDPOINT3
};

class cLIMITS
{
public:
  static REAL tolerance;
  static REAL epsilon;
  static REAL max, min;

  static VOID Tolerance(REAL tol)
  {
    tolerance = tol;
  }

  static REAL Tolerance()
  {
    return tolerance;
  }

  static VOID Epsilon(REAL eps)
  {
    epsilon = eps;
  }

  static REAL Epsilon()
  {
    return epsilon;
  }

  static REAL Max()
  {
    return max;
  }

  static REAL Min()
  {
    return min;
  }
};

inline eSIGN Sign(const REAL &val)
{
  return (val < 0) ? NEGATIVE : (0 < val) ? POSITIVE : ZERO;
}

  inline BOOL Equal(const REAL val1, const REAL val2)
  {
    return fabs(val1 - val2) < cLIMITS::Tolerance();
  }

  inline eBOX_FACE BoxFace(BOOL objectOnBoxBoundary[3][2])
  {
	DO_COORDS(c){
	  for (INT j=0; j<2; j++){
		if (objectOnBoxBoundary[c][j])
		  return (eBOX_FACE)(2*c+j);
	  }
	}
	return BOX_FACE_NONE;
  }

  #ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifdef __MSVC__
#define isnan(x) _isnan(x)
#endif

}

#endif /* GEOM_DEFS_HPP_ */
