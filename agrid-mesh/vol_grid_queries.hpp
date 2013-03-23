#ifndef _AGRID_MESH_VOL_GRID_QUERIES_HPP_
#define _AGRID_MESH_VOL_GRID_QUERIES_HPP_

#include "defs.hpp"

template <typename _Grid_Type>
class tVOL_GRID_QUERIES
{
  typedef typename _Grid_Type::cFACET cFACET;
  typedef typename _Grid_Type::cCELL cCELL;

  
public:
  tVOL_GRID_QUERIES(_Grid_Type* grid = NULL) : m_grid(grid)
  {}

public:
  BOOL BelongToSameCell(iFACET f1Index, iFACET f2Index) {
    cFACET *f1 = m_grid->Facet(f1Index);
    cFACET *f2 = m_grid->Facet(f2Index);

    iCELL incidentCells[1024];
    INT numCells = 0;
    
    typename cFACET::cell_iterator currCell = f1->CellsBegin();
    typename cFACET::cell_iterator lastCell = f1->CellsEnd();

    for( ; currCell != lastCell ; currCell++ )
      incidentCells[numCells++] = (*currCell)->Index();

    currCell = f2->CellsBegin();
    lastCell = f2->CellsEnd();

    for( ; currCell != lastCell ; currCell++ )
      if(ValIsInTable((*currCell)->Index(), incidentCells, numCells))
	return true;
    
    return false;
  }

private:
  _Grid_Type *m_grid;
};

#endif //_AGRID_MESH_VOL_GRID_QUERIES_HPP_
