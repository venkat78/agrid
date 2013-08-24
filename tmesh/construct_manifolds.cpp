//#include "tmesh_def.hpp"
#include "manifold.hpp"
#include "construct_manifolds.hpp"
#include "edge.hpp"

namespace tmesh {
  /*
   * Returns 1 if a new entry is added to entries.
   * Returns 0 if he is add
   */
  template<typename _Mesh_Type>
  INT tCONSTRUCT_MANIFOLDS<_Mesh_Type>::AddToZipper(cHALF_EDGE *he, iMANIFOLD index, sMANIFOLD_ZIPPER *entries, INT numEntries) {
    for (INT i = 0; i < numEntries; i++) {
      if (entries[i].m_index == index) {
        entries[i].m_halfEdges[entries[i].m_numEntries] = he;
        entries[i].m_numEntries++;
        ASSERT(entries[i].m_numEntries < SMALL_ARRAY_SIZE);
        return 0;
      }
    }

    entries[numEntries].m_halfEdges[entries[numEntries].m_numEntries] = he;
    entries[numEntries].m_index = index;
    entries[numEntries].m_numEntries++;

    ASSERT(entries[numEntries].m_numEntries < SMALL_ARRAY_SIZE);
    return 1;
  }

  //Methods of cCONSTRUCT_MANIFOLDS.
  template<typename _Mesh_Type>
  INT tCONSTRUCT_MANIFOLDS<_Mesh_Type>::Finish() {
    INT retVal = Do(1);

    if (m_nmEdges.size() == 0)
      return retVal;

    if (CanMergeManifolds()) {
      MateNmEdges();
      m_mesh.ClearManifolds();
      return Do(2);
    }

    ASSERT(0);
  }

  template<typename _Mesh_Type>
  BOOL tCONSTRUCT_MANIFOLDS<_Mesh_Type>::CanMergeManifolds() {
    typename cNM_EDGES::iterator currNmEdge = m_nmEdges.begin();
    typename cNM_EDGES::iterator lastNmEdge = m_nmEdges.end();

    for (; currNmEdge != lastNmEdge; currNmEdge++) {
      cHALF_EDGE *nmEdge = currNmEdge->second;
      cHALF_EDGE *loopingHe = nmEdge;

      INT numOpenManifolds = 0;

      do {
        if (!loopingHe->IsBorder() && loopingHe->Opp()->IsBorder())
          numOpenManifolds++;

        if (numOpenManifolds >= 2)
          return true;

        loopingHe = loopingHe->NextInRing();
      } while (loopingHe != nmEdge);
    }

    return false;
  }

  template<typename _Mesh_Type>
  VOID tCONSTRUCT_MANIFOLDS<_Mesh_Type>::BuildQueue(BOOL nmFacetsAtEnd) {
    m_queue.clear();

    //Optimization when there are no non-manifold edges why even
    //bother looking for them.
    if (m_nmEdges.size() == 0)
      nmFacetsAtEnd = false;

    if (!nmFacetsAtEnd) {
      typename _Mesh_Type::facet_iterator currFacet = m_mesh.FacetsBegin();
      typename _Mesh_Type::facet_iterator lastFacet = m_mesh.FacetsEnd();

      for (; currFacet != lastFacet; currFacet++)
        m_queue.push_back(currFacet.operator->());

    } else {
      typename _Mesh_Type::facet_iterator currFacet = m_mesh.FacetsBegin();
      typename _Mesh_Type::facet_iterator lastFacet = m_mesh.FacetsEnd();

      std::vector<cFACET*> nonManifoldFacets;

      for (; currFacet != lastFacet; currFacet++) {
        if (!currFacet->HasNonManifoldEdge())
          m_queue.push_back(currFacet.operator->());
        else
          nonManifoldFacets.push_back(currFacet.operator->());
      }

      m_queue.insert(m_queue.end(), nonManifoldFacets.begin(), nonManifoldFacets.end());
    }
  }

  template<typename _Mesh_Type>
  VOID tCONSTRUCT_MANIFOLDS<_Mesh_Type>::WalkTwoManifoldEdge(cHALF_EDGE *facetHe, cFACET *facet, std::vector<BOOL> &facetsToBeFlipped,
                                                             INT &numBorderEdges, cMANIFOLD *manifold) {
    cHALF_EDGE* facetHeOpp = facetHe->Opp();

    if (!facetHeOpp->IsBorder()) {
      cFACET* nextFacet = facetHeOpp->Facet();

      if (nextFacet->ManifoldIndex() < 0) {
        nextFacet->ManifoldIndex(manifold->Index());
        m_stack.push(nextFacet);

        if (facetsToBeFlipped[facet->Index()])
          facetsToBeFlipped[nextFacet->Index()] = true;
      } else {
        //checking for MOBIUS strip condition.
        BOOL facetShouldFlip = facetsToBeFlipped[facet->Index()];
        BOOL nextFacetShouldFlip = facetsToBeFlipped[nextFacet->Index()];

        if ((facetShouldFlip && !nextFacetShouldFlip) || (!facetShouldFlip && nextFacetShouldFlip))
          manifold->Orientation(NON_ORIENTABLE);
      }
    }  //nextHE is a true 2-manifold
    else
      numBorderEdges++;
  }

  template<typename _Mesh_Type>
  VOID tCONSTRUCT_MANIFOLDS<_Mesh_Type>::WalkCodirectedEdge(cHALF_EDGE *facetHe, cFACET *facet, std::vector<BOOL> &facetsToBeFlipped,
                                                            cMANIFOLD *manifold) {
    cHALF_EDGE* facetHeOpp = (facetHe->NextInRing()->NextInRing());

    cFACET* nextFacet = facetHeOpp->Facet();

    if (nextFacet->ManifoldIndex() < 0) {
      m_stack.push(nextFacet);
      nextFacet->ManifoldIndex(manifold->Index());

      if (!facetsToBeFlipped[facet->Index()])
        facetsToBeFlipped[nextFacet->Index()] = true;
    } else {  //checking for MOBIUS strip condition..
      BOOL facetShouldFlip = facetsToBeFlipped[facet->Index()];
      BOOL nextFacetShouldFlip = facetsToBeFlipped[nextFacet->Index()];

      if ((nextFacetShouldFlip && facetShouldFlip) || (!nextFacetShouldFlip && !facetShouldFlip))
        manifold->Orientation(NON_ORIENTABLE);
    }
  }

  template<typename _Mesh_Type>
  eORIENTATION tCONSTRUCT_MANIFOLDS<_Mesh_Type>::Construct(cFACET *facet, std::vector<BOOL> &facetsToBeFlipped, sTRUE a) {
    ASSERT(m_stack.empty());

    cMANIFOLD* manifold = m_mesh.NewManifold();

    INT numFacets = 0;
    INT numBorderEdges = 0;

    m_stack.push(facet);
    BOOL hasMisorientedFacets = false;

    while (!m_stack.empty()) {
      facet = m_stack.top();
      m_stack.pop();
      facet->ManifoldIndex(manifold->Index());
      numFacets++;

      BOOL hasNonManifoldEdges = facet->HasNonManifoldEdge();

      typename cFACET::half_edge_circulator nextHe = facet->HalfEdgesBegin();
      typename cFACET::half_edge_circulator lastHe = facet->HalfEdgesEnd();

      for (; nextHe != lastHe; nextHe++) {
        cHALF_EDGE *facetHe = nextHe.operator->();
        if (!facetHe->IsNonManifold()) {
          if (!facetHe->IsARing()) {
            WalkTwoManifoldEdge(facetHe, facet, facetsToBeFlipped, numBorderEdges, manifold);
          } else {
            if (!hasNonManifoldEdges) {
              hasMisorientedFacets = true;
              WalkCodirectedEdge(facetHe, facet, facetsToBeFlipped, manifold);
            }
          }  //mis-oriented facets.
        }
      }
    }

    if (hasMisorientedFacets && !(manifold->Orientation() == NON_ORIENTABLE))
      manifold->Orientation(ORIENTABLE);

    //update numFacets and border edges in manifold.
    manifold->NumFacets(numFacets);
    manifold->NumBorderEdges(numBorderEdges);

    if (manifold->Orientation() == NON_ORIENTABLE)
      return NON_ORIENTABLE;
    else if (hasMisorientedFacets)
      return ORIENTABLE;

    manifold->Orientation(ORIENTED);
    return ORIENTED;
  }

  /*
   *  Doesn't flip facets.
   */
  template<typename _Mesh_Type>
  eORIENTATION tCONSTRUCT_MANIFOLDS<_Mesh_Type>::Construct(cFACET *facet, std::vector<BOOL> &facetsToBeFlipped, sFALSE a) {
    ASSERT(m_stack.empty());

    cMANIFOLD* manifold = m_mesh.NewManifold();

    INT numFacets = 0;
    INT numBorderEdges = 0;

    m_stack.push(facet);
    BOOL hasMisorientedFacets = false;

    while (!m_stack.empty()) {
      facet = m_stack.top();
      m_stack.pop();

      facet->ManifoldIndex(manifold->Index());

      numFacets++;

      typename cFACET::half_edge_circulator nextHe = facet->HalfEdgesBegin();
      typename cFACET::half_edge_circulator lastHe = facet->HalfEdgesEnd();

      for (; nextHe != lastHe; nextHe++) {
        cHALF_EDGE *facetHe = nextHe.operator->();
        if (!facetHe->IsNonManifold() && facetHe->IsARing()) {
          hasMisorientedFacets = true;
          WalkCodirectedEdge(facetHe, facet, facetsToBeFlipped, manifold);
        } else
          WalkTwoManifoldEdge(facetHe, facet, facetsToBeFlipped, numBorderEdges, manifold);
      }
    }

    //update numFacets and border edges in manifold.
    manifold->NumFacets(numFacets);
    manifold->NumBorderEdges(numBorderEdges);

    if (manifold->Orientation() != NON_ORIENTABLE && !hasMisorientedFacets)
      manifold->Orientation(ORIENTED);
    else
      manifold->Orientation(NON_ORIENTABLE);

    return manifold->Orientation();
  }

  template<typename _Mesh_Type>
  INT tCONSTRUCT_MANIFOLDS<_Mesh_Type>::RetVal() {
    typename _Mesh_Type::manifold_iterator currManifold = m_mesh.ManifoldsBegin();
    typename _Mesh_Type::manifold_iterator lastManifold = m_mesh.ManifoldsEnd();

    for (; currManifold != lastManifold; currManifold++)
      if (currManifold->Orientation() == NON_ORIENTABLE)
        return NON_ORIENTABLE_MANIFOLDS;

    return ORIENTED_MANIFOLDS;
  }

  template<typename _Mesh_Type>
  VOID tCONSTRUCT_MANIFOLDS<_Mesh_Type>::OrientManifolds(std::vector<BOOL> &facetsToBeFlipped) {
    typename cOBJECT_FACTORY::facet_iterator currFacet = m_factory.FacetsBegin();
    typename cOBJECT_FACTORY::facet_iterator lastFacet = m_factory.FacetsEnd();

    for (; currFacet != lastFacet; currFacet++) {
      if (!currFacet->IsDeleted() && facetsToBeFlipped[currFacet->Index()] && !m_mesh.Manifold(currFacet->ManifoldIndex())->IsNonOrientable()) {
        m_mesh.FlipFacet(currFacet->Index());
      }
    }

    typename _Mesh_Type::manifold_iterator currManifold = m_mesh.ManifoldsBegin();
    typename _Mesh_Type::manifold_iterator lastManifold = m_mesh.ManifoldsEnd();

    for (; currManifold != lastManifold; currManifold++)
      if (currManifold->Orientation() == ORIENTABLE)
        currManifold->Orientation(ORIENTED);
  }

  template<typename _Mesh_Type>
  VOID tCONSTRUCT_MANIFOLDS<_Mesh_Type>::StitchManifoldsAtNmEdge(cHALF_EDGE *nmEdge) {
    sMANIFOLD_ZIPPER zipper[ARRAY_SIZE];
    INT numManifolds = 0;

    memset(zipper, 0, sizeof(sMANIFOLD_ZIPPER) * ARRAY_SIZE);
    cHALF_EDGE *loopingHe = nmEdge;

    do {
      if (!loopingHe->IsBorder())
        numManifolds += AddToZipper(loopingHe, loopingHe->Facet()->ManifoldIndex(), zipper, numManifolds);

      loopingHe = loopingHe->NextInRing();
    } while (loopingHe != nmEdge);

    ASSERT(numManifolds < ARRAY_SIZE);

    for (INT i = 0; i < numManifolds; i++) {
      sMANIFOLD_ZIPPER *entry = &zipper[i];

      if (entry->m_numEntries == 1) {
        cMANIFOLD *manifold = m_mesh.Manifold(entry->m_index);
        manifold->NumBorderEdges(manifold->NumBorderEdges() + 1);
      } else if (entry->m_numEntries == 2) {
        cHALF_EDGE *he1 = entry->m_halfEdges[0];
        cHALF_EDGE *he2 = entry->m_halfEdges[0];

        if (AreOpposite(he1, he2)) {
          m_topoOperators.MateHalfEdges(he1, he2);
        } else
          m_mesh.Manifold(entry->m_index)->Orientation(NON_ORIENTABLE);
      }
    }
  }

  template<typename _Mesh_Type>
  VOID tCONSTRUCT_MANIFOLDS<_Mesh_Type>::StitchManifoldsAtNmEdges() {
    typename cNM_EDGES::iterator currNmEdge = m_nmEdges.begin();
    typename cNM_EDGES::iterator lastNmEdge = m_nmEdges.end();

    for (; currNmEdge != lastNmEdge; currNmEdge++) {
      StitchManifoldsAtNmEdge(currNmEdge->second);
    }
  }

  template<typename _Mesh_Type>
  VOID tCONSTRUCT_MANIFOLDS<_Mesh_Type>::MateNmEdges() {
    typename cNM_EDGES::iterator currNmEdge = m_nmEdges.begin();
    typename cNM_EDGES::iterator lastNmEdge = m_nmEdges.end();

    for (; currNmEdge != lastNmEdge; currNmEdge++) {
      tEDGE_RING<_Mesh_Type> edgeRing(currNmEdge->second, &m_mesh);

      edgeRing.MateHalfEdges();

      ASSERT(currNmEdge->second->Tail()->Verify());
      ASSERT(currNmEdge->second->Head()->Verify());
    }
  }

  template<typename _Mesh_Type>
  INT tCONSTRUCT_MANIFOLDS<_Mesh_Type>::Do(INT round) {
    if (round == 1)
      BuildQueue(true);
    else if (round == 2)
      BuildQueue(false);

    typename std::vector<cFACET*>::iterator currFacet = m_queue.begin();
    typename std::vector<cFACET*>::iterator lastFacet = m_queue.end();

    std::vector<BOOL> facetsToBeFlipped(m_mesh.LargestFacetIndex() + 1, false);

    for (; currFacet != lastFacet; currFacet++) {
      cFACET* facet = *currFacet;
      if (facet->ManifoldIndex() < 0) {
        if (round == 1)
          Construct(facet, facetsToBeFlipped, sTRUE());
        else
          Construct(facet, facetsToBeFlipped, sFALSE());
      }
    }

    if (round == 2) {

      //m_mesh.Verify();
      //LookForNonOrientableManifolds();
      return RetVal();
    }

    m_topoOperators.BreakNmEdges();

    OrientManifolds(facetsToBeFlipped);
    StitchManifoldsAtNmEdges();

    return RetVal();
  }
}
