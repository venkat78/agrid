/*
 * test.cpp
 *
 *  Created on: Aug 4, 2013
 *      Author: venkat
 */

#include <cstdio>
#include "defs.hpp"
#include "generic_model_grid_defs.hpp"

VOID test_generic_model_box(char *file)

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

  sVOLUMETRIC_GRID_BOUNDARY_FILTER filter(&vGrid);
  tmesh::ExportToOff<cVOLUMETRIC_GRID,
   sVOLUMETRIC_GRID_BOUNDARY_FILTER>("bfacesMesh.off", vGrid, filter);
}

int main(int argc, char **argv) {
//  test_generic_model_box(argv[1]);
  cSURFACE_MESH mesh;
  cBOX3 box(cPOINT3(0, 0, 0), cPOINT3(2, 2, 2));
  REAL val = box.Center()[GEOM_X];
  BoxToMesh(box, mesh);

  FILE *fp = fopen("before_clipping.off", "wb+");
  ExportToOff(fp, mesh);
  fclose(fp);

  for(INT i = 0; i < 6; i++)
    tMESH_CLIPPER<cSURFACE_MESH>(&mesh).Clip(i, GEOM_X, 0.5);

  fp = fopen("after_clipping.off", "wb+");
  ExportToOff(fp, mesh);
  fclose(fp);

}

