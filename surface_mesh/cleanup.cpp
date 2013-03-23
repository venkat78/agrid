/*
 * cleanup.cpp
 *
 *  Created on: Aug 3, 2010
 */

#include "cleanup.hpp"

  //constructor
  cSURFACE_MESH_CLEANUP::cSURFACE_MESH_CLEANUP(cSURFACE_MESH *mesh, REAL charSize)
  {
    m_relativeTol = 1e-4;//init value
     m_mesh = mesh;
     m_charSize = charSize;
     m_absoluteTol = m_charSize * m_relativeTol;
     if (m_absoluteTol < cLIMITS::Tolerance())
       m_absoluteTol = cLIMITS::Tolerance();
  }

  BOOL cSURFACE_MESH_CLEANUP::Perform()
  {
    return (DeleteCoincidentFacets() &&
	    DeleteDegenerateFacets());
  }

  //removes one of the pair of coincident facets. If two faces are coincident, and
 //adjacent to each other, and one of them has more border edges than the other,
 //the one with the smaller number of border edges is removed.
 //this is a purely topological operation
 BOOL cSURFACE_MESH_CLEANUP::DeleteCoincidentFacets()
 {
    INT largestFI = m_mesh->LargestFacetIndex();

    for( iFACET facetLabel = 0; facetLabel < largestFI; facetLabel++){
      DeleteFacetIfHasCoincident(facetLabel);
    }
    return true;
  }

  BOOL cSURFACE_MESH_CLEANUP::DeleteFacetIfHasCoincident(iFACET facetLabel)
 {
    cSURFACE_MESH::cFACET *facet = m_mesh->Facet(facetLabel);
   return DeleteFacetIfHasCoincident(facet);
 }

  BOOL cSURFACE_MESH_CLEANUP::DeleteFacetIfHasCoincident(cSURFACE_MESH::cFACET *facet)
 {
   if (facet->IsDeleted())
     return false;

   cSURFACE_MESH::cFACET *coincidentFacet = CoincidentAdjacentFacet(facet);
   if (coincidentFacet == NULL){
     return false;
   }
   else {
     iFACET facetLabelToBeDeleted = CoincidentFacetToBeDeleted(facet, coincidentFacet);
     m_mesh->DeleteFacet(facetLabelToBeDeleted);
   }
   return false;
  }

  //retuns facet if it's adjacent to the input facet through all of their edges
  cSURFACE_MESH::cFACET *cSURFACE_MESH_CLEANUP::CoincidentAdjacentFacet(const cSURFACE_MESH::cFACET *facet) const
  {
    cSURFACE_MESH::cFACET *adjacentFacet = NULL;
    cSURFACE_MESH::cFACET::half_edge_circulator currHe = facet->HalfEdgesBegin();
    const cSURFACE_MESH::cFACET::half_edge_circulator lastHe = facet->HalfEdgesEnd();
    BOOL hasRingHe = false; //if there is a ring at at least one half edge

    for( ; currHe != lastHe ; currHe++) {
      cSURFACE_MESH::cFACET *currentAdjacentFacet = currHe->Opp()->Facet();
      if (currentAdjacentFacet == NULL){
        if (currHe->IsARing())
          hasRingHe = true;
        else
          return NULL; //only completely coincident facets are removed, not border facets
      }
      if      (adjacentFacet == NULL  && currentAdjacentFacet != NULL)
        adjacentFacet = currentAdjacentFacet;
      else if (adjacentFacet != currentAdjacentFacet && currentAdjacentFacet != NULL)
        return NULL;
    }

    //If some of the facet halfedges have rings, then check that
    //the computed adjacent facet is present in each ring
    if (hasRingHe && adjacentFacet != NULL){
      for(currHe = facet->HalfEdgesBegin(); currHe != lastHe ; currHe++) {

        if (currHe->IsARing()){
          cSURFACE_MESH::cHALF_EDGE *heInit = currHe.operator->();
          BOOL ringHasComputedFacet = false;

          for (cSURFACE_MESH::cHALF_EDGE *he = heInit->NextInRing();
                  he != heInit && !ringHasComputedFacet;
                  he = he->NextInRing()){
             if (he->Facet() == adjacentFacet)
               ringHasComputedFacet = true;
          }
          if (!ringHasComputedFacet)
            return NULL;
        }
      }
    }
    return adjacentFacet;
  }

 iFACET cSURFACE_MESH_CLEANUP::CoincidentFacetToBeDeleted(const cSURFACE_MESH::cFACET *facet1, const cSURFACE_MESH::cFACET *facet2) const
  {
    INT w1 = 0, w2 = 0;
    //for each facet weight is a sum of its half edge weights, where a half edge weight is
    //the number of half edges in its ring, except for the one belonging to the other facet,
    //with an opposing direction.
    //Label of the facet with the lower total weight is returned

    cSURFACE_MESH::cFACET::half_edge_circulator currHe1 = facet1->HalfEdgesBegin();
    cSURFACE_MESH::cFACET::half_edge_circulator lastHe1 = facet1->HalfEdgesEnd();

    for( ; currHe1 != lastHe1 ; currHe1++) {
      const cSURFACE_MESH::cHALF_EDGE *he1Init = currHe1.operator->();
      cSURFACE_MESH::cVERTEX *he1Vertex = he1Init->Vertex();
      for (cSURFACE_MESH::cHALF_EDGE *he1 = he1Init->NextInRing(); he1 != he1Init; he1 = he1->NextInRing()){
        if (he1->Facet() != NULL){
          if (he1->Facet() != facet2){
            if (he1->Vertex() == he1Vertex)
              w2++;//he may become opposing to he2
            else
              w1++;
          }
        }
      }
    }

    return w1 < w2 ? facet1->Index() : facet2->Index();;
  }

  //deletes degenerate facets
  //facet is degenerate if its smallest edge or smallest heigh is <= tol
  //if input tol is less than cLIMITS::Tolerance, it's replaced by this number
  BOOL cSURFACE_MESH_CLEANUP::DeleteDegenerateFacets()
  {
    INT largestFI = m_mesh->LargestFacetIndex();
    for( iFACET facetLabel = 0; facetLabel < largestFI; facetLabel++){
      DeleteFacetIfDegenerate(facetLabel);
    }
    return true;
  }

  BOOL cSURFACE_MESH_CLEANUP::DeleteFacetIfDegenerate(iFACET facetLabel)
  {
    cSURFACE_MESH::cFACET *facet = m_mesh->Facet(facetLabel);
    return DeleteFacetIfDegenerate(facet);
  }

  BOOL cSURFACE_MESH_CLEANUP::DeleteFacetIfDegenerate(cSURFACE_MESH::cFACET *facet)
  {
    if (facet->IsDeleted())
      return true;
    if (DeleteFacetWithDegenerateEdge(facet))
      return true;
    if (DeleteFacetIfNarrow(facet))
      return true;
    return true;
  }

  //deletes facet if it has a degenerate edge and returns true; if not, returns false
  BOOL cSURFACE_MESH_CLEANUP::DeleteFacetWithDegenerateEdge(cSURFACE_MESH::cFACET *facet)
  {
    const cSURFACE_MESH::cHALF_EDGE *degenerateHalfEdge = facet->HasDegenerateEdge(m_absoluteTol);
     if (degenerateHalfEdge != NULL){
       iVERTEX v1Label = degenerateHalfEdge->Tail()->Index();
       iVERTEX v2Label = degenerateHalfEdge->Head()->Index();
       cPOINT3 mergedPoint = (m_mesh->Vertex(v1Label)->Point() + m_mesh->Vertex(v2Label)->Point())/2.0;
       m_mesh->MergeVertices(v1Label, v2Label, v1Label < v2Label ? v1Label : v2Label, mergedPoint);
        return true;
     }
     else
       return false;
  }

  //facet is narrow if all of its vertices are within tol from one of its edges.
  //A narrow facet is deleted. The deletion is somewhat convoluted: The vertex furthest away
  //from the edge is projected onto the edge, and the projection point splits the edge
  //the diagonal connecting two points is shorter than tol, and the resulting two facets
  //are deleted as degenerate
  BOOL cSURFACE_MESH_CLEANUP::DeleteFacetIfNarrow(cSURFACE_MESH::cFACET *facet)
  {
    cSURFACE_MESH::cHALF_EDGE *narrowWidthHE = NULL;
    cSURFACE_MESH::cVERTEX *furthestVertex = NULL;
    iVERTEX edgeVertexLabel = INVALID_IVERTEX;
    cPOINT3 vertexProjectionOntoEdge;

    if (facet->IsNarrow(m_absoluteTol, &narrowWidthHE,
                                    &furthestVertex,
                                    &vertexProjectionOntoEdge)){

     INT ep = narrowWidthHE->Segment().IsEndPoint(vertexProjectionOntoEdge);
     if (ep == 0)
       edgeVertexLabel = narrowWidthHE->Tail()->Index();
     else if (ep == 1)
       edgeVertexLabel = narrowWidthHE->Head()->Index();
     else
       edgeVertexLabel = m_mesh->NewVertex(vertexProjectionOntoEdge);

     m_mesh->SplitEdge(edgeVertexLabel,
                                       narrowWidthHE->Tail()->Index(),
                                       narrowWidthHE->Head()->Index());

     //iFACET newFacetLabel = 
       m_mesh->InsertDiagonal(facet->Index(), furthestVertex->Index(),
			      edgeVertexLabel);
     cPOINT3 mergedPoint = (furthestVertex->Point() + vertexProjectionOntoEdge)/2;
     m_mesh->MergeVertices(furthestVertex->Index(), edgeVertexLabel, furthestVertex->Index(), mergedPoint);

     return true;
    }
    else
      return false;
  }

