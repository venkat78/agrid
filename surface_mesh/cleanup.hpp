/*
 * cleanup.hpp
 *
 *  Created on: Aug 3, 2010
 */

#ifndef  _SURFACE_MESH_CLEANUP_HPP_
#define _SURFACE_MESH_CLEANUP_HPP_

#include "surface_mesh_class.hpp"

  class cSURFACE_MESH_CLEANUP {

  public:

    cSURFACE_MESH_CLEANUP(cSURFACE_MESH *mesh, REAL charSize = 0.0); //constructor
    BOOL Perform();
    VOID RelativeTol(REAL relativeTol) {m_relativeTol = relativeTol;}
    REAL RelativeTol() {return m_relativeTol;}
    VOID CharSize(REAL charSize) {m_charSize = charSize;}
    REAL CharSize() {return m_charSize;}
    REAL AbsoluteTol() { return m_absoluteTol;} //cannot be set directly
    BOOL DeleteFacetIfDegenerate(iFACET facetLabel);

  private://methods

    BOOL DeleteCoincidentFacets();
    BOOL DeleteDegenerateFacets();
    BOOL DeleteFacetIfHasCoincident(iFACET facetLabel);
    BOOL DeleteFacetIfHasCoincident(cSURFACE_MESH::cFACET *facet);
    cSURFACE_MESH::cFACET *CoincidentAdjacentFacet(const cSURFACE_MESH::cFACET *facet) const;
    iFACET CoincidentFacetToBeDeleted(const cSURFACE_MESH::cFACET *facet1, const cSURFACE_MESH::cFACET *facet2) const;
    BOOL DeleteFacetIfDegenerate(cSURFACE_MESH::cFACET *facet);
    BOOL DeleteFacetWithDegenerateEdge(cSURFACE_MESH::cFACET *facet);
    BOOL DeleteFacetIfNarrow(cSURFACE_MESH::cFACET *facet);

  private: //fields

    cSURFACE_MESH *m_mesh;
    REAL  m_relativeTol;
    REAL m_charSize;
    REAL m_absoluteTol;
  }; //class cMESH_CLEANUP
#endif //MESH_CLEANUP_HPP_

