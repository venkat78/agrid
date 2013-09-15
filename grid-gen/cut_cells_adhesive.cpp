#include "cut_cells_adhesive.hpp"

namespace grid_gen {
//  VOID cCUT_CELLS_ADHESIVE::AddWhiteCellVertices(cCUT_CELL &cell,
//                                               INT verticesMap[1024])
//  {
//    cCUT_CELL::vertex_iterator currVertex = cell.VerticesBegin();
//    cCUT_CELL::vertex_iterator lastVertex = cell.VerticesEnd();
//
//    INT numVertices = 0;
//    for( ; currVertex != lastVertex ; currVertex++ ) {
//      iGRID_VERTEX gridIndex = m_rectGrid->VertexIndex(currVertex->Point());
//      cWHITE_VERTICES_MAP::iterator pos = m_whiteVerticesMap.find(gridIndex);
//      if(pos == m_whiteVerticesMap.end()) {
//	iVERTEX gridVertexIndex = m_grid->NewVertex(currVertex->Point());
//	m_octree.Insert(gridVertexIndex);
//	m_whiteVerticesMap[gridIndex] = gridVertexIndex;
//	verticesMap[numVertices++] = gridVertexIndex;
//      }else {
//	iVERTEX gridVertexIndex = pos->second;
//	verticesMap[numVertices++] = gridVertexIndex;
//      }
//    }
//  }

  VOID cCUT_CELLS_ADHESIVE::AddCutCellVertices(cCUT_CELL &cell,
                                               INT verticesMap[1024])
  {
    cCUT_CELL::vertex_iterator currVertex = cell.VerticesBegin();
    cCUT_CELL::vertex_iterator lastVertex = cell.VerticesEnd();

    INT numVertices = 0;
    for( ; currVertex != lastVertex ; currVertex++ ) {
      std::vector<iVERTEX> matches;
      m_octree.FindMatches(currVertex->Point(), matches);

      ASSERT(matches.size() < 2);

      if(matches.size() == 0) {
        iVERTEX gridVertexIndex = m_grid->NewVertex(currVertex->Point());
        m_octree.Insert(gridVertexIndex);
        verticesMap[numVertices++] = gridVertexIndex;
      }
      else if(matches.size() == 1) {
        //Currently only worry about closed manifolds.
        //12/12/2010 -- Bujji
        verticesMap[numVertices++] = matches[0]; 
      }
    }
  }

  iFACE cCUT_CELLS_ADHESIVE::AddGridFace(iVERTEX gridFaceVertices[1024], 
                                         INT numVertices)
  {    
    m_grid->NewFacetStart();
    
    for( INT i = 0; i < numVertices ; i++ ) {
      m_grid->NewFacetAddVertex(gridFaceVertices[i]);
    }
    
    return m_grid->NewFacetClose();
  }

  iFACE cCUT_CELLS_ADHESIVE::FindGridFace(iVERTEX gridFaceVertices[1024],
                                          INT numVertices)
  {
    ASSERT(numVertices >= 3);

    iFACE possibleFaces[1024];
    INT numPossibleFaces = 0;
    
    //Finding possible faces.
    cVOLUMETRIC_GRID::cHALF_EDGE *incidentHe = NULL;
    m_grid->Vertex(gridFaceVertices[0])->Find(m_grid->Vertex(gridFaceVertices[1]),
                                              &incidentHe, NULL);

    if(!incidentHe)
      return INVALID_IFACET;
    
    cVOLUMETRIC_GRID::cHALF_EDGE* loopHe = incidentHe;

    do {
      if(!loopHe->IsBorder())
        possibleFaces[numPossibleFaces++] = loopHe->Facet()->Index();
      loopHe = loopHe->NextInRing();
    } while(loopHe != incidentHe);

    
    INT lowerIndex = 0, upperIndex = numPossibleFaces;
    for(INT i = 1; i < numVertices ; i++) {
      INT i1 = (i + 1)%numVertices;
      incidentHe = loopHe = NULL;
      m_grid->Vertex(gridFaceVertices[i])->Find(m_grid->Vertex(gridFaceVertices[i1]),
                                                &incidentHe, NULL);
      
      if(!incidentHe)
        return INVALID_IFACET;

      loopHe = incidentHe;

      BOOL foundMatchingFace = false;

      INT oldNumPossibleFaces = numPossibleFaces;
      do {
        if(!loopHe->IsBorder()) {
          iFACE faceIndex = loopHe->Facet()->Index();

          for(INT j = lowerIndex; j < upperIndex ; j++) {
            if(possibleFaces[j] == faceIndex) {
              possibleFaces[numPossibleFaces++] = faceIndex;
            }
          }
        }
        
        loopHe = loopHe->NextInRing();
        
      } while(loopHe != incidentHe);

      lowerIndex = oldNumPossibleFaces;
      upperIndex = numPossibleFaces;
      
      if(lowerIndex == upperIndex)
	return INVALID_IFACET; 
    }

    //This assert indicates there is more than one face incident
    //on gridFaceVertices. This breaks the 2-manifold property of 
    //the grid. First hit in cell index 9, 1, 17 in workspace-24g.1
    ASSERT(lowerIndex + 1 == upperIndex);

    
    return possibleFaces[numPossibleFaces - 1];
  }

  VOID cCUT_CELLS_ADHESIVE::AddCutCellFaces(cCUT_CELL &cell, INT verticesMap[1024],
                                            INT facesMap[1024], INT numGridVerticesOld)
  {
    cCUT_CELL::facet_iterator currFace = cell.FacetsBegin();
    cCUT_CELL::facet_iterator lastFace = cell.FacetsEnd();

    INT numFaces = 0;
    for( ; currFace != lastFace ; currFace++ ) {
      iVERTEX gridFaceVertices[1024];
      INT numFaceVertices = 0;

      cCUT_CELL::cFACET::vertex_circulator currVertex = currFace->VerticesBegin();
      cCUT_CELL::cFACET::vertex_circulator lastVertex = currFace->VerticesEnd();
      
      BOOL isANewFace = false;
      for( ; currVertex != lastVertex ; currVertex++ ) {
        iVERTEX gridVertexIndex = verticesMap[currVertex->Index()];

        if(gridVertexIndex >= numGridVerticesOld)
          isANewFace = true;

        gridFaceVertices[numFaceVertices++] = gridVertexIndex; 
      }

      ASSERT(numFaceVertices >= 3);

      if(isANewFace) {
        facesMap[numFaces] = AddGridFace(gridFaceVertices, numFaceVertices);
      }
      else {
        iFACE face = FindGridFace(gridFaceVertices, numFaceVertices);

        if(face == INVALID_IFACET)
          facesMap[numFaces] = AddGridFace(gridFaceVertices, numFaceVertices);
        else
          facesMap[numFaces] = face;
      }
      
      cVOLUMETRIC_GRID::cFACET* gridFace = m_grid->Facet(facesMap[numFaces]);
      numFaces++;

      ASSERT(gridFace);
      gridFace->IsFromInputSurface(currFace->IsFromInputSurface());      
    }
  }

  iCELL cCUT_CELLS_ADHESIVE::AddCutCell(cCUT_CELL &cell,  INT verticesMap[1024],
					INT facesMap[1024], INT numGridVerticesOld)
  {
    time_t t2 = clock();

    AddCutCellFaces(cell, verticesMap, facesMap, numGridVerticesOld);

    time_t t3 = clock();

    INT numFaces = cell.NumFacets();

    m_facesTicks += (t3 - t2);

    time_t t4 = clock() ;

    m_grid->NewCellStart();
    
    for(INT i = 0; i < numFaces ; i++) {
      m_grid->NewCellAddFace(facesMap[i]);
    }
    
    iCELL gridIndex = m_grid->NewCellClose();

    time_t t5 = clock();

    m_cellTicks += (t5 - t4);

    return gridIndex;
  }

  iCELL cCUT_CELLS_ADHESIVE::Add(cCUT_CELL &cell)
  {
    INT numVerticesOld = m_grid->NumVertices();

    INT verticesMap[1024];
    INT facesMap[1024];

    time_t t0 = clock();
    AddCutCellVertices(cell, verticesMap);
    time_t t1 = clock();

    m_verticesTicks += (t1 - t0);
    
    return AddCutCell(cell, verticesMap, facesMap, numVerticesOld);
  }
  
//  iCELL cCUT_CELLS_ADHESIVE::AddWhiteCell(cCUT_CELL &cell, iCELL_INDEX index)
//  {
//    INT numVerticesOld = m_grid->NumVertices();
//
//    INT verticesMap[1024];
//    INT facesMap[1024];
//
//    time_t t0 = clock();
//    AddWhiteCellVertices(cell, verticesMap);
//    time_t t1 = clock();
//
//    m_verticesTicks += (t1 - t0);
//
//    return AddCutCell(cell, verticesMap, facesMap, numVerticesOld, index);
//  }

}

