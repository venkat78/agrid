/*
 * foam_reader.cpp
 *
 *  Created on: Jan 31, 2011
 */

#include "foam_reader.hpp"
#include <vector>
#include <iostream>
#include <cstdio>


namespace io {

  BOOL cFOAM_READER::ImportPoints()
  {
//	std::string filePath(m_filePath);
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
		char *buf = buffer+1;
		buf = strchr(buf, '(');
		buf++;
		buf = strchr(buf, '(');
		buf++;
		sscanf(buf, "%lg %lg %lg",
				vertexCoords, vertexCoords+1, vertexCoords+2);
		cPOINT3 vertexPoint(vertexCoords[0], vertexCoords[1], vertexCoords[2]);
		m_vertexPointTable.push_back(vertexPoint);
	  }
	}
	if ((INT)(m_vertexPointTable.size()) != numVertices)
	  return false;
	fclose(pointsFile);
	return true;
  }

  BOOL cFOAM_READER::ReadFacetCells(std::vector<INT>  *cellTable, char *file_name)
  {
//	std::string filePath(m_filePath);
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
		char *buf = strchr(buffer, 'C');
		buf++;
		sscanf(buf, "%d", &cellIndex);
		cellTable->push_back(cellIndex);
	  }
	}

	fclose(cellFile);
	if ((INT)(cellTable->size()) != numFacets)
	  return false;
	return true;
  }

  BOOL cFOAM_READER::ImportFacetCells()
  {
	BOOL rv = ReadFacetCells(&m_facetCell0Table, (char*)("owner"));
	if (!rv)
	  return false;
	rv = ReadFacetCells(&m_facetCell1Table, (char*)("neighbour"));
	if (!rv)
	  return false;
	return true;
  }

  BOOL cFOAM_READER::ImportFacets()
  {
//	std::string filePath(m_filePath);
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
		  INT v0Pos = (INT)(m_facetVerticesTable.size());
		  m_facetV0Table.push_back(v0Pos);
		  INT nv = -1;
		  char *buf = strchr(buffer, 'N');
		  if (buf == NULL)
			buf = strchr(buffer, 'O');
		  if (buf == NULL)
			buf = buffer;
		  buf = strchr(buf, ' ');
		  buf++;
		  sscanf(buf, "%d", &nv);
		  m_facetNVTable.push_back(nv);
		  buf = strchr(buf, '(');
		  if (buf == NULL)
			return false;
		  buf += 2;
		  for (INT i = 0; i < nv; i++){
			INT v = -1;
			sscanf(buf, "%d", &v);
			m_facetVerticesTable.push_back(v);
			if (i < nv-1){
			  buf = strchr(buf, ' ');
			  buf += 2;
			}
		  }
		}
	  }
	}
	return true;
  }

  BOOL cFOAM_READER::Import()
  {
	if (!ImportPoints())
		return false;
	if (!ImportFacetCells())
		return false;
	if (!ImportFacets())
		return false;
	return true;
  }

  VOID cFOAM_READER::ReadVertexTable(FILE *vertex_test_file)
  {
	INT vertexIndex = -1;
	while (fscanf(vertex_test_file, "%d", &vertexIndex) != EOF){
	  m_vertexIndexTable.push_back(vertexIndex);
	}

	//sort the vertices
	BOOL swapped = false;

	do {
	  swapped = false;
	  for (INT i=1; i < (INT)m_vertexIndexTable.size(); i++){
		if (m_vertexIndexTable[i] < m_vertexIndexTable[i-1]){
		  INT v = m_vertexIndexTable[i];
		  m_vertexIndexTable[i] = m_vertexIndexTable[i-1];
		  m_vertexIndexTable[i-1] = v;
		  swapped = true;
		}
	  }
	} while (swapped);
  }

  VOID cFOAM_READER::ReadFacetTable(FILE *facet_test_file)
  {
	INT facetIndex = -1;
	while (fscanf(facet_test_file, "%d", &facetIndex) != EOF){
	  m_facetIndexTable.push_back(facetIndex);
	}
	//sort the facets
	BOOL swapped = false;

	do {
	  swapped = false;
	  for (INT i=1; i < (INT)m_facetIndexTable.size(); i++){
		if (m_facetIndexTable[i] < m_facetIndexTable[i-1]){
		  INT f = m_facetIndexTable[i];
		  m_facetIndexTable[i] = m_facetIndexTable[i-1];
		  m_facetIndexTable[i-1] = f;
		  swapped = true;
		}
	  }
	} while (swapped);
  }

  BOOL cFOAM_READER::ExportFacetsContainingVertices()
  {
	if (!ImportFacets())
	  return false;

	FILE *face_test_file = fopen("facet.test", "wb+");
	INT s = (INT)m_facetV0Table.size();
	for (INT f=0; f < s; f++){
	  if (FacetContainsVertex(f))
		fprintf(face_test_file, "%d\n", f);
	}
	fclose(face_test_file);
	return true;
  }

  BOOL cFOAM_READER::ExportCellsContainingFacets()
  {
	if (!ImportFacetCells())
	  return false;

	std::vector<INT> cellTable; //may have duplicates

	for (INT f=0; f < (INT)m_facetIndexTable.size(); f++){
	  if (m_facetIndexTable[f] >= (INT)m_facetCell0Table.size() )
		break;
	  INT c = m_facetCell0Table[m_facetIndexTable[f]];
	  cellTable.push_back(c);
	}

	for (INT f=0; f < (INT)m_facetIndexTable.size(); f++){
	  if (m_facetIndexTable[f] >= (INT)m_facetCell1Table.size() )
		break;
	  INT c = m_facetCell1Table[m_facetIndexTable[f]];
	  cellTable.push_back(c);
	}

	BOOL swapped = true;

	do {
	  swapped = false;
	  for (INT i=1; i < (INT)cellTable.size(); i++){
		if (cellTable[i] < cellTable[i-1]){
		  INT c = cellTable[i];
		  cellTable[i] = cellTable[i-1];
		  cellTable[i-1] = c;
		  swapped = true;
		}
	  }
	} while (swapped);

	FILE *cell_test_file = fopen("cell.test", "wb+");

	fprintf(cell_test_file, "%d\n", cellTable[0]);

	for (INT i=1; i < (INT)cellTable.size(); i++){
	  if (cellTable[i] != cellTable[i-1])
		fprintf(cell_test_file, "%d\n", cellTable[i]);
	}
	fclose(cell_test_file);
	return true;
  }

  BOOL cFOAM_READER::FacetContainsVertex(INT f)
  {
	INT nv = m_facetNVTable[f];
	INT vPos = m_facetV0Table[f];
	for (INT v = 0; v < nv; v++){
	  INT vertexIndex = m_facetVerticesTable[vPos];
	  if (VertexIndexInTable(vertexIndex))
		return true;
	  vPos++;
	}
	return false;
  }

  BOOL cFOAM_READER::VertexIndexInTable(INT v)
  {
	INT i0 = 0, i1 = (INT)m_vertexIndexTable.size() - 1;
	INT v0 = m_vertexIndexTable[i0];
	if (v < v0)
	  return false;
	if (v == v0)
	  return true;
	INT v1 = m_vertexIndexTable[i1];
	if (v > v1)
	  return false;
	if (v == v1)
	  return true;

	while (i0 < i1 - 1){
	  INT i = (i0 + i1)/2;
	  if (m_vertexIndexTable[i] < v)
		i0 = i;
	  else if (m_vertexIndexTable[i] > v)
			i1 = i;
	  else
		return true;
	}
	return false;
  }

}//namespace
