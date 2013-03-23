/*
 * cell_off_writer.hpp
 *
 *  Created on: Jan 31, 2011
 */

#ifndef CELL_OFF_WRITER_HPP_
#define CELL_OFF_WRITER_HPP_

#include "defs.hpp"
#include "foam_reader.hpp"
#include "off_reader.hpp"
#include "cell_record_generator.hpp"

namespace io {

  class cCELL_OFF_WRITER {
  public:
	cCELL_OFF_WRITER(cFOAM_READER *foamReader) {m_foamReader = foamReader;}

  public:
	VOID AddCellIndex(INT cellIndex) {m_cellIndexTable.push_back(cellIndex);}//add 1ness check
	VOID WriteOffFiles();
	BOOL IsEmpty() { return m_foamReader->IsEmpty();}
	VOID MergeGridMeshFiles();

	template <typename _Mesh_Type>
	VOID ExportMeshInsideCellToOff(_Mesh_Type &mesh)
	{
	  for (INT c = 0; c < (INT)(m_cellIndexTable.size()); c++){
		cBOX3 cellBox = m_cellBoxTable[c];
		std::vector<INT> facetIndexTable;
		std::vector<INT>vertexIndexTable;

		typename _Mesh_Type::facet_iterator currFacet = mesh.FacetsBegin();
	    typename _Mesh_Type::facet_iterator lastFacet = mesh.FacetsEnd();

	    for( ; currFacet != lastFacet ; currFacet++) {
	      typename _Mesh_Type::cFACET::vertex_circulator facetVtx = currFacet->VerticesBegin();
	      typename _Mesh_Type::cFACET::vertex_circulator lastVtx = currFacet->VerticesEnd();
	      BOOL facetInsideBox =- true;
	      for( ; facetVtx != lastVtx && facetInsideBox; facetVtx++) {
	    	if (!cellBox.ContainsPoint(facetVtx->Point()))
	    	  facetInsideBox = false;
	      }
	      if (facetInsideBox){
	    	facetIndexTable.push_back(currFacet->Index());

	    	for(facetVtx = currFacet->VerticesBegin(); facetVtx != lastVtx; facetVtx++) {
	    		push_back_unique(&vertexIndexTable, facetVtx->Index());
	    	}
	      }
	    }//for currFacet

	    if (facetIndexTable.size() > 0){
	      char offName[128];
	      char mapName[128];
	      sprintf(offName, "%d.mesh.off", m_cellIndexTable[c]);
	      sprintf(mapName, "%d.mesh.map", m_cellIndexTable[c]);
	      FILE *offFile = fopen(offName, "wb+");
	      FILE *mapFile = fopen(mapName, "wb+");

	      fprintf(offFile, "OFF\n");
	      fprintf(offFile, "%ld %ld 0\n", vertexIndexTable.size(), facetIndexTable.size());
	      fprintf(mapFile, "MAP\n");
	      fprintf(mapFile, "%ld %ld\n", vertexIndexTable.size(), facetIndexTable.size());
	      for (INT i = 0; i < vertexIndexTable.size(); i++){
	    	INT v = vertexIndexTable[i];
	    	cPOINT3 vtxPoint = mesh.Vertex(v)->Point();
	    	fprintf(offFile, "%lf %lf %lf \n", vtxPoint[0], vtxPoint[1], vtxPoint[2]);
	    	fprintf(mapFile, "Vertex:  Off %d:\tMesh %d;\n", i, v);
	      }
	      fprintf(mapFile, "\n");
	      for (INT i = 0; i < facetIndexTable.size(); i++){
	    	INT f = facetIndexTable[i];
	  		fprintf(mapFile, "Facet:   Off %d:\tMesh %d;\n",  i, f);
	  		INT nv = mesh.Facet(f)->NumVertices();
	  		fprintf(offFile, "%d ", nv);
	  		typename _Mesh_Type::cFACET::vertex_circulator facetVtx = mesh.Facet(f)->VerticesBegin();
	  		typename _Mesh_Type::cFACET::vertex_circulator lastVtx = mesh.Facet(f)->VerticesEnd();
	    	for(; facetVtx != lastVtx; facetVtx++) {
	    	  INT vMesh =facetVtx->Index();
	    	  INT vOff = -1;
	    	  //linear search can be improved, but the vectors are typically small
	    	  for (INT j = 0; j < vertexIndexTable.size() && vOff < 0; j++){
	    		if (vertexIndexTable[j] == vMesh)
	    			vOff = j;
	    	  }
	  		  fprintf(offFile, "%d ", vOff);
	    	}//facetVtx
	    	fprintf(offFile, "1.000 0.000 0.000 1.00\n");
	  	  }//for i
	  	  fclose(offFile);
	  	  fclose(mapFile);
	    }//if tablesize
	  }//for c
	}

  private:
	VOID MergeGridMeshFiles(INT cellIndex);

	VOID WriteMergedGridMeshFiles(cOFF_READER &gridFileReader,
			cOFF_READER &meshFileReader,
			FILE *offMergedFile, FILE *mapMergedFile);
	VOID WriteMergedGridMeshOffFile(cOFF_READER &gridFileReader,
					cOFF_READER &meshFileReader, FILE *offMergedFile);
	VOID WriteMergedGridMeshMapFile(cOFF_READER &gridFileReader,
					cOFF_READER &meshFileReader, FILE *mapMergedFile);


  private:
	cFOAM_READER *m_foamReader;
	std::vector<INT> m_cellIndexTable;
	std::vector<cBOX3> m_cellBoxTable;
  };
}//namespace
#endif /* CELL_OFF_WRITER_HPP_ */
