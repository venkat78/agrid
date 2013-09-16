/*
 * test.cpp
 *
 *  Created on: Aug 4, 2013
 *      Author: venkat
 */

#include <cstdio>
#include "defs.hpp"
#include "generic_model_grid_defs.hpp"

VOID test_generic_model(char *file)

{
  cGENERIC_MODEL model;

  //Importing STL model.
  FILE *fp = fopen(file, "rb+");
  ImportStlModel(fp, model);
  fclose(fp);

  INT nCells[3] = { 20, 20, 20 };
  cBOX3 modelBoundingBox = ModelBoundingBox<cGENERIC_MODEL>(&model);
  REAL delta[3];
  for (INT i = 0; i < 3; i++) {
    delta[i] = (modelBoundingBox.Thickness(i) * 1.01) / nCells[i];
    nCells[i] += 2;
  }

  modelBoundingBox.Print();
  modelBoundingBox.GrowBy(delta);
  modelBoundingBox.Print();

  //
  cGENERIC_MODEL_GRID_GENERATOR gridGenObj(modelBoundingBox, nCells);
  cGENERIC_MODEL::cMESH *currMesh = model.MeshesBegin().operator->();
  currMesh->ConstructManifolds();

  fp = fopen("before_clipping.off", "wb+");
  ExportToOff(fp, *currMesh);
  fclose(fp);

  //Constructing volumetric grid.
  cVOLUMETRIC_GRID vGrid;
  gridGenObj.Register(&model);

  fp = fopen("after_clipping.off", "wb+");
  ExportToOff(fp, *currMesh);
  fclose(fp);

  gridGenObj.Build(&vGrid);

}

int main(int argc, char **argv) {
  test_generic_model(argv[1]);
}

