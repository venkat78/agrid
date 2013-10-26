/*
 *  amoeba_export_model.cpp
 *
 *  Created on: Sep 17, 2013
 *      Author: yury
 */

  #ifdef Grid_Export_Algorithm
  Class export_model contains all the required data from the grid.
  1. Export grid nodes
  2. Rearrange faces according to their facegroups
  3. Organize the data
  2. Export interior faces and their nodes as CRF
  3. Export boundary faces and their nodes as CRF
  4. Export interior faces and their cell indices as CRF
  5. Export boundary faces and their cell indices as CRF
  #endif //Grid_Export_Algorithm

#include "amoeba_export_model.hpp"

namespace io
{

//converts an array of object element records to Compressed Row Format (CRF) representation
	//For example, if an object is facet with nv vertices, and elements are vertices,
	//the record has nv+1 integers, nv, followed by vertex indices.
	//Same rule works for cells/facets, etc.
  VOID ConvertArrayToCRF(std::vector<INT> &objectElementArray,
				std::vector<INT> *rowArray, std::vector<INT> *colArray)
  {

	std::vector<INT>::iterator currRecord = objectElementArray.begin();
	std::vector<INT>::iterator endRecord = objectElementArray.end();
	INT currNumElements = 0;
	rowArray->push_back(currNumElements);

	while (currRecord != endRecord){
	  INT numElements = *currRecord;
	  currNumElements += numElements;
	  rowArray->push_back(currNumElements);
	  currRecord++;

	  for (INT elementCount = 0; elementCount < numElements; elementCount++){
		INT elementIndex = *currRecord;
		colArray->push_back(elementIndex);
		currRecord++;
	  }
	}
  }

  cAMOEBA_EXPORT_GRID::cAMOEBA_EXPORT_GRID()
  {
	m_gridId = -1;
  }

  cAMOEBA_EXPORT_GRID::cAMOEBA_EXPORT_GRID(INT numNodes, INT numFaces)
  {
	m_nodes.reserve(numNodes);
	m_faceNumNodes.reserve(numFaces);
	m_facesMapG2X.reserve(numFaces);
	m_facesMapX2G.reserve(numFaces);
	InitFaceCells(numFaces);
	m_currFacetExportIndex = numFaces-1;
	m_gridId = -1;
	//m_faceGroupIndex.reserve(numFaces);
	//InitFaceGroups();
  }

  VOID cAMOEBA_EXPORT_GRID::RegisterNode(iVERTEX vertexIndex, cPOINT3 & vertexPoint)
  {
	m_nodes[vertexIndex] = vertexPoint;
  }

  VOID cAMOEBA_EXPORT_GRID::RegisterFaceNumNodes(iFACET faceIndex, INT numNodes)
  {
	m_faceNumNodes[faceIndex] = (faceIndex ? m_faceNumNodes[faceIndex-1] : 0) + numNodes;
  }


  VOID cAMOEBA_EXPORT_GRID::RegisterFaceNodeIndex(iFACET faceIndex, iVERTEX nodeIndex)
  {
	  m_faceNodes.push_back(nodeIndex);
  }

  VOID cAMOEBA_EXPORT_GRID::RegisterFaceCell(iFACET faceIndex, INT cellIndex)
  {
	for (INT i=0; i<2; i++){
	  if (m_faceCells[faceIndex].m_cells[i] < 0){
		m_faceCells[faceIndex].m_cells[i] = cellIndex;
		return;
	  }
	}
	ASSERT(false);
  }
/*
  VOID cAMOEBA_EXPORT_GRID::RegisterFaceWithGroup(INT faceGroupIndex, iFACET faceIndex)
  {
	m_faceGroupIndex[faceIndex] = faceGroupIndex;
	if (faceGroupIndex > m_maxFaceGroupIndex)
		m_maxFaceGroupIndex = faceGroupIndex;
  }
*/
  VOID cAMOEBA_EXPORT_GRID::InitFaceCells(INT numFaces)
  {
	m_faceCells.reserve(numFaces);
	for (std::vector<sCELLS_PAIR>::iterator cellsPair = m_faceCells.begin() ;
			cellsPair != m_faceCells.end(); cellsPair++){
		(*cellsPair).m_cells[0] = -100; //INVALID_ICELL
		(*cellsPair).m_cells[1] = -100; //INVALID_ICELL
	}
  }

  pAMOEBA_FACEGROUP cAMOEBA_EXPORT_GRID::FaceGroupAppend(INT oppGridIndex)
  {
	pAMOEBA_FACEGROUP faceGroup = new cAMOEBA_FACEGROUP;
	faceGroup->OppGridIndex(oppGridIndex);
	faceGroup->FirstFaceExportIndex(m_currFacetExportIndex);
  }

  VOID cAMOEBA_EXPORT_GRID::RegisterInterfaceFace(iFACET facetIndex, INT oppGridIndex)
  {
	pAMOEBA_FACEGROUP currFaceGroup = m_faceGroups.back();
	if (currFaceGroup->OppGridIndex() != oppGridIndex)
	  currFaceGroup = FaceGroupAppend(oppGridIndex);
	m_facesMapG2X[facetIndex] = m_currFacetExportIndex;
	m_facesMapX2G[m_currFacetExportIndex] = facetIndex;
	m_currFacetExportIndex--;
	currFaceGroup->AppendFace();
  }

#ifdef CHANGE
  //Creates new indexation of faces according to the amoeba rules
  //and mappings between the grid indexation and export indexation
  VOID cAMOEBA_EXPORT_GRID::Prepare()
  {
	//First traverse interior faces, which are supposed to belong to no facegroup.

	iFACET gridFaceIndex=0, exportFaceIndex=0;
	m_maxCellIndex = -1;

	for (std::vector<sCELLS_PAIR>::iterator cellsPair = m_faceCells.begin() ;
			cellsPair != m_faceCells.end(); cellsPair++, gridFaceIndex++){
	  ASSERT((*cellsPair).m_cells[1] >= 0);
	  if ((*cellsPair).m_cells[0] > m_maxCellIndex)
		m_maxCellIndex = (*cellsPair).m_cells[0];

	  if ((*cellsPair).m_cells[1] >= 0){ //face is interior
		m_facesMapX2G[gridFaceIndex] = exportFaceIndex;
		m_facesMapG2X[exportFaceIndex] = gridFaceIndex;
		exportFaceIndex++;
		if ((*cellsPair).m_cells[1] > m_maxCellIndex)
		  m_maxCellIndex = (*cellsPair).m_cells[1];
	  }
	}

	//traverse faces from each facegroup.

	std::vector<INT>::iterator currFace = m_faceGroupFaces.begin();

	for (std::vector<INT>::iterator groupNumFaces = m_faceGroupNumFaces.begin();
			groupNumFaces != m_faceGroupNumFaces.end(); groupNumFaces++){
	  INT numFaces = (*groupNumFaces);
	  for (INT i=0; i < numFaces; i++){
		iFACET gridFaceIndex = (*currFace);
		m_facesMapX2G[gridFaceIndex] = exportFaceIndex;
		m_facesMapG2X[exportFaceIndex] = gridFaceIndex;
		exportFaceIndex++;
	  }
	}
  }
#endif //CHANGE

  VOID cAMOEBA_EXPORT_INTERFACE::AddInterfaceRecord(iFACET leftFacetIndex,
			iFACET rightFacetIndex)
  {
	sAMOEBA_INTERFACE_RECORD interfaceRecord;
	interfaceRecord.m_leftFacetIndex = leftFacetIndex;
	interfaceRecord.m_rightFacetIndex = rightFacetIndex;
	m_interface.push_back(interfaceRecord);
  }

  //returns index of the interface of this pair of grids.
  //if it does not exist, the new interface is added to the vector of interfaces
  INT cAMOEBA_EXPORT_MODEL::GridInterfaceIndex(INT leftGridIndex, INT rightGridIndex)
  {
	int index = 0;
	std::vector<pAMOEBA_EXPORT_INTERFACE>::iterator firstInterface = m_gridInterfaces.begin();
	std::vector<pAMOEBA_EXPORT_INTERFACE>::iterator lastInterface = m_gridInterfaces.end();
	for (std::vector<pAMOEBA_EXPORT_INTERFACE>::iterator interface = firstInterface;
			interface != lastInterface; interface++, index++){
	  if ((*interface)->LeftGridIndex() == leftGridIndex &&
			  (*interface)->RightGridIndex() == rightGridIndex)
		return index;
	}
	//the interface does not exist
	pAMOEBA_EXPORT_INTERFACE newInterface = new cAMOEBA_EXPORT_INTERFACE;
	newInterface->LeftGridIndex(leftGridIndex);
	newInterface->RightGridIndex(rightGridIndex);
	index = m_gridInterfaces.size();
	m_gridInterfaces.push_back(newInterface);
	return index;
  }

  //returns the index for the interface of this pair of grids for possible future use
  INT cAMOEBA_EXPORT_MODEL::AddInterfaceRecord(iFACET leftFacetIndex,
		  	iFACET rightFacetIndex,
			INT leftGridIndex, INT rightGridIndex)
  {
	INT interfaceIndex = GridInterfaceIndex(leftGridIndex, rightGridIndex);
	AddInterfaceRecord(leftFacetIndex, rightFacetIndex, interfaceIndex);
	return interfaceIndex;
  }

  //uses grid interface index in the interface vector to avoid linear search
  VOID cAMOEBA_EXPORT_MODEL::AddInterfaceRecord(iFACET leftFacetIndex,
		  	iFACET rightFacetIndex,
			INT gridInterfaceIndex)
  {
	pAMOEBA_EXPORT_INTERFACE interface = m_gridInterfaces[gridInterfaceIndex];
	interface->AddInterfaceRecord(leftFacetIndex, rightFacetIndex);
  }

  VOID cAMOEBA_EXPORT_MODEL::PrepareInit()
  {
  }

  VOID cAMOEBA_EXPORT_MODEL::PrepareInterface()
  {
	//traverse the interface facegroups and create the facets export indices
	std::vector<pAMOEBA_EXPORT_INTERFACE>::iterator currInterface = m_gridInterfaces.begin();
	std::vector<pAMOEBA_EXPORT_INTERFACE>::iterator lastInterface = m_gridInterfaces.end();

	for (; currInterface != lastInterface; currInterface++){
	  INT leftGridIndex = (*currInterface)->LeftGridIndex();
	  INT rightGridIndex = (*currInterface)->RightGridIndex();
	  pAMOEBA_EXPORT_GRID leftGrid = m_grids[leftGridIndex];
	  pAMOEBA_EXPORT_GRID rightGrid = m_grids[rightGridIndex];

	  std::vector<sAMOEBA_INTERFACE_RECORD>::iterator currInterfaceRecord =
				  (*currInterface)->InterfaceRecordBegin();
	  std::vector<sAMOEBA_INTERFACE_RECORD>::iterator lastInterfaceRecord =
				  (*currInterface)->InterfaceRecordEnd();

	  for (; currInterfaceRecord != lastInterfaceRecord; currInterfaceRecord++){
		iFACET leftFacetIndex = (*currInterfaceRecord).m_leftFacetIndex;
		iFACET rightFacetIndex = (*currInterfaceRecord).m_rightFacetIndex;
		leftGrid->RegisterInterfaceFace(leftFacetIndex, rightGridIndex);
		rightGrid->RegisterInterfaceFace(rightFacetIndex, leftGridIndex);
	  }
	}
  }

  VOID cAMOEBA_EXPORT_MODEL::PrepareBoundaryFaces()
  {
	//traverse the remaining boundary faces and create their export indices
  }

  VOID cAMOEBA_EXPORT_MODEL::Prepare()
  {
	PrepareInit();
	PrepareInterface();
	PrepareBoundaryFaces();
  }

  BOOL cAMOEBA_EXPORT_MODEL::OpenHdfFile()
  {
	m_exportFile = new H5File(m_hdfFilePath, H5F_ACC_RDWR);
	return (m_exportFile != NULL);
  }

  VOID cAMOEBA_EXPORT_MODEL::WriteGrids()
  {
	if (!OpenHdfFile()){
	  printf("Cannot open HDF file %s\n", m_hdfFilePath.c_str());
	  return;
	}

	//create a group 'meshes' if it does ot exist

  }

} //namespace
