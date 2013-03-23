/*
 * cell_off_writer.cpp
 *
 *  Created on: Jan 31, 2011
 */

#include "cell_off_writer.hpp"
#include "cell_record_generator.hpp"
#include <vector>
#include <iostream>
#include <cstdio>

namespace io {

  VOID cCELL_OFF_WRITER::WriteOffFiles()
  {
	INT i = 0;
	INT nCellsInit = (INT)(m_cellIndexTable.size()); //only adjacent cells of the first cells are exported
	while (i < (INT)(m_cellIndexTable.size())){
	  INT c = m_cellIndexTable[i];
	  cCELL_RECORD_GENERATOR cellRecord(m_foamReader, c);
	  cellRecord.Generate();
	  cellRecord.ExportToOff();
	  if ( i < nCellsInit){ //export adjacrent cells only for the first cells
		for (INT j = 0; j < cellRecord.NumAdjacentCells(); j++){
		  INT adjacentCell = cellRecord.AdjacentCell(j);
		  if (adjacentCell != -1 &&
				  push_back_unique(&m_cellIndexTable, adjacentCell)){
			cCELL_RECORD_GENERATOR adjCellRecord(m_foamReader, adjacentCell);
			adjCellRecord.Generate();
			adjCellRecord.ExportToOff();
		  }
		}
	  }
	  cBOX3 cellBox = cellRecord.CellBox();
	  m_cellBoxTable.push_back(cellBox);
	  i++;
	}
  }

  FILE *fileOpen(INT cellIndex, char *extension, char *mode)
  {
	char fileName[128];
	sprintf(fileName, "%d.%s", cellIndex, extension);
	return fopen(fileName, mode);
  }

  VOID cCELL_OFF_WRITER::MergeGridMeshFiles(INT cellIndex)
  {
	FILE *offGridFile = fileOpen(cellIndex, "grid.off", "rb");
	FILE *mapGridFile = fileOpen(cellIndex, "grid.map", "rb");
	FILE *offMeshFile = fileOpen(cellIndex, "mesh.off", "rb");
	FILE *mapMeshFile = fileOpen(cellIndex, "mesh.map", "rb");
	if (offGridFile == NULL || offMeshFile == NULL)
	  return;
	//map files may be empty, in which case map file is not generated

	cOFF_READER gridFileReader(offGridFile, mapGridFile);
	cOFF_READER meshFileReader(offMeshFile, mapMeshFile);
	gridFileReader.Import();
	meshFileReader.Import();
	FILE *offMergedFile = fileOpen(cellIndex, "merged.off", "wb");
	FILE *mapMergedFile = fileOpen(cellIndex, "merged.map", "wb");
	WriteMergedGridMeshFiles(gridFileReader, meshFileReader, offMergedFile, mapMergedFile);
	fclose(offMergedFile);
	fclose(mapMergedFile);
	fclose(offGridFile);
	fclose(offMeshFile);
	fclose(mapGridFile);
	fclose(mapMeshFile);
  }

  VOID cCELL_OFF_WRITER::MergeGridMeshFiles()
  {
	for (INT c = 0; c < (INT)(m_cellIndexTable.size()); c++){
	  INT cellIndex = m_cellIndexTable[c];
	  MergeGridMeshFiles(cellIndex);
	}
  }

  VOID cCELL_OFF_WRITER::WriteMergedGridMeshFiles(cOFF_READER &gridFileReader,
		  cOFF_READER &meshFileReader,
		  FILE *offMergedFile, FILE *mapMergedFile)
  {
	WriteMergedGridMeshOffFile(gridFileReader, meshFileReader, offMergedFile);
	WriteMergedGridMeshMapFile(gridFileReader, meshFileReader, mapMergedFile);

  }

  VOID cCELL_OFF_WRITER::WriteMergedGridMeshOffFile(cOFF_READER &gridFileReader,
					cOFF_READER &meshFileReader, FILE *offMergedFile)
  {
	//grid and mesh vertices are not merged if they coincide.
	fprintf(offMergedFile, "OFF\n");
	fprintf(offMergedFile, "%ld %ld 0\n",
			gridFileReader.NumVertices() + meshFileReader.NumVertices(),
			gridFileReader.NumFacets() + meshFileReader.NumFacets());
	for (INT v=0; v < gridFileReader.NumVertices(); v++){
	  gridFileReader.PrintVertexPoint(offMergedFile, v);
	}
	for (INT v=0; v < meshFileReader.NumVertices(); v++){
	  meshFileReader.PrintVertexPoint(offMergedFile, v);
	}
	for (INT f=0; f < gridFileReader.NumFacets(); f++){
	  gridFileReader.PrintFacet(offMergedFile, f);
	}
	for (INT f=0; f < meshFileReader.NumFacets(); f++){
	  meshFileReader.PrintFacet(offMergedFile, f, gridFileReader.NumVertices());
	}
  }

  VOID cCELL_OFF_WRITER::WriteMergedGridMeshMapFile(cOFF_READER &gridFileReader,
					cOFF_READER &meshFileReader, FILE *mapMergedFile)
  {
	fprintf(mapMergedFile, "MAP\n");
	fprintf(mapMergedFile, "%d %d\n",
			gridFileReader.NumVertices() + meshFileReader.NumVertices(),
			gridFileReader.NumFacets() + meshFileReader.NumFacets());
	for (INT v=0; v < gridFileReader.NumVertices(); v++){
	  gridFileReader.PrintVertexMap(mapMergedFile, v);
	}
	for (INT v=0; v < meshFileReader.NumVertices(); v++){
	  meshFileReader.PrintVertexMap(mapMergedFile, v, gridFileReader.NumVertices());
	}
	fprintf(mapMergedFile, "\n");
	for (INT f=0; f < gridFileReader.NumFacets(); f++){
	  gridFileReader.PrintFacetMap(mapMergedFile, f);
	}
	for (INT f=0; f < meshFileReader.NumFacets(); f++){
	  meshFileReader.PrintFacetMap(mapMergedFile, f, gridFileReader.NumFacets());
	}


  }

}//namespace
