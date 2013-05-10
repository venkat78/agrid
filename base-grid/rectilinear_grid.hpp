#ifndef _RECTILINEAR_GRID_HPP_
#define _RECTILINEAR_GRID_HPP_

#include "defs.hpp"

namespace base_grid {

//  class cRECTILINEAR_GRID {
//  public: //constructors:
//    /*
//     * constructors for rectilinear grid.
//     */
//    cRECTILINEAR_GRID(); //default
//    cRECTILINEAR_GRID(const cBOX3 &simVol,
//		      INT numCells[3]);
//    cRECTILINEAR_GRID(const cBOX3 &simVol,
//		      INT numCells);
//    //more constructors to be added for gradation
//    //destructor:
//
//  public: //methods
//    /*
//     * Index related methods.
//     */
//    iCELL_INDEX CellIndex(const cPOINT3 &point);
//    INT ModifiedCellIndex(const cPOINT3 &point, cVECTOR3 normal,
//			  iCELL_INDEX indices[2]);
//
//    iCELL_INDEX CellIndex(REAL x, REAL y, REAL z);
//
//    /*
//     * Cell length related methods.
//     */
//    REAL CellLength(INT i) { return m_cellSize[i]; }
//    REAL CellLengthX() { return m_cellSize[0]; }
//    REAL CellLengthY() { return m_cellSize[1]; }
//    REAL CellLengthZ() { return m_cellSize[2]; }
//    REAL CellLengthMin() { REAL min01 = ((m_cellSize[0] < m_cellSize[1]) ? m_cellSize[0] : m_cellSize[1]);
//      return ((min01 < m_cellSize[2]) ? min01 : m_cellSize[2]); }
//
//    REAL CellLengthMax() {
//      REAL max01 = ((m_cellSize[0] > m_cellSize[1]) ? m_cellSize[0] : m_cellSize[1]);
//      return ((max01 > m_cellSize[2]) ? max01 : m_cellSize[2]);
//    }
//
//    INT NumCells(INT i) { return m_numCells[i]; }
//
//    cBOX3 CellBox(iCELL_INDEX cellIndex);
//    INT NumCells() {
//      return m_numCells[0] * m_numCells[1] * m_numCells[2] ;
//    }
//
//    /*
//     * Vertex related methods
//     */
//    INT NumVertices() {
//      return (m_numCells[0] + 1) * (m_numCells[1] + 1) * (m_numCells[2] + 1);
//    }
//
//    cPOINT3 VertexPoint(INT indices[3]) {
//      cPOINT3 &min = m_simVol.Min();
//
//      return cPOINT3(min[0] + (indices[0] * m_cellSize[0]),
//		     min[1] + (indices[1] * m_cellSize[1]),
//		     min[2] + (indices[2] * m_cellSize[2]));
//    }
//
//    cPOINT3 VertexPoint(iGRID_VERTEX index) {
//      INT indices[3];
//      indices[0] = index.x ; indices[1] = index.y ; indices[2] = index.z;
//      return VertexPoint(indices);
//    }
//
//
//    VOID VertexIndex(const cPOINT3 &point, INT indices[3]) {
//      cPOINT3 &min = m_simVol.Min();
//
//      for(INT i = 0; i < 3 ; i++) {
//	indices[i] = (INT)((point[i] - min[i])/m_cellSize[i]);
//      }
//    }
//
//    //Assumes point is a grid vertex.
//    iGRID_VERTEX VertexIndex(const cPOINT3 &point) {
//      INT indices[3];
//      VertexIndex(point, indices);
//      return iGRID_VERTEX(indices[0], indices[1], indices[2]);
//    }
//
//    REAL CellUpperBound(REAL val, eCOORD coord);
//    REAL CellLowerBound(REAL val, eCOORD coord);
//
//  public: //methods
//    VOID Print();
//
//    INT Index(INT index_x, INT index_y, INT index_z);
//    INT Index(const INT index[3]);
//    cBOX3 SimulationVolume() { return m_simVol; }
//
//    BOOL IsValid(iCELL_INDEX index) {
//      return ((0 <= index.x && index.x < m_numCells[0]) &&
//	      (0 <= index.y && index.y < m_numCells[1]) &&
//	      (0 <= index.z && index.z < m_numCells[2]));
//    }
//  public:
//    //Methods related to cell and vertex colors.
//
//    VOID CellColor(iCELL_INDEX index, eCELL_COLOR color) {
//      m_cellColors[index] = color;
//    }
//
//    eCELL_COLOR CellColor(iCELL_INDEX index) ;
//
//    eCELL_COLOR VertexColor(iGRID_VERTEX index);
//  public:
//    //Methods related to walking.
//    VOID Walk(BOOL markBlackCells = false);
//    VOID ClearColorMarkers() {
//      m_cellColors.clear();
//      m_vertexColors.clear();
//    }
//
//  private:
//    VOID CollectWhiteVertices();
//    VOID AdvancingFront(std::vector<iCELL_INDEX> &, cGRID_CELL_MARKS &);
//    VOID AddToFront(eCELL_COLOR, iCELL_INDEX, std::vector<iCELL_INDEX> &, cGRID_CELL_MARKS &);
//    VOID MarkBlackCells( cGRID_CELL_MARKS &visitedCells);
//
//  private: //members:
//    INT	 m_numCells[3];
//    REAL m_cellSize[3];
//
//    cBOX3 m_simVol;
//
//    //First gray cells should always be marked.
//    //Then white cells will be marked.
//    cGRID_CELL_COLORS m_cellColors;
//
//    //This is to hold colors of white vertices.
//    cGRID_VERTEX_COLORS m_vertexColors;
//  };	//cRECTILINEAR_GRID
//
}

#endif //_RECTILINEAR_GRID_HPP_
