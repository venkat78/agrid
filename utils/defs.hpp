#ifndef DEFS_HPP_
#define DEFS_HPP_

namespace utils {
  typedef int INT;
  typedef unsigned int UINT;
  typedef double REAL;
  typedef bool BOOL;
  typedef void VOID;
  typedef unsigned long long UINT64;

  struct sTAG {};
  struct sTRUE {};
  struct sFALSE {};

  struct sCOLOR {
    REAL r, g, b, alpha;
    sCOLOR(REAL _r = 0.5, REAL _g = 0.5,
	   REAL _b = 0.5, REAL _alpha = 0.5) : r(_r), g(_g), b(_b), alpha(_alpha)
    {}
  };

  inline VOID Pause() { }

  inline sCOLOR GetColor(INT i) {
    if(i == 0)
      return sCOLOR(1.0, 0.0, 0.0, 1.0);
    else if(i == 1)
      return sCOLOR(0.0, 1.0, 0.0, 1.0);
    else if(i == 2)
      return sCOLOR(0.0, 0.0, 1.0, 1.0);

    
    return sCOLOR(1.0, 1.0, 1.0, 1.0);
  }

  BOOL IsDebugOn();
  
  VOID SetDebugOn();

  VOID SetDebugOff();
  
  //#ifdef DEBUG
#define ASSERT(cond) assert(cond)
// #else
// #define ASSERT(cond)
// #endif //DEBUG.

}

//#define DEBUG 1


#endif /*DEFS_HPP_*/
