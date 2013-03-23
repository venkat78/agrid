#include "cut_cells_boolean.hpp"

namespace agrid_boolean {
  VOID cCUT_CELLS_BOOLEAN::Init()
  {
    BoxToMesh(m_grayCell->Box(), m_clay);

    cCUT_CELL_CLAY::facet_iterator currFacet = m_clay.FacetsBegin();
    cCUT_CELL_CLAY::facet_iterator lastFacet = m_clay.FacetsEnd();
    
    for( ; currFacet != lastFacet ; currFacet++ ) {
      currFacet->Color(WHITE);
      m_octree.Insert(currFacet->Index());
    }
  }

  VOID cCUT_CELLS_BOOLEAN::Imprint(cCUT_CELL *cutCell)
  {
    static INT rr=0;
    cCUT_CELL::half_edge_iterator currHe = cutCell->HalfEdgesBegin();
    cCUT_CELL::half_edge_iterator lastHe = cutCell->HalfEdgesEnd();

    for( ; currHe != lastHe ; currHe++ ) {
      if(currHe->IsBorder()) {
	
	//Collect facets that intersect the segment.
	typedef cOCTREE::cLEAF cLEAF;
	std::vector<cLEAF*> leaves;
	
	cSEGMENT3 segment = currHe->Segment();
	m_octree.Crosses(segment, leaves);
	
	INT i = 0, n = leaves.size(), numClippableFacets = 0;
	iFACET facetsToBeClipped[1024];
	tMESH_GEOM_QUERIES<cCUT_CELL_CLAY> geomQueries(&m_clay);
	
	for( ; i < n ; i++ ) {
	  cLEAF* leaf = leaves[i];
	  cOCTREE::leaf_element_iterator currElem = leaf->begin();
	  cOCTREE::leaf_element_iterator lastElem = leaf->end();
	  
	  for( ; currElem != lastElem ; currElem++ ) {	
	    if(!ValIsInTable<iFACET>(*currElem, facetsToBeClipped, numClippableFacets)) {
	      
	      cPOINT3 point = m_clay.Facet(*currElem)->MeanPoint();
	      cVECTOR3 normal = m_clay.Facet(*currElem)->UnitNormal();
	      
	      cPLANE3 plane(point, normal);
	      cSEGMENT3 smallSeg(segment.Evaluate(0.01), segment.Evaluate(0.99));
	      
	      if(plane.HasOn(segment.Source()) && 
		 plane.HasOn(segment.Target()) &&
		 geomQueries.DoIntersect(*currElem, smallSeg))
		facetsToBeClipped[numClippableFacets++] = *currElem;
	    }
	  }
	}

	iFACET newFacets[1024];
	INT numNewFacets = 0;
    
	for( i = 0; i < numClippableFacets ; i++ ) {
	  m_octree.Delete(facetsToBeClipped[i]);
	  
	  tMESH_CLIPPER<cCUT_CELL_CLAY> clipper(&m_clay);
	  iFACET newIndex = clipper.Clip(facetsToBeClipped[i], segment);
 
	  if(!ValIsInTable(facetsToBeClipped[i], newFacets, numNewFacets)) {
	    newFacets[numNewFacets++] = facetsToBeClipped[i];
	  }
	  
	  if(newIndex != INVALID_IFACET && 
	     !ValIsInTable(newIndex, newFacets, numNewFacets)) {
	    newFacets[numNewFacets++] = newIndex;
	  }
	}

	for( i = 0; i < numNewFacets ; i++ ) {
	  m_octree.Insert(newFacets[i]);	  
	}

// 	if(m_grayCell->Index() == iCELL_INDEX(21, 19, 19)) {
// 	  char outputFile[64];
// 	  printf("m_clay %d vertices, %d facets \n", m_clay.NumVertices(), m_clay.NumFacets());

// 	  sprintf(outputFile, "boolean_imprint_%d.off\0", rr++);
// 	  sCUT_CELL_CLAY_FILTER filter(&m_clay);
// 	  ExportToOff<cCUT_CELL_CLAY, sCUT_CELL_CLAY_FILTER>(outputFile, m_clay, filter);
// 	}
      }
    }
  }

  VOID cCUT_CELLS_BOOLEAN::RemoveBlackFaces()
  {
    iFACET largestFacetIndex = m_clay.LargestFacetIndex();

    for(INT i = 0; i <= largestFacetIndex ; i++ )
      if(m_clay.Facet(i)->Color() == BLACK)
	m_clay.DeleteFacet(i);
  }
  

  /*
   * This method extracts pieces of cutcells from m_clay
   */
  BOOL cCUT_CELLS_BOOLEAN::ExtractCutCell(INT facetIndex, std::vector<bool> &visitedFacets)
  {
    std::vector<iFACET> cutCellFacets;
    std::vector<bool> visitedNeighbors(m_clay.LargestFacetIndex() + 1, false);

    cCUT_CELL_CLAY::cFACET* facet = m_clay.Facet(facetIndex);
    
    INT currIndex = 0;
    cutCellFacets.push_back(facet->Index());

    while(currIndex < cutCellFacets.size()) {
      facet = m_clay.Facet(cutCellFacets[currIndex]);
      if(!visitedFacets[facet->Index()]) {
	visitedNeighbors[facet->Index()] = true;
	visitedFacets[facet->Index()] = true;
	
	cCUT_CELL_CLAY::cHALF_EDGE *halfEdge = facet->HalfEdge();
	cCUT_CELL_CLAY::cHALF_EDGE *loopHe = halfEdge;
	
	do {
	  cCUT_CELL_CLAY::cHALF_EDGE *oppHe = loopHe->Opp();
	  if(!loopHe->IsBorder() && !oppHe->IsBorder()) {
	    cCUT_CELL_CLAY::cFACET *oppFacet = oppHe->Facet();
	    if(!visitedNeighbors[oppFacet->Index()]) {
	      cutCellFacets.push_back(oppFacet->Index());
	      visitedNeighbors[oppFacet->Index()] = true;
	    }
	  }
	  
	  loopHe = loopHe->Next();
	} while (loopHe != halfEdge);
      }
      currIndex++;
    }

    //Fix for bug during booleans 9, 1, 17 in workspace 24g.1
    if(cutCellFacets.size() > 1) { 
      cCUT_CELL *newCutCell = m_grayCell->NewCutCell();
      MeshFromFacets(cutCellFacets, m_clay, *newCutCell);
    }

    return true;    
  }

  BOOL cCUT_CELLS_BOOLEAN::ExtractCutCells()
  {
    BOOL retVal = true;
    
    std::vector<bool> visitedFacets(m_clay.LargestFacetIndex() + 1, false);
    for(INT i = 0; i < m_clay.LargestFacetIndex() + 1 ; i++ ) {
      if(!m_clay.Facet(i)->IsDeleted() && !visitedFacets[i]) {
	if(!ExtractCutCell(i, visitedFacets))
	  retVal = false;
      }
    }

    return retVal;
  }

  VOID cCUT_CELLS_BOOLEAN::FindCommon()
  {
    RemoveBlackFaces();
    ExtractCutCells();
  }
  
  VOID cCUT_CELLS_BOOLEAN::RemoveTinyEdge(iVERTEX v1, iVERTEX v2, BOOL moveToMidPoint)
  {
    cPOINT3 newPoint;
    
    if(moveToMidPoint)
      newPoint = (m_clay.Vertex(v1)->Point() + m_clay.Vertex(v2)->Point()) * 0.5;
    else
      newPoint = m_clay.Vertex(v1)->Point();
    
    m_clay.MergeVertices(v1, v2, v1, newPoint);
  }

  VOID cCUT_CELLS_BOOLEAN::RemoveEdge(sEDGE edge)
  {
    iVERTEX v1 = edge.first, v2 = edge.second;
    
    if(m_clay.Vertex(v1)->IsDeleted() || 
       m_clay.Vertex(v2)->IsDeleted())
      return;

    RemoveTinyEdge(v1, v2, true);
  }

  VOID cCUT_CELLS_BOOLEAN::RemoveEdgeWithVertexOnLatticeEdge(sEDGE edge, INT latticeEdge2)
  {
    iVERTEX v1 = edge.first, v2 = edge.second;
    
    if(m_clay.Vertex(v1)->IsDeleted() || 
       m_clay.Vertex(v2)->IsDeleted())
      return; 
        
    //v1 will always be the vertex incident on lattice edge.
    if(latticeEdge2 >= 0) {
      iVERTEX tmp = v1; v1 = v2 ; v2 = tmp; 
    }
    
    /*
     * Vertex not moved to midpoint because
     *   1. v1 may be a lattice vertex.
     *   2. To help match it with a vertex in an adjoining cell 
     *      during gluing.
     */
    RemoveTinyEdge(v1, v2, false);
  }

  VOID cCUT_CELLS_BOOLEAN::RemoveLatticeEdge(sEDGE edge)
  {
    iVERTEX v1 = edge.first, v2 = edge.second;
    
    if(m_clay.Vertex(v1)->IsDeleted() || 
       m_clay.Vertex(v2)->IsDeleted())
      return;
    
    /*
     * Vertex not moved to midpoint because
     *   1. v1 may be a lattice vertex.
     *   2. To help match it with a vertex in an adjoining cell 
     *      during gluing.
     */
    RemoveTinyEdge(v1, v2, false);
  }

  /*
   *  Order of removal.
   *  1. Edges on lattice edges.
   *     -- If one of the vertex is close to a lattice vertex then, 
   *        the edge is snapped to a lattice vertex.
   *
   *     -- Always vertex with the lower index will survive.
   *
   *     
   *  2. Edges with one vertex on lattice edges.
   *     -- Vertex on the lattice edge will survive.
   *     
   *  3. Edges on lattice planes.
   *     -- Vertices are merged to the midpoint and vertex with 
   *        lower index will survive.
   */
  VOID cCUT_CELLS_BOOLEAN::RemoveEdges(tLIST<sEDGE> &tinyEdges)
  {
    tLIST<sEDGE>::iterator currEdge = tinyEdges.begin();
    tLIST<sEDGE>::iterator lastEdge = tinyEdges.end();

    //Removing tiny lattice edges.
    while (currEdge != lastEdge) {
      sEDGE edge = currEdge.operator*();
      iVERTEX v1 = edge.first;
      iVERTEX v2 = edge.second;
      
      INT latticeEdge1 = m_box.IsOnEdge(m_clay.Vertex(v1)->Point());
      INT latticeEdge2 = m_box.IsOnEdge(m_clay.Vertex(v2)->Point());

      if(latticeEdge1 >= 0 && latticeEdge2 >= 0 
         && latticeEdge1 == latticeEdge2) {
        RemoveLatticeEdge(edge);
        currEdge = tinyEdges.erase(currEdge);
      }else
        currEdge++;
    }

    //Removing tiny edges with one vertex incident on a lattice edge.
    currEdge = tinyEdges.begin();
    lastEdge = tinyEdges.end();

    while (currEdge != lastEdge) {
      sEDGE edge = currEdge.operator*();

      iVERTEX v1 = edge.first;
      iVERTEX v2 = edge.second;
      
      INT latticeEdge1 = m_box.IsOnEdge(m_clay.Vertex(v1)->Point());
      INT latticeEdge2 = m_box.IsOnEdge(m_clay.Vertex(v2)->Point());
      
      if(latticeEdge1 * latticeEdge2 <= 0) {
        RemoveEdgeWithVertexOnLatticeEdge(edge, latticeEdge2);
        currEdge = tinyEdges.erase(currEdge);
      }
      else
        currEdge++;
    }

    //Removing other tiny edges.
    currEdge = tinyEdges.begin();
    lastEdge = tinyEdges.end();

    while (currEdge != lastEdge) {
      sEDGE edge = currEdge.operator*();
      RemoveEdge(edge);
      currEdge++;
    }
    
  }

  VOID cCUT_CELLS_BOOLEAN::CollectTinyEdges(tLIST<sEDGE> &tinyEdges)
  {
    //collecting tiny edges.
    cCUT_CELL_CLAY::vertex_iterator currVtx = m_clay.VerticesBegin();
    cCUT_CELL_CLAY::vertex_iterator lastVtx = m_clay.VerticesEnd();

    for(  ; currVtx != lastVtx ; currVtx++ ){
      cCUT_CELL_CLAY::cVERTEX::vertex_use_iterator currUse = currVtx->VertexUsesBegin();
      cCUT_CELL_CLAY::cVERTEX::vertex_use_iterator lastUse = currVtx->VertexUsesEnd();

      for( ; currUse != lastUse ; currUse++ ) {
        cCUT_CELL_CLAY::cVERTEX_USE::half_edge_circulator currHe = currUse->HalfEdgesBegin();
        cCUT_CELL_CLAY::cVERTEX_USE::half_edge_circulator lastHe = currUse->HalfEdgesEnd(); 

        for( ; currHe != lastHe ; currHe++ ) {
          iVERTEX v1 = currHe->Tail()->Index();
          iVERTEX v2 = currHe->Head()->Index();

          if(v1 < v2 && currHe->Segment().Length() <= m_lengthTol)
            tinyEdges.push_back(sEDGE(v1, v2));
        }
      }
    }
  }

  VOID cCUT_CELLS_BOOLEAN::RemoveTinyEdges()
  {
    while(1) {
      tLIST<sEDGE> tinyEdges;
      CollectTinyEdges(tinyEdges);

      if(tinyEdges.size() == 0)
        break;

      RemoveEdges(tinyEdges);
    }

    //updating octree after removing tiny edges.
    //In future should tie mesh and octree so that a change in
    //one of them is reflected in another.
    m_octree.DeleteAll();
   
    cCUT_CELL_CLAY::facet_iterator currFacet = m_clay.FacetsBegin();
    cCUT_CELL_CLAY::facet_iterator lastFacet = m_clay.FacetsEnd();
    
    for( ; currFacet != lastFacet ; currFacet++ ) {
      m_octree.Insert(currFacet->Index());
    }

    
  }

  VOID cCUT_CELLS_BOOLEAN::ImprintManifold(cGRAY_CELL_ENTRY *entry)
  {
    cGRAY_CELL_ENTRY::cut_cell_iterator currCutCell = entry->CutCellsBegin();
    cGRAY_CELL_ENTRY::cut_cell_iterator lastCutCell = entry->CutCellsEnd();
    
    for( ; currCutCell != lastCutCell ; currCutCell++ ) {
      Imprint(*currCutCell);
    }

    cCUT_CELL_CLAY::facet_iterator currFacet = m_clay.FacetsBegin();
    cCUT_CELL_CLAY::facet_iterator lastFacet = m_clay.FacetsEnd();
    
    for( ; currFacet != lastFacet ; currFacet++ ) {
      if(currFacet->Color() == WHITE) {
	BOOL facetFound = false;

	currCutCell = entry->CutCellsBegin();
	lastCutCell = entry->CutCellsEnd();
	
	for( ; currCutCell != lastCutCell ; currCutCell++ ) {
	  if((*currCutCell)->IsOnBoundary(currFacet->MeanPoint()))
	    facetFound = true;
	}

	if(!facetFound)
	  currFacet->Color(BLACK);
      }
    }

    RemoveTinyEdges();
  }

  VOID cCUT_CELLS_BOOLEAN::Do()
  {
    Init();

    cGRAY_CELL::entry_iterator currEntry = m_grayCell->begin();
    cGRAY_CELL::entry_iterator lastEntry = m_grayCell->end();
   
    INT i = 0;

    for( ; currEntry != lastEntry ; currEntry++ ) {
      ImprintManifold(currEntry.operator*());
    }

    FindCommon();
  }
}
