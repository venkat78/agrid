#ifndef _GRID_GEN_CUT_CELLS_ADHESIVE_HPP_
#define _GRID_GEN_CUT_CELLS_ADHESIVE_HPP_

#include "defs.hpp"
#include <time.h>

/*
 * This file supports gluing of cut-cells.
 *
 */
namespace grid_gen {
  class cCUT_CELLS_ADHESIVE {
      typedef cINDEX_TO_INT_MAP cWHITE_VERTICES_MAP;
     public:
      cCUT_CELLS_ADHESIVE(const cBOX3 &bbox = cBOX3(), cVOLUMETRIC_GRID *grid = NULL)
          : m_bbox(bbox.GrowBy(2)),
            m_grid(grid),
            m_octree(m_bbox, m_grid, 8, 4) {
        m_verticesTicks = 0;
        m_facesTicks = 0;
        m_cellTicks = 0;
      }

      ~cCUT_CELLS_ADHESIVE() {
      }

      iCELL Add(cCUT_CELL &cutCell);

      VOID PrintTimers() {
        printf("Adding vertices = %lf\n", (double) (m_verticesTicks));
        printf("Adding faces = %lf\n", (double) (m_facesTicks));
        printf("Adding cells = %lf\n", (double) (m_cellTicks));
      }

     private:
      iCELL AddCutCell(cCUT_CELL &cell, INT verticesMap[1024], INT facesMap[1024], INT numGridVerticesOld);

      /*
       *  Methods to add cutcell vertices.
       */
      VOID AddCutCellVertices(cCUT_CELL &cell, INT verticesMap[1024]);

      /*
       * Methods to add cutcell faces.
       */
      VOID AddCutCellFaces(cCUT_CELL &cell, INT verticesMap[1024], INT facesMap[1024], INT numGridVerticesOld);

      iFACE FindGridFace(iVERTEX gridFaceVertices[1024], INT numVertices);

      iFACE AddGridFace(iVERTEX gridFaceVertices[1024], INT numVertices);

     private:
      time_t m_verticesTicks;
      time_t m_facesTicks;
      time_t m_cellTicks;

      cBOX3 m_bbox;
      cVOLUMETRIC_GRID *m_grid;
      tMESH_VERTICES_OCTREE<cVOLUMETRIC_GRID> m_octree;
      cWHITE_VERTICES_MAP m_whiteVerticesMap;
  };
}

#endif // _AGRID_BOOLEAN_CUT_CELLS_ADHESIVE_HPP_
