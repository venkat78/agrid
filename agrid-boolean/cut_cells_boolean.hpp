#ifndef _AGRID_BOOLEAN_CUT_CELLS_BOOLEAN_HPP_
#define _AGRID_BOOLEAN_CUT_CELLS_BOOLEAN_HPP_

#include "defs.hpp"
#include "cell.hpp"
#include "manifold_record.hpp"

namespace agrid_boolean {
  class cCUT_CELLS_BOOLEAN {
    typedef tMESH_FACETS_OCTREE<cCUT_CELL_CLAY> cOCTREE;
    typedef std::pair<iVERTEX, iVERTEX> sEDGE;

  public:
    cCUT_CELLS_BOOLEAN(cGRAY_CELL *cell = NULL, 
                       REAL tol = 0.0) : m_grayCell(cell),
                                         m_box(cell->Box()),
                                         m_octree(m_box.GrowBy(2), &m_clay, 4, 4),
                                         m_lengthTol(tol)
    {}
    
    VOID Do();
    
  private:
    VOID Init();

    VOID Imprint(cCUT_CELL *cutCell);

    VOID ImprintManifold(cGRAY_CELL_ENTRY *entry);

    VOID RemoveBlackFaces();

    BOOL ExtractCutCell(INT facetIndex, 
			std::vector<bool> &visitedFacets);

    BOOL ExtractCutCells();

    VOID FindCommon();

    //Methods related to removing tiny edges during boolean
    VOID RemoveTinyEdge(iVERTEX v1, iVERTEX v2, BOOL moveToMidPoint);

    VOID RemoveEdge(sEDGE edge);

    VOID RemoveEdgeWithVertexOnLatticeEdge(sEDGE edge, INT latticeEdge2);

    VOID RemoveLatticeEdge(sEDGE edge);

    VOID RemoveEdges(tLIST<sEDGE> &tinyEdges);

    VOID CollectTinyEdges(tLIST<sEDGE> &tinyEdges);

    VOID RemoveTinyEdges();

  private:
    cGRAY_CELL *m_grayCell;
    cBOX3 m_box;
    cOCTREE m_octree;
    REAL m_lengthTol;
    cCUT_CELL_CLAY m_clay;
  };
}

#endif //_AGRID_BOOLEAN_CUT_CELLS_BOOLEAN_HPP_
