/*
 * planar_cutter.hpp
 *
 *  Created on: Jan 7, 2011
 */

//this class is used for cutting a mesh by a plane. If the initial point of the cut is given,
//the minimal loop starting and terminating at this point, is created.
#ifndef _AGRID_BOOLEAN_PLANAR_CUTTER_HPP_
#define _AGRID_BOOLEAN_PLANAR_CUTTER_HPP_

#include "defs.hpp"

namespace agrid_boolean {
  class cPLANAR_CUTTER {
  public: //constructors
    cPLANAR_CUTTER();

    cPLANAR_CUTTER(cCUT_CELL *mesh, cCUT_CELL::cVERTEX *startingVertex,
        cPLANE3 &cuttingPlane);
    cPLANAR_CUTTER(cCUT_CELL *mesh, cCUT_CELL::cHALF_EDGE *startingHalfEdge,
		   cPLANE3 &cuttingPlane, std::vector<cCUT_CELL*> *cutCells);
    cPLANAR_CUTTER(cCUT_CELL *mesh, cPLANE3 &cuttingPlane);
    cCUT_CELL *Mesh() {return m_mesh;}

  public: //methods

    cCUT_CELL *SplitMesh();
    VOID Debug(BOOL flag) {m_debug = flag;}

  private: //methods
    BOOL Debug() {return m_debug;}
    cCUT_CELL::cHALF_EDGE *IntersectingHalfEdge();
    cCUT_CELL::cHALF_EDGE *SplitFacet(cCUT_CELL::cHALF_EDGE *intersectingHE);
    cCUT_CELL::cHALF_EDGE* FacetHEPlaneIntersection(cCUT_CELL::cFACET *facet,
        REAL *p_distFacetPlane, cPOINT3 *p_intersectionPoint,
        BOOL *p_closestIntersectionIsHEHead);
    VOID FormPlaneFacets();

  private: //fields
    cCUT_CELL *m_mesh;
    cPLANE3 m_cuttingPlane;
    BOOL m_debug;
    cCUT_CELL::cVERTEX *m_startingVertex;
    cCUT_CELL::cVERTEX *m_currentEndVertex;
    cPOINT3 m_intersectionPoint;
    BOOL m_closestIntersectionIsHEHead;
    BOOL m_edgeOnFacet;
    std::vector<cCUT_CELL*> *m_cutCells;
    cCUT_CELL::cHALF_EDGE *m_lastHE;  //current last HE in loop; it is already in the list
    std::list<cCUT_CELL::cHALF_EDGE*> m_halfEdges; //half edges forming the loop
  };
}

#endif /* _MESH_PLANAR_CUTTER_HPP_ */
