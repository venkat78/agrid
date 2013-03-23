/*
 * planar_cutter.cpp
 *
 *  Created on: Jan 7, 2011
 */

//#include "defs.hpp"
#include "planar_cutter.hpp"

namespace agrid_boolean {
 cPLANAR_CUTTER::cPLANAR_CUTTER()
  {
    m_mesh = NULL;
    m_debug = false;
    m_startingVertex = NULL;
    m_currentEndVertex = NULL;
    m_lastHE = NULL;
    m_closestIntersectionIsHEHead = false;
    m_edgeOnFacet = false;
  }
//starting vertex is given
  cPLANAR_CUTTER::cPLANAR_CUTTER(cCUT_CELL *mesh,
      cCUT_CELL::cVERTEX *startingVertex,
      cPLANE3 &cuttingPlane)
  {
    m_mesh = mesh;
    m_debug = false;
    m_startingVertex = startingVertex;
    m_currentEndVertex = NULL;
    m_lastHE = NULL;
    m_cuttingPlane = cuttingPlane;
    m_closestIntersectionIsHEHead = false;
    m_edgeOnFacet = false;
    assert(false);  //implemented, but not thoroughly tested
  }

  //no initial info isgiven
  cPLANAR_CUTTER::cPLANAR_CUTTER(cCUT_CELL *mesh, cPLANE3 &cuttingPlane)
  {
    m_mesh = mesh;
    m_debug = false;
    m_currentEndVertex = NULL;
    m_lastHE = NULL;
    m_cuttingPlane = cuttingPlane;
    m_closestIntersectionIsHEHead = false;
    m_edgeOnFacet = false;
    assert(false);  //not implemented
  }

  //starting he is given
  cPLANAR_CUTTER::cPLANAR_CUTTER(cCUT_CELL *mesh, cCUT_CELL::cHALF_EDGE *startingHalfEdge,
				 cPLANE3 &cuttingPlane, std::vector<cCUT_CELL*> *cutCells)
  {
    m_mesh = mesh;
    m_debug = false;
    m_startingVertex = startingHalfEdge->Head();
    m_currentEndVertex = NULL;
    m_lastHE = startingHalfEdge;
    m_cuttingPlane = cuttingPlane;
    m_closestIntersectionIsHEHead = false;
    m_edgeOnFacet = false;
    m_cutCells = cutCells;
  }

  cCUT_CELL *cPLANAR_CUTTER::SplitMesh()
  {
    cPOINT3 loopStartPoint = m_startingVertex->Point();
    m_currentEndVertex = m_startingVertex;

    INT step = 1;
    BOOL loopIsClosed = false;
    while(!loopIsClosed){
      cCUT_CELL::cHALF_EDGE *intersectingHE = IntersectingHalfEdge();
      if (Debug()){
        if (intersectingHE)
          printf("intersectingHE : t%d h%d\n",
              intersectingHE->Tail()->Index(),
              intersectingHE->Head()->Index());
      }
      m_lastHE = SplitFacet(intersectingHE);
      if (Debug()){
        printf("m_lastHE: t%d h%d\n",
            m_lastHE->Tail()->Index(),
            m_lastHE->Head()->Index());
        printf("f0 %d f1 %d\n",
            m_lastHE->Facet()->Index(),
            m_lastHE->Opp()->Facet()->Index());
      }
      m_halfEdges.push_back(m_lastHE);
//      if (Debug()){
//        char fname[32] = {'\0'};
//        sprintf(fname, "mesh%d.off", step++);
//        ExportOff(*m_mesh, fname);
//      }
      if (Debug())
        printf("m_currentEndVertex= %d; m_startingVertex = %d\n",
            m_currentEndVertex->Index(), m_startingVertex->Index());
      if (m_currentEndVertex == m_startingVertex)
        loopIsClosed = true;
    }
    FormPlaneFacets();
    return m_mesh;
  }

  cCUT_CELL::cHALF_EDGE* cPLANAR_CUTTER::IntersectingHalfEdge()
  {
    m_closestIntersectionIsHEHead = false;
    m_edgeOnFacet = false;
    cCUT_CELL::cFACET *f0 = m_lastHE ? m_lastHE->Facet() : NULL;
    cCUT_CELL::cFACET *f1 = m_lastHE ? m_lastHE->Opp()->Facet() : NULL;
    if (Debug())
/*      printf("lastHe = (t%d; h%d}; currentEndVertex %d\n",
          m_lastHE ? m_lastHE->Tail()->Index() : -1,
          m_lastHE ? m_lastHE->Head()->Index() : -1,
          m_currentEndVertex->Index());*/
    printf("lastHe = (t%d; h%d}; f0 %d; f1 %d; currentEndVertex %d\n",
        m_lastHE ? m_lastHE->Tail()->Index() : -1,
        m_lastHE ? m_lastHE->Head()->Index() : -1,
        f0 ? f0->Index() : -1, f1 ? f1->Index() : -1,
        m_currentEndVertex->Index());
    std::vector<cCUT_CELL::cFACET*> incidentFacets;
    m_currentEndVertex->IncidentFacets(incidentFacets);
    REAL minDist = cLIMITS::Max();
    std::vector<cCUT_CELL::cFACET*>::iterator currFacet = incidentFacets.begin();
    std::vector<cCUT_CELL::cFACET*>::iterator lastFacet = incidentFacets.end();
    cCUT_CELL::cHALF_EDGE *minDistHE = NULL;

    for ( ; currFacet != lastFacet ; currFacet++) {
      cCUT_CELL::cFACET *f = *currFacet;
     if (Debug())
        printf("f = %d\n", f->Index());
      if (f != f0 && f != f1){
        REAL distFacetPlane = cLIMITS::Max();
        cPOINT3 intersectionPoint;
        BOOL intersectionIsHEHead;
        cCUT_CELL::cHALF_EDGE *intersectingHE = FacetHEPlaneIntersection(f,
            &distFacetPlane, &intersectionPoint, &intersectionIsHEHead);
        if (intersectingHE != NULL && distFacetPlane < minDist){
          minDistHE = intersectingHE;
          minDist = distFacetPlane;
          if (Debug()){
            printf("minDistHE h%d t%d\n",
                minDistHE->Tail()->Index(), minDistHE->Head()->Index());
            printf("intersectionPoint"); intersectionPoint.Print();
          }
          m_intersectionPoint = intersectionPoint;
          m_closestIntersectionIsHEHead = intersectionIsHEHead;
          if (m_edgeOnFacet)
            return minDistHE;
        }
      }
    }
    return minDistHE;
  }

  //returns the half-edge containing the closest intersection point of the plane and
  //the facet half-edge. SInce facet may be concacve, all the half edges need to be tested,
  //and the one with the closest intersection point is returned
  cCUT_CELL::cHALF_EDGE* cPLANAR_CUTTER::FacetHEPlaneIntersection(cCUT_CELL::cFACET *facet,
      REAL *p_distFacetPlane,
      cPOINT3 *p_intersectionPoint,
      BOOL *p_closestIntersectionIsHEHead)
  {
    REAL minDist = cLIMITS::Max();
    cCUT_CELL::cHALF_EDGE *minDistHE = NULL;
    BOOL closestIntersectionIsHEHead = false;
    cPOINT3 closestIntersectionPoint;

    if (Debug())
      printf("m_currentEndVertex %d\n", m_currentEndVertex->Index());
    cCUT_CELL::cFACET::half_edge_circulator currHalfEdge = facet->HalfEdgesBegin();
    cCUT_CELL::cFACET::half_edge_circulator lastHalfEdge = facet->HalfEdgesEnd();

    for ( ; currHalfEdge != lastHalfEdge ; currHalfEdge++ ) {
      cCUT_CELL::cHALF_EDGE *he = currHalfEdge.operator->();
      if (Debug())
        printf("he t%d h%d\n", he->Tail()->Index(), he->Head()->Index());
      if (he->Head() != m_currentEndVertex){
        cPOINT3 intersectionPoint(1, 2, 3);
        eRETURN_TYPE hePlaneIntersection =
                  m_cuttingPlane.SegmentIntersect(he->Segment(), &intersectionPoint);
        if (hePlaneIntersection == RT_SEGMENT3){
          if (he->Tail() == m_currentEndVertex){
            minDist = 0.0;
            minDistHE = he;
            closestIntersectionPoint = he->Head()->Point();
            closestIntersectionIsHEHead = true;
            m_edgeOnFacet = true;
            break;
          }
          else {
            REAL dist = SquaredDistance(m_currentEndVertex->Point(), he->Head()->Point());
            if (dist < minDist){
              minDist = dist;
              minDistHE = he;
              closestIntersectionPoint = he->Head()->Point();
              closestIntersectionIsHEHead = true;
            }
          }
        }
        else if (he->Tail() != m_currentEndVertex &&
            (hePlaneIntersection == RT_POINT3 || hePlaneIntersection == RT_ENDPOINT3)){
          REAL dist = SquaredDistance(m_currentEndVertex->Point(), intersectionPoint);
          if (Debug()){
            printf("intersectionPoint: "); intersectionPoint.Print();
            printf("dist %lg\n", m_cuttingPlane.SignedDistance(intersectionPoint));
          }
          if (dist < minDist){
            minDist = dist;
            minDistHE = he;
            closestIntersectionPoint = intersectionPoint;
            closestIntersectionIsHEHead = (hePlaneIntersection == RT_ENDPOINT3);
          }
        }
      }
    }
    if (minDistHE){
      if (p_distFacetPlane != NULL)
        *p_distFacetPlane = minDist;
      if (p_intersectionPoint != NULL)
        *p_intersectionPoint = closestIntersectionPoint;
      if (p_closestIntersectionIsHEHead != NULL)
        *p_closestIntersectionIsHEHead = closestIntersectionIsHEHead;
    }
    return minDistHE;
  }

  cCUT_CELL::cHALF_EDGE* cPLANAR_CUTTER::SplitFacet(cCUT_CELL::cHALF_EDGE *intersectingHE)
  {
//    if (Debug()){
//      printf("intersectingHE: t%d h%d\n",
//          intersectingHE->Tail()->Index(), intersectingHE->Head()->Index());
//    }

    iFACET facetToBeSplit = intersectingHE->Facet()->Index();

    cCUT_CELL::cVERTEX *intersectionVertex = NULL;
    if (m_closestIntersectionIsHEHead)
      intersectionVertex = intersectingHE->Head();
    else {
      iVERTEX vertexLabel = m_mesh->NewVertex(m_intersectionPoint);
      intersectionVertex = m_mesh->Vertex(vertexLabel);
    }

    if (Debug())
      printf("edge %d %d; insert %d\n",
        intersectingHE->Tail()->Index(), intersectingHE->Head()->Index(), intersectionVertex->Index());
    if (!m_closestIntersectionIsHEHead)
      m_mesh->SplitEdge(intersectionVertex->Index(),
        intersectingHE->Tail()->Index(),
        intersectingHE->Head()->Index());
    if (!m_edgeOnFacet)
      m_mesh->InsertDiagonal(facetToBeSplit,
        m_currentEndVertex->Index(), intersectionVertex->Index());

    if (Debug()){
      printf("m_currentEndVertex: %d; ", m_currentEndVertex->Index());
      m_currentEndVertex->Point().Print();
    }
    //find half edge connecting m_currentEndVertex and  intersectionVertex
    //which now is half edge of the split facet or its opposite

    cCUT_CELL::cFACET *splitFacet = m_mesh->Facet(facetToBeSplit);
    cCUT_CELL::cFACET::half_edge_circulator currHalfEdge = splitFacet->HalfEdgesBegin();
    cCUT_CELL::cFACET::half_edge_circulator lastHalfEdge = splitFacet->HalfEdgesEnd();

    for ( ; currHalfEdge != lastHalfEdge ; currHalfEdge++ ) {
      cCUT_CELL::cHALF_EDGE *he = currHalfEdge.operator->();
      if (he->Tail() == m_currentEndVertex && he->Head() == intersectionVertex){
        m_currentEndVertex = intersectionVertex;
        return he;
      }
      else if (he->Tail() == intersectionVertex && he->Head() == m_currentEndVertex){
        m_currentEndVertex = intersectionVertex;
        return he->Opp();
      }

    }
    m_currentEndVertex = intersectionVertex;
    return NULL;
  }

  VOID cPLANAR_CUTTER::FormPlaneFacets()
  {
    m_mesh->NewFacetStart();
    std::list<cCUT_CELL::cHALF_EDGE*>::iterator currHE = m_halfEdges.begin();
    std::list<cCUT_CELL::cHALF_EDGE*>::iterator lastHE = m_halfEdges.end();
    for (; currHE != lastHE; currHE++){
      if (Debug())
        printf("currHE t%d h%d\n",
            (*currHE)->Tail()->Index(), (*currHE)->Head()->Index());
      m_mesh->NewFacetAddVertex((*currHE)->Tail()->Index());
    }
    m_mesh->NewFacetClose();

    m_mesh->NewFacetStart();
    std::list<cCUT_CELL::cHALF_EDGE*>::reverse_iterator rcurrHE = m_halfEdges.rbegin();
    std::list<cCUT_CELL::cHALF_EDGE*>::reverse_iterator rlastHE = m_halfEdges.rend();
    for (; rcurrHE != rlastHE; rcurrHE++){
      if (Debug())
        printf("rcurrHE t%d h%d\n",
            (*rcurrHE)->Tail()->Index(), (*rcurrHE)->Head()->Index());
      m_mesh->NewFacetAddVertex((*rcurrHE)->Tail()->Index());
    }
    m_mesh->NewFacetClose();
//    if(Debug())
//      ExportOff(*m_mesh, "meshfinal.off");
  }
  }
