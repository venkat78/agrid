#include <vector>
#include "grid_generator.hpp"
#include "cut_cell_builder.hpp"
#include "cut_cells_adhesive.hpp"

namespace grid_gen {
  /*
   * Methods to support registration of a model in coarse grid.
   */
  template<typename _MODEL_TYPE>
  VOID tGRID_GENERATOR<_MODEL_TYPE>::Register(_MODEL_TYPE *model) {
    typename _MODEL_TYPE::mesh_iterator currMesh = model->MeshesBegin();
    typename _MODEL_TYPE::mesh_iterator lastMesh = model->MeshesEnd();

    for (; currMesh != lastMesh; currMesh++) {
      Register(currMesh.operator->());
    }

    PopulateManifoldGrids();
  }

  template<typename _MODEL_TYPE>
  VOID tGRID_GENERATOR<_MODEL_TYPE>::PopulateManifoldGrids() {
    INT numManifolds = m_manifolds.size();

    for (INT i = 0; i < numManifolds; i++) {
      cGRID *grid = m_manifolds[i].Grid();
      grid->FloodFill();
    }
  }

  template<typename _MODEL_TYPE>
  VOID tGRID_GENERATOR<_MODEL_TYPE>::Register(cMESH *mesh) {
    Slice(mesh);

    INT offset = m_manifolds.size();

    //Create record for each manifold.
    typename cMESH::manifold_iterator currManifold = mesh->ManifoldsBegin();
    typename cMESH::manifold_iterator lastManifold = mesh->ManifoldsEnd();

    for (; currManifold != lastManifold; currManifold++) {
      RegisterManifold(currManifold.operator->(), mesh);
    }

    /*
     * Method colors the global grid and individual grid for
     * each manifold.
     */
    RegisterFacetsInGrids(mesh, offset);
  }

  template<typename _MODEL_TYPE>
  VOID tGRID_GENERATOR<_MODEL_TYPE>::Slice(cMESH *mesh) {
    std::vector<iFACET> facetIndices;

    typename cMESH::facet_iterator currFacet = mesh->FacetsBegin();
    typename cMESH::facet_iterator lastFacet = mesh->FacetsEnd();

    for (; currFacet != lastFacet; currFacet++)
      facetIndices.push_back(currFacet->Index());

    std::vector<iFACET>::iterator currIndex = facetIndices.begin();
    std::vector<iFACET>::iterator lastIndex = facetIndices.end();

    for (; currIndex != lastIndex; currIndex++) {
      cBOX3 facetBox = mesh->Facet(*currIndex)->BoundingBox();
      std::vector<iFACET> facetsToBeClipped;
      facetsToBeClipped.push_back(*currIndex);

      DO_COORDS(coord)
      {
        iFACET oldLargestFacetIndex = mesh->LargestFacetIndex();
        std::vector<iFACET>::iterator currFacetToBeClipped = facetsToBeClipped.begin();
        std::vector<iFACET>::iterator lastFacetToBeClipped = facetsToBeClipped.end();

        REAL step = m_grid.CellLength(coord);
        for (; currFacetToBeClipped != lastFacetToBeClipped; currFacetToBeClipped++) {
          //Clips facet along the lines parallel to coordinate axis.
          REAL minVal = m_grid.CellUpperBound(facetBox.MinCoord(coord), coord);
          REAL maxVal = m_grid.CellUpperBound(facetBox.MaxCoord(coord), coord);

          mesh->Clip(*currFacetToBeClipped, coord, minVal, maxVal, step);
        }

        iFACET newLargestFacetIndex = mesh->LargestFacetIndex();
        for (INT facetIndex = oldLargestFacetIndex + 1; facetIndex <= newLargestFacetIndex; facetIndex++)
          facetsToBeClipped.push_back(facetIndex);
      }
    }
  }

  template<typename _MODEL_TYPE>
  VOID tGRID_GENERATOR<_MODEL_TYPE>::RegisterManifold(cMANIFOLD *manifold, cMESH *mesh) {
    cMANIFOLD_OBJ *record = m_manifolds.new_object();
    ::new (record) cMANIFOLD_OBJ(m_bounds, m_numCells, manifold, mesh);
    record->Index(m_manifolds.size() - 1);
  }

  template<typename _MODEL_TYPE>
  VOID tGRID_GENERATOR<_MODEL_TYPE>::RegisterFacetsInGrids(cMESH *mesh, INT offset) {
    typename cMESH::facet_iterator currFacet = mesh->FacetsBegin();
    typename cMESH::facet_iterator lastFacet = mesh->FacetsEnd();

    for (; currFacet != lastFacet; currFacet++) {
      iCELL_INDEX cellIndices[2];
      INT numCells = m_grid.ModifiedCellIndex(currFacet->MeanPoint(), currFacet->Normal(), cellIndices);

      for (INT i = 0; i < numCells; i++) {
        //Add facet and manifold to gray-cell entry.
        iMANIFOLD manifoldIndex = currFacet->ManifoldIndex();
        cMANIFOLD_OBJ *record = m_manifolds.object_at(offset + manifoldIndex);

        //Registering facets in local and global grids.
        cGRID_CELL *globalCell = m_grid.CoarseElement(cellIndices[i]);
        if (globalCell == NULL)
          globalCell = m_grid.AddCoarseElement(cellIndices[i]);
        globalCell->Register(record, currFacet->Index());
        globalCell->Color(GRAY);
        m_grid.CellColor(cellIndices[i], GRAY);

        cGRID_CELL *localCell = record->Grid()->CoarseElement(cellIndices[i]);
        if (localCell == NULL)
          localCell = record->Grid()->AddCoarseElement(cellIndices[i]);
        localCell->Register(record, currFacet->Index());
        localCell->Color(GRAY);
        record->Grid()->CellColor(cellIndices[i], GRAY);
      }
    }
  }

  /*
   *
   */
  template<typename _GRID_TYPE, typename _GRID_CELL>
  VOID CellsQueue(_GRID_TYPE *grid, std::vector<_GRID_CELL*> &queue) {
    for (INT i = 0; i < grid->NumCells(GEOM_X); i++) {
      for (INT j = 0; j < grid->NumCells(GEOM_Y); j++) {
        for (INT k = 0; k < grid->NumCells(GEOM_Z); k++) {
          _GRID_CELL *cell = grid->CoarseElement(iCELL_INDEX(i, j, k));
          ASSERT(cell != NULL);
          queue.push_back(cell);
        }
      }
    }
  }

  template<typename _MODEL_TYPE>
  BOOL tGRID_GENERATOR<_MODEL_TYPE>::Process(cGRID_CELL *cell, cMANIFOLD_OBJ *record) {
    //If the cells needs subdivision subdivide or generate cut-cells.
    tCUT_CELL_BUILDER<cMANIFOLD_OBJ, cGRID_CELL> builder(record, cell);
    return builder.Build();
  }

  /*
   * Collects all base grid cells and processes them as long as there are no cells
   * to process. Meat of the process.
   */
  template<typename _MODEL_TYPE>
  VOID tGRID_GENERATOR<_MODEL_TYPE>::Build(cVOLUMETRIC_GRID *grid) {
    INT numManifolds = m_manifolds.size();

    for (INT i = 0; i < numManifolds; i++) {
      std::vector<cGRID_CELL*> queue;
      cGRID *manifoldGrid = m_manifolds[i].Grid();
      CellsQueue(manifoldGrid, queue);
      INT j = 0;

      while (j < queue.size()) {
        Process(queue[j], &m_manifolds[i]);
        j++;
      }

      /*
       * Register cut-cells in m_grid.
       */
      StitchCutCells(queue, grid);
    }
  }

  template<typename _MODEL_TYPE>
  BOOL tGRID_GENERATOR<_MODEL_TYPE>::StitchCutCells(std::vector<cGRID_CELL*> &processedCells, cVOLUMETRIC_GRID *grid) {
    cCUT_CELLS_ADHESIVE adhesive(m_bounds, grid);
    typename std::vector<cGRID_CELL*>::iterator currCell = processedCells.begin();
    typename std::vector<cGRID_CELL*>::iterator lastCell = processedCells.end();

    INT numCutCells = 0;
    for (; currCell != lastCell; currCell++) {
      if ((*currCell)->NumEntries() == 1) {
        typename cGRID_CELL::entry_iterator entry = (*currCell)->begin();

        typename cGRID_CELL::cENTRY::cut_cell_iterator currCutCell = (*entry)->CutCellsBegin();
        typename cGRID_CELL::cENTRY::cut_cell_iterator lastCutCell = (*entry)->CutCellsEnd();
        for (; currCutCell != lastCutCell; currCutCell++) {
          (*currCutCell)->Close();
          adhesive.Add(*(*currCutCell));
          numCutCells++;
        }
      } else {
        typename cGRID_CELL::cENTRY::cut_cell_iterator currCutCell = (*currCell)->CutCellsBegin();
        typename cGRID_CELL::cENTRY::cut_cell_iterator lastCutCell = (*currCell)->CutCellsEnd();
        for (; currCutCell != lastCutCell; currCutCell++) {
          (*currCutCell)->Close();
          adhesive.Add(*(*currCutCell));
          numCutCells++;
        }
      }
    }
    printf("Num cut cells %d\n", numCutCells);
  }
}
