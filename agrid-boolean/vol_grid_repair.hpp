#ifndef _AGRID_BOOLEAN_VOL_GRID_REPAIR_HPP_
#define _AGRID_BOOLEAN_VOL_GRID_REPAIR_HPP_

#include <set>
#include "defs.hpp"

namespace agrid_boolean {
  class cVOLUMETRIC_GRID_REPAIR
  {
    typedef cVOLUMETRIC_GRID::cNM_EDGES cNM_EDGES;
    typedef cVOLUMETRIC_GRID::cHALF_EDGE cHALF_EDGE;
    typedef cVOLUMETRIC_GRID::cFACET cFACET;
    typedef tMESH_FACETS_OCTREE<cVOLUMETRIC_GRID> cOCTREE;

    struct sFACET {
      iFACET index;
      cPOINT3 centroid;
      BOOL operator < (const sFACET &f) const {
	return centroid < f.centroid;
      }
    };

  public:
    cVOLUMETRIC_GRID_REPAIR(const cBOX3 &bbox = cBOX3(),
			    cVOLUMETRIC_GRID *grid = NULL) : m_bbox(bbox.GrowBy(2)),
							     m_grid(grid),
							     m_octree(m_bbox, m_grid, 12, 10)
    {}

    /*
     *  Repairs the grid to make it topologically valid.
     */
    BOOL Do();

  private:
    struct sFILTER {
    public:
      sFILTER(const std::set<iFACET> &facets) : m_facets(facets)
      {}

      BOOL operator() (iFACET facetIndex)
      {
	if(m_facets.find(facetIndex) == m_facets.end())
	  return false;

	return true;
      }

      sCOLOR Color(iFACET facetIndex) {
	return sCOLOR();
      }

    private:
      const std::set<iFACET> &m_facets;
    };

    VOID PopulateTree();
    VOID VerifyOctree();

    /*
     * Checks the validity of the grid. Returns true if the grid is OK.
     */
    BOOL Check();
    BOOL FacetsTested(iFACET f1, iFACET f2);

    VOID AddProblemFacesAndCells(iFACET f1, iFACET f2);

    VOID ExportProblemsToOff(INT round);

    VOID PrintEdges();

    //Repair methods.
    VOID RemoveDuplicates();
    VOID Clip(const sVERTEX_PAIR &pair);
    VOID Clip();
    VOID Repair();

  private:
    cBOX3 m_bbox;
    cVOLUMETRIC_GRID *m_grid;
    cOCTREE m_octree;
    std::set<iFACET> m_problemFaces;
    std::set<iCELL_INDEX> m_problemCells;
    cEDGE_MARKS m_edges;
    cFACET_PAIRS m_pairs;
  };

}


#endif //_AGRID_BOOLEAN_VOL_GRID_REPAIR_HPP_
