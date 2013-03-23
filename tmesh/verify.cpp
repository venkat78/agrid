#include "tmesh_def.hpp"

namespace tmesh {
  _TMESH_TMPL_TYPE
  BOOL _TMESH_TMPL_DECL::Verify()
  {
    BOOL rv = VerifyVertices();
   rv = VerifyFacets() && rv;
   return rv;
  }

  _TMESH_TMPL_TYPE
  BOOL _TMESH_TMPL_DECL::VerifyVertices()
  {
    vertex_iterator currVtx = VerticesBegin();
    vertex_iterator lastVtx = VerticesEnd();
    BOOL rv = true;

    for ( ; currVtx != lastVtx ; currVtx++){
      if (!currVtx->Verify())
        rv = false;
    }
    return rv;
  }

  _TMESH_TMPL_TYPE
   BOOL _TMESH_TMPL_DECL::VerifyFacets()
  {
    facet_iterator currFacet = FacetsBegin();
    facet_iterator lastFacet = FacetsEnd();
    BOOL rv = true;

    for( ; currFacet != lastFacet ; currFacet++){
      if (!currFacet->Verify())
        rv = false;
  }

    return rv;
 }
}
