/*
 * misc.hpp
 *
 *  Created on: Jul 23, 2009
 */

#ifndef TMESH_MISC_HPP_
#define TMESH_MISC_HPP_

#include <list>
#include "defs.hpp"

namespace tmesh {
  struct sFACET_INFO {
    iFACET index;
    std::list<iVERTEX> vertices;
  };

  struct sMV_TRANS
  {
    iVERTEX retainedVtxIndex;
    REAL x, y, z; //coordinates of retainedVtx before
    //moving to a new 3D position.
    std::list< sFACET_INFO > oldFacets;

    sMV_TRANS() { }
    sMV_TRANS( const sMV_TRANS &trans ) : retainedVtxIndex(trans.retainedVtxIndex),
         x(trans.x), y(trans.y), z(trans.z), oldFacets(trans.oldFacets)

    {}
  };

  struct sINT_PAIR
  {
    INT i1, i2;
    
  public:
    sINT_PAIR(INT j1 = -1, INT j2 = -1) : i1(j1),
					  i2(j2)
    {}

    UINT64 Value() const {
      INT l = LargerIndex();
      INT s = SmallerIndex();

      UINT64 val = 0;
      val |= l;
      val <<= 32;

      val |= s;

      return val;
    }
    
    VOID Print() const {
      printf("Pair %d %d \n", i1, i2);
    }

    INT SmallerIndex() const {
      return i1 < i2 ? i1 : i2;
    }

    INT LargerIndex() const {
      return i2 < i1 ? i1 : i2;
    }

    BOOL operator == (const sINT_PAIR &p) const {
      return (i1 == p.i1 && i2 == p.i2) || (i1 == p.i2 && i2 == p.i1);
    }
  };

  struct sVERTEX_PAIR : public sINT_PAIR
  {
    sVERTEX_PAIR(iVERTEX v1 = INVALID_IVERTEX, 
		 iVERTEX v2 = INVALID_IVERTEX) : sINT_PAIR(v1, v2)
    {}

    iVERTEX V1() const {
      return i1;
    }

    iVERTEX V2() const {
      return i2;
    }
  };

  struct sFACET_PAIR : public sINT_PAIR
  {
    sFACET_PAIR(iFACET f1 = INVALID_IFACET, 
		iFACET f2 = INVALID_IFACET) : sINT_PAIR(f1, f2)
    {}

    iFACET F1() const {
      return i1;
    }

    iFACET F2() const {
      return i2;
    }
  };

  struct sVERTEX_PAIR_HASH_FCN 
  {
    UINT64 operator() (const sVERTEX_PAIR &p) const {
      return p.Value();
    }
  };

  struct sFACET_PAIR_HASH_FCN 
  {
    UINT64 operator() (const sFACET_PAIR &p) const {
      return p.Value();
    }
  };

  //BOOLEAN operators.
  inline BOOL operator < (const sVERTEX_PAIR &p1,
			  const sVERTEX_PAIR &p2)
  {
    return p1.Value() < p2.Value();
  }
  
  inline BOOL operator == (const sVERTEX_PAIR &p1,
			   const sVERTEX_PAIR &p2) {
    return p1.Value() == p2.Value();
  }

  inline BOOL operator < (const sFACET_PAIR &p1,
			  const sFACET_PAIR &p2)
  {
    return p1.Value() < p2.Value();
  }
  
  inline BOOL operator == (const sFACET_PAIR &p1,
			   const sFACET_PAIR &p2) {
    return p1.Value() == p2.Value();
  }

  //Hash maps to store vertex and facet pairs.
  typedef __gnu_cxx::hash_map<sVERTEX_PAIR, BOOL, sVERTEX_PAIR_HASH_FCN> cEDGE_MARKS;
  typedef __gnu_cxx::hash_map<sFACET_PAIR, BOOL, sFACET_PAIR_HASH_FCN> cFACET_PAIRS;

}


#endif /* MISC_HPP_ */
