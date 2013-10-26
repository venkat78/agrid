/*
 * amoeba_io.cpp
 *
 *  Created on: May 2, 2013
 *      Author: Bujji
 */

#include "H5Cpp.h"
using namespace H5;

#include <map>
#include "off.hpp"
#include "amoeba_io.hpp"

#define CPTR(_VAR,_CONST) ((_VAR)=(_CONST),&(_VAR)) //for reading enums

namespace io
{
	typedef cAMOEBA_MODEL::cGROUP cGROUP;

	/*
	 * Find if a group is a mesh.
	 */
	BOOL IsAmoebaGroupAMesh(Group &g) {
		BOOL hasFaceTessConn, hasFaceTessPoints, hasFaceTessSizes;
		hasFaceTessConn = hasFaceTessPoints = hasFaceTessSizes = false;

		hsize_t numObjs = g.getNumObjs();
	//	printf("YG: IsAmoebaGroupAMesh 0: numObjs %d\n", numObjs);

		for (INT i = 0; i < numObjs; i++) {
			std::string objName = g.getObjnameByIdx(i);
//			printf("YG: IsAmoebaGroupAMesh 1: i %d\n", i);

			if (objName.compare("faceTessellationConnectivity") == 0) {
				hasFaceTessConn = true;
			}
			else if (objName.compare("faceTessellationPoints") == 0) {
				hasFaceTessPoints = true;
			}
			else if (objName.compare("faceTessellationSizes") == 0) {
				hasFaceTessSizes = true;
			}

			if (hasFaceTessConn && hasFaceTessPoints && hasFaceTessSizes)
				return true;
		}

		return false;
	}

	BOOL IsAmoebaMeshASurface(Group &g) {
		//returns true if the numbers of shells and solids in a mesh group are 0.
		hsize_t numObjs = g.getNumObjs();

		for (INT i = 0; i < numObjs; i++) {
			std::string objName = g.getObjnameByIdx(i);

			if (objName.compare("entityCounts") == 0) {
				INT entityCounts[10];
				DataSet intSet = g.openDataSet("entityCounts");
				DataSpace counts = intSet.getSpace();
				intSet.read(entityCounts, PredType::NATIVE_INT, DataSpace::ALL, counts);
				return entityCounts[6] == 0; //numSolids
			}
		}
		return false;
	}

	BOOL AmoebaGroupIsCylinder(Group &g, cPOINT3 &c1, cPOINT3 &c2, REAL &r1, REAL &r2)
	{
		//First, read the attributes to find if the object is a cylinder and its radii
		INT numAttrs = g.getNumAttrs();
		INT numData = 0;
		BOOL isCyl = false;
		for (INT i = 0; i < numAttrs && numData < 3 && !isCyl; i++) {
			Attribute attr = g.openAttribute(i);
			std::string attrName = attr.getName();
			if (attrName == "radius1"){
				double r;
				attr.read(PredType::NATIVE_DOUBLE, &r);
				r1 = r;
				numData++;
			}
			else if (attrName == "radius2"){
				double r;
				attr.read(PredType::NATIVE_DOUBLE, &r);
				r2 = r;
				numData++;
			}
			else if (attrName == "hdf:className"){
				StrType sType(0,17);
				char buffer[17];
				attr.read(sType, buffer);
				if (strcmp(buffer, "Cylinder"))
					return false;
				isCyl = true;
				numData++;
			}
		}

		if (!isCyl)
			return false;

		//If the object is a cylinder read the points

		DataSet pointSet = g.openDataSet("points");
		DataSpace points = pointSet.getSpace();
		//Get the dimension size of each dimension in the dataspace

		hsize_t points_dims[2];
		INT ndims = points.getSimpleExtentDims(points_dims, NULL);
		if (points_dims[0] != 2)
			return false;
		REAL coords[2][3];

		pointSet.read(coords, PredType::NATIVE_DOUBLE, DataSpace::ALL, points);
		DO_COORDS(c){
			c1[c] = coords[0][c];
			c2[c] = coords[1][c];
		}
		return true;
	}

	VOID ReadAmoebaMeshAttributes(Group &g, cAMOEBA_MODEL::cMESH *mesh)
	{
		const INT NUM_READ_ATTRS = 6;
		INT numAttrs = g.getNumAttrs();
		INT numRead = NUM_READ_ATTRS;
		sAMOEBA_MESH_ATTRIBUTES *meshAttributes = mesh->GetAttributes();

		for (INT count = 0; count < numAttrs && numRead; count++) {
			Attribute attr = g.openAttribute(count);
			std::string attrName = attr.getName();
//			printf("attr %s\n", attrName.c_str());
			if (attrName == "color"){
				int c[3];
				attr.read(PredType::NATIVE_INT, &c);
				for (int j=0; j < 3; j++){
					meshAttributes->m_color[j] = c[j];
				}
//				printf("color %d %d %d\n", c[0], c[1], c[2]);
				numRead--;
			}
			else if (attrName == "hdf:cindex"){
				int index;
				attr.read(PredType::NATIVE_INT, &index);
//				printf("cindex %d\n", index);
				meshAttributes->m_cindex = index;
				numRead--;
			}
			else if (attrName == "is_hollow"){

				typedef enum {FALSE = 0,TRUE = 1} MyBool;

				hid_t boolEnumType = H5Tcreate(H5T_ENUM, sizeof(MyBool));

				MyBool is_hollow;
				H5Tenum_insert(boolEnumType, "FALSE", CPTR(is_hollow, FALSE ));
				H5Tenum_insert(boolEnumType, "TRUE", CPTR(is_hollow, TRUE ));

				attr.read(boolEnumType, &is_hollow);
				meshAttributes->m_is_hollow = is_hollow;
//				printf("%s:\tis_hollow %d\n", mesh->GetName().data(), hollow);
				numRead--;
			}
			else if (attrName == "meshing_priority"){
				int p;
				attr.read(PredType::NATIVE_INT, &p);
				meshAttributes->m_meshing_priority = p;
//				printf("m_pri %d\n", p);
				numRead--;
			}
			else if (attrName == "max_mesh_size"){
				double s[3];
				attr.read(PredType::NATIVE_DOUBLE, &s);
				cVECTOR3 size(s[0], s[1], s[2]);
//				printf("max_mesh_size %lg %lg %lg\n", s[0], s[1], s[2]);
				DO_COORDS(j){
					meshAttributes->m_mesh_max_size[j] = s[j];
				}
				numRead--;
			}
			else if (attrName == "min_mesh_size"){
				double s[3];
				attr.read(PredType::NATIVE_DOUBLE, &s);
				cVECTOR3 size(s[0], s[1], s[2]);
				DO_COORDS(j){
					meshAttributes->m_mesh_min_size[j] = s[j];
				}
//				printf("min_mesh_size %lg %lg %lg\n", s[0], s[1], s[2]);

				numRead--;
			}
		}
	}

	VOID ReadAmoebaMesh(Group &g, cGROUP *pgInModel, cAMOEBA_MODEL &model)
	{
		cAMOEBA_MODEL::cMESH *mesh = model.CreateMesh();
		mesh->SetName(pgInModel->GetName());
		pgInModel->SetMesh(mesh);

		cPOINT3 c1, c2;
		REAL r1, r2;
		if (AmoebaGroupIsCylinder(g, c1, c2, r1, r2)){
			mesh->AppendTessellatedCylinder(c1, c2, r1, r2);
		}

		else {
			/*
			 * Add vertices to mesh.-
			 */
			REAL coords[250000][3];
			DataSet pointSet = g.openDataSet("faceTessellationPoints");
			DataSpace points = pointSet.getSpace();
			/*
			 * Get the dimension size of each dimension in the dataspace and
			 * display them.
			 */
			hsize_t vertex_dims[2];
			//int rank = points.getSimpleExtentNdims();
			INT ndims = points.getSimpleExtentDims(vertex_dims, NULL);

			iVERTEX parentVertexIndex[5000];
			memset(parentVertexIndex, INVALID_IVERTEX, 5000);
			std::map<cPOINT3, iVERTEX> pointToVertexMap;

			//Read vertices
			pointSet.read(coords, PredType::NATIVE_DOUBLE, DataSpace::ALL, points);

			//Add vertices for mesh.
			for (INT vertexCount = 0; vertexCount < vertex_dims[0]; vertexCount++) {
				cPOINT3 vertexPoint(coords[vertexCount][0], coords[vertexCount][1], coords[vertexCount][2]);
				std::map<cPOINT3, iVERTEX>::iterator pos = pointToVertexMap.find(vertexPoint);

				if(pos == pointToVertexMap.end()) {
					iVERTEX vertexIndex = mesh->NewVertex(vertexPoint);
					pointToVertexMap[vertexPoint] = vertexIndex;
					parentVertexIndex[vertexCount] = vertexIndex;
				}
				else
					parentVertexIndex[vertexCount] = pos->second;

			}

			DataSet facetSet = g.openDataSet("faceTessellationConnectivity");
			DataSpace facets = facetSet.getSpace();

			/*
			 * Get the dimension size of each dimension in the dataspace and
			 * display them.
			 */
			hsize_t facet_dims;
			ndims = facets.getSimpleExtentDims(&facet_dims, NULL);

			INT facetNodes[500000];
			facetSet.read(facetNodes, PredType::NATIVE_INT, DataSpace::ALL, facets);

			//Read vertices
			for (INT i = 0; i < facet_dims; i = i + 3) {
				mesh->NewTriangle(parentVertexIndex[facetNodes[i]],
							  parentVertexIndex[facetNodes[i + 1]],
							  parentVertexIndex[facetNodes[i + 2]]);
			}
		}

		ReadAmoebaMeshAttributes(g, mesh);/*
		 * amoeba_export_model.hpp
		 *
		 *  Created on: Sept. 17, 2013
		 *      Author: Yury
		 */

		//return;
		char fName[1000];
		sprintf(fName, "/home/yury/datafiles/amoeba/amgfiles/test10-multiple-assemblies/%s.off\0",
				(char*) (pgInModel->GetName().c_str()));
		/*
		int l0 = strlen("/home/yury/datafiles/amoeba/amgfiles/test10-multiple-assemblies/test10-multiple-assemblies/");
		int l1 = strlen((char*) (pgInModel->GetName().c_str()));
		strcpy(fName, "/home/yury/datafiles/amoeba/amgfiles/test10-multiple-assemblies/test10-multiple-assemblies");
		strcpy(fName+l0, (char*) (pgInModel->GetName().c_str()));
		fName[l0+l1] = '.';
		fName[l0+l1+1] = 'o';
		fName[l0+l1+2] = 'f';
		fName[l0+l1+3] = 'f';
		fName[l0+l1+4] = '\0';
		*/
		printf("YG : fName %s\n", fName);
		ExportToOff<cAMOEBA_MODEL::cMESH>(fName, *mesh);
//		ExportToOff<cAMOEBA_MODEL::cMESH>((char*) (pgInModel->GetName().c_str()), *mesh);
		/*
		fName[l0+l1+1] = 'h';
		fName[l0+l1+2] = 'l';
		fName[l0+l1+3] = 's';
		*/
		fName[0] = '\0';
		sprintf(fName, "/home/yury/datafiles/amoeba/amgfiles/test10-multiple-assemblies/%s.holes\0",
				(char*) (pgInModel->GetName().c_str()));
		printf("YG : fName %s\n", fName);
		FILE *meshFile = fopen(fName, "wb");
		mesh->PrintHoles(meshFile);
		fclose(meshFile);
	}

	VOID ReadAmoebaGlobalAttributes(Group &g, cAMOEBA_MODEL &model)
	{
		const INT NUM_READ_ATTRS = 3;
		INT numAttrs = g.getNumAttrs();
		INT numRead = NUM_READ_ATTRS;

		for (INT i = 0; i < numAttrs && numRead; i++) {
			Attribute attr = g.openAttribute(i);
			std::string attrName = attr.getName();
			if (attrName == "max_mesh_size"){
				double s[3];
				attr.read(PredType::NATIVE_DOUBLE, &s);
				cVECTOR3 size(s[0], s[1], s[2]);
				model.MaxMeshSize(size);
				numRead--;
			}
			else if (attrName == "min_mesh_size"){
				double s[3];
				attr.read(PredType::NATIVE_DOUBLE, &s);
				cVECTOR3 size(s[0], s[1], s[2]);
				model.MinMeshSize(size);
				numRead--;
			}
			else if (attrName == "angleTolerance"){
				double tol;
				attr.read(PredType::NATIVE_DOUBLE, &tol);
				model.AngleTolerance(tol);
				numRead--;
			}
		}
	}

	BOOL ReadAmoebaGroup(Group &g, cGROUP *pgInModel, cAMOEBA_MODEL &model) {
		/*
		 * Constructing the tree structure
		 */
		if (IsAmoebaGroupAMesh(g) && !IsAmoebaMeshASurface(g)) {
			ReadAmoebaMesh(g, pgInModel, model);
			return true;
		}

		hsize_t numObjs = g.getNumObjs();
		for (int i = 0; i < numObjs; i++) {
			H5G_obj_t objType = g.getObjTypeByIdx(i);
			std::string objName = g.getObjnameByIdx(i);
			if (objType == H5G_GROUP) {
				Group childGroup = g.openGroup(objName);
				cGROUP *cgInModel = model.CreateGroup();
				cgInModel->SetName(objName);
				pgInModel->AddChildGroup(cgInModel);
				if (objName == "geometryTree")
					ReadAmoebaGlobalAttributes(childGroup, model);
				ReadAmoebaGroup(childGroup, cgInModel, model);
			}
			else if (objType == H5G_DATASET) {
				//readDataSet(g.openDataSet(objName), objName, absPath);
			}
		}

		return true;
	}

	BOOL ImportAmoebaModel(const std::string &fname, cAMOEBA_MODEL &model) {
		try {
			/*
			 * Turn off the auto-printing when failure occurs so that we can
			 * handle the errors appropriately
			 */
			Exception::dontPrint();
			std::string GROUP_NAME("sim");
			/*
			 * Open the specified file and the specified dataset in the file.
			 */
			H5File file(fname, H5F_ACC_RDONLY);
			cGROUP *simGroup = model.CreateGroup();
			model.SetRootGroup(simGroup);
			simGroup->SetName(GROUP_NAME);
			Group hdfGroup = file.openGroup(GROUP_NAME);

			ReadAmoebaGroup(hdfGroup, simGroup, model);
			//readGroup(simGroup, "sim", "");
			file.close();

		}

// catch failure caused by the DataSet operations
		catch (DataSetIException error) {
			//error.printError();
			return false;
		}

// catch failure caused by the DataSpace operations
		catch (DataSpaceIException error) {
			//error.printError();
			return false;
		}

// catch failure caused by the DataSpace operations
		catch (DataTypeIException error) {
			//error.printError();
			return false;
		}

		return true;

	}
} //namespace

