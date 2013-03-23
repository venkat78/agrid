#include "topo_operators.hpp"

namespace tmesh {
  template <typename _MESH_TYPE>
  VOID tTOPO_OPERATORS<_MESH_TYPE>::MakeEdgeRing(cHALF_EDGE* h1,
				     cHALF_EDGE* h2,
				     BOOL markAsNonManifold)
  {
    if(h1->IsARing()) {
      cHALF_EDGE* nextInRing = NULL;
      cHALF_EDGE* prevToH2 = NULL;

      if(!h1->IsNonManifold()) {
        m_nmEdges.insert(std::pair<cEDGE, cHALF_EDGE*>(cEDGE(h1), h1));

	nextInRing = h1;
        do {
          nextInRing->MarkAsNonManifold();
          nextInRing = nextInRing->NextInRing();
        } while(nextInRing != h1);
      }

      if(h1->OppIsNextInRing()) {
        nextInRing = h1->NextInRing()->NextInRing();
        prevToH2 = h1->NextInRing();
      }
      else {
        nextInRing = h1->NextInRing();
        prevToH2 = h1;
      }

      cHALF_EDGE* h2Opp = h2->Opp();
      h2Opp->NextInRing(nextInRing);
      prevToH2->NextInRing(h2);

      h2->MarkAsRing();
      h2Opp->MarkAsRing();

      h2->MarkOppLocInRing(true);
      h2Opp->MarkOppLocInRing(false);

      h2->MarkAsNonManifold();
      h2Opp->MarkAsNonManifold();
    }
    else {
      cHALF_EDGE* h1Opp = h1->Opp();
      cHALF_EDGE* h2Opp = h2->Opp();

      h1Opp->NextInRing(h2);
      h2Opp->NextInRing(h1);

      h1->MarkOppLocInRing(true);
      h2->MarkOppLocInRing(true);
      h1Opp->MarkOppLocInRing(false);
      h2Opp->MarkOppLocInRing(false);

      h1->MarkAsRing();
      h2->MarkAsRing();
      h1Opp->MarkAsRing();
      h2Opp->MarkAsRing();

      if(markAsNonManifold) {
        m_nmEdges.insert(std::pair<cEDGE, cHALF_EDGE*>(cEDGE(h1), h1));
        h1->MarkAsNonManifold();
        h2->MarkAsNonManifold();
        h1Opp->MarkAsNonManifold();
        h2Opp->MarkAsNonManifold();
      }
    }
  }

  template <typename _MESH_TYPE>
  VOID tTOPO_OPERATORS<_MESH_TYPE>::SplitManifoldEdge(cHALF_EDGE* he)
  {
    cVERTEX* v1 = he->Vertex();
    cHALF_EDGE* heOpp = he->Opp();

    cVERTEX* v2 = heOpp->Vertex();

    cHALF_EDGE* be1 = m_factory.HalfEdge();
    cHALF_EDGE* be2 = m_factory.HalfEdge();

    cHALF_EDGE* beIntoV1 = he->BorderEdgeIntoVertex();
    cHALF_EDGE* beIntoV2 = heOpp->BorderEdgeIntoVertex();

    cHALF_EDGE* beOutOfV1 = NULL;
    cHALF_EDGE* beOutOfV2 = NULL;

    if(beIntoV1)
      beOutOfV1 = beIntoV1->Next();

    if(beIntoV2)
      beOutOfV2 = beIntoV2->Next();

    be1->Vertex(v2);
    be2->Vertex(v1);

    //Rearranging border edges around v1.
    if(beIntoV1) {
      be1->Next(beOutOfV1);
      beIntoV1->Next(be2);
    }
    else
      be1->Next(be2);

    //Rearranging border edges around v2.
    if(beIntoV2) {
      be2->Next(beOutOfV2);
      beIntoV2->Next(be1);
    }
    else
      be2->Next(be1);

    //Introducing be1 into edge ring.
    he->NextInRing(be1);
    be1->NextInRing(heOpp);
    he->MarkOppLocInRing(true);
    be1->MarkOppLocInRing(false);

    //Introducing be2 into edge ring.
    heOpp->NextInRing(be2);
    be2->NextInRing(he);
    heOpp->MarkOppLocInRing(true);
    be2->MarkOppLocInRing(false);

    //Marking all half-edges as part of an edge ring.
    he->MarkAsRing();
    heOpp->MarkAsRing();
    be1->MarkAsRing();
    be2->MarkAsRing();

    //Have to introduce split vertices around v1 and v2.
    if(beIntoV1) {
      if(!he->HasIndex())
        v1->AddVertexUse(he);
      else
        v1->AddVertexUse(heOpp->Next());
    }

    if(beIntoV2) {
      if(!heOpp->HasIndex())
        v2->AddVertexUse(heOpp);
      else
        v2->AddVertexUse(he->Next());
    }
  }

  //   Removes he and heOpp from EdgeRing
  template <typename _MESH_TYPE>
  VOID tTOPO_OPERATORS<_MESH_TYPE>::RemoveFromEdgeRing(cHALF_EDGE *he)
  {
    ASSERT(he->IsARing() &&
	   he->IsNonManifold());

    INT numIncidentFacets = 0;
    cHALF_EDGE* heOpp = he->NextInRing();
    cHALF_EDGE* nextToHeOpp = heOpp->NextInRing();
    cHALF_EDGE* prevInRingToHe = NULL;

    cHALF_EDGE* loopingHe = nextToHeOpp;

    ASSERT(!nextToHeOpp->IsBorder());

    do {
      if(!loopingHe->IsBorder())
	numIncidentFacets++;

      prevInRingToHe=loopingHe;
      loopingHe = loopingHe->NextInRing();

    } while(loopingHe != he);

    ASSERT(prevInRingToHe->IsBorder());
    prevInRingToHe->NextInRing(nextToHeOpp);

    he->Opp(heOpp);
    heOpp->Opp(he);

    ASSERT(numIncidentFacets >= 2);

    if(numIncidentFacets == 2) {
      m_nmEdges.erase(cEDGE(he));

      cHALF_EDGE* firstFacetHe = nextToHeOpp;
      cHALF_EDGE* firstFacetHeOpp = nextToHeOpp->NextInRing();
      cHALF_EDGE* secondFacetHe = firstFacetHeOpp->NextInRing();
      cHALF_EDGE* secondFacetHeOpp = prevInRingToHe;

      //Unmark non-manifold bit on each of the half-edges.
      firstFacetHe->MarkAsNonManifold(false);
      firstFacetHeOpp->MarkAsNonManifold(false);
      secondFacetHe->MarkAsNonManifold(false);
      secondFacetHeOpp->MarkAsNonManifold(false);

      //The remaining half-edges become a two manifold.
      if((firstFacetHe->Vertex() == secondFacetHe->Next()->Vertex()) &&
	 (secondFacetHe->Vertex() == firstFacetHe->Next()->Vertex())) {
	CollapseEdgeRing(firstFacetHe);
      }
    }
    else {
      m_nmEdges.erase(cEDGE(he));
      m_nmEdges.insert(std::pair<cEDGE, cHALF_EDGE*>(cEDGE(nextToHeOpp), nextToHeOpp));
    }
  }

  template <typename _MESH_TYPE>
  VOID tTOPO_OPERATORS<_MESH_TYPE>::CollapseEdgeRing(cHALF_EDGE* firstHE)
  {
    if(!firstHE->IsARing())
      return ;

    cHALF_EDGE* otherHE = firstHE->NextInRing()->NextInRing();

    ASSERT(firstHE->Next()->Vertex() ==
	   otherHE->Vertex());

    cVERTEX* firstVertex = firstHE->Vertex();
    cVERTEX* otherVertex = otherHE->Vertex();

    cHALF_EDGE* firstHeOpp = firstHE->NextInRing();
    cHALF_EDGE* otherHeOpp = otherHE->NextInRing();

    cHALF_EDGE* firstHeOppPrev = firstHeOpp->Prev2();
    cHALF_EDGE* otherHeOppPrev = otherHeOpp->Prev2();

    ASSERT(firstHeOpp->Next()->Vertex() ==
	   otherHeOpp->Vertex());

    //Un-tangling the border-edges.
    if(firstHeOpp->Next() != otherHeOpp) {
      cVERTEX_USE vUse = firstVertex->Find(firstHE);
      otherHeOppPrev->Next(firstHeOpp->Next());
      firstVertex->RemoveVertexUse(vUse);
    }

    if(otherHeOpp->Next() != firstHeOpp) {
      cVERTEX_USE vUse = otherVertex->Find(otherHE);
      firstHeOppPrev->Next(otherHeOpp->Next());
      otherVertex->RemoveVertexUse(vUse);
    }

    //Adding border edges to free-edge list.
    m_factory.Return(firstHeOpp);
    m_factory.Return(otherHeOpp);

    //Setting the real half-edges to be opposite.
    firstHE->Opp(otherHE);
    otherHE->Opp(firstHE);
  }

  template <typename _MESH_TYPE>
  VOID tTOPO_OPERATORS<_MESH_TYPE>::BreakNmEdge(cHALF_EDGE *nmEdge)
  {
    BOOL foundHeToStart = false;
    cHALF_EDGE* currHE = nmEdge;
    do {
      if(!currHE->IsBorder() &&
         currHE->OppIsNextInRing()
         && !currHE->NextInRing()->IsBorder()) {
        nmEdge = currHE;
        foundHeToStart = true;
        break;
      }

      currHE = currHE->NextInRing();

    }while(currHE != nmEdge);

    //The half-edges are already untangled.
    if(!foundHeToStart)
      return;

    cHALF_EDGE* nextHE = nmEdge;
    cHALF_EDGE* prevHE = NULL;

    do {
      if(!nextHE->IsBorder() && nextHE->OppIsNextInRing()) {
        cHALF_EDGE* nextHeNextInRing = nextHE->NextInRing();
        if(!nextHeNextInRing->IsBorder()) {
          cHALF_EDGE* nextHeToBreak = nextHeNextInRing->NextInRing();
          BreakHePair(nextHE, nextHeToBreak);
          nextHE = nextHeToBreak;
        }
        else
          nextHE = nextHeNextInRing->NextInRing();
      }
      else if(nextHE->OppIsNextInRing()
              && nextHE->IsBorder()) {
        prevHE = nextHE->NextInRing();
        nextHE = prevHE->NextInRing();

        ASSERT(0);
        //This also can't happen
      }
      else{ //This shouldn't happen. but left for catching any errors
        //in the b-rep.
        ASSERT(!nextHE->OppIsNextInRing());
        ASSERT(!nextHE->IsBorder() &&
               !nextHE->NextInRing()->IsBorder());
      }
    } while(nextHE != nmEdge);

  }

  template <typename _MESH_TYPE>
  VOID tTOPO_OPERATORS<_MESH_TYPE>::BreakNmEdges()
  {
    typename cNM_EDGES::iterator currNmEdge = m_nmEdges.begin();
    typename cNM_EDGES::iterator lastNmEdge = m_nmEdges.end();

    for( ; currNmEdge != lastNmEdge ; currNmEdge++)
      BreakNmEdge(currNmEdge->second);
  }

  //shorthand for NextInRing
#define NIR(edge)				\
  edge ## NextInRing

//shorthand for prev_in_ring
#define PIR(edge)				\
  edge ## PrevInRing

  template <typename _MESH_TYPE>
  VOID tTOPO_OPERATORS<_MESH_TYPE>::MateHalfEdges(cHALF_EDGE *firstHe, cHALF_EDGE *secondHe)
  {
    //Checking that border edges don't get in.
    ASSERT(!firstHe->IsBorder() && !secondHe->IsBorder());

  //Checking that firstHe and secondHe are true opposites.
    ASSERT((firstHe->Vertex() == secondHe->Next()->Vertex()) &&
	   (firstHe->Next()->Vertex() == secondHe->Vertex()));

    cVERTEX* firstVertex = firstHe->Vertex();
    cVERTEX* secondVertex = secondHe->Vertex();

    cVERTEX_USE firstSplit = firstVertex->Find(firstHe);
    cVERTEX_USE secondSplit = secondVertex->Find(secondHe);

    BOOL removeSplitAtV1 = firstHe->IsReachable(secondHe, false) ? false : true;
    BOOL removeSplitAtV2 = secondHe->IsReachable(firstHe, false) ? false : true;

    if(removeSplitAtV1)
      ASSERT(!firstSplit.IsEmpty());
    
    if(removeSplitAtV2)
      ASSERT(!secondSplit.IsEmpty());

    cHALF_EDGE* firstHeOpp = firstHe->NextInRing();
    cHALF_EDGE* secondHeOpp = secondHe->NextInRing();

    cHALF_EDGE* PIR(secondHe) = secondHe->PrevInRing();

    cHALF_EDGE* NIR(firstHeOpp) = firstHeOpp->NextInRing();
    cHALF_EDGE* NIR(secondHeOpp) = secondHeOpp->NextInRing();

    //note the following 2 prev half_edges are border edges.
    cHALF_EDGE* firstHeOppPrev = firstHeOpp->Prev2();
    cHALF_EDGE* secondHeOppPrev = secondHeOpp->Prev2();

    //Adjusting ring around secondHe.
    if(NIR(secondHeOpp) == firstHe) {
      PIR(secondHe)->NextInRing(firstHe);
      secondHe->NextInRing(NIR(firstHeOpp));
    }
    else if(NIR(firstHeOpp) == secondHe)
      secondHe->NextInRing(NIR(secondHeOpp));
    else {
      PIR(secondHe)->NextInRing(NIR(secondHeOpp));
      secondHe->NextInRing(NIR(firstHeOpp));
    }

    //Now secondHe is free to mate with firstHe.
    firstHe->NextInRing(secondHe);
    firstHe->MarkOppLocInRing(true);
    secondHe->MarkOppLocInRing(false);

    //Now old opposite border edges of firstHe and secondHe
    //can be removed.
    if(firstHeOpp->Next() != secondHeOpp) {
      secondHeOppPrev->Next(firstHeOpp->Next());

      if(removeSplitAtV1)
	firstVertex->RemoveVertexUse(firstSplit);

    } //Now firstHeOpp is totally free.

    if(secondHeOpp->Next() != firstHeOpp) {
      firstHeOppPrev->Next(secondHeOpp->Next());

      if(removeSplitAtV2) {
	secondVertex->RemoveVertexUse(secondSplit);
      }
    } //Now secondHeOpp is totally free.

    m_factory.Return(firstHeOpp);
    m_factory.Return(secondHeOpp);
  }

#undef PIR //prev_in_ring
#undef NIR //next_in_ring

  template <typename _MESH_TYPE>
  VOID tTOPO_OPERATORS<_MESH_TYPE>::BreakHePair(cHALF_EDGE* hePairToBreak,
				    cHALF_EDGE* nextHeInRing)
  {
    ASSERT(nextHeInRing != NULL &&
	   !nextHeInRing->IsBorder());

  //Checking for true opp. he condition.
    ASSERT(hePairToBreak->Next()->Vertex() ==
	   hePairToBreak->NextInRing()->Vertex());

    cHALF_EDGE* he1 = hePairToBreak;
    cHALF_EDGE* he2 = hePairToBreak->NextInRing();

    cVERTEX* v1 = he1->Vertex();
    cVERTEX* v2 = he1->Next()->Vertex();

    cHALF_EDGE* be1 = m_factory.HalfEdge();
    cHALF_EDGE* be2 = m_factory.HalfEdge();

    be1->Vertex(v2);
    be2->Vertex(v1);

    cHALF_EDGE* beIntoV1 = he1->BorderEdgeIntoVertex();
    cHALF_EDGE* beIntoV2 = he2->BorderEdgeIntoVertex();

    cHALF_EDGE* beOutOfV2 = NULL;
    cHALF_EDGE* beOutOfV1 = NULL;

    if(beIntoV1)
      beOutOfV1 = beIntoV1->Next();

    if(beIntoV2)
      beOutOfV2 = beIntoV2->Next();

    if(beIntoV1) {
      be1->Next(beOutOfV1);
      beIntoV1->Next(be2);
    }
    else
      be1->Next(be2);

    //----------------------------------
    // Arrange border edges around v2.
    //----------------------------------
    if(beIntoV2) {
      be2->Next(beOutOfV2);
      beIntoV2->Next(be1);
    }
    else
      be2->Next(be1);

    //Introducing be1 into edge-ring.
    he1->NextInRing(be1);
    be1->NextInRing(he2);
    he1->MarkOppLocInRing(true);
    be1->MarkOppLocInRing(false);

    //Introducing be2 into edge-ring
    he2->NextInRing(be2);
    he2->MarkOppLocInRing(false);
    be2->NextInRing(nextHeInRing);
    he2->MarkOppLocInRing(true);
    be2->MarkOppLocInRing(false);

    //Marking bits of border edges.
    be1->MarkAsRing();
    be2->MarkAsRing();
    be1->MarkAsNonManifold();
    be2->MarkAsNonManifold();

    //Add vertex uses for  v1 and v2 if necessary.
    BOOL addVUseForV1 = (he1->IsReachable(he2, false) == true ?
			 false : true);
    if(addVUseForV1) {
      if(!he1->HasIndex())
	v1->AddVertexUse(he1);
      else
	v1->AddVertexUse(he2->Next());
    }

    //Splitting v2.
    BOOL addVUseForV2 = (he2->IsReachable(he1, false) == true ?
			 false : true);
    if(addVUseForV2) {
      if(!he2->HasIndex())
	v2->AddVertexUse(he2);
      else
	v2->AddVertexUse(he1->Next());
    }

    //post-condition.
    //Checking new border-edge configuration.
    ASSERT(he1->Vertex() == be1->Next()->Vertex());
    ASSERT(he2->Vertex() == be2->Next()->Vertex());

    //Checking whether the edge-ring is updated as expected
    ASSERT(he1->NextInRing()->NextInRing() == he2);
  }

  template <typename _MESH_TYPE>
  iFACET tTOPO_OPERATORS<_MESH_TYPE>::InsertDiagonal(iFACET facetIndex,
					 iVERTEX diagEndPoint1,
					 iVERTEX diagEndPoint2,
					 iFACET newFacetIndex)
  {
#ifdef DEBUG
    ASSERT(diagEndPoint1 != diagEndPoint2);
    ASSERT(facetIndex != newFacetIndex);
#endif //DEBUG

#ifdef DEBUG
    if(newFacetIndex != INVALID_IFACET)
      ASSERT(m_mesh.Facet(newFacetIndex)->IsDeleted());
#endif //DEBUG

    iVERTEX v1 = diagEndPoint1;
    iVERTEX v2 = diagEndPoint2;

    cFACET* f = m_mesh.Facet(facetIndex);

    cHALF_EDGE* facetHe = f->HalfEdge();

    cHALF_EDGE *v1He = NULL, *v2He = NULL,
      *v1HePrev = NULL, *v2HePrev = NULL;

    cHALF_EDGE* loopingHe = facetHe;

    cVERTEX* vertex1 = m_mesh.Vertex(v1);
    cVERTEX* vertex2 = m_mesh.Vertex(v2);

    INT count = 0;

    //Finding half-edges of facet incident on v1 and v2.
    do {
      cHALF_EDGE* nextHe = loopingHe->Next();
      if(nextHe->Vertex() == vertex1) {
        v1HePrev = loopingHe;
        v1He = nextHe;
        count++;
      }

      if(nextHe->Vertex() == vertex2) {
        v2HePrev = loopingHe;
        v2He = nextHe;
        count++;
      }

      if(count == 2)
        break;

      loopingHe = nextHe;
    } while(loopingHe != facetHe);

    ASSERT(count == 2);

    if(count != 2)
      return INVALID_IFACET;

    ASSERT(v1He->Next() != v2He ||
	   v2He->Next() != v1He);

    if(v1He->Next() == v2He ||
       v2He->Next() == v1He)
      return INVALID_IFACET;

    cHALF_EDGE* v1ToV2He = NULL;
    vertex1->Find(vertex2, &v1ToV2He, NULL);

    //Adjusting next pointers for each he in diagonal.
    cHALF_EDGE* newHePair = m_factory.CreateAPair(vertex1, vertex2);
    cHALF_EDGE* newHePairOpp = newHePair->Opp();

    newHePair->Next(v2He);
    newHePairOpp->Next(v1He);
    v2HePrev->Next(newHePairOpp);
    v1HePrev->Next(newHePair);

    if(v1ToV2He) {
      if(v1ToV2He->IsBorder())
	MakeEdgeRing(v1ToV2He->Opp(), newHePair, true);
      else
	MakeEdgeRing(v1ToV2He, newHePair, true);
    }

    newHePair->Facet(f);

    cFACET *newFacet = NULL;
    if(newFacetIndex == INVALID_IFACET) {
      newFacet = m_factory.NewFacet(*f);
#ifdef DEBUG
      newFacetIndex = newFacet->Index();
#endif //DEBUG
    }
    else {
      newFacet = m_factory.Facet(newFacetIndex);
      m_mesh.m_numDeletedFacets--;
    }

    newFacet->HalfEdge(newHePairOpp);
    f->HalfEdge(newHePair);

    loopingHe = newHePairOpp;

    do {
      loopingHe->Facet(newFacet);
      loopingHe = loopingHe->Next();
    } while(loopingHe != newHePairOpp);

#ifdef DEBUG
  VerifyFacet(facetIndex);
  VerifyVertex(diagEndPoint1);
  VerifyVertex(diagEndPoint2);
  VerifyFacet(newFacetIndex);
#endif //DEBUG

    return newFacet->Index();
  }

  template <typename _MESH_TYPE>
  BOOL tTOPO_OPERATORS<_MESH_TYPE>::RemoveDiagonal(iVERTEX diagEndPoint1, iVERTEX diagEndPoint2,
				       iFACET facetIndex1, iFACET facetIndex2,
				       iFACET facetIndexToBeDeleted)
  {
#ifdef DEBUG
    ASSERT(diagEndPoint1 != diagEndPoint2);
    ASSERT(facetIndex1 != facetIndex2);
#endif //DEBUG

    iVERTEX v1 = diagEndPoint1, v2 = diagEndPoint2;
    iFACET f1 = facetIndex1, f2 = facetIndex2;

    cVERTEX *vertex1 = m_factory.Vertex(v1), *vertex2 = m_factory.Vertex(v2);

    cHALF_EDGE* v1ToV2He = NULL;
    if(!vertex1->Find(vertex2, &v1ToV2He, NULL))
      return false;

    cFACET *facet1 = m_factory.Facet(f1), *facet2 = m_factory.Facet(f2);

    cHALF_EDGE* heInRing = v1ToV2He;
    cHALF_EDGE* heToBeRemoved = NULL;

    do {
      cFACET *heFacet = heInRing->Facet();

      if(facet1 == heFacet || facet2 == heFacet) {
	heToBeRemoved = heInRing;
	break;
      }

      heInRing = heInRing->NextInRing();
    }while (heInRing != v1ToV2He);

    cHALF_EDGE* heToBeRemovedOpp = heToBeRemoved->Opp();

    if(heToBeRemoved == NULL ||
       heToBeRemovedOpp->IsBorder())
      return false;


    if(heToBeRemoved->Facet() != facet1 &&
       heToBeRemoved->Facet() != facet2)
      return false;

    if(heToBeRemovedOpp->Facet() != facet1 &&
       heToBeRemovedOpp->Facet() != facet2)
      return false;

    cFACET *survivingFacet = ((facetIndex1 == facetIndexToBeDeleted) ?
			      facet2 : facet1);

    cFACET *vanishingFacet = ((facetIndex1 == facetIndexToBeDeleted) ?
			      facet1 : facet2);


    cHALF_EDGE* heToBeRemovedPrev = heToBeRemoved->Prev2();
    cHALF_EDGE* heToBeRemovedNext = heToBeRemoved->Next();

    cHALF_EDGE* heToBeRemovedOppNext = heToBeRemovedOpp->Next();
    cHALF_EDGE* heToBeRemovedOppPrev = heToBeRemovedOpp->Prev2();

    heToBeRemovedPrev->Next(heToBeRemovedOppNext);
    heToBeRemovedOppPrev->Next(heToBeRemovedNext);

    if(heToBeRemoved->IsFirstOnVertex()) {
      heToBeRemoved->Vertex()
	->ReplaceHalfEdge(heToBeRemoved, heToBeRemovedOppNext);
      heToBeRemoved->UnmarkFirstOnVertex();
      heToBeRemovedOppNext->MarkFirstOnVertex();
    }

    if(heToBeRemovedOpp->IsFirstOnVertex()) {
      heToBeRemovedOpp->Vertex()
	->ReplaceHalfEdge(heToBeRemovedOpp, heToBeRemovedNext);
      heToBeRemovedOpp->UnmarkFirstOnVertex();
      heToBeRemovedNext->MarkFirstOnVertex();
    }

    survivingFacet->HalfEdge(heToBeRemovedNext);

    if(heToBeRemoved->IsARing())
      RemoveFromEdgeRing(heToBeRemoved);

    cHALF_EDGE* loopingHe = heToBeRemovedNext;
    do {
      loopingHe->Facet(survivingFacet);
      loopingHe = loopingHe->Next();
    } while(loopingHe != heToBeRemovedNext);

    m_factory.Return(heToBeRemoved);
    m_factory.Return(heToBeRemovedOpp);

    vanishingFacet->HalfEdge(NULL);
    m_mesh.m_numDeletedFacets++;

#ifdef DEBUG
  VerifyVertex(diagEndPoint1);
  VerifyVertex(diagEndPoint2);

  if(!m_mesh.Facet(facetIndex1)->IsDeleted())
    VerifyFacet(facetIndex1);

  if(!m_mesh.Facet(facetIndex2)->IsDeleted())
    VerifyFacet(facetIndex2);
#endif //DEBUG

    return true;
  }

  template <typename _MESH_TYPE>
  BOOL tTOPO_OPERATORS<_MESH_TYPE>::SplitEdge(iVERTEX vertexToBeInserted,
				  iVERTEX edgeTailVtxIndex,
				  iVERTEX edgeHeadVtxIndex)
  {
#ifdef DEBUG
    ASSERT(edgeTailVtxIndex != edgeHeadVtxIndex);
    ASSERT(edgeTailVtxIndex != vertexToBeInserted);
    ASSERT(edgeHeadVtxIndex != vertexToBeInserted);
#endif //DEBUG

    iVERTEX newVertexIndex = vertexToBeInserted;
    cVERTEX*  newVertex = m_factory.Vertex(newVertexIndex);

    cVERTEX*  tailVertex = m_factory.Vertex(edgeTailVtxIndex);
    cVERTEX*  headVertex = m_factory.Vertex(edgeHeadVtxIndex);

    cHALF_EDGE* edge = NULL;
    if(!tailVertex->Find(headVertex, &edge, NULL))
      return false;

    //Need to revisit this --Bujji 02/24/2010.
    //Book keeping error.
    if(newVertex->IsDeleted())
      m_mesh.m_numDeletedVertices--;

    cHALF_EDGE* heInEdge = edge;

    BOOL isAnNmEdge = false;
    if(edge->IsNonManifold()) {
      isAnNmEdge = true;
      m_nmEdges.erase(cEDGE(edge));
    }

    cHALF_EDGE* newHalfEdges[ARRAY_SIZE];
    cHALF_EDGE* oldHalfEdges[ARRAY_SIZE];
    INT numNewHalfEdges = 0;

    //tying next, vertex and facet pointers
    //for new half-edges created.
    do {
      cHALF_EDGE* heInNewEdge = m_factory.HalfEdge();

      heInNewEdge->Vertex(newVertex);
      heInNewEdge->Facet(heInEdge->Facet());
      heInNewEdge->Next(heInEdge->Next());
      heInEdge->Next(heInNewEdge);

      newHalfEdges[numNewHalfEdges] = heInNewEdge;
      oldHalfEdges[numNewHalfEdges] = heInEdge;

      numNewHalfEdges++;

      heInEdge = heInEdge->NextInRing();
    } while(heInEdge != edge);

    ASSERT(numNewHalfEdges < TMP_ARRAY_SIZE);

    //Insert the two new nm-edges
    if(isAnNmEdge) {
      for(INT k = 0; k < numNewHalfEdges; k++) {
	if(!oldHalfEdges[k]->IsBorder()) {
	  m_nmEdges.insert(std::pair<cEDGE, cHALF_EDGE*>(cEDGE(oldHalfEdges[k]),
							 oldHalfEdges[k]));
	  m_nmEdges.insert(std::pair<cEDGE, cHALF_EDGE*>(cEDGE(newHalfEdges[k]),
							 newHalfEdges[k]));
	  break;
	}
      }
    }

    //Adjusting bits to locate the right opposite.
    heInEdge = edge;
    BOOL edgeBelongsToRing = edge->IsARing();

    for(INT i = 0; i < numNewHalfEdges; i++ ) {
      if(edgeBelongsToRing)
	newHalfEdges[i]->MarkAsRing();

      if(heInEdge->IsARing()) {
	if(heInEdge->OppIsNextInRing())
	  newHalfEdges[i]->MarkOppLocInRing(true);
	else
	  newHalfEdges[i]->MarkOppLocInRing(false);
      }

      if(heInEdge->IsNonManifold())
	newHalfEdges[i]->MarkAsNonManifold();

      heInEdge = heInEdge->NextInRing();
    }

    //Adjusting edge-rings.
    for( INT j1 = 0; j1 < numNewHalfEdges ; j1++) {
      INT j1PlusOne = (j1 + 1)%numNewHalfEdges;

      if(!AreOpposite(oldHalfEdges[j1], oldHalfEdges[j1PlusOne]) &&
	 !AreCoDirected(oldHalfEdges[j1], oldHalfEdges[j1PlusOne])) {
	newHalfEdges[j1]->NextInRing(oldHalfEdges[j1PlusOne]);
	oldHalfEdges[j1]->NextInRing(newHalfEdges[j1PlusOne]);
      }
      else {
	oldHalfEdges[j1]->NextInRing(oldHalfEdges[j1PlusOne]);
	newHalfEdges[j1]->NextInRing(newHalfEdges[j1PlusOne]);
      }
    }

    for(INT j = 0; j < numNewHalfEdges; j++) {
      if(!newHalfEdges[j]->IsBorder() &&
	 !newHalfEdges[j]->HasIndex()) {
        newVertex->AddVertexUse(newHalfEdges[j]);
      }
    }

#ifdef DEBUG
  if(isAnNmEdge) {
    for(INT r = 0 ; r < numNewHalfEdges ; r++) {
      if(!oldHalfEdges[r]->IsBorder()) {
        VerifyNmEdge(oldHalfEdges[r]);
        VerifyNmEdge(newHalfEdges[r]);
        break;
      }
    }
  }

  for(INT s = 0; s < numNewHalfEdges ; s++ ) {
    if(!newHalfEdges[s]->IsBorder())
      VerifyFacet(newHalfEdges[s]->Facet()->Index());
  }

  VerifyVertex(vertexToBeInserted);
  VerifyVertex(edgeTailVtxIndex);
  VerifyVertex(edgeHeadVtxIndex);
#endif //DEBUG

    return true;
  }

  //Inverse of split-edge.
  template <typename _MESH_TYPE>
  BOOL tTOPO_OPERATORS<_MESH_TYPE>::MergeEdgesAtVertex(iVERTEX vertexIndex)
  {
    cVERTEX *vertexToBeRemoved = m_mesh.Vertex(vertexIndex);

    ASSERT(vertexToBeRemoved != NULL);
    ASSERT(!vertexToBeRemoved->IsDeleted());

    if(vertexToBeRemoved == NULL || !vertexToBeRemoved->IsDeleted())
      return false;

    cHALF_EDGE* finalRing[ARRAY_SIZE];
    cHALF_EDGE* vanishingHes[ARRAY_SIZE];
    cHALF_EDGE* nextAndPrevHes[ARRAY_SIZE];

    INT numHalfEdges = 0;

    INT i = 0;

     cHALF_EDGE *incidentHe = (cHALF_EDGE*)vertexToBeRemoved->FirstVertexUse().HalfEdge();
     cHALF_EDGE *loopingHe = incidentHe;

    do {
      i = numHalfEdges;
      finalRing[numHalfEdges++] = loopingHe;

      if(finalRing[i]->Vertex() != vertexToBeRemoved) {
	vanishingHes[i] = finalRing[i]->Next();
	nextAndPrevHes[i] = vanishingHes[i]->Next();
      }
      else {
	vanishingHes[i] = finalRing[i]->Prev2();
	nextAndPrevHes[i] = vanishingHes[i]->Prev2();
      }

      loopingHe = loopingHe->NextInRing();
    } while(loopingHe != incidentHe );

    if(finalRing[0]->IsNonManifold()) {
      m_nmEdges.erase(cEDGE(finalRing[0]));
      m_nmEdges.erase(cEDGE(finalRing[0]->Prev2()));
    }


  //Removing the vertex to be removed.
    for( i=0 ; i < numHalfEdges; i++) {
      if(finalRing[i]->Vertex() != vertexToBeRemoved)
	finalRing[i]->Next(nextAndPrevHes[i]);
      else {
	//Managing index half-edges
	if(vanishingHes[i]->IsFirstOnVertex())
	  vanishingHes[i]->Vertex()->RemoveVertexUse(cVERTEX_USE(vanishingHes[i]));

	if(finalRing[i]->IsFirstOnVertex())
	  finalRing[i]->UnmarkFirstOnVertex();

	nextAndPrevHes[i]->Next(finalRing[i]);
	finalRing[i]->Vertex(vanishingHes[i]->Vertex());
      }
    }


  //Adding the edge to NM-edge table.
    if(finalRing[0]->IsNonManifold()) {
      for(INT n = 0; n < numHalfEdges ; n++) {
	if(!finalRing[n]->IsBorder()) {
	  m_nmEdges.insert(std::pair<cEDGE, cHALF_EDGE*>(cEDGE(finalRing[n]), finalRing[n]));
	  break;
	}
      }
    }

    for(INT j = 0; j < numHalfEdges ; j++) {
      //Making sure incident facets always have a valid
      //half-edge.
      if(!finalRing[j]->IsBorder()) {
	finalRing[j]->Facet()->HalfEdge(finalRing[j]);
	if(!finalRing[j]->HasIndex())
	  finalRing[j]->Vertex()->AddVertexUse(finalRing[j]);
      }
    }

    //Adding vanishing hes to free list.
    for(INT p = 0; p < numHalfEdges; p++)
      m_factory.Return(vanishingHes[p]);

    vertexToBeRemoved->MarkDeleted(true);
    m_mesh.m_numDeletedVertices++;

#ifdef DEBUG
    for(INT q = 0; q < numHalfEdges ; q++) {
      ASSERT(!finalRing[q]->IsDeleted());
    }

    for(INT r = 0; r < numHalfEdges ; r++ ) {
      VerifyHalfEdge(finalRing[r]);
    }

    VerifyVertex(finalRing[0]->Tail()->Index());
    VerifyVertex(finalRing[0]->Head()->Index());
#endif //DEBUG

  return true;

  }

  template <typename _MESH_TYPE>
  VOID tTOPO_OPERATORS<_MESH_TYPE>::VerifyVertex(iVERTEX vertexIndex)
  {
    cVERTEX* v = m_mesh.Vertex(vertexIndex);
    ASSERT(v->Verify());

    typename cVERTEX::vertex_use_iterator currVtxUse = v->VertexUsesBegin();
    typename cVERTEX::vertex_use_iterator lastVtxUse = v->VertexUsesEnd();

    for( ; currVtxUse != lastVtxUse ; currVtxUse++ ) {
      typename cVERTEX_USE::half_edge_circulator currHalfEdge = currVtxUse->HalfEdgesBegin();
      typename cVERTEX_USE::half_edge_circulator lastHalfEdge = currVtxUse->HalfEdgesEnd();

      for( ; currHalfEdge != lastHalfEdge ; currHalfEdge++ )
	currHalfEdge->Verify();
    }

    currVtxUse = v->VertexUsesBegin();
    lastVtxUse = v->VertexUsesEnd();

    for( ; currVtxUse != lastVtxUse ; currVtxUse++ ) {
      typename cVERTEX_USE::facet_circulator currFacet = currVtxUse->FacetsBegin();
      typename cVERTEX_USE::facet_circulator lastFacet = currVtxUse->FacetsEnd();

      for( ; currFacet != lastFacet ; currFacet++ )
	currFacet->Verify();
    }

  }

  template <typename _MESH_TYPE>
  VOID tTOPO_OPERATORS<_MESH_TYPE>::VerifyFacet(iFACET facetIndex)
  {
    cFACET *f = m_mesh.Facet(facetIndex);

    ASSERT(f->Verify());

    typename cFACET::half_edge_circulator currHe = f->HalfEdgesBegin();
    typename cFACET::half_edge_circulator lastHe = f->HalfEdgesEnd();

    for( ; currHe != lastHe ; currHe++ ) {
      currHe->Verify();
    }

    typename cFACET::vertex_circulator currVertex = f->VerticesBegin();
    typename cFACET::vertex_circulator lastVertex = f->VerticesEnd();

    for( ; currVertex != lastVertex ; currVertex++ ) {
      currVertex->Verify();
    }
  }

  template <typename _MESH_TYPE>
  BOOL tTOPO_OPERATORS<_MESH_TYPE>::VerifyNmEdge(cHALF_EDGE *nmEdge, BOOL checkNmBit)
  {
    ASSERT(!nmEdge->IsDeleted() && !nmEdge->IsBorder());
    if(nmEdge->IsDeleted() || nmEdge->IsBorder())
      return false;

    ASSERT(nmEdge->IsARing());
    if(!nmEdge->IsARing())
      return false;

    if(checkNmBit) {
      ASSERT(nmEdge->IsNonManifold());

      if(!nmEdge->IsNonManifold())
	return false;
    }

    cHALF_EDGE* loopingHe = nmEdge;

    do {
      ASSERT(AreOpposite(loopingHe, nmEdge) ||
	     AreCoDirected(loopingHe, nmEdge));

      if(!AreOpposite(loopingHe, nmEdge) &&
	 !AreCoDirected(loopingHe, nmEdge))
	return false;

      loopingHe = loopingHe->NextInRing();
    } while(loopingHe != nmEdge);

    return true;
  }

  template <typename _MESH_TYPE>
  VOID tTOPO_OPERATORS<_MESH_TYPE>::VerifyHalfEdge(cHALF_EDGE *he)
  {
    ASSERT(!he->IsDeleted());

    if(he->IsBorder())
      return;

    if(he->IsARing()) {
      if(he->IsNonManifold())
	VerifyNmEdge(he);
      else
	VerifyNmEdge(he, false);
    }
  }
}
