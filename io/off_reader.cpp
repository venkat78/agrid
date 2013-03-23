/*
 * off_reader.cpp
 *
 *  Created on: Feb 1, 2011
 */
#include "off_reader.hpp"
#include <vector>
#include <iostream>
#include <cstdio>

namespace io {
  cOFF_READER::cOFF_READER()
  {
	m_offFile = NULL;
	m_mapFile = NULL;
  }

  cOFF_READER::cOFF_READER(FILE *offFile, FILE *mapFile)
  {
	m_offFile = offFile;
	m_mapFile = mapFile;
  }

  VOID cOFF_READER::Import()
  {
	ImportOff();
	ImportMap();
  }

  VOID cOFF_READER::ImportOff()
  {
	char buffer[1024];
    INT lineNumber = 0; //for debugging
    INT readingMode = 0;
    INT numVertices, numFacets;

    while (fgets(buffer, 1024, m_offFile) != NULL) {
      lineNumber++;
      if (readingMode == 3){
    	INT nv = -1;
    	char *currentBuffer = buffer;
    	sscanf(currentBuffer, "%d", &nv);
    	m_facetNVTable.push_back(nv);
    	INT vInit = (INT)(m_facetVerticesTable.size());
    	m_facetV0Table.push_back(vInit);

    	for (INT i=0; i < nv; i++){
    	  currentBuffer = strchr(currentBuffer, ' ');
    	  currentBuffer++;
    	  INT v=-1;
    	  sscanf(currentBuffer, "%d", &v);
    	  m_facetVerticesTable.push_back(v);
    	}

    	sOFF_RGBA rgba;

    	currentBuffer = strchr(currentBuffer, ' ');
    	currentBuffer++;
    	sscanf(currentBuffer, "%f %f %f %f",
    			&(rgba.rgba_elt[0]),
    			&(rgba.rgba_elt[1]),
    			&(rgba.rgba_elt[2]),
    			&(rgba.rgba_elt[3]));
    	m_facetRgbaTable.push_back(rgba);
      }
      else if (readingMode == 2){
    	cPOINT3 vertexCoords;
    	sscanf(buffer, "%lg %lg %lg",
    			&(vertexCoords[0]), &(vertexCoords[1]), &(vertexCoords[2]));
    	m_vertexPointTable.push_back(vertexCoords);
    	if (m_vertexPointTable.size() == numVertices)
    	  readingMode = 3;
      }
      else if (readingMode == 0){
    	if (strncmp(buffer, "OFF", 3))
    	  return; //something is wrong with the file
    	readingMode = 1;
      }
      else if (readingMode == 1){
    	INT a;
    	sscanf(buffer, "%d %d %d", &numVertices, &numFacets, &a);
    	m_facetNVTable.reserve(numFacets);
    	m_facetV0Table.reserve(numFacets);
    	m_vertexPointTable.reserve(numVertices);
    	m_facetRgbaTable.reserve(numFacets);
    	readingMode = 2;
      }
    }
  }

  VOID cOFF_READER::PrintVertexPoint(FILE *file, INT v)
  {
	cPOINT3 vtxPoint = m_vertexPointTable[v];
	fprintf(file, "%lf %lf %lf \n", vtxPoint[0], vtxPoint[1], vtxPoint[2]);
  }

  VOID cOFF_READER::PrintVertexMap(FILE *file, INT v, INT offset)
  {
	INT a = m_vertexMapTable[v];
	if (a >= 0)
	  fprintf(file, "Vertex:  Off %d:	Grid %d;\n", v+offset, a);
	else
	  fprintf(file, "Vertex:  Off %d:	Mesh %d;\n", v+offset, -a-1);
  }

  VOID cOFF_READER::PrintFacetMap(FILE *file, INT f, INT offset)
  {
	INT a = m_facetMapTable[f];
	if (a >= 0) {
	  INT c = m_facetCellTable[f];
	  fprintf(file, "Facet:   Off %d:	Grid %d;\tCell %d;\n", f+offset, a, c);
	}
	else
	  fprintf(file, "Facet:   Off %d:	Mesh %d;\n", f+offset, -a-1);
  }

  VOID cOFF_READER::PrintFacet(FILE *file, INT f, INT offset)
  {
    fprintf(file, "%d", m_facetNVTable[f]);
    INT v0 = m_facetV0Table[f];
    for (INT v=0; v < m_facetNVTable[f]; v++)
      fprintf(file, " %d", m_facetVerticesTable[v0 + v] + offset);
    PrintFacetRGBA(file, f);
  }

  VOID cOFF_READER::PrintFacetRGBA(FILE *file, INT f)
  {
    if (m_facetRgbaTable.size() > 0){
      sOFF_RGBA facet_rgba =  m_facetRgbaTable[f];
      fprintf(file, " %.3f %.3f %.3f %.2f",
    		  facet_rgba.rgba_elt[0],
    		  facet_rgba.rgba_elt[1],
    		  facet_rgba.rgba_elt[2],
    		  facet_rgba.rgba_elt[3]);
    }
    fprintf(file, "\n");
  }

  //finds the index of this vertex in the VertexMapTable.
  //if not found, returns -1
  INT cOFF_READER::FindIndexInVertexMap(INT gridVertexIndex)
  {
	for (INT i=0; i < NumVertices(); i++){
	  if (m_vertexMapTable[i] == gridVertexIndex)
		return i;
	}
	return -1;
  }

  VOID cOFF_READER::ImportMap()
  {
	char buffer[1024];
	INT lineNumber = 0; //for debugging
	INT readingMode = 0;
	INT numVertices, numFacets;

	while (fgets(buffer, 1024, m_mapFile) != NULL) {
	  lineNumber++;
	  if (buffer[0] == '\n')
		continue;
	  if (readingMode >= 2){
		INT k = -1;
		BOOL isGrid;
		char *currentBuffer = strstr(buffer, "Grid");
		if (currentBuffer != NULL){
		  currentBuffer += sizeof("Grid");
		  isGrid = true;
		}
		else {
		  currentBuffer = strstr(buffer, "Mesh");
		  if (currentBuffer != NULL){
			currentBuffer += sizeof("Mesh");
			isGrid = false;
		  }
		}
		if (currentBuffer != NULL){
		  sscanf(currentBuffer, "%d", &k);
		  if (!isGrid)
			k = -k -1;
		}
		if (readingMode == 2){
		  m_vertexMapTable.push_back(k);
		  if (m_vertexMapTable.size() == numVertices)
			readingMode = 3;
		}
		else {
		  m_facetMapTable.push_back(k);
		  currentBuffer = strstr(currentBuffer, "Cell");
		  INT c = -2;
		  if (currentBuffer != NULL){
			currentBuffer += strlen("Cell");
			sscanf(currentBuffer, "%d", &c);
		  }
		  m_facetCellTable.push_back(c);
		}
	  }
	  else if (readingMode == 0){
		if (strncmp(buffer, "MAP", 3))
		  return; //something is wrong with the file
		readingMode = 1;
	  }
	  else if (readingMode == 1){
		sscanf(buffer, "%d %d", &numVertices, &numFacets);
		m_facetMapTable.reserve(numFacets);
		m_vertexMapTable.reserve(numVertices);
		m_facetCellTable.reserve(numFacets);
		readingMode = 2;
	  }
	}
  }

  //returns j-th vertex of facet f
  INT cOFF_READER::FacetVertex(INT f, INT j)
  {
	INT v0 = m_facetV0Table[f];
	return m_facetVerticesTable[v0+j];
  }
}//namespace
