#ifndef _AGRID_BOOLEAN_GRID_OBJECT_FACTORY_HPP_
#define _AGRID_BOOLEAN_GRID_OBJECT_FACTORY_HPP_

#include "defs.hpp"
#include "cell.hpp"

namespace agrid_boolean {

  class cGRID_OBJECT_FACTORY {
  public:
    cGRID_OBJECT_FACTORY() { }

    //    INT NumVertices() const { return m_vertices.size(); }

    INT NumGrayCells() const { return m_grayCells.size(); }
    
    //    INT NumWhiteCells() const { return m_whiteCells.size(); }

    /*
     * Methods to add cells.
     */
    //    cWHITE_CELL *AddWhiteCell(iCELL_INDEX cellIndex);
    cGRAY_CELL *AddGrayCell(iCELL_INDEX cellIndex);
    
//     /*
//      * Methods to add grid vertices.
//      */
//     cGRID_VERTEX* AddGridVertex(iGRID_VERTEX vertexIndex);

//     /*
//      * Methods to access cells and grid vertices.
//      */
//     cWHITE_CELL *WhiteCell(iCELL_INDEX cellIndex);
    
    cGRAY_CELL *GrayCell(iCELL_INDEX cellIndex);

    //    cGRID_VERTEX* GridVertex(iGRID_VERTEX vertexIndex);

    //Release all storage.
    VOID Destroy();

  public:
    typedef tTABLE<cGRAY_CELL>::iterator gray_cells_iterator;

    gray_cells_iterator GrayCellsBegin() {
      return m_grayCells.begin();
    }

    gray_cells_iterator GrayCellsEnd() {
      return m_grayCells.end();
    }

  private:
    /*
     * Table of gray cells.
     */
    tTABLE<cGRAY_CELL> m_grayCells;

    /*
     * Only white cells adjacent to gray cells are stored.
     */
    //    tTABLE<cWHITE_CELL> m_whiteCells;

    cINDEX_TO_INT_MAP m_grayCellsMap;
  };


}

#endif //_AGRID_BOOLEAN_GRID_OBJECT_FACTORY_HPP_
