#include "add_facet.hpp"

namespace tmesh {
  template <typename _Mesh_Type>
  VOID tADD_FACET<_Mesh_Type>::CreateHalfEdgePair(sEDGE_INFO *edgeInfo,
						  cVERTEX *vertex1, cVERTEX *vertex2) {
    edgeInfo->facetHe = m_factory.CreateAPair(vertex1, vertex2);
    edgeInfo->borderHe = edgeInfo->facetHe->Opp();
    edgeInfo->isANewPair = true;
  }

  template <typename _Mesh_Type>
  iFACET tADD_FACET<_Mesh_Type>::Finish() {
    ASSERT(m_facetVertices.size() >= 3);
    if (m_facetVertices.size() <= 2)
      return INVALID_IFACET;

    m_facet = NULL;

    if (m_facetIndex == INVALID_IFACET)
      m_facet = m_factory.NewFacet();
    else {
      m_facet = m_factory.Facet(m_facetIndex);
      if (m_facet->IsDeleted())
	m_mesh.DecrementNumDeletedFacets(); //m_numDeletedFacets--;
    }

    for (INT i = 0; i < m_numVertices; i++) {
      INT iPlusOne = (i + 1) % m_numVertices;

      FindEdge(m_facetVertices[i], m_facetVertices[iPlusOne],
	       &m_edgeInfo[i]);
    }

    //Step 2.
    Construct();
    
#ifdef DEBUG
    m_topoOperators.VerifyFacet(m_facet->Index());

    cFACET::vertex_circulator currVtx = m_facet->VerticesBegin();
    cFACET::vertex_circulator lastVtx = m_facet->VerticesEnd();
    
    for ( ; currVtx != lastVtx ; currVtx++ )
      (*currVtx)->Verify();
#endif //DEBUG

    return m_facet->Index();
  }

  template <typename _Mesh_Type>
  VOID tADD_FACET<_Mesh_Type>::Construct() {
    m_facet->HalfEdge(m_edgeInfo[0].facetHe);

    for (INT i = 0; i < m_numVertices; i++) {
      INT iMinusOne = (m_numVertices + i - 1) % m_numVertices;
      StitchEdgesAtVertex(&m_edgeInfo[i], &m_edgeInfo[iMinusOne]);
    }
  }

  template <typename _Mesh_Type>
  VOID tADD_FACET<_Mesh_Type>::FindEdge(const iVERTEX vertexIndex1,
					const iVERTEX vertexIndex2, sEDGE_INFO *edgeInfo) {
    cVERTEX* vertex1 = m_factory.Vertex(vertexIndex1);
    cVERTEX* vertex2 = m_factory.Vertex(vertexIndex2);

    if (vertex1->NumVertexUses() == 0) {
      CreateHalfEdgePair(edgeInfo, vertex1, vertex2);
      edgeInfo->isFirstHeOnV = true;

      if (vertex1->IsDeleted())
	m_mesh.DecrementNumDeletedVertices();

      vertex1->AddVertexUse(edgeInfo->facetHe);
      return;
    }

    edgeInfo->isFirstHeOnV = false;
    BOOL canReachV2 = vertex1->Find(vertex2, &(edgeInfo->facetHe),
				    &(edgeInfo->vertexUse));

    if (!canReachV2)
      CreateHalfEdgePair(edgeInfo, vertex1, vertex2);
    else {
      cHALF_EDGE* facetHe = edgeInfo->facetHe;
      switch (facetHe->IsARing()) {
      case true:
	CreateHalfEdgePair(edgeInfo, vertex1, vertex2);

	if (facetHe->IsBorder())
	  facetHe = facetHe->Opp();

	m_topoOperators.MakeEdgeRing(facetHe, edgeInfo->facetHe, true);
	return;
      case false:
	switch (facetHe->IsBorder()) {
	case true:
	  edgeInfo->isANewPair = false;
	  return;
	case false:
	  if (!(facetHe->Opp())->IsBorder()) { //Non-manifold situation.
	    m_topoOperators.SplitManifoldEdge(facetHe);
	    CreateHalfEdgePair(edgeInfo, vertex1, vertex2);
	    m_topoOperators.MakeEdgeRing(facetHe, edgeInfo->facetHe,
					 true);
	  } else { //Mis-oriented set of facets.
	    CreateHalfEdgePair(edgeInfo, vertex1, vertex2);
	    m_topoOperators.MakeEdgeRing(facetHe, edgeInfo->facetHe,
					 false);
	  }
	  return;
	}
      }
    }
  }

  template <typename _Mesh_Type>
  VOID tADD_FACET<_Mesh_Type>::StitchEdgesAtVertex(sEDGE_INFO *edgeOutOfV,
						   sEDGE_INFO *edgeIntoV) {
    cHALF_EDGE* heIntoV = edgeIntoV->facetHe;
    cHALF_EDGE* heOutOfV = edgeOutOfV->facetHe;

    heOutOfV->Facet(m_facet);

    switch (edgeOutOfV->isFirstHeOnV) {
    case true:
      edgeOutOfV->borderHe->Next(edgeIntoV->borderHe);
      heIntoV->Next(heOutOfV);
      return;
    case false:{
      switch (edgeOutOfV->isANewPair) {
      case true:
	switch (edgeIntoV->isANewPair) {
	case true:
	  heOutOfV->Vertex()->AddVertexUse(heOutOfV);
	  edgeOutOfV->borderHe->Next(edgeIntoV->borderHe);
	  heIntoV->Next(heOutOfV);
	  return;
	case false:
	  edgeOutOfV->borderHe->Next(heIntoV->Next());
	  heIntoV->Next(heOutOfV);
	  return;
	}
      case false:
	switch (edgeIntoV->isANewPair) {
	case true: {
	  heOutOfV->Prev2()->Next(edgeIntoV->borderHe);
	  heIntoV->Next(heOutOfV);
	  return;
	}
	case false: {
	  if(heIntoV->Next() == heOutOfV)
	    return;
	  else {
	    heOutOfV->Prev2()->Next(heIntoV->Next());
	    heIntoV->Next(heOutOfV);
	    heOutOfV->Vertex()->RemoveVertexUse(edgeOutOfV->vertexUse);
	  }

	}
	}
      }
    }
    }
  }
}
