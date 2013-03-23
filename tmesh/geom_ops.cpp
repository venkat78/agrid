#include "tmesh_def.hpp"

namespace tmesh {
  _TMESH_TMPL_TYPE
  VOID _TMESH_TMPL_DECL::GetFacetVertices(iFACET facetIndex, eCOORD coord, REAL coordVal, iVERTEX v[2])
  {

    cHALF_EDGE* facetHes[2] = { NULL, NULL };
    BOOL verticesAreNew[2] = { false, false };

    cHALF_EDGE *startingHe = Facet(facetIndex)->FindMinHalfEdge(coord);
    cHALF_EDGE *currHe = startingHe;

    INT i = 0;
    do {
      cPOINT3 tailPoint = currHe->Tail()->Point();
      cPOINT3 headPoint = currHe->Head()->Point();

      REAL heTailCoord = tailPoint[coord];
      REAL heHeadCoord = headPoint[coord];

      REAL diff1 = coordVal - heTailCoord;
      REAL diff2 = heHeadCoord - coordVal;

      if(fabs(diff2) < cLIMITS::Tolerance()) {
	currHe = currHe->Next();
	continue;
      }

      if(fabs(diff1) < cLIMITS::Tolerance())
	v[i++] = currHe->Tail()->Index();
      else if(diff1 * diff2 > 0.0) {
	cPOINT3 newPoint = ((tailPoint*diff2) + (headPoint*diff1))/(diff1+diff2);
	v[i++] = NewVertex(newPoint);
	facetHes[i-1] = currHe;
	verticesAreNew[i-1] = true;
      }

      if(i == 2) break;

      currHe = currHe->Next();
    } while( currHe != startingHe);

    if(verticesAreNew[0])
      SplitEdge(v[0], facetHes[0]->Tail()->Index(),
		facetHes[0]->Head()->Index());

    if(verticesAreNew[1])
      SplitEdge(v[1], facetHes[1]->Tail()->Index(),
		facetHes[1]->Head()->Index());
  }

  _TMESH_TMPL_TYPE
  iFACET _TMESH_TMPL_DECL::Clip(iFACET facetIndex, eCOORD coord,  REAL coordVal)
  {
    iVERTEX v[2] = {INVALID_IVERTEX, INVALID_IVERTEX };

    GetFacetVertices(facetIndex, coord, coordVal, v);

    if(v[0] == v[1] || v[0] == INVALID_IVERTEX || v[1] == INVALID_IVERTEX)
      return INVALID_IFACET;

    return InsertDiagonal(facetIndex, v[1], v[0]);
  }

  _TMESH_TMPL_TYPE
  VOID _TMESH_TMPL_DECL::Clip(iFACET facetIndex, eCOORD coord, REAL minVal, REAL maxVal, REAL step)
  {
    for (REAL val = minVal ;  val <= maxVal; val += step ) {
      Clip(facetIndex, coord, val);
    }
  }

  _TMESH_TMPL_TYPE
  iFACET _TMESH_TMPL_DECL::Clip(iFACET facetIndex, const cSEGMENT3 &cut)
  {
    cFACET* facet = Facet(facetIndex);

    if(facet == NULL || facet->IsDeleted())
      return INVALID_IFACET;

    iVERTEX cutVertices[2] = { INVALID_IVERTEX, INVALID_IVERTEX };
    BOOL insertCutVertices[2] = { false, false };
    cPOINT3 points[2];
    cEDGE splitEdges[2];

    const cPOINT3 &cutTail = cut.Source();
    const cPOINT3 &cutHead = cut.Target();

    typename cFACET::half_edge_circulator currFacetHe = facet->HalfEdgesBegin();
    typename cFACET::half_edge_circulator lastFacetHe = facet->HalfEdgesEnd();

    INT numCuts = 0;
    for( ; currFacetHe != lastFacetHe ; currFacetHe++) {
      cSEGMENT3 heSegment = currFacetHe->Segment();

      if(heSegment.HasOn(cutTail)) {
        if(!(heSegment.Target() == cutTail)) {
          if(heSegment.Source() == cutTail) {
            cutVertices[numCuts++] = currFacetHe->Tail()->Index();
          }
          else {
            insertCutVertices[numCuts] = true;

            splitEdges[numCuts].v1 = currFacetHe->Tail()->Index();
            splitEdges[numCuts].v2 = currFacetHe->Head()->Index();

            points[numCuts] = cutTail;
            numCuts++;
          }
        }
      }

      if(heSegment.HasOn(cutHead)) {
        if(!(heSegment.Target() == cutHead)) {
          if(heSegment.Source() == cutHead) {
            cutVertices[numCuts++] = currFacetHe->Tail()->Index();
          }
          else {
            insertCutVertices[numCuts] = true;

            splitEdges[numCuts].v1 = currFacetHe->Tail()->Index();
            splitEdges[numCuts].v2 = currFacetHe->Head()->Index();

            points[numCuts] = cutHead;
            numCuts++;
          }
        }
      }

      if(numCuts == 2)
        break;
    }

    //Cuts found.
    assert(numCuts == 2);
    if(numCuts != 2)
      return INVALID_IFACET;

    if(insertCutVertices[0]) {
      cutVertices[0] = NewVertex(points[0]);
      SplitEdge(cutVertices[0], splitEdges[0].v1, splitEdges[0].v2);
    }

    if(insertCutVertices[1]) {
      cutVertices[1] = NewVertex(points[1]);
      SplitEdge(cutVertices[1], splitEdges[1].v1, splitEdges[1].v2);
    }

    return InsertDiagonal(facetIndex, cutVertices[0], cutVertices[1]);
  }

  _TMESH_TMPL_TYPE
  typename _TMESH_TMPL_DECL::cHALF_EDGE*  _TMESH_TMPL_DECL::MostConcaveEdge()
   {
     facet_iterator currFacet = FacetsBegin();
     facet_iterator lastFacet = FacetsEnd();

     REAL maxConcavity = -cLIMITS::Tolerance();
     cHALF_EDGE *maxConcaveHe = NULL;
/*
     for( ; currFacet != lastFacet ; currFacet++) {
       INT currFacetLabel = currFacet->Index();
       cVECTOR3 currFacetNormal = currFacet->UnitNormal();

       typename cFACET::half_edge_circulator currHe = currFacet->HalfEdgesBegin();
       typename cFACET::half_edge_circulator lastHe = currFacet->HalfEdgesEnd();

       for( ; currHe != lastHe ; currHe++ ){
         cHALF_EDGE *oppHe = currHe->Opp();
         if (oppHe != NULL){
           iFACET oppFacetLabel = oppHe->Facet()->Index();
           if (oppFacetLabel < currFacetLabel){
             REAL concavity = currHe->ConcavityMeasure(currFacetNormal);
             if (concavity > cLIMITS::Tolerance()){
               cVECTOR3 oppFacetNormal = oppHe->Facet()->UnitNormal();
               REAL oppConcavity = oppHe->ConcavityMeasure(oppFacetNormal);
               if (oppConcavity < concavity)
                 concavity = oppConcavity;
               if (concavity > maxConcavity){
                 maxConcavity = concavity;
                 maxConcaveHe = *currHe;
               }
             //the smaller of the two numbers is used because if one of the facetts is very small
             //this will eliminate this half edge from being the most concave
             }
           }
         }
       }
     }
*/
     return maxConcaveHe;
   }
}//namespace
