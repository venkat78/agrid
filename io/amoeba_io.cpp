/*
 * amoeba_io.cpp
 *
 *  Created on: May 2, 2013
 *      Author: Bujji
 */

#include "H5Cpp.h"
using namespace H5;

#include "amoeba_io.hpp"

namespace io
{
	typedef cAMOEBA_MODEL::cGROUP cGROUP;

	BOOL ReadAmoebaGroup(Group &g, cGROUP *pgInModel, cAMOEBA_MODEL &model) {
		/*
		 * Constructing the tree structure
		 */
		hsize_t numObjs = g.getNumObjs();
		for (int i = 0; i < numObjs; i++) {
			H5G_obj_t objType = g.getObjTypeByIdx(i);
			std::string objName = g.getObjnameByIdx(i);
			if (objType == H5G_GROUP) {
				Group childGroup = g.openGroup(objName);
				cGROUP *cgInModel = model.CreateGroup();
				cgInModel->SetName(objName);
				std::cout<<pgInModel->GetName().c_str()<< '/'<<cgInModel->GetName().c_str()<<std::endl;
				pgInModel->AddChildGroup(cgInModel);
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
}

