#include "vol_grid_repair.hpp"
#include <time.h>

namespace agrid_boolean {
  VOID cVOLUMETRIC_GRID_REPAIR::PopulateTree()
  {
    const cBOX3 &simulationVolume = m_grid->SimulationVolume();
    
    //    ASSERT(m_grid->HasInputSurface());

    cVOLUMETRIC_GRID::facet_iterator currFacet = m_grid->FacetsBegin();
    cVOLUMETRIC_GRID::facet_iterator lastFacet = m_grid->FacetsEnd();

    for( ; currFacet != lastFacet ; currFacet++ ) {
      if(currFacet->IsBoundary() && !currFacet->IsFromInputSurface() &&
	 !simulationVolume.HasOnBoundary(currFacet->MeanPoint())) {
	m_octree.Insert(currFacet->Index());
      }
    }
  }

  VOID cVOLUMETRIC_GRID_REPAIR::VerifyOctree()
  {
    cOCTREE::leaf_iterator currLeaf = m_octree.Begin();
    cOCTREE::leaf_iterator lastLeaf = m_octree.End();

    for( ; currLeaf != lastLeaf ; currLeaf++ ) {
      cOCTREE::leaf_element_iterator currElement = currLeaf->begin();
      cOCTREE::leaf_element_iterator lastElement = currLeaf->end();

      for( ; currElement != lastElement ; currElement++ ) {
	ASSERT(!m_grid->Facet(*currElement)->IsFromInputSurface());
      }
    }

  }

  BOOL cVOLUMETRIC_GRID_REPAIR::Do() {

//     ASSERT(m_grid->Verify());

//     sCUT_CELLS_FILTER filter(m_grid);
//     filter.Add(iCELL_INDEX(21, 18, 19));
//     filter.Add(iCELL_INDEX(21, 19, 19));
//     //filter.Add(iCELL_INDEX(9, 40, 5));
    
//     ExportToOff<cVOLUMETRIC_GRID, sCUT_CELLS_FILTER>("cut_cells.off", *m_grid, filter);
    
    time_t t0 = clock();
    PopulateTree();
    time_t t1 = clock();

    //    printf("Building facets octree %lf\n", (double)(t1 - t0));
    //    printf("Validating grid....\n");
    fflush(stdout);

    //    printf("Grid has %d cells\n", m_grid->NumCells());
    //    printf("Grid has %d facets\n", m_grid->NumFacets());
    //    printf("Grid has %d vertices\n", m_grid->NumVertices());

    BOOL problemsFound = Check();
    if(!problemsFound) {
      ASSERT(m_grid->Verify());
      return true;
    }

    printf("Problems found in grid....\n");

    ExportProblemsToOff(0);
    //    printf("Trying to repair the problems....\n");

    //m_grid->VerifyFacets();

    //PrintEdges();

    //     ExportProblemsToOff();
    //     return;
    //    FILE *gf = fopen("grid.txt", "wb+");
    //    m_grid->Print(gf);
    //    fclose(gf);
    //    gf = NULL;

    Repair();

    m_problemFaces.clear();
    m_problemCells.clear();
    m_edges.clear();

    SetDebugOn();

    problemsFound = Check();

    SetDebugOff();
    //    PrintEdges();

    if(problemsFound) {
      printf("Problems still not fixed....\n");
      //       printf("Trying further repair....\n");

      //       Repair();
      
      //       m_problemFaces.clear();
      //       m_edges.clear();
      
      //       problemsFound = Check();
      
      //       if(problemsFound) {
      // 	printf("Problems not fixed after second round also...\n");
      // 	ExportProblemsToOff(1);
      // 	return false;
      //       }else {
      // 	printf("Problems repaired in the grid....\n");
      //       }

      ExportProblemsToOff(1);
      return false;
    }

    printf("Problems repaired in the grid....\n");

    //    m_grid->VerifyFacets();
    fflush(stdout);
    return true;
  }

  BOOL cVOLUMETRIC_GRID_REPAIR::Check()
  {
    BOOL problemsFound = false;

    cOCTREE::leaf_iterator currLeaf = m_octree.Begin();
    cOCTREE::leaf_iterator lastLeaf = m_octree.End();
    BOOL doDebug = false;;
    tMESH_GEOM_QUERIES<cVOLUMETRIC_GRID> geomQueries(m_grid);
    tVOL_GRID_QUERIES<cVOLUMETRIC_GRID> gridQueries(m_grid);

    for( ; currLeaf != lastLeaf ; currLeaf++ ) {
      cOCTREE::leaf_element_iterator currElement = currLeaf->begin();
      cOCTREE::leaf_element_iterator lastElement = currLeaf->end();

      for( ; currElement != lastElement ; currElement++ ){
    	cOCTREE::leaf_element_iterator nextElement = currElement;
    	nextElement++;

    	for( ; nextElement != lastElement ; nextElement++ ) {
    	  if(!FacetsTested(*currElement, *nextElement) &&
	     !gridQueries.BelongToSameCell(*currElement, *nextElement)) {
	    if(geomQueries.IntersectingEdges(*currElement, *nextElement, m_edges)) {
	      problemsFound = true;
	      AddProblemFacesAndCells(*currElement, *nextElement);
	    }
    	  }
    	}
      }
    }
    
    return problemsFound;
  }

  VOID cVOLUMETRIC_GRID_REPAIR::Clip(const sVERTEX_PAIR &pair)
  {
    iVERTEX v1 = pair.V1();
    iVERTEX v2 = pair.V2();

    cVOLUMETRIC_GRID::cHALF_EDGE *he = NULL;
    m_grid->Vertex(v1)->Find(m_grid->Vertex(v2), &he, NULL);

//     if(!he)
//       return;

    iFACET incidentFacets[1024];
    INT numIncidentFacets = 0;

    cVOLUMETRIC_GRID::cHALF_EDGE *loopHe = he;

    //Collect facets incident on he.
    if(he) {
      do {
	if(!loopHe->IsBorder())
	  incidentFacets[numIncidentFacets++] = loopHe->Facet()->Index();
	
	loopHe = loopHe->NextInRing();
      } while(loopHe != he);
    }

    //Collect facets that intersect the segment.
    typedef cOCTREE::cLEAF cLEAF;
    std::vector<cLEAF*> leaves;
    cSEGMENT3 segment = he ? he->Segment() : cSEGMENT3(m_grid->Vertex(v1)->Point(),
						       m_grid->Vertex(v2)->Point());

    m_octree.Crosses(segment, leaves);

    INT i = 0, n = leaves.size(), numClippableFacets = 0;
    iFACET facetsToBeClipped[1024];
    tMESH_GEOM_QUERIES<cVOLUMETRIC_GRID> geomQueries(m_grid);

    for( ; i < n ; i++ ) {
      cLEAF* leaf = leaves[i];
      cOCTREE::leaf_element_iterator currElem = leaf->begin();
      cOCTREE::leaf_element_iterator lastElem = leaf->end();

      for( ; currElem != lastElem ; currElem++ ) {
	if(!ValIsInTable<iFACET>(*currElem, incidentFacets, numIncidentFacets)) {
	  if(!ValIsInTable<iFACET>(*currElem, facetsToBeClipped, numClippableFacets)) {

	    cPOINT3 point = m_grid->Face(*currElem)->MeanPoint();
	    cVECTOR3 normal = m_grid->Face(*currElem)->UnitNormal();

	    cPLANE3 plane(point, normal);
	    cSEGMENT3 smallSeg(segment.Evaluate(0.01), segment.Evaluate(0.99));

	    if(plane.HasOn(segment.Source()) &&
	       plane.HasOn(segment.Target()) &&
	       geomQueries.DoIntersect(*currElem, smallSeg))
	      facetsToBeClipped[numClippableFacets++] = *currElem;
	  }
	}
      }
    }

    iFACET newFacets[1024];
    INT numNewFacets = 0;

    for( i = 0; i < numIncidentFacets ; i++ ) {
      m_problemFaces.insert(incidentFacets[i]);
    }

    for( i = 0; i < numClippableFacets ; i++ ) {
      m_octree.Delete(facetsToBeClipped[i]);

      tMESH_CLIPPER<cVOLUMETRIC_GRID> gridClipper(m_grid);
      iFACET newIndex = gridClipper.Clip(facetsToBeClipped[i], segment, true);

      //m_grid->VerifyFacets();

      if(!ValIsInTable(facetsToBeClipped[i], newFacets, numNewFacets)) {
	newFacets[numNewFacets++] = facetsToBeClipped[i];
      }

      if(newIndex != INVALID_IFACET &&
	 !ValIsInTable(newIndex, newFacets, numNewFacets)) {
	newFacets[numNewFacets++] = newIndex;
      }
    }

    //Insert new facets back into octree.
    for( i = 0; i < numNewFacets ; i++ ) {
      m_octree.Insert(newFacets[i]);
      m_grid->Facet(newFacets[i])->Verify();
      m_problemFaces.insert(newFacets[i]);
    }

  }

  VOID cVOLUMETRIC_GRID_REPAIR::Clip()
  {
    m_problemFaces.clear();

    cEDGE_MARKS::iterator currEdge = m_edges.begin();
    cEDGE_MARKS::iterator lastEdge = m_edges.end();

    for( ; currEdge != lastEdge ; currEdge++ ) {
      Clip(currEdge->first);
    }
  }

  VOID cVOLUMETRIC_GRID_REPAIR::PrintEdges()
  {
    printf("Edges out...\n");
    cEDGE_MARKS::iterator currEdge = m_edges.begin();
    cEDGE_MARKS::iterator lastEdge = m_edges.end();

    for( ; currEdge != lastEdge ; currEdge++ ) {
      printf("%d %d \n", currEdge->first.V1(), currEdge->first.V2());
    }
  }

  VOID cVOLUMETRIC_GRID_REPAIR::RemoveDuplicates()
  {
    std::set<iFACET>::iterator currFacet = m_problemFaces.begin();
    std::set<iFACET>::iterator lastFacet = m_problemFaces.end();

    std::vector<sFACET> facets;
    for( ; currFacet != lastFacet ; currFacet++ ) {
      sFACET facet;
      facet.index = *currFacet;
      ASSERT(!m_grid->Facet(facet.index)->IsDeleted());
      facet.centroid = m_grid->Facet(facet.index)->MeanPoint();
      facets.push_back(facet);
    }

    std::sort(facets.begin(), facets.end());

    INT i = 0, n = facets.size();
    while(i < n-1) {
      INT j = i + 1;
      for( ; j < n ; j++ ) {
	if(facets[i].centroid != facets[j].centroid) {
	  i = j;
	  break;
	}

	tMESH_FACETS_OPS<cVOLUMETRIC_GRID> facetsOps(m_grid);

	iCELL iCellIndex = m_grid->Facet(facets[i].index)->LeftCellIndex();
	iCELL jCellIndex = m_grid->Facet(facets[j].index)->LeftCellIndex();

	iFACET lowerIndex = iCellIndex < jCellIndex ? facets[i].index : facets[j].index;
	iFACET upperIndex = iCellIndex < jCellIndex ? facets[j].index : facets[i].index;

	facetsOps.MergeFacets(lowerIndex, upperIndex);

	cVOLUMETRIC_GRID::cFACET* facetI = m_grid->Facet(lowerIndex);
	cVOLUMETRIC_GRID::cFACET* facetJ = m_grid->Facet(upperIndex);

	cVOLUMETRIC_GRID::cFACET::cell_iterator currCell = facetJ->CellsBegin();
	cVOLUMETRIC_GRID::cFACET::cell_iterator lastCell = facetJ->CellsEnd();

	for( ; currCell != lastCell ; currCell++ ) {
	  facetI->AppendCell(*currCell);
	  (*currCell)->AppendFace(facetI);
	  (*currCell)->RemoveFace(facetJ);
	}

	facetJ->RemoveCells();
	m_octree.Delete(facetJ->Index());
	m_grid->DeleteFacet(facetJ->Index());
      }

      if(j == n)
	break;
    }

  }

  VOID cVOLUMETRIC_GRID_REPAIR::Repair()
  {
    Clip();
    RemoveDuplicates();
  }

  BOOL cVOLUMETRIC_GRID_REPAIR::FacetsTested(iFACET f1, iFACET f2)
  {
//    ASSERT(!m_grid->Facet(f1)->IsDeleted()
// 	  && !m_grid->Facet(f2)->IsDeleted());
   
    if(m_grid->Facet(f1)->IsDeleted() ||
       m_grid->Facet(f2)->IsDeleted())
      return true;

   if(!m_grid->Facet(f1)->IsBoundary() ||
      !m_grid->Facet(f2)->IsBoundary())
     return true;
   
   sFACET_PAIR pair(f1, f2);
   return m_pairs.find(pair) == m_pairs.end() ? false : true;
  }

  VOID cVOLUMETRIC_GRID_REPAIR::AddProblemFacesAndCells(iFACET f1, iFACET f2)
  {
    m_problemFaces.insert(f1);
    m_problemFaces.insert(f2);

    cVOLUMETRIC_GRID::cFACET::cell_iterator problemCell = m_grid->Facet(f1)->CellsBegin();
    cVOLUMETRIC_GRID::cFACET::cell_iterator lastCell = m_grid->Facet(f1)->CellsEnd();

    INT count = 0;
    for( ; problemCell != lastCell ; problemCell++ ) {
      m_problemCells.insert((*problemCell)->GridIndex());
      count++;
    }
    
    ASSERT(count == 1);

    problemCell = m_grid->Facet(f2)->CellsBegin();
    lastCell = m_grid->Facet(f2)->CellsEnd();
    
    count = 0;
    for( ; problemCell != lastCell ; problemCell++) {
      m_problemCells.insert((*problemCell)->GridIndex());
      count++;
    }

    ASSERT(count == 1);
  }

  VOID cVOLUMETRIC_GRID_REPAIR::ExportProblemsToOff(INT round)
  {
    if(m_problemFaces.size() > 0) {
      //      printf("Problems found in volumetric grid....\n");
      //      printf("Indices of problematic cells are written to file : problem_cell_indices.txt....\n");
      //      printf("Problem faces are exported to problem_faces.off...\n");
      fflush(stdout);
      sFILTER filter(m_problemFaces);
      
      char filename[128];
      sprintf(filename, "problem_faces_%d.off\0", round);
      ExportToOff<cVOLUMETRIC_GRID, sFILTER>(filename, *m_grid, filter);

      char filename1[128];
      sprintf(filename1, "cell_indices_%d.txt\0", round);
      FILE *file = fopen(filename1, "wb+");
      
      std::set<iCELL_INDEX>::iterator problemCell = m_problemCells.begin();
      std::set<iCELL_INDEX>::iterator lastCell = m_problemCells.end();
      
      for( ; problemCell != lastCell ; problemCell++ )
	(*problemCell).Print(file);
      
      
      fclose(file);
    }
  }
}

