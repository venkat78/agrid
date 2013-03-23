#ifndef _TMESH_GEOM_QUERIES_HPP_
#define _TMESH_GEOM_QUERIES_HPP_

#include "defs.hpp"
#include "misc.hpp"

namespace tmesh {
  /*
   * Generic class to support geometric queries.
   */
  template <typename _Mesh_Type>
  class tMESH_GEOM_QUERIES
  {
    _TMESH_ENTITIES(_Mesh_Type);
  public:
    tMESH_GEOM_QUERIES(_Mesh_Type *mesh = NULL) : m_mesh(mesh)
    {}

    BOOL DoIntersect(iFACET f1Index, iFACET f2Index);

    BOOL DoIntersect(iFACET facetIndex, cSEGMENT3 &segment);

    //Skips edges that are already calculated.
    //Any new intersecting edge is added to edges.
    BOOL IntersectingEdges(iFACET f1Index, iFACET f2Index,
			   cEDGE_MARKS &edges, BOOL doDebug = false);

  private:
    VOID FindCommonVertices(iFACET f1Index, iFACET f2Index,
			    cVERTEX *v[1024], INT &numCommonVertices);

    BOOL DoIntersect(iFACET srcIndex, iFACET targetIndex,
		     cVERTEX *v[1024], INT numCommonVertices);

    BOOL IntersectingEdges(iFACET srcIndex, iFACET targetIndex,
			   cVERTEX *v[1024], INT numCommonVertices,
			   cEDGE_MARKS &edges, BOOL doDebug = false);

  private:
    _Mesh_Type *m_mesh;
  };
}

#endif //_TMESH_GEOM_QUERIES_HPP_
