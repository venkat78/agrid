#include "tmesh_def.hpp"
#include "construct_manifolds.hpp"

namespace tmesh {
  _TMESH_TMPL_TYPE
  INT _TMESH_TMPL_DECL::ConstructManifolds()
  {
    ClearManifolds();
    tCONSTRUCT_MANIFOLDS<cSELF> constructManifolds(this);
    return constructManifolds.Finish();
  }

  _TMESH_TMPL_TYPE
  VOID _TMESH_TMPL_DECL::ClearManifolds()
  {
    m_manifolds.flush();

    facet_iterator currFacet = FacetsBegin();
    facet_iterator lastFacet = FacetsEnd();

    for( ; currFacet != lastFacet ; currFacet++ )
      currFacet->ManifoldIndex(INVALID_IMANIFOLD);

    m_numDeletedManifolds = 0;
  }

  _TMESH_TMPL_TYPE
  BOOL _TMESH_TMPL_DECL::DeleteManifold(iMANIFOLD index)
  {
    cMANIFOLD *manifold = m_manifolds.object_at(index);

    if(manifold->IsDeleted())
      return false;

    std::vector<iFACET> facetsToBeDeleted;

    typename cMANIFOLD::facet_iterator currFacet = manifold->FacetsBegin();
    typename cMANIFOLD::facet_iterator lastFacet = manifold->FacetsEnd();

    for ( ; currFacet != lastFacet ; currFacet++ ) {
      facetsToBeDeleted.push_back(currFacet->Index());
    }

    INT n = facetsToBeDeleted.size();

    for(INT i = 0 ; i < n ; i++) {
//       printf("Facet to be deleted %d \n", facetsToBeDeleted[i]);
//       fflush(stdout);

//       VerifyFacets();

//       printf("After first VerifyFacets()\n");
      DeleteFacet(facetsToBeDeleted[i]);

//       VerifyFacets();
    }

    m_numDeletedManifolds++;
    manifold->MarkDeleted();

    return true;
  }

 }
