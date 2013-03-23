#ifndef UTILS_TEMPL_HPP_
#define UTILS_TEMPL_HPP_

#include "defs.hpp"

namespace utils {
  template <typename T, INT N>
  struct tNTUPLE
  {
    T a[N];
    
    T& operator[](INT i){
      ASSERT(i>=0 && i < N);
      return a[i];
    }

    const T& operator[](INT i) const {
      ASSERT(i>=0 && i < N);
      return a[i];
    }
  };

  template <typename T>
  struct tPAIR : public tNTUPLE<T, 2>
  {
    typedef tNTUPLE<T, 2> sBASE;
  public:
    tPAIR() {}

    tPAIR(const T &a1, const T &a2)
    {
      sBASE::a[0] = a1; sBASE::a[1] = a2;
    }
  };

  template <typename T>
  struct tTRIPLE : public tNTUPLE<T, 3>
  {
    typedef tNTUPLE<T, 3> sBASE;

  public:
    tTRIPLE() {}

    tTRIPLE(const T& a1, const T& a2, const T& a3)
    {
      sBASE::a[0] = a1;
      sBASE::a[1] = a2;
      sBASE::a[2] = a3;
    }
  };

  template <typename _ValType>
  BOOL ValIsInTable(_ValType valToBeSearched,
		    _ValType *valTable,
		    INT numVals)
  {
    for(INT i = 0; i < numVals ; i++) {
      if(valTable[i] == valToBeSearched)
	return true;
    }

    return false;
  }

}

#endif /* UTILS_TEMPL_HPP_ */
