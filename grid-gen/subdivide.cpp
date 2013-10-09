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

      bigCellClay.Vertex(i)->Color(m_cell->Color(p, q, r));
    }

    //Imprint mid plane in big cell clay.
    REAL val = m_box.Center()[m_coord];

    for (INT i = 0; i < 6; i++)
      tMESH_CLIPPER<cCUT_CELL_CLAY>(&bigCellClay).Clip(i, m_coord, val);

    cCUT_CELL_CLAY childClay[2];
    Split(bigCellClay, childClay);
    return true;

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
  VOID tSUB_DIVIDE<_GRID_TYPE, _GRID_CELL>::ClipManifolds()
  {

  }
}
/* namespace grid_gen */
