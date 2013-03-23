#include "tmesh_geom_queries.hpp"

namespace tmesh {
//   /**
//    * Planarity queries
//    */
//   template <typename _Mesh_Type>
//   BOOL tMESH_GEOM_QUERIES<_Mesh_Type>::AreCoplanar(iFACET facetIndex1, iFACET facetIndex2)
//   {

//     return false;
//   }

  /**
   * Check if fan-triangulation of the facet intersects with the segment.
   */
  template <typename _Mesh_Type>
  BOOL tMESH_GEOM_QUERIES<_Mesh_Type>::DoIntersect(iFACET facetIndex, cSEGMENT3 &segment)
  {
    cFACET* facet = m_mesh->Facet(facetIndex);

    ASSERT(!facet->IsDeleted());
    cHALF_EDGE *firstHe = facet->HalfEdge();
    cHALF_EDGE *secondHe = firstHe->Next();
    cHALF_EDGE *thirdHe = secondHe->Next();

    do {
    	//skip degenerate triangles
//      while (doDebug && PointsCollinear(firstHe->Tail()->Point(), secondHe->Tail()->Point(), thirdHe->Tail()->Point())){
//    	thirdHe = thirdHe->Next();
//      }
      cTRIANGLE3 triangle(firstHe->Tail()->Point(),
			  secondHe->Tail()->Point(),
			  thirdHe->Tail()->Point());
      iVERTEX v0 = firstHe->Tail()->Index();
      iVERTEX v1 = secondHe->Tail()->Index();
      iVERTEX v2 = thirdHe->Tail()->Index();
// 	  if (doDebug){
// 		printf("v0 = %d; v1 = %d; v2 = %d\n", v0, v1, v2);
// //		triangle.Print();
// 	  }
      if(!triangle.IsDegenerate() &&
	 geom::DoIntersect(triangle, segment) == SUCCESS){
//     	  if (doDebug){
//     		printf("v0 = %d; v1 = %d; v2 = %d\n", v0, v1, v2);
//     		triangle.Print();
//     		segment.Print();
//     		geom::DoIntersect(triangle, segment, true);
//     	  }
    	  return true;
      }
      secondHe = thirdHe;
      thirdHe = thirdHe->Next();
    } while (thirdHe != firstHe);

    return false;
  }

  template <typename _Mesh_Type>
  VOID tMESH_GEOM_QUERIES<_Mesh_Type>::FindCommonVertices(iFACET f1Index, iFACET f2Index,
							  cVERTEX *v[1024], INT &numCommonVertices)
  {
    cFACET *f1 = m_mesh->Facet(f1Index);
    cVERTEX *vertices[1024];

    INT numVertices = 0;

    typename cFACET::vertex_circulator currVertex = f1->VerticesBegin();
    typename cFACET::vertex_circulator lastVertex = f1->VerticesEnd();

    for( ; currVertex != lastVertex ; currVertex++ ) {
      vertices[numVertices++] = currVertex.operator->();
    }

    cFACET *f2 = m_mesh->Facet(f2Index);

    currVertex = f2->VerticesBegin();
    lastVertex = f2->VerticesEnd();

    for( ; currVertex != lastVertex ; currVertex++ ) {
      if(utils::ValIsInTable(currVertex.operator->(), vertices, numVertices)) {
	v[numCommonVertices++] = currVertex.operator->();
      }
    }
  }

  template <typename _Mesh_Type>
  BOOL tMESH_GEOM_QUERIES<_Mesh_Type>::DoIntersect(iFACET srcIndex, iFACET targetIndex,
						   cVERTEX *v[1024], INT numCommonVertices)
  {
    cFACET *f1 = m_mesh->Facet(srcIndex);

    typename cFACET::half_edge_circulator currHe = f1->HalfEdgesBegin();
    typename cFACET::half_edge_circulator lastHe = f1->HalfEdgesEnd();

    for( ; currHe != lastHe ; currHe++ ) {
      BOOL tailFound = ValIsInTable(currHe->Tail(), v, numCommonVertices);
      BOOL headFound = ValIsInTable(currHe->Head(), v, numCommonVertices);

      cSEGMENT3 segment = currHe->Segment();
      if(tailFound && !headFound) {
    	cSEGMENT3 newSeg(segment.Evaluate(0.01), segment.Target());
    	if(DoIntersect(targetIndex, newSeg))
    	  return true;
      }
      else if(!tailFound && headFound) {
    	cSEGMENT3 newSeg(segment.Source(), segment.Evaluate(0.99));
    	if(DoIntersect(targetIndex, newSeg))
    	  return true;

      }
      else if(!headFound && !tailFound) {
    	if(DoIntersect(targetIndex, segment))
    	  return true;
      }
    }

    return false;
  }

  template <typename _Mesh_Type>
  BOOL tMESH_GEOM_QUERIES<_Mesh_Type>::DoIntersect(iFACET f1Index,
						   iFACET f2Index)
  {
    cVERTEX *v[1024]; INT numCommonVertices = 0;

    FindCommonVertices(f1Index, f2Index, v, numCommonVertices);

    if(DoIntersect(f1Index, f2Index, v, numCommonVertices) ||
       DoIntersect(f2Index, f1Index, v, numCommonVertices))
      return true;

    return false;
  }

  template <typename _Mesh_Type>
  BOOL tMESH_GEOM_QUERIES<_Mesh_Type>::IntersectingEdges(iFACET srcIndex, iFACET targetIndex,
							 cVERTEX *v[1024], INT numCommonVertices,
							 cEDGE_MARKS &edges, BOOL doDebug)
  {
    cFACET *f1 = m_mesh->Facet(srcIndex);
    if (doDebug){
      f1->Print();
      m_mesh->Facet(targetIndex)->Print();
      printf("f1Area = %.12lf; f2Area = %.12lf\n", f1->Area(), m_mesh->Facet(targetIndex)->Area());
    }

    typename cFACET::half_edge_circulator currHe = f1->HalfEdgesBegin();
    typename cFACET::half_edge_circulator lastHe = f1->HalfEdgesEnd();

    BOOL retVal = false;
    INT count = 0; //for debugging
    for( ; currHe != lastHe ; currHe++ ) {
      sVERTEX_PAIR edge(currHe->Tail()->Index(), currHe->Head()->Index());
      if(edges.find(edge) == edges.end()) {
    	if (doDebug)
    	  printf("count %d:\tv0 %d; v1 %d\n", count, currHe->Tail()->Index(), currHe->Head()->Index());
    	BOOL tailFound = ValIsInTable(currHe->Tail(), v, numCommonVertices);
    	BOOL headFound = ValIsInTable(currHe->Head(), v, numCommonVertices);

    	BOOL localRetVal = false;

    	cSEGMENT3 segment = currHe->Segment();
    	if(tailFound && !headFound) {
    	  cSEGMENT3 newSeg(segment.Evaluate(0.01), segment.Target());
    	  if(DoIntersect(targetIndex, newSeg)){
    		localRetVal = true;
    		if (doDebug)
    		  printf("count %d: case 1: currHe %d %d\n", count, currHe->Tail()->Index(), currHe->Head()->Index());
    	  }
    	}
    	else if(!tailFound && headFound) {
    	  cSEGMENT3 newSeg(segment.Source(), segment.Evaluate(0.99));
    	  if(DoIntersect(targetIndex, newSeg)){
      		localRetVal = true;
      		if (doDebug)
      		  printf("count %d: case 2: currHe %d %d\n", count, currHe->Tail()->Index(), currHe->Head()->Index());
      	  }
    	}
    	else if(!headFound && !tailFound) {
    	  if(DoIntersect(targetIndex, segment)){
      		localRetVal = true;
      		if (doDebug)
      		  printf("count %d: case 3: currHe %d %d\n", count, currHe->Tail()->Index(), currHe->Head()->Index());
      	  }
    	}

    	if(localRetVal) {
    	  retVal = localRetVal;
    	  edges[edge] = true;
    	}
      }
      count++;
    }

    return retVal;
  }

  template <typename _Mesh_Type>
  BOOL tMESH_GEOM_QUERIES<_Mesh_Type>::IntersectingEdges(iFACET f1Index, iFACET f2Index,
							 cEDGE_MARKS &edges, BOOL doDebug)
  {

    cVERTEX *v[1024]; INT numCommonVertices = 0;
    FindCommonVertices(f1Index, f2Index, v, numCommonVertices);
    BOOL retVal1 = IntersectingEdges(f1Index, f2Index, v, numCommonVertices, edges, doDebug);
    BOOL retVal2 = IntersectingEdges(f2Index, f1Index, v, numCommonVertices, edges, doDebug);

    if ((retVal1 || retVal2) && IsDebugOn()) {
      printf("IntersectingEdges: f1 = %d; f2 = %d\n", f1Index, f2Index);
    }
    return retVal1 || retVal2;
  }

}
