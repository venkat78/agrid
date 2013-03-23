#include "delete_facet.hpp"

namespace tmesh {
  template <typename _Mesh_Type>
VOID tDELETE_FACET<_Mesh_Type>::Finish()
{
  cFACET* facetToBeDeleted = m_mesh.Facet(m_facetIndex);

  cHALF_EDGE* firstHeInFacet = const_cast<cHALF_EDGE*>(facetToBeDeleted->HalfEdge());
  cHALF_EDGE* nextHeInFacet = firstHeInFacet;

#ifdef DEBUG
  std::vector<iVERTEX> vertices;

  cHALF_EDGE *loopingHe = firstHeInFacet;

  do {
    vertices.push_back(loopingHe->Vertex()->Index());
    loopingHe = loopingHe->Next();
  } while (loopingHe != firstHeInFacet);
  
  for(INT i = 0 ; i < vertices.size() ; i++ ) {
    m_topoOperators.VerifyVertex(vertices[i]);
  }
#endif //DEBUG

    do {
      sEDGE_INFO currEdgeInfo;
      currEdgeInfo.edge = nextHeInFacet;

      if(nextHeInFacet->IsNonManifold()) {
        currEdgeInfo.edgeType = NON_MANIFOLD;
       	m_topoOperators.BreakNmEdge(nextHeInFacet);
      }

      else if(nextHeInFacet->IsARing() &&
              !nextHeInFacet->IsNonManifold())
        currEdgeInfo.edgeType = MISORIENTED;
      else {
        if(nextHeInFacet->Opp()->IsBorder())
          currEdgeInfo.edgeType = BOUNDARY;
        else
          currEdgeInfo.edgeType = TWO_MANIFOLD;
      }
      
      nextHeInFacet = nextHeInFacet->Next();
      m_edgeInfo.push_back(currEdgeInfo);
    } while(nextHeInFacet != firstHeInFacet);
    
#ifdef DEBUG
    for(INT i = 0 ; i < vertices.size() ; i++ ) {
      m_topoOperators.VerifyVertex(vertices[i]);
    }
#endif //DEBUG
    
    ModifyTopology();
    facetToBeDeleted->HalfEdge(NULL);
    m_mesh.IncrementNumDeletedFacets();//m_numDeletedFacets++;

#ifdef DEBUG
    for(INT i = 0 ; i < vertices.size() ; i++ ) {
      m_topoOperators.VerifyVertex(vertices[i]);
    }
#endif //DEBUG
}

template <typename _Mesh_Type>
VOID tDELETE_FACET<_Mesh_Type>::ModifyTopology()
{
  INT numEdges = m_edgeInfo.size();

  for(INT i = 0; i < numEdges ; i++ ) {
    INT iMinusOne = (i + numEdges - 1)%numEdges;
    ModifyBorderEdgesAtVertex(m_edgeInfo[i],
                              m_edgeInfo[iMinusOne]);

  }

  for(INT i = 0; i < numEdges ; i++ ) {
    INT iMinusOne = (i + numEdges - 1)%numEdges;
    ModifyVertexUses(m_edgeInfo[i], m_edgeInfo[iMinusOne]);
  }

  ModifyEdgeRings();
  ReturnFreeEdges();
}

  template <typename _Mesh_Type>
VOID tDELETE_FACET<_Mesh_Type>::ModifyVertexUses(sEDGE_INFO &edgeOut,
                                           sEDGE_INFO &edgeIn)
{
  cHALF_EDGE* heOutOfV = edgeOut.edge;
  cHALF_EDGE* heIntoV = edgeIn.edge;
  cVERTEX* vtx = heOutOfV->Vertex();

  eEDGE_TYPE edgeOutType = edgeOut.edgeType;
  eEDGE_TYPE edgeInType = edgeIn.edgeType;

  if(edgeOutType == MISORIENTED ||
     edgeOutType == NON_MANIFOLD)
    edgeOutType = BOUNDARY;

  if(edgeInType == MISORIENTED ||
     edgeInType == NON_MANIFOLD)
    edgeInType = BOUNDARY;


  //----------------------------------------------
  //Case a when both vertices into v become
  //border edges.
  //----------------------------------------------
  if(edgeOutType == TWO_MANIFOLD &&
     edgeInType == TWO_MANIFOLD) {
    cHALF_EDGE* beIntoVtx = heOutOfV->BorderEdgeIntoVertex();

    //vtx has to be split.
    if(beIntoVtx) {
      cHALF_EDGE* newHeForManifold = heOutOfV->Opp()->Next();
      if(heOutOfV->HasIndex()) {
        if(heOutOfV->IsFirstOnVertex()) {
          vtx->ReplaceHalfEdge(heOutOfV, newHeForManifold);
          heOutOfV->UnmarkFirstOnVertex();
          newHeForManifold->MarkFirstOnVertex();
        }
      }
      else
        vtx->AddVertexUse(newHeForManifold);

      cHALF_EDGE* heIntoVOpp = heIntoV->Opp();

      if(!heIntoVOpp->HasIndex()) {
        vtx->AddVertexUse(heIntoVOpp);
      }
    }
    else {
      if(heOutOfV->IsFirstOnVertex()) {
        cHALF_EDGE* newHeForManifold = heOutOfV->Opp()->Next();
        vtx->ReplaceHalfEdge(heOutOfV, newHeForManifold);
        heOutOfV->UnmarkFirstOnVertex();
        newHeForManifold->MarkFirstOnVertex();
      }
    }
  }

  else if(edgeOutType == TWO_MANIFOLD &&
          edgeInType == BOUNDARY) {
    if(heOutOfV->IsFirstOnVertex()) {
      cHALF_EDGE* newHeForManifold = heOutOfV->Opp()->Next();
      vtx->ReplaceHalfEdge(heOutOfV, newHeForManifold);
      heOutOfV->UnmarkFirstOnVertex();
      newHeForManifold->MarkFirstOnVertex();
    }
  }

  else if(edgeOutType == BOUNDARY &&
          edgeInType == TWO_MANIFOLD) {
    if(heOutOfV->IsFirstOnVertex()) {
      cHALF_EDGE* newHeForManifold = heIntoV->Opp();
      vtx->ReplaceHalfEdge(heOutOfV, newHeForManifold);
      heOutOfV->UnmarkFirstOnVertex();
      newHeForManifold->MarkFirstOnVertex();
    }
  }

  else {
    if(heOutOfV->IsFirstOnVertex()) {
      if(!vtx->IsNonManifold()) {
        vtx->MarkDeleted(true);
        m_mesh.IncrementNumDeletedVertices();
      }
      else
        vtx->RemoveVertexUse(cVERTEX_USE(heOutOfV));
    }
  }
}

template <typename _Mesh_Type>
VOID tDELETE_FACET<_Mesh_Type>::ModifyBorderEdgesAtVertex(sEDGE_INFO &edgeOut,
							  sEDGE_INFO &edgeIn)
{
  cHALF_EDGE* heIntoV = edgeIn.edge;
  cHALF_EDGE* heOutOfV = edgeOut.edge;

  eEDGE_TYPE edgeOutType = edgeOut.edgeType;
  eEDGE_TYPE edgeInType = edgeIn.edgeType;

  if(edgeOutType == MISORIENTED ||
     edgeOutType == NON_MANIFOLD)
    edgeOutType = BOUNDARY;

  if(edgeInType == MISORIENTED ||
     edgeInType == NON_MANIFOLD)
    edgeInType = BOUNDARY;

  
  if(edgeOutType == TWO_MANIFOLD &&
     edgeInType == TWO_MANIFOLD) {
    
    cHALF_EDGE* beIntoV = heOutOfV->BorderEdgeIntoVertex();
    
    if(beIntoV) {
      heIntoV->Next(beIntoV->Next());
      beIntoV->Next(heOutOfV);
    }
  }
  else if(edgeOutType == TWO_MANIFOLD &&
          edgeInType == BOUNDARY) {
    heIntoV->Opp()->Prev2()->Next(heOutOfV);
  }
  
  else if(edgeOutType == BOUNDARY &&
          edgeInType == TWO_MANIFOLD)
    heIntoV->Next(heOutOfV->Opp()->Next());
}
  
template <typename _Mesh_Type>
VOID tDELETE_FACET<_Mesh_Type>::ModifyEdgeRings()
{
  INT numEdges = m_edgeInfo.size();
  
  for(INT i = 0; i < numEdges; i++) {
    eEDGE_TYPE edgeType = m_edgeInfo[i].edgeType;
    
    if(edgeType == MISORIENTED) {
      cHALF_EDGE* misOrientedHe = m_edgeInfo[i].edge;
      
      cHALF_EDGE* misOrientedHeOpp = misOrientedHe->NextInRing();
      cHALF_EDGE* heThatRemains = misOrientedHeOpp->NextInRing();
      cHALF_EDGE* oppOfHeThatRemains = heThatRemains->NextInRing();
      
      oppOfHeThatRemains->Opp(heThatRemains);
      heThatRemains->Opp(oppOfHeThatRemains);
      
      misOrientedHe->Opp(misOrientedHeOpp);
      misOrientedHeOpp->Opp(misOrientedHe);
    }
    else if(edgeType == NON_MANIFOLD) {
      m_topoOperators.RemoveFromEdgeRing(m_edgeInfo[i].edge);
    }
  }
}

template <typename _Mesh_Type>
VOID tDELETE_FACET<_Mesh_Type>::ReturnFreeEdges()
{
  INT numEdges = m_edgeInfo.size();
  for(INT i = 0; i < numEdges ; i++) {
    if(m_edgeInfo[i].edgeType == BOUNDARY ||
       m_edgeInfo[i].edgeType == MISORIENTED ||
       m_edgeInfo[i].edgeType == NON_MANIFOLD) {
      cHALF_EDGE* h1 = m_edgeInfo[i].edge;
      cHALF_EDGE* h1Opp = h1->Opp();
      m_factory.Return(h1);
      m_factory.Return(h1Opp);
    }
    else {
      cHALF_EDGE* newBorderEdge = m_edgeInfo[i].edge;
      newBorderEdge->Facet(NULL);
    }
  }
}
}

