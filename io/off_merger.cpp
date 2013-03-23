/*
 * off_merger.cpp
 *
 *  Created on: Feb 1, 2011
 */

#include "off_merger.hpp"
namespace io {

  cOFF_MERGER::cOFF_MERGER(){
	m_offBase = NULL;
	m_offAdd = NULL;
	m_mapBase = NULL;
	m_mapAdd = NULL;
	m_offExport = NULL;
	m_mapExport = NULL;
  }

  cOFF_MERGER::cOFF_MERGER(FILE *offBase, FILE *mapBase)
  {
	m_offBase = offBase;
	m_mapBase = mapBase;
	m_offAdd = NULL;
	m_mapAdd = NULL;
	m_offExport = NULL;
	m_mapExport = NULL;
	m_baseReader = cOFF_READER(offBase, mapBase);
  }

  VOID cOFF_MERGER::Append(FILE *offAdd, FILE *mapAdd)
  {
	m_offAdd = offAdd;
	m_mapAdd = mapAdd;
	m_addReader = cOFF_READER(offAdd, mapAdd);
  }

	VOID cOFF_MERGER::ExportFiles(FILE *offFile, FILE *mapFile)
	{
	  m_offExport = offFile;
	  m_mapExport = mapFile;
	}

  VOID cOFF_MERGER::Import()
  {
	ImportBase();
	ImportAdd();
  }

  VOID cOFF_MERGER::Export()
  {
	std::vector<INT> vertexMap;
	vertexMap.reserve(m_addReader.NumVertices());
	INT numAddedvertices = 0; //vertices which are not part of base cell geometry

	for (INT i=0; i < m_addReader.NumVertices(); i++){
	  INT vertexGridIndex = m_addReader.VertexMap(i);
	  INT vertexIndexInBase = m_baseReader.FindIndexInVertexMap(vertexGridIndex);
	  if (vertexIndexInBase < 0){
//		vertexIndexInBase = i;
		numAddedvertices++;
	  }
	  vertexMap.push_back(vertexIndexInBase);
	}

	INT totalNumFacets = m_baseReader.NumFacets() + m_addReader.NumFacets();
	INT totalNumVertices = m_baseReader.NumVertices() + numAddedvertices;

	fprintf(m_offExport, "OFF\n");
	fprintf(m_mapExport, "MAP\n");
	fprintf(m_offExport, "%d %d 0\n", totalNumVertices, totalNumFacets);
	fprintf(m_mapExport, "%d %d\n", totalNumVertices, totalNumFacets);

	//first print out the base vertices
	for (INT v=0; v < m_baseReader.NumVertices(); v++){
	  m_baseReader.PrintVertexPoint(m_offExport, v);
	  m_baseReader.PrintVertexMap(m_mapExport, v);
	}
	//print added vertices and recompute the vertexmap
	INT addVertexIndex = m_baseReader.NumVertices();
	for (INT v=0; v < vertexMap.size(); v++){
	  if (vertexMap[v] < 0){
		m_addReader.PrintVertexPoint(m_offExport, v);
		INT gridVertex = m_addReader.VertexMap(v);
		fprintf(m_mapExport, "Vertex:  Off %d:	Grid %d;\n", addVertexIndex, gridVertex);
		vertexMap[v] = addVertexIndex;
		addVertexIndex++;
	  }
	}

	fprintf(m_mapExport, "\n");
	//print base facets
	for (INT f=0; f < m_baseReader.NumFacets(); f++){
	  m_baseReader.PrintFacet(m_offExport, f);
	  m_baseReader.PrintFacetMap(m_mapExport, f);
	}

	//print added facets
	for (INT f=0; f < m_addReader.NumFacets(); f++){
	  //recompute the vertex indices
	  INT nv = m_addReader.FacetNumVertices(f);
	  fprintf(m_offExport, "%d", nv);
	  for (INT j = 0; j < nv; j++){
		INT v = m_addReader.FacetVertex(f, j);
		fprintf(m_offExport, " %d", vertexMap[v]);
	  }
	  m_addReader.PrintFacetRGBA(m_offExport, f);
	  m_addReader.PrintFacetMap(m_mapExport, f, m_baseReader.NumFacets());
	}
  }

}//namespace
