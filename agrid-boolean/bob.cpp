/*
 * bob.cpp
 *
 *  Created on: Feb 14, 2011
 */

//Box Outside of Box

#include "bob.hpp"

namespace agrid_boolean {

  cBOB::cBOB()
  {
	m_initCellIndex = 0;
	m_initVertexIndex = 0;
	m_initFacetIndex = 0;
	m_tightSimVol = false;
	InitFacetVertexTable();
  }

  cBOB::cBOB(cBOX3i &externalBox, cBOX3i &internalBox, BOOL tightSimVol)
  {
	m_tightSimVol = tightSimVol;
	m_internalBox = internalBox;
	m_externalBoxInit = externalBox;
	m_externalBox = externalBox;
	if (m_tightSimVol)
		m_externalBox = internalBox;
	ComputeWhiteBoxes();
	m_initCellIndex = 0;
	m_initVertexIndex = 0;
	m_initFacetIndex = 0;
	InitFacetVertexTable();
	m_gridVertexMap.reserve(m_externalBox.NumVertices());
	m_gridVertexMap.assign(m_externalBox.NumVertices(), EXTERIOR_VERTEX);
    for (int i0 = m_internalBox.XMin() + 1; i0 < m_internalBox.XMax(); i0++){
      for (int i1 = m_internalBox.YMin() + 1; i1 < m_internalBox.YMax(); i1++){
    	for (int i2 = m_internalBox.ZMin() + 1; i2 < m_internalBox.ZMax(); i2++){
    	  cBOX3i::cVertexPos vertexPos(i0, i1, i2);
    	  int mapIndex = m_externalBox.VertexIndex(vertexPos);
    	  m_gridVertexMap[mapIndex] = INTERIOR_VERTEX;
    	}
      }
    }
    INT numBorderFacets = m_internalBox.SurfaceArea();
    sFACE_CELL_PAIR cfPairInit = {-1, -1};
    m_interfaceFacetCellTable.reserve(numBorderFacets);
    m_interfaceFacetCellTable.assign(numBorderFacets, cfPairInit);
  }

  VOID cBOB::InitFacetVertexTable()
  {
	m_facetVertexTable[0][0] = 0;
	m_facetVertexTable[0][1] = 1;
	m_facetVertexTable[0][2] = 3;
	m_facetVertexTable[0][3] = 2;

	m_facetVertexTable[2][0] = 0;
	m_facetVertexTable[2][1] = 4;
	m_facetVertexTable[2][2] = 5;
	m_facetVertexTable[2][3] = 1;

	m_facetVertexTable[4][0] = 0;
	m_facetVertexTable[4][1] = 2;
	m_facetVertexTable[4][2] = 6;
	m_facetVertexTable[4][3] = 4;

	m_facetVertexTable[1][0] = 4;
	m_facetVertexTable[1][1] = 6;
	m_facetVertexTable[1][2] = 7;
	m_facetVertexTable[1][3] = 5;

	m_facetVertexTable[3][0] = 2;
	m_facetVertexTable[3][1] = 3;
	m_facetVertexTable[3][2] = 7;
	m_facetVertexTable[3][3] = 6;

	m_facetVertexTable[5][0] = 1;
	m_facetVertexTable[5][1] = 5;
	m_facetVertexTable[5][2] = 7;
	m_facetVertexTable[5][3] = 3;
  }
  VOID cBOB::ComputeWhiteBoxes()
  {
	m_whiteBox[0][0] = m_whiteBox[0][1] = m_externalBox;

	m_whiteBox[0][0].XMax(m_internalBox.XMin());
	m_whiteBox[0][1].XMin(m_internalBox.XMax());

	m_whiteBox[1][0] = m_whiteBox[1][1] = m_externalBox;

	m_whiteBox[1][0].XMin(m_internalBox.XMin());
	m_whiteBox[1][0].XMax(m_internalBox.XMax());
	m_whiteBox[1][1].XMin(m_internalBox.XMin());
	m_whiteBox[1][1].XMax(m_internalBox.XMax());

	m_whiteBox[1][0].YMax(m_internalBox.YMin());
	m_whiteBox[1][1].YMin(m_internalBox.YMax());

	m_whiteBox[2][0] = m_whiteBox[2][1] = m_internalBox;

	m_whiteBox[2][0].ZMin(m_externalBox.ZMin());
	m_whiteBox[2][0].ZMax(m_internalBox.ZMin());
	m_whiteBox[2][1].ZMin(m_internalBox.ZMax());
	m_whiteBox[2][1].ZMax(m_externalBox.ZMax());
/*
	for (int i=0; i < 3; i++){
	  for (int j=0; j < 2; j++){
		m_whiteBox[i][j].Print();
	  }
	}
	*/
	//for debugging
	INT volExternal = m_externalBox.Volume();
	INT volPieceMeal= m_internalBox.Volume();
	for (int i=0; i < 3; i++){
	  for (int j=0; j < 2; j++){
		volPieceMeal += m_whiteBox[i][j].Volume();
	  }
	}
	ASSERT(volExternal == volPieceMeal);
  }

  //it also computes the grayGridVol wich is a minimal SImVol-like box
  VOID cBOB::RegisterSimVol(const cBOX3 &simVol)
  {
	cPOINT3 p, q;
	DO_COORDS(coord){
	  REAL u0 = (REAL)(m_internalBox.MinCoord(coord) - m_externalBoxInit.MinCoord(coord));
	  REAL w = (REAL)(m_externalBoxInit.MaxCoord(coord) - m_externalBoxInit.MinCoord(coord));
	  REAL u = u0/w;
	  p[coord] = (1.0-u)*simVol.MinCoord(coord) + u*simVol.MaxCoord(coord);
	  REAL v0 = (REAL)(m_internalBox.MaxCoord(coord) - m_externalBoxInit.MinCoord(coord));
	  REAL v = v0/w;
	  q[coord] = (1.0-v)*simVol.MinCoord(coord) + v*simVol.MaxCoord(coord);
	}
	m_grayGridVol += p;
	m_grayGridVol += q;
//	m_grayGridVol.Print();
	m_simVol = simVol;
	if (m_tightSimVol)
	  m_simVol = m_grayGridVol;
  }

  VOID cBOB::NumGrayGridCells(INT numCells[3])
  {
	DO_COORDS(coord){
	  numCells[coord] = m_internalBox.Thickness(coord);
	}
  }

  REAL cBOB::cellSize(eCOORD coord)
  {
	INT numGrayCells = m_internalBox.Thickness(coord);
	return m_grayGridVol.Thickness(coord)/((REAL)numGrayCells);
  }

  //point is supposed to be a grid vertex
  cBOX3i::cVertexPos cBOB::PointVertexPos(const cPOINT3 &vertexPoint) const
  {
	cBOX3i::cVertexPos vertexPos;
	DO_COORDS(coord){
	  REAL u = (vertexPoint[coord] - m_simVol.MinCoord(coord))/m_simVol.Thickness(coord);
	  REAL p = (1.0 - u)*m_externalBox.MinCoord(coord) + u * m_externalBox.MaxCoord(coord);
	  INT p_int = (INT)(p + cLIMITS::Tolerance());
	  ASSERT(fabs(p-(REAL)p_int) <= cLIMITS::Tolerance());
	  vertexPos[coord] = p_int;
	}
	return vertexPos;
  }

  //returns true if the point is on the grid
  BOOL cBOB::PointVertexPos(const cPOINT3 &vertexPoint, cBOX3i::cVertexPos &vertexPos) const
  {
	DO_COORDS(coord){
	  REAL u = (vertexPoint[coord] - m_simVol.MinCoord(coord))/m_simVol.Thickness(coord);
	  REAL p = (1.0 - u)*m_externalBox.MinCoord(coord) + u * m_externalBox.MaxCoord(coord);
	  INT p_int = (INT)(p + cLIMITS::Tolerance());
	  if (fabs(p-(REAL)p_int) > cLIMITS::Tolerance())
		return false;
	  vertexPos[coord] = p_int;
	}
	return true;
  }

  BOOL cBOB::RegisterVertexIfOnGrid(const cPOINT3 &vertexPoint, INT vertexIndex)
  {
	cBOX3i::cVertexPos vertexPos;
	if (!PointVertexPos(vertexPoint, vertexPos))
	  return false;
	int mapIndex = m_externalBox.VertexIndex(vertexPos);
	if (m_gridVertexMap[mapIndex] == EXTERIOR_VERTEX)
		m_gridVertexMap[mapIndex] = vertexIndex;
	return true;
  }

  VOID cBOB::PrintGridVertexMap(FILE *stream)
  {
	return;
	for (INT i = 0; i < (INT)m_gridVertexMap.size(); i++){
	  cBOX3i::cVertexPos vertexPos = m_externalBox.VertexPosition(i);
	  fprintf(stream, "i %d:\tPos %d %d %d\t, vertexMap %d\n",
			  i, vertexPos[0], vertexPos[1], vertexPos[2], m_gridVertexMap[i]);
	}
  }

  INT cBOB::VertexMapSize()
  {
	return (INT)m_gridVertexMap.size();
  }

  cPOINT3 cBOB::VertexPoint(INT index)
  {
	cPOINT3 point;
	cBOX3i::cVertexPos vertexPos;
	m_externalBox.VertexPosition(index, vertexPos);
	DO_COORDS(coord){
	  REAL w = (REAL)(m_externalBox.MaxCoord(coord) - m_externalBox.MinCoord(coord));
	  REAL u = (REAL)(vertexPos[coord] - m_externalBox.MinCoord(coord))/w;
	  point[coord] = (1.0-u)*m_simVol.MinCoord(coord) + u*m_simVol.MaxCoord(coord);
	}
	return point;
  }

  //the vertex point lies on the internal box, and its two other coordinates represent
  //vertex with the lowest coordinate values. That's why if it is located on the min side
  //of the internal box, the coord value is decreased by one to match the cell position
  INT cBOB::VertexPointCellIndex(cPOINT3 &vertexPoint, eCOORD coord) const
  {
	cBOX3i::cVertexPos vertexPos = PointVertexPos(vertexPoint);
	if (vertexPos[coord] == m_internalBox.MinCoord(coord))
	  (vertexPos[coord])--;
	cBOX3i::cCellPos cellPos = 	vertexPos;
	INT cellIndex = CellIndex(cellPos);
	if (cellIndex >= 0){
	  cBOX3i::cCellPos debugPos;
	  ASSERT(CellPosInWhiteBox(cellIndex,debugPos) && debugPos == cellPos);
	}
	return cellIndex;
  }

  INT cBOB::InterfaceFacetTableIndex(cPOINT3 &minCoordVertexPoint, eCOORD coord)
  {
	cBOX3i::cVertexPos vertexPos = PointVertexPos(minCoordVertexPoint);
	INT index = m_internalBox.BorderCellIndex(vertexPos, coord);
	return index;
  }

  VOID cBOB::RegisterInterfaceFacet(cPOINT3 &minCoordVertexPoint,
  		eCOORD facetCoord, INT facetIndex, iCELL cellIndex)
  {
	INT interfaceFacetTableIndex = InterfaceFacetTableIndex(minCoordVertexPoint, facetCoord);
	m_interfaceFacetCellTable[interfaceFacetTableIndex].faceIndex = facetIndex;
	m_interfaceFacetCellTable[interfaceFacetTableIndex].cellIndex = cellIndex;
  }

  INT cBOB::CellIndex(cBOX3i::cCellPos &cellPos) const
  {
	INT whiteBoxInitCelIndex = m_initCellIndex;
	DO_COORDS(coord){
	  for(INT i=0; i < 2; i++){
		if (m_whiteBox[coord][i].ContainsCell(cellPos))
		  return whiteBoxInitCelIndex + m_whiteBox[coord][i].CellIndex(cellPos);
		whiteBoxInitCelIndex += m_whiteBox[coord][i].NumCells();
	  }
	}
//also check if the cell is on the internal box boundary
//	if (m_internalBox.CellIsOnBorder(cellPos)){
//	}
	return -1;
  }

  BOOL cBOB::RegisterExternalVertex(INT gridIndex, INT vertexIndex)
  {
	if (m_gridVertexMap[gridIndex] == EXTERIOR_VERTEX){
		m_gridVertexMap[gridIndex] = vertexIndex;
		return true;
	}
	else
	  return false;
  }


  //computes cell position in the external box (same as the cell position in the white box
  //index is not offset by the index init
  BOOL cBOB::CellPosInWhiteBox(INT cellIndex, cBOX3i::cCellPos &cellPos) const
  {
	INT index = cellIndex - m_initCellIndex;
	for (INT c = 0; c < 3; c++){
	  for (INT k = 0; k < 2; k++){
		INT WhiteBoxSize = m_whiteBox[c][k].NumCells();
		if (index < WhiteBoxSize){
		  m_whiteBox[c][k].CellPosition(index, cellPos);
		  return true;
		}
		index -= WhiteBoxSize;
	  }
	}
	ASSERT(false);
	return false;
  }

  cWHITE_CELL cBOB::ComputeWhiteCell(INT count) const
  {
	cWHITE_CELL whiteCell;
	cBOX3i::cCellPos cellPos;
	INT cellIndex = m_initCellIndex + count;
	whiteCell.CellIndex(cellIndex);
	if (!CellPosInWhiteBox(cellIndex, cellPos))
	  ASSERT(false);
	//compute vertex positions and grid and export indices
//	printf("count %d; cellPos %d %d %d\n", count, cellPos[0], cellPos[1], cellPos[2]);
	INT exportVertexIndex[8];

	INT v=0;
	for (INT x=0; x<2; x++){
	  for (INT y=0; y<2; y++){
		for (INT z=0; z<2; z++){
		  cBOX3i::cVertexPos vertexPos;
		  vertexPos[0] = cellPos[0] + x;
		  vertexPos[1] = cellPos[1] + y;
		  vertexPos[2] = cellPos[2] + z;
		  INT gridVertexIndex = m_externalBox.VertexIndex(vertexPos);
		  exportVertexIndex[v] = m_gridVertexMap[gridVertexIndex];
		  v++;
		}
	  }
	}

	//fill the facet records vertices
	for (INT f=0; f<6; f++)
	  ComputeFacetRecord(whiteCell, f, exportVertexIndex);

	//compute adjacent cells
	INT f = 0;
	DO_COORDS(c){
	  for (INT j=0; j < 2; j++){
		cBOX3i::cCellPos adjacentCellPos = cellPos;
		adjacentCellPos[c] += (2*j - 1); // - or + 1

		INT adjacentCellIndex = CellIndex(adjacentCellPos);
//		printf("f %d; adjPos %d %d %d; adjIndex %d\n",
//				f, adjacentCellPos[0], adjacentCellPos[1], adjacentCellPos[2], adjacentCellIndex);
		if (adjacentCellIndex >= 0){
		  cBOX3i::cCellPos debugCellPos;
		  CellPosInWhiteBox(adjacentCellIndex, debugCellPos);
		  ASSERT(adjacentCellPos == debugCellPos);
		  whiteCell.AdjacentCellIndex(f, adjacentCellIndex);
		}
		else if (m_internalBox.ContainsCell(adjacentCellPos)){
		  if (adjacentCellPos[c] == m_internalBox.MaxCoord(c) - 1)
			  adjacentCellPos[c] = m_internalBox.MaxCoord(c);
		  INT faceIndex = m_internalBox.BorderCellIndex(adjacentCellPos, c);
//		  printf("ComputeWhiteCell: cellPos = (%d %d %d); coord = %d; faceIndex = %d\n",
//				  adjacentCellPos[0], adjacentCellPos[1], adjacentCellPos[2], c, faceIndex);
		  ASSERT(faceIndex >= 0);
		  whiteCell.AdjacentFacetCell(f, faceIndex, adjacentCellIndex);
		}
		f++;
	  }
	}
	whiteCell.SortFacetRecords();
	return whiteCell;
  }

  VOID cBOB::ComputeFacetRecord(cWHITE_CELL &whiteCell, INT f,
					INT *exportVertexIndex) const
  {
	cWHITE_FACET_RECORD &facetRecord = whiteCell.WhiteFacetRecord(f);
	facetRecord.CellFace(f);
	for (INT i=0; i < 4; i++)
	  facetRecord.ExportVertexIndex(i, exportVertexIndex[m_facetVertexTable[f][i]]);
  }

  VOID cWHITE_FACET_RECORD::Init()
  {
	  m_cfPair.faceIndex = -1;
	  m_cfPair.cellIndex = -1;
	  m_cellFace = BOX_FACE_NONE;
	  for (INT k=0; k < 4; k++)
		m_vertexIndex[k] = -1;
  }

  cWHITE_CELL::cWHITE_CELL()
  {
	m_cellIndex = -1;
	InitIndices();
  }

  cWHITE_CELL::cWHITE_CELL(INT cellIndex)
  {
	m_cellIndex = cellIndex;
	InitIndices();
  }

  VOID cWHITE_CELL::InitIndices()
  {
	for (INT i=0; i < 6; i++){
	  m_facetRecord[i].Init();
	}
  }

  VOID cWHITE_CELL::SortFacetRecords()
  {
	for (INT i1=0; i1 < 6; i1++){
	  for (INT i2=i1+1; i2 < 6; i2++){
		if (!CompareFacetRecords(i1, i2))
		  SwapFacetRecords(i1, i2);
	  }
	}
  }

  BOOL cWHITE_CELL::CompareFacetRecords(INT i1, INT i2) const
  {
	INT c1 = m_facetRecord[i1].AdjacentCellIndex();
	INT c2 = m_facetRecord[i2].AdjacentCellIndex();
	if (c1 == -1)
	  return (c2 == -1);
	if (c2 == -1 || c1 < c2)
	  return true;
	return false;
  }

  VOID cWHITE_CELL::SwapFacetRecords(INT i1, INT i2)
  {
	cWHITE_FACET_RECORD swap = m_facetRecord[i1];
	m_facetRecord[i1] = m_facetRecord[i2];
	m_facetRecord[i2] = swap;
  }

  cBORDER_FACET_RECORD::cBORDER_FACET_RECORD()
 {
	m_cellIndex = -1;
	for (INT v=0; v<4; v++)
		m_vertexIndex[v] = -1;
 }

  cBORDER_FACET_RECORD::cBORDER_FACET_RECORD(iCELL cellIndex,
						  INT vertexIndex[4])
  {
 	m_cellIndex = cellIndex;
 	for (INT v=0; v<4; v++)
 		m_vertexIndex[v] = vertexIndex[v];
  }
}//namespace
