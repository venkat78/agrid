#ifndef _TMESH_FACETS_OPS_HPP_
#define _TMESH_FACETS_OPS_HPP_

#include "defs.hpp"
#include "misc.hpp"

namespace tmesh {
  /*
   * Generic class to support geometric queries.
   * Will add other clipper methods of tMESH to this class.
   */
  template <typename _Mesh_Type>
  class tMESH_FACETS_OPS
  {
    _TMESH_ENTITIES(_Mesh_Type);

    struct sVERTEX {
      iVERTEX index;
      cPOINT3 point;
      BOOL operator < (const sVERTEX &v) const {
       return point < v.point;
     }
    };

  public:
    tMESH_FACETS_OPS(_Mesh_Type *mesh = NULL) : m_mesh(mesh)
    {}
  
    BOOL MergeFacets(iFACET f1, iFACET f2);
  
  private:
    BOOL CheckVertices(sVERTEX f1Vertices[256],
		       sVERTEX f2Vertices[256],
		       INT numVertices);

    INT CollectVertices(iFACET facetIndex, sVERTEX vertices[256]);

  private:
    _Mesh_Type *m_mesh;
  };
}

#endif //_TMESH_FACETS_OPS_HPP_
