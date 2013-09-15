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
  INT nCells[3] = {20, 20, 20};
  cBOX3 modelBoundingBox = ModelBoundingBox<cGENERIC_MODEL>(&model);

  //
  cGENERIC_MODEL_GRID_GENERATOR gridGenObj(modelBoundingBox, nCells);
  cGENERIC_MODEL::cMESH *currMesh = model.MeshesBegin().operator->();
  currMesh->ConstructManifolds();

  //Constructing volumetric grid.
  cVOLUMETRIC_GRID vGrid;
  gridGenObj.Register(&model);
  gridGenObj.Build(&vGrid);
  fclose(fp);
}

int main(int argc, char **argv) {
  test_generic_model(argv[1]);
}

