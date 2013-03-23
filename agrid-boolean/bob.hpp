/*
 * bob.hpp
 *
 *  Created on: Feb 14, 2011
 */

#ifndef BOB_HPP_
#define BOB_HPP_

//Box Outside of Box
#include "defs.hpp"

namespace agrid_boolean {

class cBORDER_FACET_RECORD{
public:
	cBORDER_FACET_RECORD();
	cBORDER_FACET_RECORD(iCELL cellIndex, INT vertexIndex[4]);

	INT VertexIndex(int v) {return m_vertexIndex[v];}
	INT CellIndex() {return m_cellIndex;}

private:
	iCELL m_cellIndex;
	INT m_vertexIndex[4]; //export indices
};

typedef  std::vector<cBORDER_FACET_RECORD> BORDER_FACETS_TABLE;
//typedef  BORDER_FACETS_TABLE  BORDER_FACETS_TABLES[6];

class cWHITE_FACET_RECORD{
public:
	cWHITE_FACET_RECORD() {};
	VOID Init();
	VOID ExportVertexIndex(INT v, INT index) {m_vertexIndex[v] = index;}
	INT VertexIndex(INT v) const {return m_vertexIndex[v];}
	VOID AdjacentCellIndex(INT index) {m_cfPair.cellIndex = index;}
	iCELL AdjacentCellIndex() const {return m_cfPair.cellIndex;}
	iFACET FacetIndex() const {return m_cfPair.faceIndex;}
	VOID FacetIndex(INT index) {m_cfPair.faceIndex = index;}
	VOID AdjacentFacetCell(INT cellIndex, INT faceIndex = -1)
		{m_cfPair.faceIndex = faceIndex; m_cfPair.cellIndex = cellIndex;}
	INT *Vertices() {return m_vertexIndex;}
	VOID CellFace(eBOX_FACE cellFace)  {m_cellFace = cellFace;}
	VOID CellFace(INT cellFace)  {m_cellFace = (eBOX_FACE)cellFace;}
	eBOX_FACE CellFace() {return m_cellFace;}

private:
	sFACE_CELL_PAIR m_cfPair;
	INT m_vertexIndex[4]; //export indices
	eBOX_FACE m_cellFace;
};

class cWHITE_CELL {
public:
	cWHITE_CELL();
	cWHITE_CELL(INT cellIndex);

	iCELL CellIndex() const {return m_cellIndex;}
	VOID CellIndex(INT cellIndex) {m_cellIndex = cellIndex;}
	cWHITE_FACET_RECORD &WhiteFacetRecord(INT f) {return m_facetRecord[f];}
	iCELL AdjacentCellIndex(INT f) const {return m_facetRecord[f].AdjacentCellIndex();}
	iFACET FacetIndex(INT f) const {return m_facetRecord[f].FacetIndex();}
	VOID AdjacentCellIndex(INT f, INT index)
			{m_facetRecord[f].AdjacentCellIndex(index);}
	VOID AdjacentFacetCell(INT f, INT cellIndex, INT faceIndex = -1)
			{m_facetRecord[f].AdjacentFacetCell(cellIndex, faceIndex);}
	INT VertexIndex(INT f, INT v) const {return m_facetRecord[f].VertexIndex(v);}
	INT *FacetVertices(INT f) {return m_facetRecord[f].Vertices();}
	VOID SortFacetRecords();
	BOOL CompareFacetRecords(INT i1, INT i2) const;
	VOID SwapFacetRecords(INT i1, INT i2);

private:
	VOID InitIndices();
	INT m_cellIndex;
	cWHITE_FACET_RECORD m_facetRecord[6]; //x0, x1, y0, y1, z0, z1
};

class cBOB {
public:
	cBOB();
	cBOB(cBOX3i &externalBox, cBOX3i &internalBox, BOOL tightSimVol = false);
	VOID InitCellIndex(int initCellIndex) { m_initCellIndex = initCellIndex;}
	INT InitCellIndex() { return m_initCellIndex;}
	VOID InitVertexIndex(int initVertexIndex) { m_initVertexIndex = initVertexIndex;}
	VOID InitFacetIndex(int initFacetIndex) { m_initFacetIndex = initFacetIndex;}
	VOID RegisterSimVol(const cBOX3 &simVol);
	cBOX3 &SimVol() {return m_simVol;}
	cBOX3 &GrayGridVol() {return m_grayGridVol;}
	cBOX3i &InternalBox() {return m_internalBox;}
	cBOX3i &ExternalBox() {return m_externalBox;}
	REAL cellSize(eCOORD coord);
	VOID NumGrayGridCells(INT numCells[3]);
	INT NumVertices() const
				{return m_externalBox.NumVertices() - m_internalBox.NumVertices();}
	INT NumCells() const
				{return m_externalBox.NumCells() - m_internalBox.NumCells();}
	INT NumFacets() const
				{return m_externalBox.NumFacets() - m_internalBox.NumFacets();}
	INT NumExternalBoundaryFacets() const
				{return m_externalBox.SurfaceArea();}
	cBOX3i::cVertexPos PointVertexPos(const cPOINT3 &vertexPoint) const;
	BOOL PointVertexPos(const cPOINT3 &vertexPoint, cBOX3i::cVertexPos &vertexPos) const;
    BOOL RegisterVertexIfOnGrid(const cPOINT3 &vertexpoint, INT vertexIndex);
    cPOINT3 VertexPoint(INT index);
    BOOL RegisterExternalVertex(INT gridIndex, INT vertexIndex);
    INT VertexMapSize();
    INT VertexPointCellIndex(cPOINT3 &vertexPoint, eCOORD coord) const;
    INT CellIndex(cBOX3i::cCellPos &cellPos) const;
    cWHITE_CELL ComputeWhiteCell(INT count) const;
    VOID RegisterInterfaceFacet(cPOINT3 &minCoordVertexPoint,
    		eCOORD facetCoord, INT facetIndex,  iCELL cellIndex);
    VOID PrintGridVertexMap(FILE *stream = stdout);

private:
	VOID ComputeWhiteBoxes();
	BOOL CellPosInWhiteBox(INT cellIndex, cBOX3i::cCellPos &cellPos) const;
	VOID InitFacetVertexTable();
	VOID ComputeFacetRecord(cWHITE_CELL &whiteCell, INT f,
			INT *exportVertexIndex) const;
	INT InterfaceFacetTableIndex(cPOINT3 &minCoordVertexPoint, eCOORD coord);

	enum eVERTEX_TYPE {INTERIOR_VERTEX = -2, EXTERIOR_VERTEX = -1};

private:
  cBOX3i m_externalBox;
  cBOX3i m_externalBoxInit; //different if tightSimVol = true
  cBOX3i m_internalBox;
  cBOX3i m_whiteBox[3][2]; //boxes covering the white cells
  cBOX3 m_simVol;
  cBOX3 m_grayGridVol; //computations are performed inside of the grayGridVol only
  INT m_initCellIndex;
  INT m_initVertexIndex;
  INT m_initFacetIndex;
  std::vector<sFACE_CELL_PAIR> m_interfaceFacetCellTable; //table of border facets of the internal box
  //it has six sub-tables, and the edge and corner cells are repeated
  std::vector<INT> m_gridVertexMap;
  INT m_facetVertexTable[6][4]; //const correspondence table between facets and their vertices'
  BOOL m_tightSimVol; // default false. If changed to true, the tight simvol is built around the mesh,
  //and only grid in the tight simvol is returned. All its cut cells should b identical to those
  //generated by using the given simvol.
};


}//namespace
#endif /* BOB_HPP_ */
