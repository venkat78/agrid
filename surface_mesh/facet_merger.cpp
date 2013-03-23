/*
 * facet_merger.cpp
 *
 *  Created on: Feb 11, 2013
 */

#include "facet_merger.hpp"

cFACET_MERGER::cFACET_MERGER(cSURFACE_MESH *mesh)
{
  m_mesh = mesh;
}

VOID cFACET_MERGER::MergeCoplanarFacets()
{
  INT numFacets = m_mesh->NumFacets();
  for (INT f1 = 0; f1 < numFacets; f1++){
    cSURFACE_MESH::cFACET *facet = m_mesh->Facet(f1);
    if (!facet->IsDeleted()){
      BOOL do_merge = true;
      cVECTOR3 facetNormal =  facet->UnitNormal();
      while (do_merge){
        cSURFACE_MESH::cHALF_EDGE *he = CoplanarFacetHE(f1, facetNormal);
        if (he == NULL)
          do_merge = false;
        else {
          iVERTEX vertexIndex1 = he->Tail()->Index();
          iVERTEX vertexIndex2 = he->Head()->Index();
          iFACET f2 = he->Opp()->Facet()->Index();
          m_mesh->RemoveDiagonal(vertexIndex1, vertexIndex2, f1, f2, f2);
 /*      printf("f1 %d; v1 %d; v2 %d; f2 %d\n", f1, vertexIndex1, vertexIndex2, f2);
          char fname[24];
          fname[0] = '\0';
          sprintf(fname, "mesh%d_%d.txt", f1, f2);
          FILE *mf = fopen(fname, "wb+");
          m_mesh->Print(mf);
          fclose(mf);
          */
          m_mesh->Verify();
        }
      }
    }
  }
}


BOOL cFACET_MERGER::FacetsAreCoplanar(iFACET facetIndex1, iFACET facetIndex2)
{
  //temporary simple code - YG Amoeba
  //needs to be replaced by a tolerance/distance based query
  cSURFACE_MESH::cFACET *facet1 = m_mesh->Facet(facetIndex1);
  cSURFACE_MESH::cFACET *facet2 = m_mesh->Facet(facetIndex2);
  cVECTOR3 normal1 = facet1->UnitNormal();
  cVECTOR3 normal2 = facet2->UnitNormal();
  return (normal1 == normal2);
}

//returns first adjacent coplanar facet index to this one

cSURFACE_MESH::cHALF_EDGE *cFACET_MERGER::CoplanarFacetHE(iFACET facetIndex, cVECTOR3 &facetNormal)
{

  cSURFACE_MESH::cFACET *facet = m_mesh->Facet(facetIndex);
  cSURFACE_MESH::cFACET::half_edge_circulator currHe = facet->HalfEdgesBegin();
  cSURFACE_MESH::cFACET::half_edge_circulator lastHe = facet->HalfEdgesEnd();
  for( ; currHe != lastHe ; currHe++ ) {
    if(!currHe->IsNonManifold()){ //this needs to be removed and verified later
      cSURFACE_MESH::cHALF_EDGE *heOpp = currHe->Opp();
      cSURFACE_MESH::cFACET *oppFacet = heOpp->Facet();
      if (oppFacet != NULL){
        if (facetNormal == oppFacet->UnitNormal())
          return currHe.operator->();
      }
    }
  }
  return NULL;
}

