/*
 * cell_record_generator.cpp
 *
 *  Created on: Jan 31, 2011
 */

#include "cell_record_generator.hpp"

namespace io{
//if a is not in the vector, it's added and true is returned.
//otherwise, false is returned.
//it's admittedly slow
BOOL push_back_unique(std::vector<INT>  *intVector, INT a)
{
	for (INT i = 0; i < (INT)(intVector->size()); i++){
	  INT v = intVector->at(i);
	  if (v == a)
		return false;
	}
	intVector->push_back(a);
	return true;
}

  cCELL_RECORD_GENERATOR::cCELL_RECORD_GENERATOR(cFOAM_READER *foamReader, INT cellIndex)
  {
	m_cellIndex = cellIndex;
	m_foamReader = foamReader;
  }

  VOID cCELL_RECORD_GENERATOR::GenerateFacets()
  {
	for(INT f = 0; f < m_foamReader->OwnerCellTableSize(); f++){
	  if (m_foamReader->FacetOwnerCell(f) == m_cellIndex){
		m_facetIndex.push_back(f);
	  }
	}
	for(INT f = 0; f < m_foamReader->NeighborCellTableSize(); f++){
	  if (m_foamReader->FacetNeighborCell(f) == m_cellIndex){
		m_facetIndex.push_back(-f-1);
	  }
	}
  }

  VOID cCELL_RECORD_GENERATOR::GenerateVertices()
  {
	for (INT i = 0; i < (INT)(m_facetIndex.size()); i++){
	  INT f = m_facetIndex[i];
	  if (f < 0)
		f = -f-1;
	  INT nv = m_foamReader->FacetNV(f);
	  for (INT j = 0; j < nv; j++){
		INT v = m_foamReader->FacetVertex(f, j);
		if (push_back_unique(&m_vertexIndex, v)){
		  cPOINT3 p = m_foamReader->VertexPoint(v);
		  m_cellBox += p;
		}
	  }
	}
  }

  VOID cCELL_RECORD_GENERATOR::GenerateAdjacentCells()
  {
	for (INT i = 0; i < (INT)(m_facetIndex.size()); i++){
	  INT f = m_facetIndex[i];
	  if (f >= 0){
		if (f < m_foamReader->NeighborCellTableSize()){
		  INT c = m_foamReader->FacetNeighborCell(f);
		  m_adjacentCellIndex.push_back(c);
		}
		else
		  m_adjacentCellIndex.push_back(-1);
	  }
	  else {
		f = -f - 1;
		INT c = m_foamReader->FacetOwnerCell(f);
		m_adjacentCellIndex.push_back(c);
	  }
	}
  }

  VOID cCELL_RECORD_GENERATOR::Generate()
  {
	GenerateFacets();
	GenerateVertices();
	GenerateAdjacentCells();
  }

  INT cCELL_RECORD_GENERATOR::VertexIndexGridToOff(INT vGrid)
  {
	//linear search can be improved, but the vectors are typically small
	for (INT i = 0; i < m_vertexIndex.size(); i++){
	  if (m_vertexIndex[i] == vGrid)
		return i;
	}
	return -1; //should not happen
  }

  VOID cCELL_RECORD_GENERATOR::ExportToOff()
  {
	char offFileName[32];
	char mapFileName[32];
	offFileName[0] = mapFileName[0] = '\0';
	sprintf(offFileName, "%d.grid.off", m_cellIndex);
	sprintf(mapFileName, "%d.grid.map", m_cellIndex);
	FILE *offFile = fopen(offFileName, "wb+");
	FILE *mapFile = fopen(mapFileName, "wb+");
	fprintf(offFile, "OFF\n");
	fprintf(mapFile, "MAP\n");
	fprintf(offFile, "%ld %ld 0\n", m_vertexIndex.size(), m_facetIndex.size());
	fprintf(mapFile, "%ld %ld\n", m_vertexIndex.size(), m_facetIndex.size());
	for (INT i = 0; i < m_vertexIndex.size(); i++){
	  INT v = m_vertexIndex[i];
	  cPOINT3 vtxPoint = m_foamReader->VertexPoint(v);
	  fprintf(offFile, "%lf %lf %lf \n", vtxPoint[0], vtxPoint[1], vtxPoint[2]);
	  fprintf(mapFile, "Vertex:  Off %d:\tGrid %d;\n", i, v);
	}
	fprintf(mapFile, "\n");
	for (INT i = 0; i < m_facetIndex.size(); i++){
	  INT f = m_facetIndex[i];
	  INT c = m_adjacentCellIndex[i];
	  if (f >= 0){
		fprintf(mapFile, "Facet:   Off %d:\tGrid %d;\tCell %d;\n",  i, f, c);
		INT nv = m_foamReader->FacetNV(f);
		fprintf(offFile, "%d ", nv);
		for (INT j = 0; j < nv; j++){
		  INT vGrid = m_foamReader->FacetVertex(f, j);
		  INT vOff = VertexIndexGridToOff(vGrid);
		  fprintf(offFile, "%d ", vOff);
		}
	  }
	  else {
		f = -f - 1;
		fprintf(mapFile, "Facet:   Off %d:\tGrid %d;\tCell %d;\n", i, f, c);
		INT nv = m_foamReader->FacetNV(f);
		fprintf(offFile, "%d ", nv);
		for (INT j = 0; j < nv; j++){
		  INT vGrid = m_foamReader->FacetVertex(f, nv-j-1);
		  INT vOff = VertexIndexGridToOff(vGrid);
		  fprintf(offFile, "%d ", vOff);
		}
	  }
	  fprintf(offFile, "0.000 0.000 1.000 0.75\n");
	}
	fclose(offFile);
	fclose(mapFile);
  }

}//namespace
