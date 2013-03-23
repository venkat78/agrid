/*
 * cell_record_generator.hpp
 *
 *  Created on: Jan 31, 2011
 */

#ifndef CELL_RECORD_GENERATOR_HPP_
#define CELL_RECORD_GENERATOR_HPP_

#include "defs.hpp"
#include "foam_reader.hpp"

namespace io {

BOOL push_back_unique(std::vector<INT>  *intVector, INT a);
  class cCELL_RECORD_GENERATOR {
  public:
	cCELL_RECORD_GENERATOR();
	cCELL_RECORD_GENERATOR(cFOAM_READER *foamReader, INT cellIndex);
  public:
	VOID Generate();
	cBOX3 CellBox() {return m_cellBox;}
	VOID ExportToOff();
	INT NumAdjacentCells() {return (INT)(m_adjacentCellIndex.size());}
	INT AdjacentCell(INT i) {return m_adjacentCellIndex[i];}

  private:
	VOID GenerateFacets();
	VOID GenerateVertices();
	VOID GenerateAdjacentCells();
	INT VertexIndexGridToOff(INT vGrid);

  private:
	// all the indices below are based on the OF import files
	cFOAM_READER *m_foamReader;
	INT m_cellIndex; //cell being processed
	std::vector<INT> m_facetIndex; //facet, vertex, and adjacent cell indices
	std::vector<INT> m_vertexIndex;	//refer to the current cell during its processing
	std::vector<INT> m_adjacentCellIndex;
	cBOX3 m_cellBox;
  };
}//namespace


#endif /* CELL_RECORD_GENERATOR_HPP_ */
