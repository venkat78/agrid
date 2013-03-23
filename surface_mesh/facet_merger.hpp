/*
 * facet_merger.hpp
 *
 *  Created on: Feb 11, 2013
 */

#ifndef FACET_MERGER_HPP_
#define FACET_MERGER_HPP_


#include "surface_mesh_class.hpp"

  class cFACET_MERGER {

  private:
    BOOL FacetsAreCoplanar(iFACET if1, iFACET if2);
    cSURFACE_MESH::cHALF_EDGE *CoplanarFacetHE(iFACET facetIndex, cVECTOR3 &facetNormal);

  public:

    cFACET_MERGER(cSURFACE_MESH *mesh); //constructor
    VOID MergeCoplanarFacets();

  private: //methods

   private: //fields

    cSURFACE_MESH *m_mesh;
  };

#endif // FACET_MERGER_HPP_
