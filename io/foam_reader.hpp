/*
 * foam_reader.hpp
 *
 *  Created on: Jan 31, 2011
 */

#ifndef FOAM_READER_HPP_
#define FOAM_READER_HPP_


#include "defs.hpp"

namespace io {

class cFOAM_READER {
public:
	cFOAM_READER(){};
	cFOAM_READER(char *filePath){m_filePath = filePath;}
	BOOL Import();
	INT OwnerCellTableSize() { return (INT)(m_facetCell0Table.size());}
	INT NeighborCellTableSize() { return (INT)(m_facetCell1Table.size());}
	INT FacetOwnerCell(INT f) {return m_facetCell0Table[f];}
	INT FacetNeighborCell(INT f) {return m_facetCell1Table[f];}
	INT FacetNV(INT f) { return m_facetNVTable[f];}
	INT FacetVertex(INT f, INT j) {return m_facetVerticesTable[m_facetV0Table[f]+j];}
	cPOINT3 VertexPoint(INT v) {return m_vertexPointTable[v];}
	BOOL IsEmpty() { return m_facetNVTable.size() == 0 && m_vertexPointTable.size() == 0;}
	VOID ReadVertexTable(FILE *vertex_test_file);
	VOID ReadFacetTable(FILE *facettest_file);
	BOOL ExportFacetsContainingVertices();
	BOOL ExportCellsContainingFacets();

private:
	BOOL ImportPoints();
	BOOL ReadFacetCells(std::vector<INT>  *cellTable, char *file_name);
	BOOL ImportFacetCells();
	BOOL ImportFacets();
	BOOL FacetContainsVertex(INT f);
	BOOL VertexIndexInTable(INT v);

private:
	char *m_filePath;
	std::vector<INT> m_facetNVTable; //number of facet vertices
	std::vector<INT> m_facetVerticesTable;//sequential table of facet vertices
	std::vector<INT> m_facetV0Table; //table of facet init vertex index in the previous table
	std::vector<INT> m_facetCell0Table; //owner
	std::vector<INT> m_facetCell1Table; //neighbour or -1
	std::vector<cPOINT3> m_vertexPointTable; //vertex coordinates
	std::vector<INT> m_vertexIndexTable; //for reading vertex table
	std::vector<INT> m_facetIndexTable; //for reading facet table
};

} //namespace

#endif /* FOAM_READER_HPP_ */
