#include "grid_object_factory.hpp"

namespace agrid_boolean {
  cGRAY_CELL *cGRID_OBJECT_FACTORY ::GrayCell(iCELL_INDEX cellIndex)
  {
    cINDEX_TO_INT_MAP::iterator grayCellPos = m_grayCellsMap.find(cellIndex);
     if(grayCellPos == m_grayCellsMap.end())
       return NULL;
     else
       return m_grayCells.object_at(grayCellPos->second);
     
     return NULL;
  }

//   cWHITE_CELL *cGRID_OBJECT_FACTORY ::WhiteCell(iCELL_INDEX cellIndex)
//   {
//     std::map<iCELL_INDEX, INT>::iterator whiteCellPos = m_whiteCellsMap.find(cellIndex);
//      if(whiteCellPos == m_whiteCellsMap.end())
//        return NULL;
//      else
//        return m_whiteCells.object_at(whiteCellPos->second);
     
//      return NULL;
//   }

//   cGRID_VERTEX *cGRID_OBJECT_FACTORY ::GridVertex(iGRID_VERTEX vertexIndex)
//   {
//     std::map<iGRID_VERTEX, INT>::iterator vertexPos = m_verticesMap.find(vertexIndex);
//     if(vertexPos == m_verticesMap.end())
//       return NULL;
//     else
//       return m_vertices.object_at(vertexPos->second);
    
//     return NULL;
//   }

  cGRAY_CELL *cGRID_OBJECT_FACTORY ::AddGrayCell(iCELL_INDEX cellIndex)
  {
    cGRAY_CELL *grayCell = m_grayCells.new_object();
    m_grayCellsMap[cellIndex] = m_grayCells.size() - 1;
    return grayCell;
  }

//   cWHITE_CELL *cGRID_OBJECT_FACTORY ::AddWhiteCell(iCELL_INDEX cellIndex)
//   {
//     cWHITE_CELL* whiteCell = m_whiteCells.new_object();
//     m_whiteCellsMap[cellIndex] = m_whiteCells.size() - 1;
//     return whiteCell;
//   }

//   cGRID_VERTEX *cGRID_OBJECT_FACTORY ::AddGridVertex(iGRID_VERTEX vertexIndex)
//   {
//     cGRID_VERTEX *vertex = m_vertices.new_object();
//     m_verticesMap[vertexIndex] = m_vertices.size() - 1;
//     return vertex;
//   }

  VOID cGRID_OBJECT_FACTORY::Destroy() {
//     m_vertices.clear();
    m_grayCells.clear();
//     m_whiteCells.clear();
    m_grayCellsMap.clear();
//     m_whiteCellsMap.clear();
  }

}
