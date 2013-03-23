/*
 * hole_filler.hpp
 *
 *  Created on: Nov 28, 2011
 */

#ifndef _SURFACE_MESH_HOLE_FILLER_HPP_
#define _SURFACE_MESH_HOLE_FILLER_HPP_

#include <queue>
using namespace std;

#include "surface_mesh_class.hpp"

  class cHOLE_VERTEX_RECORD {
  public:
    cHOLE_VERTEX_RECORD();
    VOID VertexIndex(cSURFACE_MESH::cVERTEX* vertex) {VertexIndex(vertex->Index());}
    VOID VertexIndex(iVERTEX vertexIndex) {m_vertex_index = vertexIndex;}
    VOID IncomingHE(cSURFACE_MESH::cHALF_EDGE*  inHE) {m_in_half_edge = inHE;}
    VOID OutgoingHE(cSURFACE_MESH::cHALF_EDGE*  outHE) {m_out_half_edge = outHE;}
    cSURFACE_MESH::cHALF_EDGE *IncomingHE() {return m_in_half_edge;}
    cSURFACE_MESH::cHALF_EDGE *OutgoingHE() {return m_out_half_edge;}
    REAL ComputeAngleCosine(cSURFACE_MESH *m_mesh);
    REAL AngleCosine() const {return m_angle_cosine;}
    iVERTEX VertexIndex() const {return m_vertex_index;}

  private: //fields
    iVERTEX m_vertex_index;
    cSURFACE_MESH::cHALF_EDGE *m_in_half_edge;
    cSURFACE_MESH::cHALF_EDGE *m_out_half_edge;
    REAL m_angle_cosine;
  };

  struct sHOLE_VERTEX_RECORD_CMP {
    BOOL operator() ( const cHOLE_VERTEX_RECORD &hvr1, const cHOLE_VERTEX_RECORD &hvr2) {
      if (fabs(hvr1.AngleCosine() - hvr2.AngleCosine()) <= cLIMITS::Epsilon())
      return hvr1.VertexIndex() < hvr2.VertexIndex();
      else
        return hvr1.AngleCosine() < hvr2.AngleCosine();
    }
  };

  class cHOLE_FILLER {


  public:

    cHOLE_FILLER(cSURFACE_MESH *mesh); //constructor
    BOOL Perform();

  private: //methods

    BOOL CloseHole1();
    BOOL FindCycle();
    BOOL FillHole();
    VOID CollectCycleVertices(iVERTEX terminalVtx,
                              BOOL checkTerminalVtx);
    VOID PrepareVetexRecords();
    cSURFACE_MESH::cHALF_EDGE*
      RegisterVertexRecord(cSURFACE_MESH::cHALF_EDGE* currBorderHE);
    INT FillAtSmallestAngleVertex();
    INT FillAtVertexZeroTriangles(cHOLE_VERTEX_RECORD &vertexRecord);
    INT FillAtVertexOneTriangle(cHOLE_VERTEX_RECORD &vertexRecord);
    INT FillAtVertexTwoTriangles(cHOLE_VERTEX_RECORD &vertexRecord);
    INT FillAtVertexThreeTriangles(cHOLE_VERTEX_RECORD &vertexRecord);
//    std::list<cHOLE_VERTEX_RECORD>::iterator FindSmallestAngleVertexRecord();

  private: //fields

    cSURFACE_MESH *m_mesh;
    std::vector<BOOL> m_vertexVisited;
    std::vector<iVERTEX> m_cycleVertices;
    std::stack<cSURFACE_MESH::cHALF_EDGE*> m_boundaryHalfEdges;
    std::priority_queue<cHOLE_VERTEX_RECORD, std::vector<cHOLE_VERTEX_RECORD>,  sHOLE_VERTEX_RECORD_CMP  > m_vertex_records;
    REAL m_cosine_0; //adjacent edges are merged
    REAL m_cosine_1; //generates a single triangle
    REAL m_cosine_2; //generates two triangles
    REAL m_small_edge_length_squared; //size of a degenerate edge to be immediately removed
    REAL m_comparable_edge_size_coeff;
    REAL m_max_half_edge_squared_length;
    REAL m_min_half_edge_squared_length;
    iFACET m_first_hole_facet_index;
    iVERTEX m_first_hole_vertex_index;
  };

#endif // _SURFACE_MESH_HOLE_FILLER_HPP_
