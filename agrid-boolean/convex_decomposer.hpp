/*
 * convex_decomposer.hpp
 *
 *  Created on: Dec 31, 2010
 */

#ifndef _AGRID_BOOLEAN_CONVEX_DECOMPOSER_HPP_
#define _AGRID_BOOLEAN_CONVEX_DECOMPOSER_HPP_

//#ifdef YG_DID_NOT_COMPILE

#include "defs.hpp"
#include "cell.hpp"

//#include "cut_cell_grid.hpp"

namespace agrid_boolean{
  class cCONVEX_DECOMPOSER {
  public:  //constructor
    cCONVEX_DECOMPOSER();
    cCONVEX_DECOMPOSER(std::vector<cCUT_CELL*> *cutCells);

  public: //methods
    VOID Decompose();
    VOID Debug(BOOL flag) {m_debug = flag;}

  private: //methods
    BOOL DecomposeMesh(cCUT_CELL *mesh);
    BOOL FindConcaveEdge();
    VOID BuildSplittingPlane();
    VOID ClipMeshByPlane();
    BOOL Debug() {return m_debug;}
    
    
  private: //fields
    std::vector<cCUT_CELL*> *m_cutCells;
    std::vector<cCUT_CELL*>::iterator m_currCutCell;
    cCUT_CELL *m_mesh;
    cCUT_CELL::cHALF_EDGE *m_mostConcaveHE;
    cPLANE3 m_splittingPlane;
    BOOL m_debug;
  };
  
  VOID GenerateConvexPartitions(std::vector<cCUT_CELL*> &cutCells);
  VOID GenerateConvexPartitions(cGRAY_CELL_ENTRY *entry);
  VOID GenerateConvexPartitions(cGRAY_CELL *cell);

}//namespace
//#endif
#endif // _AGRID_BOOLEAN_CONVEX_DECOMPOSER_HPP_
