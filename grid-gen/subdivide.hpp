/*
 * subdivide.hpp
 *
 *  Created on: Sep 30, 2013
 *      Author: venkat
 */

#ifndef _GRID_GEN_SUBDIVIDE_HPP_
#define _GRID_GEN_SUBDIVIDE_HPP_

#include "defs.hpp"
namespace grid_gen {

  template<typename _GRID_TYPE, typename _GRID_CELL>
  class tSUB_DIVIDE {
      typedef typename _GRID_CELL::cENTRY cGRID_ENTRY;
     public:
      tSUB_DIVIDE(_GRID_TYPE *grid = NULL, _GRID_CELL *cell = NULL, eCOORD coord = GEOM_COORD_NONE)
          : m_grid(grid),
            m_cell(cell),
            m_coord(coord),
            m_box(cell->Box()) {
      }
      ~tSUB_DIVIDE() {

      }

     public:
      BOOL Do();

     private:
      VOID Split(cCUT_CELL_CLAY &bigCellClay, cCUT_CELL_CLAY children[2]);
      VOID Split(INT facetIndex, std::vector<bool> &visitedFacets, cCUT_CELL_CLAY &parentClay, cCUT_CELL_CLAY &childClay);
      VOID VerticesColor(cCUT_CELL_CLAY &parentClay, cCUT_CELL_CLAY &childClay);
      VOID DistributeManifolds(_GRID_CELL *parentCell, _GRID_CELL *leftChild, _GRID_CELL *rightChild);
      VOID DistributeManifold(cGRID_ENTRY *entry, _GRID_CELL *leftChild, _GRID_CELL *rightChild);
      VOID RegisterEntryVertices(cCUT_CELL_CLAY &clay, cGRID_ENTRY *entry);
      VOID RegisterOnClayEdges(cCUT_CELL_CLAY &clay, const cBOX3 &clayBox, const cPOINT3 &point, iVERTEX meshVertexIndex);

     private:
      struct sLOCAL_GRID_STORAGE_POLICY {
          static const INT numObjectsInPage = 2;
          static const INT logOfObjectsInPage = 1;
          static const INT numInitialPages = 2;
      };

      typedef tGRID<_GRID_CELL, sLOCAL_GRID_STORAGE_POLICY> cSMALL_GRID;

     private:
      _GRID_TYPE *m_grid;
      _GRID_CELL *m_cell;
      eCOORD m_coord;
      cBOX3 m_box;
  };
} /* namespace grid_gen */
#endif /* SUBDIVIDE_HPP_ */
