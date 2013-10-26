/*
 * amoeba_export_model.hpp
 *
 *  Created on: Sep 17, 2013
 *      Author: yury
 */

#ifndef AMOEBA_EXPORT_MODEL_HPP_
#define AMOEBA_EXPORT_MODEL_HPP_

#include "defs.hpp"
#include "H5Cpp.h"
using namespace H5;

namespace io {
  VOID ConvertArrayToCRF(std::vector<INT> &objectElementArray,
			std::vector<INT> *rowArray, std::vector<INT> *colArray);

  struct sCELLS_PAIR {
	INT m_cells[2]; //replace INT by iCELL
  };

  class cAMOEBA_FACEGROUP {
  public:
	VOID OppGridIndex(INT oppGridIndex) {m_oppGridIndex = oppGridIndex;}
	INT OppGridIndex() {return m_oppGridIndex;}
	VOID FirstFaceExportIndex(INT firstFaceExportIndex = 0)
		{m_firstFaceExportIndex = firstFaceExportIndex;}
	INT FirstFaceExportIndex() {return m_firstFaceExportIndex;}
	VOID NumFaces (INT numFaces=0) {m_numFaces = numFaces;}
	INT NumFaces () {return m_numFaces;}
	VOID AppendFace() {m_numFaces++;m_firstFaceExportIndex--;}

  private:
	int m_numFaces; //number of faces in the group
	INT m_firstFaceExportIndex;
	INT m_oppGridIndex;
  };

  typedef cAMOEBA_FACEGROUP *pAMOEBA_FACEGROUP;

  class cAMOEBA_EXPORT_GRID {

  public:
	cAMOEBA_EXPORT_GRID();
	cAMOEBA_EXPORT_GRID(INT numNodes, INT numFaces);
	VOID RegisterNode(iVERTEX vertexIndex, cPOINT3 & vertexPoint);
	VOID RegisterFaceNumNodes(iFACET faceIndex, INT numNodes);
	//this method needs to be called in order of facet indices. If that may not
	//be guaranteed, a simple workaround will be added
	VOID RegisterFaceNodeIndex(iFACET faceIndex, iVERTEX nodeIndex); //it also nees to be called
	//by order of faces and nodes within the faces
	VOID RegisterFaceCell(iFACET faceIndex, INT cellIndex);
//	VOID RegisterFaceWithGroup(INT faceGroupIndex, iFACET faceIndex);
//	VOID Prepare();
	VOID RegisterInterfaceFace(iFACET facetIndex, INT oppGridIndex);

  private:
	VOID InitFaceCells(INT numFaces);
	pAMOEBA_FACEGROUP FaceGroupAppend(INT oppGridIndex);
//	VOID InitFaceGroups();

  private:	//data fields
	INT m_gridId;
	std::string m_gridName;
//	INT m_maxFaceGroupIndex;
	INT m_maxCellIndex;
	INT m_currFacetExportIndex; //starts with the greatest value, and goes down
	std::vector<cPOINT3> m_nodes;
	std::vector<INT> m_faceNumNodes; //for each face it contains
	//the cumulative number of nodes of all the faces preceding and including this one
	std::vector<INT> m_faceNodes; //for each face it contains its nodes
	std::vector<INT> m_facesMapG2X; //map from grid face indices to the export indices
	std::vector<INT> m_facesMapX2G; //map from export face indices to the grid indices
	std::vector<sCELLS_PAIR> m_faceCells;
//	std::vector<INT> m_faceGroupIndex; //for each face it contains its group index. The values are initialized to -20.
	//The group indices are sequential numbers. If needed, a mapping will be added.
	std::vector<pAMOEBA_FACEGROUP> m_faceGroups;
  };

  typedef cAMOEBA_EXPORT_GRID *pAMOEBA_EXPORT_GRID;

  struct sAMOEBA_INTERFACE_RECORD {
	iFACET m_leftFacetIndex;
	iFACET m_rightFacetIndex;
  };

  class cAMOEBA_EXPORT_INTERFACE{

  public:
	VOID AddInterfaceRecord(iFACET leftFacetIndex,
			iFACET rightFacetIndex);
	INT LeftGridIndex() {return m_leftGridIndex;}
	INT RightGridIndex() {return m_rightGridIndex;}
	VOID LeftGridIndex(INT leftGridIndex) {leftGridIndex = m_leftGridIndex;}
	VOID RightGridIndex(INT rightGridIndex) {rightGridIndex = m_rightGridIndex;}
	std::vector<sAMOEBA_INTERFACE_RECORD>::iterator InterfaceRecordBegin()
	{return m_interface.begin();}
	std::vector<sAMOEBA_INTERFACE_RECORD>::iterator InterfaceRecordEnd()
	{return m_interface.end();}
  private:
	INT m_leftGridIndex;
	INT m_rightGridIndex;
	std::vector<sAMOEBA_INTERFACE_RECORD> m_interface;
  };

  typedef cAMOEBA_EXPORT_INTERFACE *pAMOEBA_EXPORT_INTERFACE;

  class cAMOEBA_EXPORT_MODEL{
  public:
	VOID WriteGrids();
	VOID HdfFilePath(std::string & filePath) {m_hdfFilePath = filePath;}
	std::string & HdfFilePath() { return m_hdfFilePath;}
	INT AddInterfaceRecord(iFACET leftFacetIndex, iFACET rightFacetIndex,
			INT leftGridIndex, INT rightGridIndex);
	VOID AddInterfaceRecord(iFACET leftFacetIndex, iFACET rightFacetIndex,
			INT gridInterfaceIndex);
VOID Prepare();


  private:
	INT GridInterfaceIndex(INT leftGridIndex, INT rightGridIndex);
	BOOL OpenHdfFile();
	VOID PrepareInit();
	VOID PrepareInterface();
	VOID PrepareBoundaryFaces();


  private: //data fields
	std::string m_hdfFilePath;
	H5File *m_exportFile;

	std::vector<pAMOEBA_EXPORT_GRID> m_grids;
	std::vector<pAMOEBA_EXPORT_INTERFACE> m_gridInterfaces;
  };
} //namespace



#endif // AMOEBA_EXPORT_MODEL_HPP_


