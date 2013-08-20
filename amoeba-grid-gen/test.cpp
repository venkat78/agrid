/*
 * test.cpp
 *
 *  Created on: Aug 4, 2013
 *      Author: venkat
 */

#include "defs.hpp"
#include "generic_model_grid_defs.hpp"

VOID test_generic_model(char *file)

{
  cGENERIC_MODEL model;
  FILE *fp = fopen(file, "rb+");
  ImportStlModel(fp, model);
  cBOX3 bounds(cPOINT3(-20, -20, -20), cPOINT3(20, 20, 20));
  INT nCells[3] = { 4, 4, 4 };
  cGENERIC_MODEL_GRID_GENERATOR gridGenObj(bounds, nCells);
  cGENERIC_MODEL::cMESH *currMesh = model.MeshesBegin().operator->();
  currMesh->PrintFacets();
  currMesh->ConstructManifolds();
  gridGenObj.Register(&model);
  currMesh->PrintFacets();
  fclose(fp);
}

int main(int argc, char **argv) {
  test_generic_model(argv[1]);
}

