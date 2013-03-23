#ifndef _AGRID_BOOLEAN_CUT_CELLS_GENERATOR_HPP_
#define _AGRID_BOOLEAN_CUT_CELLS_GENERATOR_HPP_

#include "rectilinear_grid.hpp"
#include "manifold_record.hpp"
#include "cell.hpp"
#include "grid_object_factory.hpp"
#include "cut_cells_adhesive.hpp"

namespace agrid_boolean {
  class cCUT_CELLS_GENERATOR
  {
  public:
    cCUT_CELLS_GENERATOR() {}
    cCUT_CELLS_GENERATOR(const cBOX3 &simVol, INT numCells[3]) : m_baseGrid(simVol, numCells)
    {}

  public:
    /*
     *  Registers mesh in the grid generator.
     *  1. Registers one manifold at a time
     */
    VOID Register(cSURFACE_MESH *mesh);


    //Will generate cut-cells for all gray cells.
    VOID Generate();

    BOOL BuildGrid(cVOLUMETRIC_GRID *grid, cSURFACE_MESH *bfacesMesh = NULL);

    //Exports gray-cells entries and correspoding cut-cells to OFF.
    VOID Export();

    VOID Print() {
      printf("Base grid....\n");
      m_baseGrid.Print();

      INT numManifolds = m_manifolds.size();

      for(INT i = 0 ; i < numManifolds ; i++ ) {
	printf("\n\n Manifold %d \n", i);
	m_manifolds[i].Print();
      }
    }

  private:
    /*
     * Slices the facets of a mesh, so that they are confined to cells of base grid.
     */
    VOID Slice(cSURFACE_MESH *mesh);

    VOID Register(cSURFACE_MESH::cMANIFOLD *manifold,
                  cSURFACE_MESH *mesh);

    VOID PaintCurrManifoldGrids(cSURFACE_MESH *mesh, INT numManifoldRecordsOld);

    VOID PaintAllGrids();

    VOID ProcessGrayCell(cGRAY_CELL *cell);
    VOID DoGrayCellBooleans(cGRAY_CELL *cell);

    VOID ProcessGrayCells();

    cGRAY_CELL* GrayCell(iCELL_INDEX index) {
      cGRAY_CELL *cell = m_factory.GrayCell(index);

      if(cell == NULL) {
	cell = m_factory.AddGrayCell(index);
	::new (cell) cGRAY_CELL(index, m_baseGrid.CellBox(index));
      }

      return cell;
    }

    VOID ClearManifoldGridColorMarkers();

  private:
    /*
     * manifolds recorded in the grid.
     */
    tTABLE<cMANIFOLD_RECORD> m_manifolds;
    //tTABLE<cGRAY_CELL> m_grayCells;
    cRECTILINEAR_GRID m_baseGrid;
    cGRID_OBJECT_FACTORY m_factory;
  };

}

#endif //_AGRID_BOOLEAN_CUT_CELLS_GENERATOR_HPP_
