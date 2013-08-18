/*
 * amoeba_io.cpp
 *
 *  Created on: May 2, 2013
 *      Author: Bujji
 */

#include "H5Cpp.h"
using namespace H5;

#include "off.hpp"
#include "amoeba_io.hpp"

namespace io {
  typedef cAMOEBA_MODEL::cGROUP cGROUP;

  /*
   * Find if a group is a mesh.
   */
  BOOL IsAmoebaGroupAMesh(Group &g) {
    BOOL hasFaceTessConn, hasFaceTessPoints, hasFaceTessSizes;
    hasFaceTessConn = hasFaceTessPoints = hasFaceTessSizes = false;

    hsize_t numObjs = g.getNumObjs();
    for (INT i = 0; i < numObjs; i++) {
      std::string objName = g.getObjnameByIdx(i);
      if (objName.compare("faceTessellationConnectivity") == 0) {
        hasFaceTessConn = true;
      } else if (objName.compare("faceTessellationPoints") == 0) {
        hasFaceTessPoints = true;
      } else if (objName.compare("faceTessellationSizes") == 0) {
        hasFaceTessSizes = true;
      }

      if (hasFaceTessConn && hasFaceTessPoints && hasFaceTessSizes)
        return true;
    }

    return false;
  }

  VOID ReadAmoebaMesh(Group &g, cGROUP *pgInModel, cAMOEBA_MODEL &model) {
    REAL coords[250000][3];
    cAMOEBA_MODEL::cMESH *mesh = model.CreateMesh();
    mesh->SetName(pgInModel->GetName());
    pgInModel->SetMesh(mesh);

    /*
     * Add vertices to mesh.-
     */
    DataSet pointSet = g.openDataSet("faceTessellationPoints");
    DataSpace points = pointSet.getSpace();
    /*
     * Get the dimension size of each dimension in the dataspace and
     * display them.
     */
    hsize_t vertex_dims[2];
    int rank = points.getSimpleExtentNdims();
    INT ndims = points.getSimpleExtentDims(vertex_dims, NULL);

    //Read vertices
    pointSet.read(coords, PredType::NATIVE_DOUBLE, DataSpace::ALL, points);

    //Add vertices for mesh.
    for (INT i = 0; i < vertex_dims[0]; i++) {
      mesh->NewVertex(coords[i][0], coords[i][1], coords[i][2]);
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
      mesh->NewTriangle(facetNodes[i], facetNodes[i + 1], facetNodes[i + 2]);
    }

    //ExportToOff<cAMOEBA_MODEL::cMESH>((char*) (pgInModel->GetName().c_str()), *mesh);
  }

  BOOL ReadAmoebaGroup(Group &g, cGROUP *pgInModel, cAMOEBA_MODEL &model) {
    /*
     * Constructing the tree structure
     */
    if (IsAmoebaGroupAMesh(g)) {
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
        ReadAmoebaGroup(childGroup, cgInModel, model);
      } else if (objType == H5G_DATASET) {
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

