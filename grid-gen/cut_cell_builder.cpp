#include "cut_cell_builder.hpp"

/*
 * A cBOX3 topology enumeration.
 * Should be moved out of here at a later time.
 */
const INT vertex_face_incidence[8][3] = { { 0, 1, 2 }, { 0, 1, 5 }, { 0, 2, 4 }, { 0, 5, 5 }, { 1, 2, 3 }, { 1, 3, 5 }, { 2, 3, 4 }, { 3, 4, 5 } };

//This is to store tuples info
//for box vertices
const INT vertex_enumeration[8][3] = { { 0, 0, 0 }, { 0, 0, 1 }, { 0, 1, 0 }, { 0, 1, 1 }, { 1, 0, 0 }, { 1, 0, 1 }, { 1, 1, 0 }, { 1, 1, 1 }, };

const INT edge_vertices[12][2] = { { 0, 1 }, { 0, 2 }, { 0, 4 }, { 1, 3 }, { 1, 5 }, { 2, 3 }, { 2, 6 }, { 4, 5 }, { 4, 6 }, { 7, 6 }, { 7, 5 }, { 7, 3 } };

const INT edge_faces[12][2] = { { 0, 1 }, { 0, 2 }, { 1, 2 }, { 0, 5 }, { 1, 5 }, { 0, 4 }, { 2, 4 }, { 1, 3 }, { 2, 3 }, { 3, 4 }, { 3, 5 }, { 4, 5 } };

const INT face_vertices[6][4] = { { 0, 1, 3, 2 }, { 0, 4, 5, 1 }, { 0, 2, 6, 4 }, { 6, 7, 5, 4 }, { 2, 3, 7, 6 }, { 1, 5, 7, 3 } };

namespace grid_gen {
//  BOOL tCUT_CELL_BUILDER<_MANIFOLD_OBJ, _GRID_CELL>::m_debug = false;

  /*
   * Registers a point on cellMesh edges.
   * Can be optimized since multiple facets, share the same vertex.
   */
  template<typename _MANIFOLD_OBJ, typename _GRID_CELL>
  VOID tCUT_CELL_BUILDER<_MANIFOLD_OBJ, _GRID_CELL>::RegisterOnCellEdges(const cPOINT3 &point, iVERTEX meshVertexIndex) {
    INT edge = m_box.IsOnEdge(point);
    if (edge < 0)
      return;

    INT tailIndex, headIndex, newVertexIndex;
    tailIndex = headIndex = newVertexIndex = INVALID_IVERTEX;

    ASSERT(edge >= 0 && edge < 12);

    INT cellFaceIndex = edge_faces[edge][0];

    cCUT_CELL_CLAY::cFACET* cellFace = m_clay.Facet(cellFaceIndex);

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
            m_clay.Vertex(vertex->Index())->Color(GRAY);
//            m_clay.Vertex(vertex->Index())->Source(meshVertexIndex, m_manifold->Mesh());
          }

        } else {
          //Insert it on half-edge
          tailIndex = currHalfEdge->Tail()->Index();
          headIndex = currHalfEdge->Head()->Index();

          newVertexIndex = m_clay.NewVertex(point);

          ASSERT(tailIndex != INVALID_IVERTEX && headIndex != INVALID_IVERTEX && newVertexIndex != INVALID_IVERTEX);

          m_clay.SplitEdge(newVertexIndex, tailIndex, headIndex);
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
  template<typename _MANIFOLD_OBJ, typename _GRID_CELL>
  VOID tCUT_CELL_BUILDER<_MANIFOLD_OBJ, _GRID_CELL>::RegisterFacetVertices() {
    typename _MANIFOLD_OBJ::cMESH *facetMesh = m_manifold->Mesh();
    std::vector<iFACET> &facets = m_cellEntry->Facets();

    std::vector<iFACET>::iterator currFacet = facets.begin();
    std::vector<iFACET>::iterator lastFacet = facets.end();

    //printf("Facets : ");
    for (; currFacet != lastFacet; currFacet++) {
      cSURFACE_MESH::cFACET *facet = facetMesh->Facet(*currFacet);
      //printf("%d ", facet->Index());

      cSURFACE_MESH::cFACET::vertex_circulator currVertex = facet->VerticesBegin();
      cSURFACE_MESH::cFACET::vertex_circulator lastVertex = facet->VerticesEnd();

      for (; currVertex != lastVertex; currVertex++)
        RegisterOnCellEdges(currVertex->Point(), currVertex->Index());
    }

    //printf("\n");
  }

  /*
   * Creates a WHITE face for the faceIndex
   */
  template<typename _MANIFOLD_OBJ, typename _GRID_CELL>
  VOID tCUT_CELL_BUILDER<_MANIFOLD_OBJ, _GRID_CELL>::CreateWhiteFace(iVERTEX vertexIndex, iFACET faceIndex) {
    ASSERT(vertexIndex >= 0 && vertexIndex < 8);
    //ASSERT(faceIndex > 0 && faceIndex < 6);

    cCUT_CELL_CLAY::cFACET *cellFace = m_clay.Facet(faceIndex);

    cCUT_CELL_CLAY::cHALF_EDGE *startingHe = NULL;

    cCUT_CELL_CLAY::cFACET::half_edge_circulator currHeCirculator = cellFace->HalfEdgesBegin();
    cCUT_CELL_CLAY::cFACET::half_edge_circulator lastHeCirculator = cellFace->HalfEdgesEnd();

    for (; currHeCirculator != lastHeCirculator; currHeCirculator++) {
      if (currHeCirculator->Tail()->Index() == vertexIndex) {
        startingHe = currHeCirculator.operator->();
        break;
      }
    }

    /*
     * Record transitions from GRAY-->WHITE and WHITE-->GRAY along the face boundary.
     */
    std::vector<INT> transitionNodes;

    INT prevVertexIndex = startingHe->Tail()->Index();
    startingHe = startingHe->Next();
    cCUT_CELL_CLAY::cHALF_EDGE* loopingHe = startingHe;

    do {
      INT vertexIndex = loopingHe->Tail()->Index();
      eCELL_COLOR prevVtxColor = prevVertexIndex < 8 ? m_clay.Vertex(prevVertexIndex)->Color() : GRAY;
      eCELL_COLOR currVtxColor = vertexIndex < 8 ? m_clay.Vertex(vertexIndex)->Color() : GRAY;

      if (prevVtxColor == GRAY && currVtxColor == WHITE)
        transitionNodes.push_back(prevVertexIndex);

      if (prevVtxColor == WHITE && currVtxColor == GRAY)
        transitionNodes.push_back(vertexIndex);

      prevVertexIndex = vertexIndex;
      loopingHe = loopingHe->Next();
    } while (loopingHe != startingHe);

//     if(transitionNodes.size() == 1)
//       m_cell->Export();

//ASSERT(transitionNodes.size() > 1 || transitionNodes.size() == 0);

    if (transitionNodes.size() <= 1)
      return;

    iVERTEX transitionVertex1 = transitionNodes.front();
    iVERTEX transitionVertex2 = transitionNodes.back();

    if (transitionVertex1 == transitionVertex2)
      return;

    //If both vertices are on the same cell-edge don't insert the diagonal.
    INT edge1 = m_box.IsOnEdge(m_clay.Vertex(transitionVertex1)->Point());
    INT edge2 = m_box.IsOnEdge(m_clay.Vertex(transitionVertex2)->Point());

    if (edge1 == edge2)
      return;

    m_clay.InsertDiagonal(faceIndex, transitionVertex1, transitionVertex2);

    //At this point, if adjoining cell is a white-cell then cuts need to be
    //introduced in the white cell. skipping for now. --B 02/24/2010.
  }

//Breaks cell faces incident on vertexIndex to black and white.
//vertexIndex always needs to be corner vertex of the original cell.
  template<typename _MANIFOLD_OBJ, typename _GRID_CELL>
  VOID tCUT_CELL_BUILDER<_MANIFOLD_OBJ, _GRID_CELL>::CreateWhiteFacesAtVertex(iVERTEX vertexIndex) {
    //Do this for all faces incident on vertexIndex.
    std::vector<cCUT_CELL_CLAY::cFACET*> incidentFacets;
    m_clay.Vertex(vertexIndex)->IncidentFacets(incidentFacets);

    std::vector<cCUT_CELL_CLAY::cFACET*>::iterator currFacet = incidentFacets.begin();
    std::vector<cCUT_CELL_CLAY::cFACET*>::iterator lastFacet = incidentFacets.end();

    for (; currFacet != lastFacet; currFacet++) {
      CreateWhiteFace(vertexIndex, (*currFacet)->Index());
    }
  }

  /*
   * Creates exterior and interior faces on cell-faces based on mesh geometry.
   */
  template<typename _MANIFOLD_OBJ, typename _GRID_CELL>
  VOID tCUT_CELL_BUILDER<_MANIFOLD_OBJ, _GRID_CELL>::CreateWhiteFaces() {
    for (INT i = 0; i < 8; i++) {
      if (m_clay.Vertex(i)->Color() == WHITE) {
        CreateWhiteFacesAtVertex(i);
      }
    }
  }

  /*
   * Removes cell faces that don't contain any white vertices.
   */
  template<typename _MANIFOLD_OBJ, typename _GRID_CELL>
  BOOL tCUT_CELL_BUILDER<_MANIFOLD_OBJ, _GRID_CELL>::RemoveBlackFaces() {
    std::vector<iFACET> facets;

    cCUT_CELL_CLAY::facet_iterator currFacet = m_clay.FacetsBegin();
    cCUT_CELL_CLAY::facet_iterator lastFacet = m_clay.FacetsEnd();

    for (; currFacet != lastFacet; currFacet++) {
      facets.push_back(currFacet->Index());
    }

    INT numFacesBeforeDeletion = m_clay.NumFacets();

    std::vector<iFACET>::iterator currFacetIndex = facets.begin();
    std::vector<iFACET>::iterator lastFacetIndex = facets.end();

    for (; currFacetIndex != lastFacetIndex; currFacetIndex++) {
      if (IsBlack(*currFacetIndex))
        m_clay.DeleteFacet(*currFacetIndex);
    }

    INT numFacesAfterDeletion = m_clay.NumFacets();

    return numFacesBeforeDeletion - numFacesAfterDeletion > 0 ? true : false;
  }

  /*
   * Remove non-corner gray vertices introduced into cellMesh. This
   * is called when corner vertices of the cell are white and
   * there are no black faces on cell-faces.
   */
  template<typename _MANIFOLD_OBJ, typename _GRID_CELL>
  VOID tCUT_CELL_BUILDER<_MANIFOLD_OBJ, _GRID_CELL>::RemoveNonCornerVertices() {
    //ASSERT(m_clay.NumVertices() > 8);

    INT largestVertexIndex = m_clay.LargestVertexIndex();

    if (largestVertexIndex >= 8 && m_clay.LargestFacetIndex() >= 6) {
      printf("%d\n",m_cell->Index());
      //      m_cell->ExportLastEntryToOff();
      assert(0);
    }

    for (INT i = 8; i <= largestVertexIndex; i++)
      if (!m_clay.Vertex(i)->IsDeleted())
        m_clay.MergeEdgesAtVertex(i);
  }

  /*
   * If a face doesn't contain white vertex, then it is considered
   * to be black.
   */
  template<typename _MANIFOLD_OBJ, typename _GRID_CELL>
  BOOL tCUT_CELL_BUILDER<_MANIFOLD_OBJ, _GRID_CELL>::IsBlack(iFACET facetIndex) {
    cCUT_CELL_CLAY::cFACET *facet = m_clay.Facet(facetIndex);

    cCUT_CELL_CLAY::cFACET::vertex_circulator currVertex = facet->VerticesBegin();
    cCUT_CELL_CLAY::cFACET::vertex_circulator lastVertex = facet->VerticesEnd();

    for (; currVertex != lastVertex; currVertex++)
      if (currVertex->Index() < 8 && m_clay.Vertex(currVertex->Index())->Color() == WHITE)
        return false;

    return true;
  }

  template<typename _MANIFOLD_OBJ, typename _GRID_CELL>
  VOID tCUT_CELL_BUILDER<_MANIFOLD_OBJ, _GRID_CELL>::ReflectGrayVertices() {
//    INT largestVertexIndex = m_clay.LargestVertexIndex();
//    std::vector < iVERTEX > grayVertices;
//
//    for (INT i = 0; i < largestVertexIndex; i++)
//      if (m_clay.Vertex(i)->Color() == GRAY)
//        grayVertices.push_back(i);
//
//    INT numGrayVertices = grayVertices.size();
//
//    /*
//     * Reflect vertices.
//     */
//    for (INT i = 0; i < numGrayVertices; i++) {
//      cCUT_CELL_CLAY::cVERTEX* srcVertex = m_clay.Vertex(grayVertices[i]);
//      iVERTEX reflectedIndex = m_clay.NewVertex(srcVertex->Point());
//      cCUT_CELL_CLAY::cVERTEX* reflectedVertex = m_clay.Vertex(reflectedIndex);
//
//      reflectedVertex->Source(grayVertices[i], m_manifold->Mesh());
//      srcVertex->Reflection(reflectedIndex);
//      reflectedVertex->Reflection(grayVertices[i]);
//      reflectedVertex->IsReflection(true);
//    }
//
//    /*
//     * Reflect gray edges.
//     */
//    for (INT i = 0; i < numGrayVertices; i++) {
//      cCUT_CELL_CLAY::cVERTEX* srcVertex = m_clay.Vertex(grayVertices[i]);
//      std::vector<cCUT_CELL_CLAY::cVERTEX*> incidentVertices;
//
//      srcVertex->IncidentVertices(incidentVertices);
//
//      for (INT j = 0; j < incidentVertices.size(); j++) {
//        iVERTEX v = incidentVertices[j]->Index();
//
//        if (v < grayVertices[i])
//          continue;
//
//        //Reflect diagonal here.
//        cCUT_CELL_CLAY::cHALF_EDGE *boundary = NULL;
//        srcVertex->Find(incidentVertices[j], &boundary, NULL);
//
//        cCUT_CELL_CLAY::cFACET *f1 = boundary->Facet();
//        cCUT_CELL_CLAY::cFACET *f2 = boundary->Opp()->Facet();
//
//        cPOINT3 point1 = f1->MeanPoint();
//        cPOINT3 point2 = f2->MeanPoint();
//
//        cPOINT3 midPoint = boundary->MidPoint();
//
//        cVECTOR3 vec1 = (point1 - midPoint).Normalize();
//        cVECTOR3 vec2 = (point2 - midPoint).Normalize();
//
//        cSURFACE_MESH *mesh = m_manifold->Mesh();
//        cVECTOR3 meshNormal = ((mesh->Vertex(srcVertex->SourceIndex())->AverageNormal()
//            + mesh->Vertex(incidentVertices[j]->SourceIndex())->AverageNormal()) * 0.5).Normalize();
//
//        REAL f1Dir = meshNormal * vec1, f2Dir = meshNormal * vec2;
//        if (f1Dir < f2Dir) {
//          if (!m_clay.Vertex(incidentVertices[j]->Reflection())->HasIncidentFacets())
//            m_clay.SplitEdge(incidentVertices[j]->Reflection(), incidentVertices[j]->Index(), boundary->Next()->Head()->Index());
//
//          if (!m_clay.Vertex(srcVertex->Reflection())->HasIncidentFacets())
//            m_clay.SplitEdge(srcVertex->Reflection(), srcVertex->Index(), boundary->Prev2()->Tail()->Index());
//
//          m_clay.InsertDiagonal(f1->Index(), srcVertex->Reflection(), incidentVertices[j]->Reflection());
//        } else {
//          cCUT_CELL_CLAY::cHALF_EDGE* boundaryOpp = boundary->Opp();
//
//          if (!m_clay.Vertex(incidentVertices[j]->Reflection())->HasIncidentFacets())
//            m_clay.SplitEdge(incidentVertices[j]->Reflection(), boundaryOpp->Prev2()->Tail()->Index(), incidentVertices[j]->Index());  //boundaryNext->Head()->Index());
//
//          if (!m_clay.Vertex(srcVertex->Reflection())->HasIncidentFacets())
//            m_clay.SplitEdge(srcVertex->Reflection(), srcVertex->Index(), boundaryOpp->Next()->Head()->Index());
//
//          m_clay.InsertDiagonal(f2->Index(), srcVertex->Reflection(), incidentVertices[j]->Reflection());
//        }
//
//      }
//
//    }
  }

  /*
   * This method extracts pieces of cutcells from m_clay and closes them.
   */
  template<typename _MANIFOLD_OBJ, typename _GRID_CELL>
  BOOL tCUT_CELL_BUILDER<_MANIFOLD_OBJ, _GRID_CELL>::ExtractCutCell(INT facetIndex, std::vector<bool> &visitedFacets, cSURFACE_MESH *bfacesMesh) {
    std::vector<iFACET> cutCellFacets;
    std::vector<bool> visitedNeighbors(m_clay.LargestFacetIndex() + 1, false);

    cCUT_CELL_CLAY::cFACET* facet = m_clay.Facet(facetIndex);

    INT currIndex = 0;
    cutCellFacets.push_back(facet->Index());

    while (currIndex < cutCellFacets.size()) {
      facet = m_clay.Facet(cutCellFacets[currIndex]);
      if (!visitedFacets[facet->Index()]) {
        visitedNeighbors[facet->Index()] = true;
        visitedFacets[facet->Index()] = true;

        cCUT_CELL_CLAY::cHALF_EDGE *halfEdge = facet->HalfEdge();
        cCUT_CELL_CLAY::cHALF_EDGE *loopHe = halfEdge;

        do {
          cCUT_CELL_CLAY::cHALF_EDGE *oppHe = loopHe->Opp();
          if (!loopHe->IsBorder() && !oppHe->IsBorder() && (loopHe->Tail()->Index() < 8 || loopHe->Head()->Index() < 8)) {
            cCUT_CELL_CLAY::cFACET *oppFacet = oppHe->Facet();
            if (!visitedNeighbors[oppFacet->Index()]) {
              cutCellFacets.push_back(oppFacet->Index());
              visitedNeighbors[oppFacet->Index()] = true;
            }
          }

          loopHe = loopHe->Next();
        } while (loopHe != halfEdge);
      }
      currIndex++;
    }

    cCUT_CELL *newCutCell = m_cellEntry->NewCutCell();
    MeshFromFacets(cutCellFacets, m_clay, *newCutCell);

    return true;
  }

  template<typename _MANIFOLD_OBJ, typename _GRID_CELL>
  BOOL tCUT_CELL_BUILDER<_MANIFOLD_OBJ, _GRID_CELL>::ExtractCutCells(cSURFACE_MESH *bfacesMesh) {
    BOOL retVal = true;

    std::vector<bool> visitedFacets(m_clay.LargestFacetIndex() + 1, false);
    for (INT i = 0; i < m_clay.LargestFacetIndex() + 1; i++) {
      if (!m_clay.Facet(i)->IsDeleted() && !visitedFacets[i]) {
        if (!ExtractCutCell(i, visitedFacets, bfacesMesh))
          retVal = false;
      }
    }

    return retVal;
  }

  template<typename _MANIFOLD_OBJ, typename _GRID_CELL>
  VOID tCUT_CELL_BUILDER<_MANIFOLD_OBJ, _GRID_CELL>::Init() {
    BoxToMesh(m_box, m_clay);
    iCELL_INDEX index = m_cell->Index();

    for (INT i = 0; i < 8; i++) {
      INT indices[3];
      indices[0] = vertex_enumeration[i][0];
      indices[1] = vertex_enumeration[i][1];
      indices[2] = vertex_enumeration[i][2];

      iGRID_VERTEX vIndex(index.x + indices[0], index.y + indices[1], index.z + indices[2]);

      m_clay.Vertex(i)->Color(m_manifold->Grid()->VertexColor(vIndex));
    }
  }

  template<typename _MANIFOLD_OBJ, typename _GRID_CELL>
  VOID tCUT_CELL_BUILDER<_MANIFOLD_OBJ, _GRID_CELL>::PrintMeshVertices() {
    printf("Mesh vertex indices : ");
    /*
     for(INT i = 0; i < m_meshVertexIndices.size() ; i++)
     printf("%d ", m_meshVertexIndices[i]);

     */
    printf("\n");
  }

  template<typename _MANIFOLD_OBJ, typename _GRID_CELL>
  BOOL tCUT_CELL_BUILDER<_MANIFOLD_OBJ, _GRID_CELL>::AnyCornerVerticesColorUnknown() {
    for (INT i = 0; i < 8; i++) {
      if (m_clay.Vertex(i)->Color() == UNKNOWN)
        return true;
    }

    return false;
  }

  template<typename _MANIFOLD_OBJ, typename _GRID_CELL>
  BOOL tCUT_CELL_BUILDER<_MANIFOLD_OBJ, _GRID_CELL>::AreAllCornerVerticesWhite() {
    for (INT i = 0; i < 8; i++) {
      eCELL_COLOR cornerColor = m_clay.Vertex(i)->Color();

      if (cornerColor != WHITE && cornerColor != GRAY)
        return false;
    }

    return true;
  }

  template<typename _MANIFOLD_OBJ, typename _GRID_CELL>
  BOOL tCUT_CELL_BUILDER<_MANIFOLD_OBJ, _GRID_CELL>::AreAllCornerVerticesNonWhite() {
    for (INT i = 0; i < 8; i++) {
      if (m_clay.Vertex(i)->Color() == WHITE)
        return false;
    }

    return true;
  }

  template<typename _MANIFOLD_OBJ, typename _GRID_CELL>
  BOOL tCUT_CELL_BUILDER<_MANIFOLD_OBJ, _GRID_CELL>::AreAllCornerVerticesSameColor() {
    if (AreAllCornerVerticesWhite() || AreAllCornerVerticesNonWhite())
      return true;
  }

  template<typename _MANIFOLD_OBJ, typename _GRID_CELL>
  BOOL tCUT_CELL_BUILDER<_MANIFOLD_OBJ, _GRID_CELL>::AreAllCornerVerticesUnknown() {
    for (INT i = 0; i < 8; i++) {
      if (m_clay.Vertex(i)->Color() != UNKNOWN)
        return false;
    }

    return true;
  }

  template<typename _MANIFOLD_OBJ, typename _GRID_CELL>
  BOOL tCUT_CELL_BUILDER<_MANIFOLD_OBJ, _GRID_CELL>::FindCornerColorByTopology(INT i) {
    cCUT_CELL_CLAY::cVERTEX *cornerVertex = m_clay.Vertex(i);

    std::vector<cCUT_CELL_CLAY::cVERTEX*> neighbours;
    cornerVertex->IncidentVertices(neighbours);

    std::vector<cCUT_CELL_CLAY::cVERTEX*>::iterator currNeighbour = neighbours.begin();
    std::vector<cCUT_CELL_CLAY::cVERTEX*>::iterator lastNeighbour = neighbours.end();

    for (; currNeighbour != lastNeighbour; currNeighbour++) {
      iVERTEX neighbourIndex = (*currNeighbour)->Index();
      if (neighbourIndex < 8 && m_clay.Vertex(neighbourIndex)->Color() != GRAY && m_clay.Vertex(neighbourIndex)->Color() != UNKNOWN) {
        m_clay.Vertex(i)->Color(m_clay.Vertex(neighbourIndex)->Color());
        return true;;
      }
    }

    return false;
  }

  template<typename _MANIFOLD_OBJ, typename _GRID_CELL>
  VOID tCUT_CELL_BUILDER<_MANIFOLD_OBJ, _GRID_CELL>::FindUnknownCornerVertexColors() {
    /*
     * First find color by topology and then use normals to decide
     * color, normals can't always deduce the correct color, we have to
     * use ray casting to find corner colors.
     *
     */
    for (INT i = 0; i < 8; i++) {
      if (m_clay.Vertex(i)->Color() == UNKNOWN)
        FindCornerColorByTopology(i);
    }

    //     for(INT i = 0; i < 8 ; i++ ) {
    //       if(m_cellVertexColor[i] == UNKNOWN) {
    //         FindCornerColorByNormal(i);
    //       }
    //     }

    /*
     *  In some cases, even now the corner vertex color might be unknown
     *  Try to find out using topology.
     */

    for (INT i = 0; i < 8; i++) {
      if (m_clay.Vertex(i)->Color() == UNKNOWN)
        FindCornerColorByTopology(i);
    }

    //If still some corner vertices colors are unknown, something strange is
    //happening.
  }

  template<typename _MANIFOLD_OBJ, typename _GRID_CELL>
  VOID tCUT_CELL_BUILDER<_MANIFOLD_OBJ, _GRID_CELL>::RemoveTinyEdge(iVERTEX v1, iVERTEX v2) {
    cPOINT3 newPoint;

    if (m_clay.Vertex(v1)->IsDeleted() || m_clay.Vertex(v2)->IsDeleted())
      return;

    BOOL moveToMidPoint = false;
    if (v1 >= 8 && v2 >= 8)
      moveToMidPoint = true;

    if (moveToMidPoint)
      newPoint = (m_clay.Vertex(v1)->Point() + m_clay.Vertex(v2)->Point()) * 0.5;
    else
      newPoint = m_clay.Vertex(v1)->Point();

    m_clay.MergeVertices(v1, v2, v1, newPoint);
    m_clay.Vertex(v1)->Color(m_clay.Vertex(v2)->Color());

    if (v1 < 8 && m_clay.Vertex(v1)->Color() == UNKNOWN)
      m_clay.Vertex(v1)->Color(GRAY);
  }

  template<typename _MANIFOLD_OBJ, typename _GRID_CELL>
  VOID tCUT_CELL_BUILDER<_MANIFOLD_OBJ, _GRID_CELL>::RemoveEdges(tLIST<sEDGE> &tinyEdges) {
    tLIST<sEDGE>::iterator currEdge = tinyEdges.begin();
    tLIST<sEDGE>::iterator lastEdge = tinyEdges.end();

    while (currEdge != lastEdge) {
      sEDGE edge = currEdge.operator*();
      iVERTEX v1 = edge.first;
      iVERTEX v2 = edge.second;

      RemoveTinyEdge(v1, v2);
      currEdge++;
    }
  }

  template<typename _MANIFOLD_OBJ, typename _GRID_CELL>
  VOID tCUT_CELL_BUILDER<_MANIFOLD_OBJ, _GRID_CELL>::CollectTinyEdges(tLIST<sEDGE> &tinyEdges) {
    //collecting tiny edges.
    cCUT_CELL_CLAY::vertex_iterator currVtx = m_clay.VerticesBegin();
    cCUT_CELL_CLAY::vertex_iterator lastVtx = m_clay.VerticesEnd();

    for (; currVtx != lastVtx; currVtx++) {
      cCUT_CELL_CLAY::cVERTEX::vertex_use_iterator currUse = currVtx->VertexUsesBegin();
      cCUT_CELL_CLAY::cVERTEX::vertex_use_iterator lastUse = currVtx->VertexUsesEnd();

      for (; currUse != lastUse; currUse++) {
        cCUT_CELL_CLAY::cVERTEX_USE::half_edge_circulator currHe = currUse->HalfEdgesBegin();
        cCUT_CELL_CLAY::cVERTEX_USE::half_edge_circulator lastHe = currUse->HalfEdgesEnd();

        for (; currHe != lastHe; currHe++) {
          iVERTEX v1 = currHe->Tail()->Index();
          iVERTEX v2 = currHe->Head()->Index();

          if (v1 < v2 && currHe->Segment().Length() <= m_lengthTol)
            tinyEdges.push_back(sEDGE(v1, v2));
        }
      }
    }
  }

  template<typename _MANIFOLD_OBJ, typename _GRID_CELL>
  VOID tCUT_CELL_BUILDER<_MANIFOLD_OBJ, _GRID_CELL>::RemoveTinyEdges() {
    while (1) {
      tLIST<sEDGE> tinyEdges;
      CollectTinyEdges(tinyEdges);

      if (tinyEdges.size() == 0)
        break;

      RemoveEdges(tinyEdges);
    }
  }

  template<typename _MANIFOLD_OBJ, typename _GRID_CELL>
  VOID tCUT_CELL_BUILDER<_MANIFOLD_OBJ, _GRID_CELL>::SplitWhiteAndBlackPortions(cCUT_CELL_CLAY &whiteClay, cCUT_CELL_CLAY &blackClay) {
    std::vector<iFACET> whiteFacets, blackFacets;

    cCUT_CELL_CLAY::facet_iterator currFacet = m_clay.FacetsBegin();
    cCUT_CELL_CLAY::facet_iterator lastFacet = m_clay.FacetsEnd();

    for (; currFacet != lastFacet; currFacet++) {
      iFACET facetIndex = currFacet->Index();
      if (IsBlack(facetIndex))
        blackFacets.push_back(facetIndex);
      else
        whiteFacets.push_back(facetIndex);
    }

    MeshFromFacets(whiteFacets, m_clay, whiteClay);
    MeshFromFacets(blackFacets, m_clay, blackClay);
  }
  /*
   *   Algorithm.
   *   The basic idea is to start with a mesh representing the box,
   *   edit the mesh using euler operators and carve out the cut-cells
   *   by repeatedly editing the mesh.
   *
   *   From here on mesh representing the cell box is refered to as
   *   cellMesh.
   *
   *   NOTE: On creation of cellMesh, the first eight vertices are
   *         are corner vertices and cell faces are represented as
   *         quads.
   *
   *   1. Derive color of corner vertices of cellMesh from the grid
   *      based on advancing front walk.
   *
   *   2. Register input geometry vertices on cellMesh edges. If the
   *      vertices coincide with corner vertices of cell box, then
   *      the corresponding corner vertices are marked as GRAY.
   *
   *      NOTE: Since new GRAY vertices are inserted on half-edges,
   *            all of them are in correct order on the cell edge.
   *
   *
   *   3. For each WHITE corner vertex, for all faces incident
   *      on the vertex,
   *
   *      Start walking the facet from the white vertex, note vertex
   *      transitions from WHITE-->GRAY and GRAY-->WHITE. If there
   *      are successful transitions, insert a diagonal between the
   *      transition vertices.
   *
   *      At this point all the requisite cuts are made in cellMesh.
   *
   *   DEFN : A facet is BLACK, if there are no WHITE vertices.
   *
   *   4. Delete all BLACK facets in cellMesh.
   *
   *      NOTE: At this point, portions of the cut-cells on cell box faces remain
   *
   *   5. Do a flood-fill to collect connected pieces (a.k.a SHELL building)
   *      For each connected piece
   *        -- Create a cut-cell mesh, register in the corresponding gray-cell
   *
   *        NOTE: Open boundary in the cut-cell mesh is the b-face polygon.
   *
   *      Create a facet to fill the open-boundary, this closes the cut-cell.
   *
   */

  template<typename _MANIFOLD_OBJ, typename _GRID_CELL>
  BOOL tCUT_CELL_BUILDER<_MANIFOLD_OBJ, _GRID_CELL>::Build(cSURFACE_MESH *bfacesMesh) {
    BOOL retVal = true;

    Init();
    m_status = INITIALIZATION_COMPLETE;

    if (m_debug) {
      printf("%d %d\n", m_clay.NumVertices(), m_clay.NumFacets());
      sCUT_CELL_CLAY_FILTER filter(&m_clay);
      ExportToOff<cCUT_CELL_CLAY, sCUT_CELL_CLAY_FILTER>("init.off", m_clay, filter);
    }

    RegisterFacetVertices();

    if (AnyCornerVerticesColorUnknown())
      FindUnknownCornerVertexColors();

    m_status = REGISTERED_FACET_VERTICES;

    if (m_debug) {
      printf("%d %d\n", m_clay.NumVertices(), m_clay.NumFacets());
      sCUT_CELL_CLAY_FILTER filter(&m_clay);
      ExportToOff<cCUT_CELL_CLAY, sCUT_CELL_CLAY_FILTER>("out1.off", m_clay, filter);

      FILE *file1 = fopen("facet_vertices.off", "wb+");

      ExportToOff(file1, m_clay);
      fclose(file1);
    }

    RemoveTinyEdges();

    if (m_debug) {
      printf("%d %d\n", m_clay.NumVertices(), m_clay.NumFacets());
      sCUT_CELL_CLAY_FILTER filter(&m_clay);
      ExportToOff<cCUT_CELL_CLAY, sCUT_CELL_CLAY_FILTER>("removed_tiny_edges.off", m_clay, filter);
    }

    //Create faces that originate at white vertices.
    CreateWhiteFaces();

//    if (m_manifold->IsOpen())
//      ReflectGrayVertices();

    m_status = CREATED_WHITE_FACES;

    if (m_debug) {
      printf("%d %d\n", m_clay.NumVertices(), m_clay.NumFacets());
      sCUT_CELL_CLAY_FILTER filter(&m_clay);
      ExportToOff<cCUT_CELL_CLAY, sCUT_CELL_CLAY_FILTER>("white_and_black_faces.off", m_clay, filter);
    }

    cCUT_CELL_CLAY whiteClay, blackClay;
    SplitWhiteAndBlackPortions(whiteClay, blackClay);

    whiteClay.ConstructManifolds();
    blackClay.ConstructManifolds();

    typename cCUT_CELL_CLAY::manifold_iterator currManifold = whiteClay.ManifoldsBegin();
    typename cCUT_CELL_CLAY::manifold_iterator lastManifold = whiteClay.ManifoldsEnd();

    for (; currManifold != lastManifold; currManifold++) {
      cCUT_CELL *cutCell = m_cellEntry->NewCutCell();
      MeshFromManifold(currManifold->Index(), whiteClay, *cutCell);
    }

    currManifold = blackClay.ManifoldsBegin();
    lastManifold = blackClay.ManifoldsEnd();

    for (; currManifold != lastManifold; currManifold++) {
      cCUT_CELL *cutCell = m_cellEntry->NewCutCell();
      MeshFromManifold(currManifold->Index(), blackClay, *cutCell);
    }

//    if (m_debug) {
//      printf("%d %d\n", m_clay.NumVertices(), m_clay.NumFacets());
//      sCUT_CELL_CLAY_FILTER filter(&m_clay);
//      ExportToOff<cCUT_CELL_CLAY, sCUT_CELL_CLAY_FILTER>("removed_black_faces.off", m_clay, filter);
//    }

    //     //Extract cut-cells from the m_clay.
    BOOL retVal1 = true;
    retVal1 = ExtractCutCells(bfacesMesh);

    m_status = EXTRACTED_CUT_CELLS;

    m_debug = false;
    return retVal && retVal1;
  }
}
