/*
 * subdivide.cpp
 *
 *  Created on: Sep 30, 2013
 *      Author: venkat
 */

#include "subdivide.hpp"

namespace grid_gen {

  template<typename _GRID_TYPE, typename _GRID_CELL>
  BOOL tSUB_DIVIDE<_GRID_TYPE, _GRID_CELL>::Do() {
    cCUT_CELL_CLAY bigCellClay;
    BoxToMesh(m_box, bigCellClay);

    //Initialize big cell clay colors.
    for (INT i = 0; i < 8; i++) {
      INT p = vertex_enumeration[i][0];
      INT q = vertex_enumeration[i][1];
      INT r = vertex_enumeration[i][2];

      bigCellClay.Vertex(i)->Color(m_cell->VertexColor(p, q, r));
    }

    //Imprint mid plane in big cell clay.
    REAL val = m_box.Center()[m_coord];

    for (INT i = 0; i < 6; i++)
      tMESH_CLIPPER<cCUT_CELL_CLAY>(&bigCellClay).Clip(i, m_coord, val);

    //Split big cell box into small boxes.
    cCUT_CELL_CLAY childClay[2];
    Split(bigCellClay, childClay);

    //Clip all facets.
    m_cell->Clip(m_coord, val);

    _GRID_CELL *childCell1 = m_grid->AddCell();
    _GRID_CELL *childCell2 = m_grid->AddCell();

    childCell1->Box(childClay[0].BoundingBox());
    childCell2->Box(childClay[1].BoundingBox());

    DistributeManifolds(m_cell, childCell1, childCell2);
    return true;
  }

  template<typename _GRID_TYPE, typename _GRID_CELL>
  VOID tSUB_DIVIDE<_GRID_TYPE, _GRID_CELL>::DistributeManifolds(_GRID_CELL *parentCell, _GRID_CELL *leftCell, _GRID_CELL *rightCell) {
    typename _GRID_CELL::entry_iterator currEntryItr = parentCell->begin();
    typename _GRID_CELL::entry_iterator lastEntryItr = parentCell->end();

    for (; currEntryItr != lastEntryItr; currEntryItr++) {
      DistributeManifold(currEntryItr.operator*(), leftCell, rightCell);
    }
  }

  template<typename _GRID_TYPE, typename _GRID_CELL>
  VOID tSUB_DIVIDE<_GRID_TYPE, _GRID_CELL>::DistributeManifold(cGRID_ENTRY *entry, _GRID_CELL *leftCell, _GRID_CELL *rightCell) {
    INT nCells[3] = { 1, 1, 1 };
    nCells[m_coord] = 2;
    cSMALL_GRID localGrid(m_box, nCells);

    iCELL_INDEX leftCellIndex = localGrid.CellIndex(leftCell->Box().Center());
    iCELL_INDEX rightCellIndex = localGrid.CellIndex(rightCell->Box().Center());

    std::vector<iFACET> &facets = entry->Facets();

    typename std::vector<iFACET>::iterator currFacet = facets.begin();
    typename std::vector<iFACET>::iterator lastFacet = facets.end();

    typedef typename cGRID_ENTRY::cMESH cMESH;

    cMESH* mesh = entry->Mesh();
    typename cGRID_ENTRY::cMANIFOLD_RECORD *record = entry->Record();

    for (; currFacet != lastFacet; currFacet++) {
      iCELL_INDEX childIndices[2];
      typename cMESH::cFACET *facet = mesh->Facet(*currFacet);
      localGrid.ModifiedCellIndex(facet->MeanPoint(), facet->Normal(), childIndices);

      if (childIndices[0] == leftCellIndex) {
        leftCell->Register(record, *currFacet);
      } else {
        rightCell->Register(record, *currFacet);
      }
    }
  }

  template<typename _GRID_TYPE, typename _GRID_CELL>
  VOID tSUB_DIVIDE<_GRID_TYPE, _GRID_CELL>::Split(INT facetIndex, std::vector<bool> &visitedFacets, cCUT_CELL_CLAY &parentClay,
                                                  cCUT_CELL_CLAY &childClay) {
    std::vector<iFACET> childFacets;
    std::vector<bool> visitedNeighbors(parentClay.LargestFacetIndex() + 1, false);

    cCUT_CELL_CLAY::cFACET* facet = parentClay.Facet(facetIndex);

    INT currIndex = 0;
    childFacets.push_back(facet->Index());

    while (currIndex < childFacets.size()) {
      facet = parentClay.Facet(childFacets[currIndex]);
      if (!visitedFacets[facet->Index()]) {
        visitedNeighbors[facet->Index()] = true;
        visitedFacets[facet->Index()] = true;

        cCUT_CELL_CLAY::cHALF_EDGE *halfEdge = facet->HalfEdge();
        cCUT_CELL_CLAY::cHALF_EDGE *loopHe = halfEdge;

        do {
          cCUT_CELL_CLAY::cHALF_EDGE *oppHe = loopHe->Opp();
          if (loopHe->Tail()->Index() < 8 || loopHe->Head()->Index() < 8) {
            cCUT_CELL_CLAY::cFACET *oppFacet = oppHe->Facet();
            if (!visitedNeighbors[oppFacet->Index()]) {
              childFacets.push_back(oppFacet->Index());
              visitedNeighbors[oppFacet->Index()] = true;
            }
          }

          loopHe = loopHe->Next();
        } while (loopHe != halfEdge);
      }
      currIndex++;
    }

    MeshFromFacets(childFacets, parentClay, childClay);
  }

  template<typename _GRID_TYPE, typename _GRID_CELL>
  VOID tSUB_DIVIDE<_GRID_TYPE, _GRID_CELL>::Split(cCUT_CELL_CLAY &bigCellClay, cCUT_CELL_CLAY children[2]) {
    std::vector<bool> visitedFacets(bigCellClay.LargestFacetIndex() + 1, false);
    INT numChildren = 0;
    for (INT i = 0; i < bigCellClay.LargestFacetIndex() + 1; i++) {
      if (!visitedFacets[i])
        Split(i, visitedFacets, bigCellClay, children[numChildren++]);

    }
    Close<cCUT_CELL_CLAY>(&children[0]);
    Close<cCUT_CELL_CLAY>(&children[1]);

    VerticesColor(bigCellClay, children[0]);
    VerticesColor(bigCellClay, children[1]);
    ASSERT(numChildren == 2);
  }

  template<typename _GRID_TYPE, typename _GRID_CELL>
  VOID tSUB_DIVIDE<_GRID_TYPE, _GRID_CELL>::VerticesColor(cCUT_CELL_CLAY &parentClay, cCUT_CELL_CLAY &childClay) {
    cCUT_CELL_CLAY::vertex_iterator currVtx = childClay.VerticesBegin();
    cCUT_CELL_CLAY::vertex_iterator lastVtx = childClay.VerticesEnd();

    for (; currVtx != lastVtx; currVtx++) {
      for (INT i = 0; i < 8; i++) {
        if (currVtx->Point() == parentClay.Vertex(i)->Point()) {
          currVtx->Color(parentClay.Vertex(i)->Color());
          break;
        }
      }
    }
  }

  template<typename _GRID_TYPE, typename _GRID_CELL>
  VOID tSUB_DIVIDE<_GRID_TYPE, _GRID_CELL>::RegisterOnClayEdges(cCUT_CELL_CLAY &clay, const cBOX3 &clayBox, const cPOINT3 &point,
                                                                iVERTEX meshVertexIndex) {
    INT edge = clayBox.IsOnEdge(point);
    if (edge < 0)
      return;

    INT tailIndex, headIndex, newVertexIndex;
    tailIndex = headIndex = newVertexIndex = INVALID_IVERTEX;

    ASSERT(edge >= 0 && edge < 12);

    INT cellFaceIndex = edge_faces[edge][0];

    cCUT_CELL_CLAY::cFACET* cellFace = clay.Facet(cellFaceIndex);

    cCUT_CELL_CLAY::cFACET::half_edge_circulator currHalfEdge = cellFace->HalfEdgesBegin();
    cCUT_CELL_CLAY::cFACET::half_edge_circulator lastHalfEdge = cellFace->HalfEdgesEnd();

    for (; currHalfEdge != lastHalfEdge; currHalfEdge++) {
      cSEGMENT3 edgeSegment = currHalfEdge->Segment();

      //Found the half-edge to insert the point.
      if (edgeSegment.HasOn(point)) {
        INT endPointIndex = edgeSegment.IsEndPoint(point);
        if (endPointIndex >= 0) {
          cCUT_CELL_CLAY::cVERTEX *vertex = NULL;
          if (endPointIndex == 0)
            vertex = currHalfEdge->Tail();
          else
            vertex = currHalfEdge->Head();

          if (vertex->Index() < 8) {
            clay.Vertex(vertex->Index())->Color(GRAY);
//            m_clay.Vertex(vertex->Index())->Source(meshVertexIndex, m_manifold->Mesh());
          }

        } else {
          //Insert it on half-edge
          tailIndex = currHalfEdge->Tail()->Index();
          headIndex = currHalfEdge->Head()->Index();

          newVertexIndex = clay.NewVertex(point);

          ASSERT(tailIndex != INVALID_IVERTEX && headIndex != INVALID_IVERTEX && newVertexIndex != INVALID_IVERTEX);

          clay.SplitEdge(newVertexIndex, tailIndex, headIndex);
          //m_meshVertexIndices[newVertexIndex] = meshVertexIndex;
//          m_clay.Vertex(newVertexIndex)->Source(meshVertexIndex, m_manifold->Mesh());
        }

        return;
      }
    }
  }

  /*
   *  Registers facet vertices on cellMesh.
   */
  template<typename _GRID_TYPE, typename _GRID_CELL>
  VOID tSUB_DIVIDE<_GRID_TYPE, _GRID_CELL>::RegisterEntryVertices(cCUT_CELL_CLAY &clay, cGRID_ENTRY *entry) {
    /*
     * Nothing to register for a white or black cell.
     */
    if (entry == NULL)
      return;

    cBOX3 clayBox = clay.BoundingBox();

    typedef typename cGRID_ENTRY::cMESH cMESH;
    cMESH *facetMesh = entry->Mesh();
    std::vector<iFACET> &facets = entry->Facets();

    std::vector<iFACET>::iterator currFacet = facets.begin();
    std::vector<iFACET>::iterator lastFacet = facets.end();

    //printf("Facets : ");
    for (; currFacet != lastFacet; currFacet++) {
      typename cMESH::cFACET *facet = facetMesh->Facet(*currFacet);
      //printf("%d ", facet->Index());

      typename cMESH::cFACET::vertex_circulator currVertex = facet->VerticesBegin();
      typename cMESH::cFACET::vertex_circulator lastVertex = facet->VerticesEnd();

      for (; currVertex != lastVertex; currVertex++)
        RegisterOnClayEdges(clay, clayBox, currVertex->Point(), currVertex->Index());
    }

    //printf("\n");
  }

}
/* namespace grid_gen */
