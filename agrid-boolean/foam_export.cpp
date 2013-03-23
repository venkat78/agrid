#include "foam_export.hpp"

namespace agrid_boolean {

  typedef  char BOUNDARY_NAME[24];
  BOUNDARY_NAME boundaryName[7] =
		  {"Inlet", "Outlet", "Sidewall1", "Sidewall2", "Floor", "Ceiling", "InputGeometry"};
  BOOL foam_export_print_debug_files = false;
  BOOL print_indices = false; //prints indices into openfoam files. That violates legal syntax, but helps with debugging
  INT face_debug_count = 0;

  BOOL PrintDebugFiles() { return foam_export_print_debug_files;}
  VOID PrintDebugFiles(BOOL doPrint) {foam_export_print_debug_files = doPrint;}

  struct sFOAM_FILE_POINTERS {
    FILE *ownerFile, *neighbourFile,
      *pointFile, *boundaryFile, *faceFile;
  };

  VOID AddFacetBox(cVOLUMETRIC_GRID::cFACET* facet, cBOX3 &box)
  {
	cVOLUMETRIC_GRID::cFACET::vertex_circulator currVertex = facet->VerticesBegin();
	cVOLUMETRIC_GRID::cFACET::vertex_circulator lastVertex = facet->VerticesEnd();

	for( ; currVertex != lastVertex ; currVertex++ ) {
	  cPOINT3 vertexPoint = currVertex->Point();
	  box += vertexPoint;
	}
  }


  VOID AddFacetBox(cVOLUMETRIC_GRID &grid, iFACET faceIndex, cBOX3 &box)
  {
	cVOLUMETRIC_GRID::cFACET* face = grid.Facet(faceIndex);
	AddFacetBox(face, box);
  }

  FILE *OpenFoamNeighbourFile(const std::string &filePath,
			     cVOLUMETRIC_GRID &grid)
  {
    std::string fileName = filePath; fileName += "/constant/polyMesh/neighbour";

    FILE *neighbourFile = fopen(fileName.c_str(), "wb+");

    fprintf(neighbourFile, "/*--------------------------------*- C++ -*----------------------------------*\\ \n");
    fprintf(neighbourFile, "| =========                 |                                                           |\n");
    fprintf(neighbourFile, "| \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox    |\n");
    fprintf(neighbourFile, "|  \\    /   O peration     | Version:  1.5.x                                            |\n");
    fprintf(neighbourFile, "|   \\  /    A nd           | Web:      http://www.OpenFOAM.org               |\n");
    fprintf(neighbourFile, "|    \\/     M anipulation  |                                                                  |\n");
    fprintf(neighbourFile, " \\*-----------------------------------------------------------------------------*/ \n");
    fprintf(neighbourFile, "FoamFile\n");
    fprintf(neighbourFile, "{\n");
    fprintf(neighbourFile, "version     2.0;\n");
    fprintf(neighbourFile, "format      ascii;\n");
    fprintf(neighbourFile, "class      labelList;\n");
    fprintf(neighbourFile, "note \"nPoints: %d nCells : %d nFaces : %d nInternalFaces : %d \" \n", grid.NumVertices(), grid.NumCells(),
            grid.NumFaces(), grid.NumInteriorFaces());
    fprintf(neighbourFile, "location    \"constant/polyMesh\";\n");
    fprintf(neighbourFile, "object     neighbor;\n");
    fprintf(neighbourFile,"}\n");
    fprintf(neighbourFile, "// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //\n");

    return neighbourFile;

  }

  FILE* OpenFoamOwnerFile(const std::string &filePath,
			  cVOLUMETRIC_GRID &grid)
  {
    std::string fileName = filePath; fileName += "/constant/polyMesh/owner";

    FILE *ownerFile = fopen(fileName.c_str(), "wb+");
    fprintf(ownerFile, "/*--------------------------------*- C++ -*----------------------------------*\\ \n");
    fprintf(ownerFile, "| =========                 |                                                           |\n");
    fprintf(ownerFile, "| \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox    |\n");
    fprintf(ownerFile, "|  \\    /   O peration     | Version:  1.5.x                                            |\n");
    fprintf(ownerFile, "|   \\  /    A nd           | Web:      http://www.OpenFOAM.org               |\n");
    fprintf(ownerFile, "|    \\/     M anipulation  |                                                                  |\n");
    fprintf(ownerFile, " \\*-----------------------------------------------------------------------------*/ \n");
    fprintf(ownerFile, "FoamFile\n");
    fprintf(ownerFile, "{\n");
    fprintf(ownerFile, "version     2.0;\n");
    fprintf(ownerFile, "format      ascii;\n");
    fprintf(ownerFile, "class     labelList;\n");
    fprintf(ownerFile, "note \"nPoints: %d nCells : %d nFaces : %d nInternalFaces : %d \" \n", grid.NumVertices(), grid.NumCells(),
	    grid.NumFaces(), grid.NumInteriorFaces());
    fprintf(ownerFile, "location    \"constant/polyMesh\";\n");
    fprintf(ownerFile, "object      owner;\n");
    fprintf(ownerFile,"}\n");
    fprintf(ownerFile, "// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //\n");
    return ownerFile;
  }

  FILE* OpenFoamBoundaryFile(const std::string &filePath,
			     cVOLUMETRIC_GRID &grid)
  {
    std::string fileName = filePath; fileName += "/constant/polyMesh/boundary";

    FILE *boundaryFile = fopen(fileName.c_str(), "wb+");
//    FILE *boundaryFile = stdout;
    fprintf(boundaryFile, "/*--------------------------------*- C++ -*----------------------------------*\\ \n");
    fprintf(boundaryFile, "| =========                 |                                                           |\n");
    fprintf(boundaryFile, "| \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox    |\n");
    fprintf(boundaryFile, "|  \\    /   O peration     | Version:  1.5.x                                            |\n");
    fprintf(boundaryFile, "|   \\  /    A nd           | Web:      http://www.OpenFOAM.org               |\n");
    fprintf(boundaryFile, "|    \\/     M anipulation  |                                                                  |\n");
    fprintf(boundaryFile, " \\*-----------------------------------------------------------------------------*/ \n");
    fprintf(boundaryFile, "FoamFile\n");
    fprintf(boundaryFile, "{\n");
    fprintf(boundaryFile, "version     2.0;\n");
    fprintf(boundaryFile, "format      ascii;\n");
    fprintf(boundaryFile, "class     polyBoundaryMesh;\n");
    fprintf(boundaryFile, "location    \"constant/polyMesh\";\n");
    fprintf(boundaryFile, "object      boundary;\n");
    fprintf(boundaryFile,"}\n");
    fprintf(boundaryFile, "// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //\n");

    return boundaryFile;
  }

  FILE* OpenFoamFaceFile(const std::string &filePath)
   {
    std::string fileName = filePath; fileName += "/constant/polyMesh/faces";

    FILE *faceFile = fopen(fileName.c_str(), "wb+");
    fprintf(faceFile, "/*--------------------------------*- C++ -*----------------------------------*\\ \n");
    fprintf(faceFile, "| =========                 |                                                           |\n");
    fprintf(faceFile, "| \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox    |\n");
    fprintf(faceFile, "|  \\    /   O peration     | Version:  1.5.x                                            |\n");
    fprintf(faceFile, "|   \\  /    A nd           | Web:      http://www.OpenFOAM.org               |\n");
    fprintf(faceFile, "|    \\/     M anipulation  |                                                                  |\n");
    fprintf(faceFile, " \\*-----------------------------------------------------------------------------*/ \n");
    fprintf(faceFile, "FoamFile\n");
    fprintf(faceFile, "{\n");
    fprintf(faceFile, "version     2.0;\n");
    fprintf(faceFile, "format      ascii;\n");
    fprintf(faceFile, "class     faceList;\n");
    fprintf(faceFile, "location    \"constant/polyMesh\";\n");
    fprintf(faceFile, "object      faces;\n");
    fprintf(faceFile,"}\n");
    fprintf(faceFile, "// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //\n");

    return faceFile;
  }

  FILE* OpenFoamPointFile(const std::string &filePath)
  {
    std::string fileName = filePath; fileName += "/constant/polyMesh/points";

    FILE *file = fopen(fileName.c_str(), "wb+");
    fprintf(file, "/*--------------------------------*- C++ -*----------------------------------*\\ \n");
    fprintf(file, "| =========                 |                                                           |\n");
    fprintf(file, "| \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox    |\n");
    fprintf(file, "|  \\    /   O peration     | Version:  1.5.x                                            |\n");
    fprintf(file, "|   \\  /    A nd           | Web:      http://www.OpenFOAM.org               |\n");
    fprintf(file, "|    \\/     M anipulation  |                                                                  |\n");
    fprintf(file, " \\*-----------------------------------------------------------------------------*/ \n");
    fprintf(file, "FoamFile\n");
    fprintf(file, "{\n");
    fprintf(file, "version     2.0;\n");
    fprintf(file, "format      ascii;\n");
    fprintf(file, "class      vectorField;\n");
    fprintf(file, "location    \"constant/polyMesh\";\n");
    fprintf(file, "object      points;\n");
    fprintf(file,"}\n");
    fprintf(file, "// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //\n");

    return file;
  }

  VOID OpenFoamFiles(sFOAM_FILE_POINTERS &ptr,
		     const std::string &filePath,
		     cVOLUMETRIC_GRID &grid) {
    ptr.pointFile = OpenFoamPointFile(filePath);
    ptr.faceFile = OpenFoamFaceFile(filePath);
    ptr.ownerFile = OpenFoamOwnerFile(filePath, grid);
    ptr.neighbourFile = OpenFoamNeighbourFile(filePath, grid);
    ptr.boundaryFile = OpenFoamBoundaryFile(filePath, grid);
  }

  VOID CloseFoamFiles(sFOAM_FILE_POINTERS &ptr)
  {
    fclose(ptr.pointFile);
    fclose(ptr.faceFile);
    fclose(ptr.ownerFile);
    fclose(ptr.neighbourFile);
    fclose(ptr.boundaryFile);
  }

  VOID ExportGridVerticesToFoam(FILE *file,
				cVOLUMETRIC_GRID &grid,
				std::vector<INT> &validVtxMap, cBOB *bob)
  {
    cVOLUMETRIC_GRID::vertex_iterator currVertex = grid.VerticesBegin();
    cVOLUMETRIC_GRID::vertex_iterator lastVertex = grid.VerticesEnd();

    INT numVertices = 0;
    FILE *vertices_debug_file = PrintDebugFiles() ? fopen("vertices.debug", "wb+") : NULL;
    fprintf(file,"%d\n", grid.NumVertices() + bob->NumVertices());
    fprintf(file, "(\n");
    if (PrintDebugFiles())
      fprintf(vertices_debug_file, "NumVertices %d\n", grid.NumVertices());
   for( ; currVertex != lastVertex ; currVertex++ ) {
	 iVERTEX vtxIndex = currVertex->Index(); //for debugging
      cPOINT3 point = currVertex->Point();
      bob->RegisterVertexIfOnGrid(point, numVertices);
      validVtxMap[currVertex->Index()] = numVertices++;
 	 if (print_indices)
 	   fprintf(file, "V%d  (G%d)  ", numVertices-1, vtxIndex);
      fprintf(file, "(%lg %lg %lg)\n", point[0], point[1], point[2]);
      if (PrintDebugFiles())
        fprintf(vertices_debug_file, "V%d\t{%lg, %lg, %lg}\n",
    		  currVertex->Index(), point[0], point[1], point[2]);
    }

   //print out vertices from the external grid
   for (INT i = 0; i < bob->VertexMapSize(); i++){
	 if (bob->RegisterExternalVertex(i, numVertices)){
	   cPOINT3 point = bob->VertexPoint(i);
	   if (print_indices)
		fprintf(file, "V%d (EXTERNAL  ", numVertices);
	   fprintf(file, "(%lg %lg %lg)\n", point[0], point[1], point[2]);
	   if (PrintDebugFiles())
		   fprintf(vertices_debug_file, "V%d\t{%lg, %lg, %lg}\n",
				   numVertices, point[0], point[1], point[2]);
	   numVertices++;
	 }
   }
    fprintf(file, ")\n");
    if (PrintDebugFiles())
      fclose(vertices_debug_file);
  }

  VOID ExportFace(sFOAM_FILE_POINTERS &files, cVOLUMETRIC_GRID &grid,
		  iFACE faceIndex, std::vector<INT> &validVtxMap,
		  INT numFacesWritten,
		  iCELL neighborCellIndex = INVALID_ICELL)
  {
	if (PrintDebugFiles()){
	  FILE *faces_debug_file = fopen("faces.debug", "ab+");
	  fprintf(faces_debug_file, "f%d:\t", faceIndex);
	  face_debug_count++;
	  fclose(faces_debug_file);
	}
    cVOLUMETRIC_GRID::cFACET* face = grid.Facet(faceIndex);

    INT numVertices = face->NumVertices();

    fprintf(files.faceFile,"\n");
    if (print_indices){
    	fprintf(files.faceFile,"F%d:  (G%d)  O%d:  ",
    			numFacesWritten, faceIndex, face->LeftCellIndex());
        if(face->RightCellIndex() != INVALID_ICELL)
        	fprintf(files.faceFile,"N%d:  ",   face->RightCellIndex());
    }
    fprintf(files.faceFile,"%d(", numVertices);

    cVOLUMETRIC_GRID::cFACET::vertex_circulator currVertex = face->VerticesBegin();
    cVOLUMETRIC_GRID::cFACET::vertex_circulator lastVertex = face->VerticesEnd();

    for( ; currVertex != lastVertex ; currVertex++ ) {
      INT vertexIndex = validVtxMap[currVertex->Index()];
      ASSERT(vertexIndex >= 0);
      if (print_indices)
    	fprintf(files.faceFile, "V");
	  fprintf(files.faceFile, "%d ", vertexIndex);
      if (PrintDebugFiles()){
    	FILE *faces_debug_file = fopen("faces.debug", "ab+");
    	fprintf(faces_debug_file, "%d; ", vertexIndex);
    	fclose(faces_debug_file);
      }
    }

    fprintf(files.faceFile, ")\n");
    if (PrintDebugFiles()){
      FILE *faces_debug_file = fopen("faces.debug", "ab+");
      fprintf(faces_debug_file, "\n");
      fclose(faces_debug_file);
    }

    if (print_indices)
     	fprintf(files.ownerFile,"F %d  (G%d)  C", numFacesWritten, faceIndex);
    fprintf(files.ownerFile, "%d\n", face->LeftCellIndex());

    if(face->RightCellIndex() != INVALID_ICELL)
    	ASSERT(face->RightCellIndex() == neighborCellIndex);
    if (neighborCellIndex != INVALID_ICELL){
      if (print_indices)
         	fprintf(files.neighbourFile,"F %d  (G%d)  C", numFacesWritten, faceIndex);
      fprintf(files.neighbourFile, "%d\n", neighborCellIndex);
    }
  }


  //facets loacted on the bob graygrid box are not boundary facets
  eBOX_FACE  FacetIsOnInterface(cVOLUMETRIC_GRID &grid,
				  iFACET facetIndex, cBOB *bob)
  {
	cBOX3 &grayGridBox = bob->GrayGridVol();
    cVOLUMETRIC_GRID::cFACET* facet = grid.Facet(facetIndex);
    return facet->IsOnBoxBoundary(grayGridBox);
  }

  //return index of a white cell adjacent to the inoput facet. The inout facet lies on the
  //gray grid boundary. It is also assumed that this facet is rectangular which it should be.
  iCELL InterfaceFacetCellIndex(cVOLUMETRIC_GRID &grid,
		  iFACET facetIndex, cBOB *bob)
  {
	//1. find facet vertex with the smallest coordinates.
	cVOLUMETRIC_GRID::cFACET* facet = grid.Facet(facetIndex);

	cVOLUMETRIC_GRID::cFACET::vertex_circulator currVertex = facet->VerticesBegin();
	cVOLUMETRIC_GRID::cFACET::vertex_circulator lastVertex = facet->VerticesEnd();
	BOOL facetCoordPlane[3] = {true, true, true};

	//use first vertex differently
	cPOINT3 firstPoint = currVertex->Point();
	currVertex++;
	cPOINT3 minCoordVertexPoint = firstPoint;

	for( ; currVertex != lastVertex ; currVertex++ ) {
	  cPOINT3 vertexPoint = currVertex->Point();
	  DO_COORDS(coord){
		if (vertexPoint[coord] < minCoordVertexPoint[coord])
			minCoordVertexPoint[coord] = vertexPoint[coord];
		if (fabs(vertexPoint[coord] - firstPoint[coord]) > cLIMITS::Tolerance())
		  facetCoordPlane[coord] = false;
	  }
	}

	eCOORD facetCoord = GEOM_COORD_NONE;
	DO_COORDS(coord){
	  if (facetCoordPlane[coord])
		facetCoord = coord;
	}
	iCELL cellIndex = bob->VertexPointCellIndex(minCoordVertexPoint, facetCoord);
	if (cellIndex >=0) //YG added on 03-21
	  bob->RegisterInterfaceFacet(minCoordVertexPoint, facetCoord,
			facetIndex, cellIndex);
	return cellIndex;
  }

  BOOL RegisterIfOnExternalBoundary(INT cellIndex, INT facetIndex,
		  cVOLUMETRIC_GRID &grid, cBOB *bob, std::vector<INT> &validVtxMap,
		  eBOX_FACE boxFacePos, BORDER_FACETS_TABLE *borderWhiteFacets)
  {
	cBOX3 &simVol = bob->SimVol();
	cVOLUMETRIC_GRID::cFACET* facet = grid.Facet(facetIndex);
    if (facet->IsOnBoxBoundary(simVol, boxFacePos) == BOX_FACE_NONE)
      return false; //the facet is on the simvol boundary

    ASSERT(facet->NumVertices() == 4);

	cVOLUMETRIC_GRID::cFACET::vertex_circulator currVertex = facet->VerticesBegin();
	cVOLUMETRIC_GRID::cFACET::vertex_circulator lastVertex = facet->VerticesEnd();
	INT i = 0;
	INT vertices[4];

	for( ; currVertex != lastVertex ; currVertex++ )
	  vertices[i++] = validVtxMap[currVertex->Index()];

	cBORDER_FACET_RECORD borderFacet(cellIndex, vertices);
	borderWhiteFacets->push_back(borderFacet);
	return true;
  }

  //check pairs which don't have a cell, and if a facet lies on the graygrid boundary,
  //replace it with the corresponding white cell, and if not, remove
  VOID ComputeNeighborsOnInterface(std::vector<sFACE_CELL_PAIR> &neighbors,
		  cVOLUMETRIC_GRID &grid, cBOB *bob, INT cellIndex,
		  std::vector<INT> &validVtxMap,
		  BORDER_FACETS_TABLE *borderWhiteFacets)
  {
	std::vector<sFACE_CELL_PAIR>::iterator currPair = neighbors.begin();
	std::vector<sFACE_CELL_PAIR>::iterator lastPair = neighbors.end();
	INT count = -1;
	for( ; currPair != lastPair ; currPair++ ) {
	  count++;
	  if(currPair->cellIndex < 0) {
		eBOX_FACE boxFacePos = FacetIsOnInterface(grid, currPair->faceIndex, bob);
//		grid.Facet(currPair->faceIndex)->Print();
		if (boxFacePos != BOX_FACE_NONE){
			//improve performance by passing the facePos to the call below
		  currPair->cellIndex = InterfaceFacetCellIndex(grid, currPair->faceIndex, bob);
//		  printf("count %d; boxFacePos = %d; cellIndex = %d\n", count, boxFacePos, currPair->cellIndex);
		  if (currPair->cellIndex >= 0)
		    currPair->faceIndex = -currPair->faceIndex + INVALID_IFACET - 1; //as a sign of non-interior face
		  else
			RegisterIfOnExternalBoundary(cellIndex, currPair->faceIndex, grid, bob, validVtxMap,
					boxFacePos, borderWhiteFacets+boxFacePos);
		}
	  }
	}
  }

  INT faceCount = 0; //for debugging

  VOID ExportCell(sFOAM_FILE_POINTERS &files, cVOLUMETRIC_GRID &grid,
		  cBOB *bob, INT cellIndex,
		  std::vector<BOOL> &facesExported,
		  std::vector<INT> &validVtxMap,
		  BORDER_FACETS_TABLE *borderWhiteFacets,
		  INT &numFacesWritten)
  {
    std::vector<sFACE_CELL_PAIR> neighbors;
    FILE *cell_debug_file = PrintDebugFiles() ?  fopen("cells.debug", "ab+") : NULL;
    FILE *nei_debug_file = PrintDebugFiles() ?  fopen("neighbors.debug", "ab+") : NULL;
    if (PrintDebugFiles())
      fprintf(cell_debug_file, "c%d:\t", cellIndex);
    BOOL borderNeighbors = grid.Cell(cellIndex)->Neighbours(neighbors);
    if (borderNeighbors){
    	//some of the neighbors may be on the interface and need to be computed
      ComputeNeighborsOnInterface(neighbors, grid, bob, cellIndex,
    		  validVtxMap, borderWhiteFacets);
    }
    sFACE_CELL_PAIR_CMP faceCellPairCmp;
    std::sort(neighbors.begin(), neighbors.end(), faceCellPairCmp);

    std::vector<sFACE_CELL_PAIR>::iterator currPair = neighbors.begin();
    std::vector<sFACE_CELL_PAIR>::iterator lastPair = neighbors.end();

    for( ; currPair != lastPair ; currPair++ ) {
      if (PrintDebugFiles()){
    	if (currPair->cellIndex >= 0){ //temporary
    	  fprintf(cell_debug_file, "f%d; ", currPair->faceIndex);
        fprintf(nei_debug_file, "written face %d\tthis cell %d\tface %d\t neighbor %d;",
        		faceCount, cellIndex, currPair->faceIndex, currPair->cellIndex);
    	}
      }
      if(currPair->cellIndex > cellIndex) {
    	BOOL FaceIsNotInterior = false;
    	if (currPair->faceIndex < INVALID_IFACET){
    		currPair->faceIndex = -currPair->faceIndex + INVALID_IFACET - 1;
    		FaceIsNotInterior = true;
    	}
        cVOLUMETRIC_GRID::cFACET* face = grid.Facet(currPair->faceIndex);
        if((FaceIsNotInterior || face->IsInterior()) && !facesExported[currPair->faceIndex]) {
          ExportFace(files,grid, currPair->faceIndex, validVtxMap,
        		  numFacesWritten, currPair->cellIndex);
          numFacesWritten++;
          facesExported[currPair->faceIndex] = true;
          if (PrintDebugFiles()){
        	fprintf(nei_debug_file, "exported");
        	faceCount++;
          }
        }
      }
      if (PrintDebugFiles()){
    	if (currPair->cellIndex >= 0)
    	  fprintf(nei_debug_file, "\n");
      }
    }
    if (PrintDebugFiles()){
      fprintf(cell_debug_file, "\n");
      fclose(cell_debug_file);
      fclose(nei_debug_file);
    }
  }

  VOID ExportWhiteCell(sFOAM_FILE_POINTERS &files, cWHITE_CELL &whiteCell,
		  BORDER_FACETS_TABLE *borderWhiteFacets, INT &numFacesWritten)
  {
	for (INT f = 0; f < 6; f++){
//	  printf("f = %d; borderWhiteFacets size %ld; cell %d; adjCell %d; cellFace = %d\n",
//			  f, borderWhiteFacets[f].size(), whiteCell.CellIndex(),
//			  whiteCell.AdjacentCellIndex(f), whiteCell.WhiteFacetRecord(f).CellFace());
	  if (whiteCell.AdjacentCellIndex(f) > whiteCell.CellIndex()){
		INT vertexIndex[4];
		for (INT v=0; v<4; v++){
		  vertexIndex[v] = whiteCell.VertexIndex(f, v);
		  ASSERT(vertexIndex[v] >= 0);
		}
		if (PrintDebugFiles()){
		  FILE *faces_debug_file = fopen("faces.debug", "ab+");
		  fprintf(faces_debug_file,
					"f%d:\t%d; %d; %d; %d; \n",
					face_debug_count++,	vertexIndex[0], vertexIndex[1], 	vertexIndex[2], vertexIndex[3]);
		  fclose(faces_debug_file);
		}

		fprintf(files.faceFile,"\n");
		if (print_indices){
			fprintf(files.faceFile,"F%d: O%d: N%d: ",
						numFacesWritten,
						whiteCell.CellIndex(),
						whiteCell.AdjacentCellIndex(f));
			  fprintf(files.faceFile,"4(V%d V%d V%d V%d )\n",
						whiteCell.VertexIndex(f, 0),
						whiteCell.VertexIndex(f, 1),
						whiteCell.VertexIndex(f, 2),
						whiteCell.VertexIndex(f, 3));
		}
		else
		  fprintf(files.faceFile,"4(%d %d %d %d )\n",
					whiteCell.VertexIndex(f, 0),
					whiteCell.VertexIndex(f, 1),
					whiteCell.VertexIndex(f, 2),
					whiteCell.VertexIndex(f, 3));
	    if (print_indices)
	     	fprintf(files.ownerFile,"F %d C", numFacesWritten);
		fprintf(files.ownerFile, "%d\n", whiteCell.CellIndex());
	    if (print_indices)
	     	fprintf(files.neighbourFile,"F %d C", numFacesWritten);
		fprintf(files.neighbourFile, "%d\n", whiteCell.AdjacentCellIndex(f));
		numFacesWritten++;
	  }
	  else if(whiteCell.AdjacentCellIndex(f) == -1){
		iCELL cellIndex = whiteCell.CellIndex();
		eBOX_FACE cellFace = whiteCell.WhiteFacetRecord(f).CellFace();
		ASSERT(cellFace >= 0 && cellFace < 6);
		INT *vertices = whiteCell.FacetVertices(f);
		cBORDER_FACET_RECORD borderFacet(cellIndex, vertices);
		borderWhiteFacets[cellFace].push_back(borderFacet);
//		printf("f = %d; cellFace = %d; borderWhiteFacets size %ld\n",
//				f, cellFace, borderWhiteFacets[cellFace].size());
	  }
	}
  }

  VOID ExportBorderWhiteFacet(sFOAM_FILE_POINTERS &files,
		  cBORDER_FACET_RECORD &borderFacet, INT &numFacesWritten)
  {
	if (PrintDebugFiles()){
	  FILE *faces_debug_file = fopen("faces.debug", "ab+");
	  fprintf(faces_debug_file,
			  //"f%d:\t, %d; %d; %d; %d; \n",
			  //face_debug_count++,	borderFacet.VertexIndex(0), borderFacet.VertexIndex(1),
			  "%d; %d; %d; %d; \n",
			  borderFacet.VertexIndex(0), borderFacet.VertexIndex(1),
			  borderFacet.VertexIndex(2), borderFacet.VertexIndex(3));
	  fclose(faces_debug_file);
	}
	fprintf(files.faceFile,"\n");
	if (print_indices){
	  fprintf(files.faceFile,"F%d ", numFacesWritten);
		fprintf(files.faceFile,"4(V%d V%d V%d V%d )\n",
				borderFacet.VertexIndex(0),
				borderFacet.VertexIndex(1),
				borderFacet.VertexIndex(2),
				borderFacet.VertexIndex(3));
	}
	else
		fprintf(files.faceFile,"4(%d %d %d %d )\n",
				borderFacet.VertexIndex(0),
				borderFacet.VertexIndex(1),
				borderFacet.VertexIndex(2),
				borderFacet.VertexIndex(3));
	if (print_indices)
	  fprintf(files.ownerFile,"F %d  C", numFacesWritten);
	fprintf(files.ownerFile, "%d\n", borderFacet.CellIndex());
	numFacesWritten++;
  }

  VOID ExportExteriorBoundaryFaces(sFOAM_FILE_POINTERS &files,
		  BORDER_FACETS_TABLE &borderWhiteFacets, INT boundaryIndex,
		  INT &numFacesWritten)
  {
	//write data into the boundry file
//	printf("%s: boundaryWhiteFacets %d\n", boundaryName[boundaryIndex], borderWhiteFacets.size());
	fprintf(files.boundaryFile, "%s\n{\n\ttype\t wall;\n\tnFaces\t %d;\n\tstartFace\t %d;\n}\n",
			boundaryName[boundaryIndex],
			(INT)borderWhiteFacets.size(),
			numFacesWritten);

	for (INT i=0; i < (INT)borderWhiteFacets.size(); i++){
		ExportBorderWhiteFacet(files, borderWhiteFacets[i], numFacesWritten);
	}
//	numFacesWritten += (INT)borderWhiteFacets.size();
  }

  VOID ExportInputGeometryBoundary(sFOAM_FILE_POINTERS &files,
		  cVOLUMETRIC_GRID &grid, std::vector<INT> &interiorBoundaryFaces,
		  std::vector<INT> &validVtxMap, INT &numFacesWritten)
  {
	//write data into the boundry file
	fprintf(files.boundaryFile, "%s\n{\n\ttype\t wall;\n\tnFaces\t %d;\n\tstartFace\t %d;\n}\n",
			  boundaryName[6],
			  (INT)interiorBoundaryFaces.size(),
			  numFacesWritten);

	for (INT i=0; i < (INT)interiorBoundaryFaces.size(); i++){
	  ExportFace(files, grid, interiorBoundaryFaces[i], validVtxMap, numFacesWritten);
	  numFacesWritten++;
	}
  }

  /*
   * This file will write owner, neighbour, boundary file and faces file.
   * First all the interior faces are written then, boundary faces are written
   * out and boundary file is setup.
   *
   * A very dumb implementation.
   */
  VOID ExportGridFacesToFoam(sFOAM_FILE_POINTERS &files,
			     cVOLUMETRIC_GRID &grid, cBOB *bob,
			     std::vector<INT> &validVtxMap)
  {
    cBOX3 simVol = grid.SimulationVolume();
    bob->InitCellIndex(grid.NumCells());
    cVOLUMETRIC_GRID::facet_iterator currFacet = grid.FacetsBegin();
    cVOLUMETRIC_GRID::facet_iterator lastFacet = grid.FacetsEnd();
    std::vector<INT> interiorBoundaryFaces;

    for( ; currFacet != lastFacet ; currFacet++ ){
      if(currFacet->IsBoundary() &&
    		  (FacetIsOnInterface(grid, currFacet->Index(), bob) == BOX_FACE_NONE)){
        interiorBoundaryFaces.push_back(currFacet->Index());
      }
    }

    std::vector<BOOL> facesExported(grid.LargestFacetIndex() + 1, false);

    INT totalNumFacets = grid.NumFaces() + bob->NumFacets();
    INT numBoundaryFacets = (INT)interiorBoundaryFaces.size()+
				bob->NumExternalBoundaryFacets();

    fprintf(files.faceFile, "%d\n(\n", totalNumFacets);
    fprintf(files.ownerFile, "%d\n(\n", totalNumFacets);
    fprintf(files.neighbourFile, "%d\n(\n", totalNumFacets - numBoundaryFacets);

    INT numFacesWritten = 0;

    BORDER_FACETS_TABLE borderWhiteFacets[6];
    //If any of the cells are deleted, the loop will fail.
    for(INT i = 0; i < (INT)grid.NumCells(); i++) {
      ExportCell(files, grid, bob, i, facesExported,
    		  validVtxMap, borderWhiteFacets, numFacesWritten);
    }

//    printf("numFacesWritten %d\n", numFacesWritten);
//    for (INT i=0; i < 6; i++)
//    	printf("%d: numBorderFacets = %ld\n", i, borderWhiteFacets[i].size());
//    printf("numWhiteCells = %d\n", bob->NumCells());
    for(INT i = 0; i < bob->NumCells(); i++) {
      cWHITE_CELL whiteCell = bob->ComputeWhiteCell(i);
      ExportWhiteCell(files, whiteCell, borderWhiteFacets, numFacesWritten);
    }

    fprintf(files.boundaryFile,"7\n(\n");
    for (INT c=0; c<6; c++)
      ExportExteriorBoundaryFaces(files, borderWhiteFacets[c], c, numFacesWritten);

    //Writing input geometry face
    ExportInputGeometryBoundary(files, grid, interiorBoundaryFaces, validVtxMap,
    		numFacesWritten);

    fprintf(files.faceFile, ")\n");
    fprintf(files.ownerFile, ")\n");
    fprintf(files.neighbourFile, ")\n");
    fprintf(files.boundaryFile, ")\n");
  }

  VOID ExportGridToFoam(const std::string &filePath,
			  cVOLUMETRIC_GRID &cutCellGrid, cBOB *bob)
  {
    sFOAM_FILE_POINTERS files;

    OpenFoamFiles(files, filePath, cutCellGrid);

    /*
     * Export points and export faces.
     */
    std::vector<INT> validVtxMap(cutCellGrid.LargestVertexIndex() + 1);
    printf("Exporting grid vertices to OpenFOAM files....\n");
    ExportGridVerticesToFoam(files.pointFile, cutCellGrid, validVtxMap, bob);

    printf("Exporting grid cells to OpenFOAM files....\n");

    ExportGridFacesToFoam(files, cutCellGrid, bob, validVtxMap);
    CloseFoamFiles(files);
  }

}//namespace
