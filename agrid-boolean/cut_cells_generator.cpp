#include "cut_cells_generator.hpp"
#include "cut_cell_builder.hpp"
#include "cut_cells_boolean.hpp"
#include "convex_decomposer.hpp"
#include "vol_grid_repair.hpp"

namespace agrid_boolean {
  /*
   * Slices up facets of a mesh to confine to one cell of the grid.
   */
  VOID cCUT_CELLS_GENERATOR::Slice(cSURFACE_MESH *mesh)
  {
    std::vector<iFACET> facetIndices;

    cSURFACE_MESH::facet_iterator currFacet = mesh->FacetsBegin();
    cSURFACE_MESH::facet_iterator lastFacet = mesh->FacetsEnd();

    for( ; currFacet != lastFacet ; currFacet++)
      facetIndices.push_back(currFacet->Index());

    std::vector<iFACET>::iterator currIndex = facetIndices.begin();
    std::vector<iFACET>::iterator lastIndex = facetIndices.end();

    for( ; currIndex != lastIndex ; currIndex++) {
      cBOX3 facetBox = mesh->Facet(*currIndex)->BoundingBox();
      std::vector<iFACET> facetsToBeClipped;
      facetsToBeClipped.push_back(*currIndex);

      DO_COORDS(coord) {
	iFACET oldLargestFacetIndex = mesh->LargestFacetIndex();
	std::vector<iFACET>::iterator currFacetToBeClipped = facetsToBeClipped.begin();
	std::vector<iFACET>::iterator lastFacetToBeClipped = facetsToBeClipped.end();

	REAL step = m_baseGrid.CellLength(coord);
	for( ; currFacetToBeClipped != lastFacetToBeClipped ; currFacetToBeClipped++) {
	  //Clips facet along the lines parallel to coordinate axis.
	  REAL minVal = m_baseGrid.CellUpperBound(facetBox.MinCoord(coord), coord);
	  REAL maxVal = m_baseGrid.CellUpperBound(facetBox.MaxCoord(coord), coord);

	  mesh->Clip(*currFacetToBeClipped, coord, minVal, maxVal, step);
	}

	iFACET newLargestFacetIndex = mesh->LargestFacetIndex();
	for( INT facetIndex = oldLargestFacetIndex+1 ;
	     facetIndex <= newLargestFacetIndex ;
	     facetIndex++)
	  facetsToBeClipped.push_back(facetIndex);
      }
    }
  }

  VOID cCUT_CELLS_GENERATOR::ClearManifoldGridColorMarkers()
  {
    INT numManifolds = m_manifolds.size();
    
    for(INT i = 0 ; i < numManifolds ; i++ ) {
      m_manifolds[i].ClearGridMarkers();
    }

  }

  VOID cCUT_CELLS_GENERATOR::Register(cSURFACE_MESH::cMANIFOLD *manifold,
                                      cSURFACE_MESH *mesh)
  {
    INT numCells[3] = { m_baseGrid.NumCells(0),
                        m_baseGrid.NumCells(1),
                        m_baseGrid.NumCells(2) };

    cMANIFOLD_RECORD *record = m_manifolds.new_object();
    ::new (record)cMANIFOLD_RECORD(manifold, mesh,
                                   m_baseGrid.SimulationVolume(),
                                   numCells);
  }

  VOID cCUT_CELLS_GENERATOR::PaintCurrManifoldGrids(cSURFACE_MESH *mesh,
						    INT numManifoldRecordsOld)
  {
    cSURFACE_MESH::facet_iterator currFacet = mesh->FacetsBegin();
    cSURFACE_MESH::facet_iterator lastFacet = mesh->FacetsEnd();

    for( ; currFacet != lastFacet ; currFacet++ ) {
      iCELL_INDEX cellIndices[2];
      INT numCells = m_baseGrid.ModifiedCellIndex(currFacet->MeanPoint(), currFacet->Normal(), cellIndices);

      for(INT i = 0; i < numCells; i++){
      //Add facet and manifold to gray-cell entry.
	iMANIFOLD manifoldIndex = currFacet->ManifoldIndex();
	cMANIFOLD_RECORD *record = m_manifolds.object_at(numManifoldRecordsOld + manifoldIndex);
	cGRAY_CELL *grayCell = GrayCell(cellIndices[i]);
	grayCell->Register(record, currFacet->Index());
	m_baseGrid.CellColor(cellIndices[i], GRAY);
	record->Grid()->CellColor(cellIndices[i], GRAY);
      }
    }

  }

  VOID cCUT_CELLS_GENERATOR::Register(cSURFACE_MESH *mesh)
  {
    Slice(mesh);

    INT numManifoldRecordsOld = m_manifolds.size();

    //Create record for each manifold.
    cSURFACE_MESH::manifold_iterator currManifold = mesh->ManifoldsBegin();
    cSURFACE_MESH::manifold_iterator lastManifold = mesh->ManifoldsEnd();

    for( ; currManifold != lastManifold ; currManifold++ ) {
      Register(currManifold.operator->(), mesh);
    }

    INT numManifoldRecordsNew = m_manifolds.size();

    /*
     * Method colors the global grid and individual grid for
     * each manifold.
     */
    PaintCurrManifoldGrids(mesh, numManifoldRecordsOld);
  }

  BOOL cCUT_CELLS_GENERATOR::BuildGrid(cVOLUMETRIC_GRID *grid, cSURFACE_MESH *bfacesMesh)
  {

    //Clearing rectilinear grids in individual manifolds.
    // Not required at this point.
    ClearManifoldGridColorMarkers();

    cCUT_CELLS_ADHESIVE adhesive(m_baseGrid.SimulationVolume(), grid, &m_baseGrid);
    //    printf("BuildGrid : xmax = %d\n", m_baseGrid.NumCells(GEOM_X));
    INT numWhiteCells = 0, numCutCells = 0, numNonClosedCells = 0;

    for(INT i = 0 ; i < m_baseGrid.NumCells(GEOM_X) ; i++ ) {
      //      printf("BuildGrid : i = %d\n", i);
      for(INT j = 0 ; j < m_baseGrid.NumCells(GEOM_Y) ; j++ ) {
    	for(INT k = 0 ; k < m_baseGrid.NumCells(GEOM_Z) ; k++ ) {
    	  iCELL_INDEX index(i, j, k);
    	  eCELL_COLOR cellColor = m_baseGrid.CellColor(index);

    	  if(cellColor == WHITE) {
	    cBOX3 box(m_baseGrid.CellBox(index));
	    cCUT_CELL whiteCell;
	    BoxToMesh(box, whiteCell);
	    iCELL cIndex = adhesive.Add(whiteCell, index);
	    numWhiteCells++;
	  }
	  else if(cellColor == GRAY) {
	    cGRAY_CELL* grayCell = m_factory.GrayCell(index);
	    grayCell->Close();

	   // GenerateConvexPartitions(grayCell);
	    if(grayCell->NumEntries() > 1) {
	      cGRAY_CELL::cut_cell_iterator currCutCell = grayCell->CutCellsBegin();
	      cGRAY_CELL::cut_cell_iterator lastCutCell = grayCell->CutCellsEnd(); //cutCells.end();
	      for( ; currCutCell != lastCutCell ; currCutCell++ ) {
		adhesive.Add(*(*currCutCell), index);
		numCutCells++;
	      }
	    }
	    else {
	      cGRAY_CELL::entry_iterator currEntry = grayCell->begin(); //entries.begin();
	      cGRAY_CELL::entry_iterator lastEntry = grayCell->end(); //entries.end();
	      
	      for( ; currEntry != lastEntry ; currEntry++ ) {
		cGRAY_CELL_ENTRY::cut_cell_iterator currCutCell = (*currEntry)->CutCellsBegin();
		cGRAY_CELL_ENTRY::cut_cell_iterator lastCutCell = (*currEntry)->CutCellsEnd(); //cutCells.end();

		for( ; currCutCell != lastCutCell ; currCutCell++ ) {
		  adhesive.Add(*(*currCutCell), index);
		  numCutCells++;
		  
		}
	      }
	    }
	    
	    grayCell->DeleteCutCells();
	  }
	}
      }
    }
    
    printf("Num white cells = %d, num cut cells = %d\n", numWhiteCells, numCutCells);
    //adhesive.PrintTimers();

    //     sVOLUMETRIC_GRID_BOUNDARY_FILTER filter(grid);
    //     tmesh::ExportToOff<cVOLUMETRIC_GRID,
    //       sVOLUMETRIC_GRID_BOUNDARY_FILTER>("bfacesMesh_before_repair.off", *grid, filter);

    cVOLUMETRIC_GRID_REPAIR repair(m_baseGrid.SimulationVolume(), grid);
    if (!repair.Do())
      return false;;

    //     tmesh::ExportToOff<cVOLUMETRIC_GRID,
    //       sVOLUMETRIC_GRID_BOUNDARY_FILTER>("bfacesMesh_after_repair.off", *grid, filter);
    return true;
  }

  VOID cCUT_CELLS_GENERATOR::PaintAllGrids()
  {
//     printf("Started walking grids...\n");
//     printf("\n\n\n\n\n\n\n\n\n");
//     printf("Before walking...\n");
//     m_baseGrid.Print();
//     printf("\n\n\n\n\n\n\n\n\n");

//    m_baseGrid.Walk(true);

//     printf("\n\n\n\n\n\n\n\n\n");
//     printf("After walking...\n");
//     m_baseGrid.Print();
//     printf("\n\n\n\n\n\n\n\n\n");

    tTABLE<cMANIFOLD_RECORD>::iterator currRecord = m_manifolds.begin();
    tTABLE<cMANIFOLD_RECORD>::iterator lastRecord = m_manifolds.end();

    for( ; currRecord != lastRecord ; currRecord++ ) {
      currRecord->Grid()->Walk(true);
    }


    for(INT i = 0 ; i < m_baseGrid.NumCells(GEOM_X) ; i++ ) {
      for(INT j = 0 ; j < m_baseGrid.NumCells(GEOM_Y) ; j++ ) {
    	for(INT k = 0 ; k < m_baseGrid.NumCells(GEOM_Z) ; k++ ) {
    	  iCELL_INDEX index(i, j, k);
	  BOOL isBlack = false, isGray = false;
	  
	  currRecord = m_manifolds.begin();
	  lastRecord = m_manifolds.end();
	  
	  for( ; currRecord != lastRecord ; currRecord++ ) {
	    eCELL_COLOR cellColor = currRecord->Grid()->CellColor(index);
	    if(cellColor == BLACK) {
	      isBlack = true;
	      break;
	    }
	    else if(cellColor == GRAY)
	      isGray = true;
	  }
	  
	  if(isBlack)
	    m_baseGrid.CellColor(index, BLACK);
	  else if(isGray)
	    m_baseGrid.CellColor(index, GRAY);
	  else
	    m_baseGrid.CellColor(index, WHITE);
	}
      }
    }

    printf("Ended walking grids...\n");
  }

  VOID cCUT_CELLS_GENERATOR::ProcessGrayCell(cGRAY_CELL *cell)
  {
    /*
     * Black cells don't need to be processed.
     */
    if(m_baseGrid.CellColor(cell->Index()) == BLACK) {
//       printf("Black index...\n");
//       cell->Index().Print();
//       printf("\n");
      return;
    }

    cGRAY_CELL::entry_iterator currEntry = cell->begin();
    cGRAY_CELL::entry_iterator lastEntry = cell->end();

    REAL lengthTol = m_baseGrid.CellLengthMax() * 0.02;

    for( ; currEntry != lastEntry ; currEntry++ ) {
      cCUT_CELL_BUILDER builder((*currEntry)->Record(), cell, *currEntry, 1e-4);
      builder.Build();
    }
  }

  VOID cCUT_CELLS_GENERATOR::DoGrayCellBooleans(cGRAY_CELL *cell)
  {
    /*
     * Black cells don't need to be processed.
     */
    if(m_baseGrid.CellColor(cell->Index()) == BLACK)
      return;

    if(cell->NumEntries() == 1)
      return;

    cCUT_CELLS_BOOLEAN booleanOp(cell, 1e-4);
    booleanOp.Do();

    cell->DeleteIndividualManifoldCutCells();
  }

  VOID cCUT_CELLS_GENERATOR::ProcessGrayCells()
  {
    cGRID_OBJECT_FACTORY::gray_cells_iterator currGrayCell = m_factory.GrayCellsBegin();
    cGRID_OBJECT_FACTORY::gray_cells_iterator lastGrayCell = m_factory.GrayCellsEnd();

    for( ; currGrayCell != lastGrayCell ; currGrayCell++ ) {
      ProcessGrayCell(currGrayCell.operator->());
    }

    currGrayCell = m_factory.GrayCellsBegin();
    lastGrayCell = m_factory.GrayCellsEnd();

    for( ; currGrayCell != lastGrayCell ; currGrayCell++ ) {
      BOOL exportResults = false;
      //       if(currGrayCell->Index() == iCELL_INDEX(21, 18, 19) || 
      // 	 currGrayCell->Index() == iCELL_INDEX(21, 19, 19)) {
      //  	exportResults = true; 
      //       }
      
      if(exportResults)
	currGrayCell->Export();
      
      DoGrayCellBooleans(currGrayCell.operator->());
      
      if(exportResults) {
	currGrayCell->Export();
	currGrayCell->ExportManifolds();
      }
    }
  }

  VOID cCUT_CELLS_GENERATOR::Generate()
  {
    PaintAllGrids();
    ProcessGrayCells();
  }

  VOID cCUT_CELLS_GENERATOR::Export()
  {
    cGRID_OBJECT_FACTORY::gray_cells_iterator currGrayCell = m_factory.GrayCellsBegin();
    cGRID_OBJECT_FACTORY::gray_cells_iterator lastGrayCell = m_factory.GrayCellsEnd();

    for( ; currGrayCell != lastGrayCell ; currGrayCell++ ) {
      currGrayCell->Export();
    }

  }
}

