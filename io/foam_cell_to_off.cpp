/*
 * foam_cell_to_off.cpp
 *
 *  Created on: Jan 28, 2011
 */

#include "foam_cell_to_off.hpp"
#include <vector>
#include <iostream>
#include <cstdio>

namespace io {

  struct sFOAM_TABLES {
	std::vector<INT> facetNVTable; //number of facet vertices
	std::vector<INT> facetVerticesTable;//sequential table of facet vertices
	std::vector<INT> facetV0Table; //table of facet init vertex index in the previous table
	std::vector<INT> facetCell0Table; //owner
	std::vector<INT> facetCell1Table; //neighbour or -1
	std::vector<cPOINT3> vertexTable; //vertex coordinates
  };

  struct sCELL_DATA {
	INT cellIndex;
	std::vector<INT> facetIndex;
	std::vector<INT> vertexIndex;
	std::vector<INT> adjacentCellIndex;
  };

  BOOL ImportVertexTable(sFOAM_TABLES *tables, char *file_path)
  {
//	std::string filePath(file_path);
//	std::string pointsFileName = "../" + filePath + "/0/Polymesh/points";
//	FILE *pointsFile = fopen(pointsFileName.c_str(), "rb+");
	FILE *pointsFile = fopen("points", "rb+");
	if (pointsFile == NULL)
	  return false;

	char buffer[1024];
	INT lineNumber = 0; //for debugging
	INT readingLevel = 0;
	INT numVertices = -1;

	while (fgets(buffer, 1024, pointsFile) != NULL){
	  lineNumber++;
	  if (readingLevel == 0 && buffer[0] == '/' && buffer[1] == '/')
		readingLevel = 1;
	  else if (readingLevel == 1){
		sscanf(buffer, "%d", &numVertices);
		readingLevel = 2;
	  }
	  else if (readingLevel == 2){
		readingLevel = 3;
	  }
	  else if (readingLevel == 3){
		if (buffer[0] == ')')
		  break;
		REAL vertexCoords[3];
		sscanf(buffer+1, "%lg %lg %lg",
				vertexCoords, vertexCoords+1, vertexCoords+2);
		cPOINT3 vertexPoint(vertexCoords[0], vertexCoords[1], vertexCoords[2]);
		tables->vertexTable.push_back(vertexPoint);
	  }
	}
	if ((INT)(tables->vertexTable.size()) != numVertices)
	  return false;
	return true;
  }

  BOOL ReadFacetCellTable(std::vector<INT>  *cellTable, char *file_path, char *file_name)
  {
//	std::string filePath(file_path);
//	std::string fileName(file_name);
//	std::string fn = filePath + "/0/Polymesh/" + fileName;
//	FILE *cellFile = fopen(fn.c_str(), "rb+");
	FILE *cellFile = fopen(file_name, "rb+");
	if (cellFile == NULL)
	  return false;

	char buffer[1024];
	INT lineNumber = 0; //for debugging
	INT readingLevel = 0;
	INT numFacets = -1;

	while (fgets(buffer, 1024, cellFile) != NULL) {
	  lineNumber++;
	  if (readingLevel == 0 && buffer[0] == '/' && buffer[1] == '/')
		readingLevel = 1;
	  else if (readingLevel == 1){
		sscanf(buffer, "%d", &numFacets);
		readingLevel = 2;
	  }
	  else if (readingLevel == 2){
		readingLevel = 3;
	  }
	  else if (readingLevel == 3){
		if (buffer[0] == ')')
		  break;
		INT cellIndex = -1;
		sscanf(buffer, "%d", &cellIndex);
		cellTable->push_back(cellIndex);
	  }
	}

	fclose(cellFile);
	if ((INT)(cellTable->size()) != numFacets)
	  return false;
	return true;
  }

  BOOL importFacetCellTables(sFOAM_TABLES *tables, char *file_path)
  {
	BOOL rv = ReadFacetCellTable(&(tables->facetCell0Table), file_path, (char*)("owner"));
	if (!rv)
	  return false;
	rv = ReadFacetCellTable(&(tables->facetCell1Table), file_path, (char*)("neighbour"));
	if (!rv)
	  return false;
	return true;
  }

  BOOL ImportFacetTables(sFOAM_TABLES *tables, char *file_path)
  {
//	std::string filePath(file_path);
//	std::string facetsFileName = filePath + "/0/Polymesh/faces";
//	FILE *facetsFile = fopen(facetsFileName.c_str(), "rb+");
	FILE *facetsFile = fopen("faces", "rb+");
	if (facetsFile == NULL)
	  return false;

	char scanBuffer[1024];
	INT lineNumber = 0; //for debugging
	INT readingLevel = 0;
	INT numFacets = -1;

	while (fgets(scanBuffer, 1024, facetsFile) != NULL) {
	  char *buffer = scanBuffer;
	  lineNumber++;
	  if (readingLevel == 0 && buffer[0] == '/' && buffer[1] == '/')
		readingLevel = 1;
	  else if (readingLevel == 1){
		sscanf(buffer, "%d", &numFacets);
		readingLevel = 2;
	  }
	  else if (readingLevel == 2){
		readingLevel = 3;
	  }
	  else if (readingLevel == 3){
		if (buffer[0] == ')')
		  break;
		if (strlen(buffer) > 1){
		  INT v0Pos = (INT)(tables->facetVerticesTable.size());
		  tables->facetV0Table.push_back(v0Pos);
		  INT nv = -1;
		  sscanf(buffer, "%d", &nv);
		  tables->facetNVTable.push_back(nv);
		  buffer = strchr(buffer, '(');
		  if (buffer == NULL)
			return false;
		  buffer++;
		  for (INT i = 0; i < nv; i++){
			INT v = -1;
			sscanf(buffer, "%d", &v);
			tables->facetVerticesTable.push_back(v);
			if (i < nv-1){
			  buffer = strchr(buffer, ' ');
			  buffer++;
			}
		  }
		}
	  }
	}
	return true;
  }

  BOOL ImportTables(sFOAM_TABLES *tables, char *file_path)
  {
	if (!ImportVertexTable(tables, file_path))
	  return false;
	if (!importFacetCellTables(tables, file_path))
	  return false;
	if (!ImportFacetTables(tables, file_path))
	  return false;
	return true;
  }

  //if a is not in the vector, it's added and true is returned.
  //otherwise, false is returned.
  //it's admittedly slow
  BOOL 	push_back_unique1(std::vector<INT>  *intVector, INT a)
  {
	for (INT i = 0; i < (INT)(intVector->size()); i++){
	  INT v = intVector->at(i);
	  if (v == a)
		return false;
	}
	intVector->push_back(a);
	return true;
  }

  VOID FindCellFacets(sCELL_DATA *cellInfo, sFOAM_TABLES *tables)
  {
	for(int f = 0; f < (int)(tables->facetCell0Table.size()); f++){
	  if (tables->facetCell0Table[f] == cellInfo->cellIndex){
		cellInfo->facetIndex.push_back(f);
	  }
	}
	for(int f = 0; f < (int)(tables->facetCell1Table.size()); f++){
	  if (tables->facetCell1Table[f] == cellInfo->cellIndex){
		cellInfo->facetIndex.push_back(-f-1);
	  }
	}
  }

  VOID FindCellVertices(sCELL_DATA *cellInfo, sFOAM_TABLES *tables, cBOX3 *p_cellBox = NULL)
  {
	for (int i = 0; i < cellInfo->facetIndex.size(); i++){
	  int f = cellInfo->facetIndex[i];
	  if (f < 0)
		f = -f-1;
	  printf("f = %d\n", f);
	  int nv = tables->facetNVTable[f];
	  int v0 = tables->facetV0Table[f];
	  for (int j = 0; j < nv; j++){
		int v = tables->facetVerticesTable[v0+j];
		printf("\tv = %d\t", v);
		cPOINT3 vp = tables->vertexTable[v];
		printf("vp %lf %lf %lf\n", vp[0], vp[1], vp[2]);
		if (push_back_unique1(&cellInfo->vertexIndex, v) && p_cellBox != NULL){
		  (*p_cellBox) += tables->vertexTable[v];
		  p_cellBox->Print();
		}
	  }
	}
  }

  VOID FindAdjacentCells(sCELL_DATA *cellInfo, sFOAM_TABLES *tables)
  {
	for (int i = 0; i < (int)(cellInfo->facetIndex.size()); i++){
	  int f = cellInfo->facetIndex[i];
	  if (f >= 0){
		if (f < (int)(tables->facetCell1Table.size())){
		  int c = tables->facetCell1Table[f];
		  cellInfo->adjacentCellIndex.push_back(c);
		}
		else
		  cellInfo->adjacentCellIndex.push_back(-1);
	  }
	  else {
		f = -f - 1;
		int c = tables->facetCell0Table[f];
		cellInfo->adjacentCellIndex.push_back(c);
	  }
	}
  }

  VOID GenerateCellData(sCELL_DATA *cellInfo, sFOAM_TABLES *tables, cBOX3 *p_cellBox = NULL)
  {
	FindCellFacets(cellInfo, tables);
	FindCellVertices(cellInfo, tables, p_cellBox);
	FindAdjacentCells(cellInfo, tables);
  }

  INT VertexIndexGridToOff(INT vGrid, sCELL_DATA *cellInfo)
  {
	//linear search can be improved, but the vectors are typically small
	for (INT i = 0; i < cellInfo->vertexIndex.size(); i++){
	  if (cellInfo->vertexIndex[i] == vGrid)
		return i;
	}
	return -1; //should not happen
  }

  VOID CellToOff(sCELL_DATA *cellInfo, sFOAM_TABLES *tables)
  {
	char offFileName[32];
	char mapFileName[32];
	offFileName[0] = mapFileName[0] = '\0';
	sprintf(offFileName, "%d.off", cellInfo->cellIndex);
	sprintf(mapFileName, "%d.map", cellInfo->cellIndex);
	FILE *offFile = fopen(offFileName, "wb+");
	FILE *mapFile = fopen(mapFileName, "wb+");
	fprintf(offFile, "OFF\n");
	fprintf(mapFile, "MAP\n");
	fprintf(offFile, "%ld %ld 0\n", cellInfo->vertexIndex.size(), cellInfo->facetIndex.size());
	fprintf(mapFile, "%ld %ld\n", cellInfo->vertexIndex.size(), cellInfo->facetIndex.size());
	for (INT i = 0; i < cellInfo->vertexIndex.size(); i++){
	  INT v = cellInfo->vertexIndex[i];
	  cPOINT3 vtxPoint = tables->vertexTable[v];
	  fprintf(offFile, "%lf %lf %lf \n", vtxPoint[0], vtxPoint[1], vtxPoint[2]);
	  fprintf(mapFile, "Vertex:  Off %d:\tGrid %d;\n", i, v);
	}
	fprintf(mapFile, "\n");
	for (INT i = 0; i < cellInfo->facetIndex.size(); i++){
	  INT f = cellInfo->facetIndex[i];
	  INT c = cellInfo->adjacentCellIndex[i];
	  if (f >= 0){
		fprintf(mapFile, "Facet:   Off %d:\tGrid %d;\tCell %d;\n",  i, f, c);
		INT nv = tables->facetNVTable[f];
		INT v0 = tables->facetV0Table[f];
		fprintf(offFile, "%d ", nv);
		for (INT j = 0; j < nv; j++){
		  INT vGrid = tables->facetVerticesTable[v0+j];
		  INT vOff = VertexIndexGridToOff(vGrid, cellInfo);
		  fprintf(offFile, "%d ", vOff);
		}
	  }
	  else {
		f = -f - 1;
		fprintf(mapFile, "Facet:   Off %d:\tGrid %d;\tCell %d;\n", i, f, c);
		INT nv = tables->facetNVTable[f];
		INT v0 = tables->facetV0Table[f];
		fprintf(offFile, "%d ", nv);
		for (INT j = 0; j < nv; j++){
		  INT vGrid = tables->facetVerticesTable[v0+nv-j-1];
		  INT vOff = VertexIndexGridToOff(vGrid, cellInfo);
		  fprintf(offFile, "%d ", vOff);
		}
	  }
	  fprintf(offFile, "\n");
	}
	fclose(offFile);
	fclose(mapFile);
  }

  BOOL GenerateCellOffFiles(INT cellIndex, char *file_path,
		  		  std::vector<INT> *p_cellIndexVector,
		  		  std::vector<cBOX3> *p_cellBoxVector)
  {
	sFOAM_TABLES tables;
	ImportTables(&tables, file_path);

	if (!push_back_unique1(p_cellIndexVector, cellIndex))
	  return false;
	sCELL_DATA cellInfo;
	cellInfo.cellIndex = cellIndex;
	GenerateCellData(&cellInfo, &tables);

	CellToOff(&cellInfo, &tables);

	//do the adjacent cells
	for (INT i = 0; i < (INT)(cellInfo.adjacentCellIndex.size()); i++){
	  if (cellInfo.adjacentCellIndex[i] >= 0){
		sCELL_DATA adjacentCellInfo;
		adjacentCellInfo.cellIndex = cellInfo.adjacentCellIndex[i];
		GenerateCellData(&adjacentCellInfo, &tables);
		CellToOff(&adjacentCellInfo, &tables);
	  }
	}
	return true;
  }
}//namespace
