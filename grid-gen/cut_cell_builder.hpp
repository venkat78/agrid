#ifndef _GRID_GEN_CUT_CELL_BUILDER_HPP_
#define _GRID_GEN_CUT_CELL_BUILDER_HPP_

#include "defs.hpp"
#include "manifold_obj.hpp"

namespace grid_gen {
  /*
   * Class builds cut cells for _GRID_CELL_ENTRY.
   */
  template<typename _MANIFOLD_OBJ, typename _GRID_CELL>
  class tCUT_CELL_BUILDER {
      typedef std::pair<iVERTEX, iVERTEX> sEDGE;
      typedef typename _GRID_CELL::cENTRY cGRID_CELL_ENTRY;
     public:
      /*
       * Status give the current state of the build process
       */
      enum eSTATUS {
        NOT_STARTED,
        INITIALIZATION_COMPLETE,
        REGISTERED_FACET_VERTICES,
        CREATED_WHITE_FACES,
        REMOVED_BLACK_FACES,
        EXTRACTED_CUT_CELLS
      };

     public:
      tCUT_CELL_BUILDER(_MANIFOLD_OBJ *manifold = NULL, _GRID_CELL *cell = NULL, REAL tol = 0.0)
          : m_manifold(manifold),
            m_cell(cell),
            m_cellEntry(cell->begin().operator*()),
            m_status(NOT_STARTED),
            m_box(m_cell->Box()),
            m_lengthTol(tol),
            m_debug(false)

      {
      }

      /*
       *  On successful completion m_cellEntry has cut-cells generated for the mesh.
       */
      BOOL Build(cSURFACE_MESH *bfacesMesh = NULL);

      /*
       * Returns current status of the cut-cell builder.
       */
      eSTATUS Status() const {
        return m_status;
      }
     private:
      VOID Init();
      /*
       * Methods to support various operations during building.
       */

      /*
       * Method registers facet vertices on cell edges.
       */
      VOID RegisterFacetVertices();

      /*
       * Method registers a point on cell edge.
       */
      VOID RegisterOnCellEdges(const cPOINT3 &point, iVERTEX meshVertexIndex);

      /*
       * Methods to support creation of white faces.
       */

      /*
       * Methods to create white faces on cell faces.
       */
      VOID CreateWhiteFaces();

      /*
       * Creates white faces at vertex.
       */
      VOID CreateWhiteFacesAtVertex(iVERTEX vertexIndex);

      /*
       * Create a white face in a facet.
       */
      VOID CreateWhiteFace(iVERTEX vertexIndex, iFACET facetIndex);

      /*
       * Removes black faces in m_cellMesh.
       */
      BOOL RemoveBlackFaces();

      /*
       * Remove non-corner vertices in m_cellMesh.
       */
      VOID RemoveNonCornerVertices();

      BOOL AnyCornerVerticesColorUnknown();

      /*
       * Status of corner vertices.
       */
      BOOL AreAllCornerVerticesWhite();
      BOOL AreAllCornerVerticesUnknown();
      BOOL AreAllCornerVerticesSameColor();
      BOOL AreAllCornerVerticesNonWhite();

      /*
       * Methods which determine colors of corner vertices.
       */
      BOOL FindCornerColorByNormal(INT cornerVertexIndex);
      BOOL FindCornerColorByTopology(INT cornerVertexIndex);
      VOID FindCornerVertexColors();
      VOID FindUnknownCornerVertexColors();
      BOOL UseNormalsToRemoveBlackFaces();

      /*
       * If a facet doesn't contain a white vertex, then it is considered black.
       */
      BOOL IsBlack(iFACET facetIndex);

      //Extract cut-cells from m_cellMesh.
      BOOL ExtractCutCells(cSURFACE_MESH *bfacesMesh = NULL);

      BOOL ExtractCutCell(iFACET cellFace, std::vector<BOOL> &visitedFaces, cSURFACE_MESH *bfacesMesh = NULL);

      VOID PrintMeshVertices();

      /*
       * Methods to support cut cell generation for open manifolds.
       */
      VOID ReflectGrayVertices();

      //Methods related to removing tiny edges during boolean
      VOID RemoveTinyEdge(iVERTEX v1, iVERTEX v2);

//     VOID RemoveEdge(sEDGE edge);

//     VOID RemoveEdgeWithVertexOnLatticeEdge(sEDGE edge, INT latticeEdge2);

//     VOID RemoveLatticeEdge(sEDGE edge);

      VOID SplitWhiteAndBlackPortions(cCUT_CELL_CLAY &whiteClay, cCUT_CELL_CLAY &blackClay);

      VOID RemoveEdges(tLIST<sEDGE> &tinyEdges);

      VOID CollectTinyEdges(tLIST<sEDGE> &tinyEdges);

      VOID RemoveTinyEdges();

     private:
      /*
       *  m_cellMesh initially is a box representing the cell, the cell is edited
       *  to create cut-cells out of the mesh.
       */
      cCUT_CELL_CLAY m_clay;

      /*
       * For gray vertices registered on m_cellMesh store mesh
       * vertices. In some cases the information is useful to resolve
       * black and white status of corner vertices.
       */
      //   std::map<iVERTEX, iVERTEX> m_meshVertexIndices;
      _MANIFOLD_OBJ* m_manifold;
      _GRID_CELL *m_cell;
      cGRID_CELL_ENTRY *m_cellEntry;
      cBOX3 m_box;
      REAL m_lengthTol;
      eSTATUS m_status;
      BOOL m_debug = false;

  };
}

#endif //_GRID_GEN_CUT_CELL_BUILDER_HPP_
