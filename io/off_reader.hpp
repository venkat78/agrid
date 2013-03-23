/*
 * off_reader.hpp
 *
 *  Created on: Feb 1, 2011
 */

#ifndef OFF_READER_HPP_
#define OFF_READER_HPP_

#include "defs.hpp"
#include <cstdio>

namespace io {

  struct sOFF_RGBA
  {
	float rgba_elt[4];
  };

  class cOFF_READER {
  public:
	cOFF_READER();
	cOFF_READER(FILE *offFile, FILE *mapFile);
	VOID Import();
	INT NumFacets() {return (INT)(m_facetNVTable.size());}
	INT NumVertices() {return (INT)(m_vertexPointTable.size());}
	cPOINT3 VertexPoint(INT v) {return m_vertexPointTable[v];}
	INT VertexMap(INT v) {return m_vertexMapTable[v];}
	VOID PrintVertexPoint(FILE *file, INT v);
	VOID PrintVertexMap(FILE *file, INT v, INT offset = 0);
	VOID PrintFacet(FILE *file, INT f, INT offset = 0);
	VOID PrintFacetRGBA(FILE *file, INT f);
	VOID PrintFacetMap(FILE *file, INT f, INT offset = 0);
	INT FindIndexInVertexMap(INT gridVertexIndex);
	INT FacetNumVertices(INT f) {return m_facetNVTable[f];}
	INT FacetVertex(INT f, INT j); //returns j-th vertex of facet f

  private:
	VOID ImportOff();
	VOID ImportMap();

  private: //members
	FILE *m_offFile;
	FILE *m_mapFile;
	std::vector<INT> m_facetNVTable; //number of facet vertices
	std::vector<INT> m_facetVerticesTable;//sequential table of facet vertices
	std::vector<INT> m_facetV0Table; //table of facet init vertex index in the previous table
	std::vector<sOFF_RGBA> m_facetRgbaTable;
	std::vector<cPOINT3> m_vertexPointTable; //vertex coordinates
	std::vector<INT> m_facetMapTable;
	std::vector<INT> m_facetCellTable;
	std::vector<INT> m_vertexMapTable;
};
}//namespace
#endif /* OFF_READER_HPP_ */
