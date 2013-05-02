#include <iostream>
#include <cstdio>
#include <time.h>

#include "utils.hpp"
#include "geom.hpp"
#include "spatial_trees.hpp"
#include "tmesh.hpp"
#include "surface_mesh.hpp"
#include "io.hpp"
#include "agrid_mesh.hpp"
#include "agrid_boolean.hpp"
#include "model_export.hpp"

using namespace utils;
using namespace geom;
// using namespace mesh;
using namespace tmesh;
using namespace spatial_trees;
using namespace mesh_utils;
using namespace model;
using namespace io;
using namespace agrid_boolean;

INT printoutLevel = 0;
BOOL tightSimVol = false; //if changed to true, the tight simvol is built around the mesh,
//and only grid in the tight simvol is returned. All its cut cells should b identical to those
//generated by using the given simvol.

BOOL shadeAreaOnly = false;

class cGEOM_MGR {
public:
  cGEOM_MGR() : m_bounds(cPOINT2(0, 0), cPOINT2(2, 2))
  {
    seg[0] = cSEGMENT2(cPOINT2(0.5, 0.5), cPOINT2(1.5, 0.5));
    seg[1] = cSEGMENT2(cPOINT2(1.5, 0.5), cPOINT2(1.5, 1.5));
    seg[2] = cSEGMENT2(cPOINT2(1.5, 1.5), cPOINT2(0.5, 1.5));
    seg[3] = cSEGMENT2(cPOINT2(0.5, 1.5), cPOINT2(0.5, 0.5));
  }

  VOID Contains(INT id, const cBOX2 &box, BOOL partitions[4])
  {
    for(INT i = 0; i < 4; i++ )
      if(DoIntersect(PartitionBound(box, i), seg[id]) == SUCCESS)
	partitions[i] = true;

  }

  cBOX2 PartitionBound(const cBOX2 &box, INT i)
  {
    ASSERT(0 <= i && i < 4);
    const cPOINT2& min = box.Min();
    const cPOINT2& max = box.Max();

    cVECTOR2 vec = (box.Max() - box.Min()) * 0.5;

    INT localIndex[2] = {0, 0};
    localIndex[1] = i%2; localIndex[0] = i/2;

    cPOINT2 localMin(min[0] + vec[0]*localIndex[0],
		     min[1] + vec[1]*localIndex[1]);

    cBOX2 localBound(localMin, localMin + vec);
    return cBOX2(localMin, localMin + vec);
  }

private:
  cSEGMENT2 seg[4];
  cBOX2 m_bounds;
};

void test_quad_tree()
{
  typedef tQUAD_TREE<cGEOM_2D_KERNEL, cGEOM_MGR, INT> cQUAD_TREE;
  cGEOM_MGR mgr;

  cQUAD_TREE tree(cBOX2(cPOINT2(0.0, 0.0),
			cPOINT2(2.0, 2.0)), &mgr);

  tree.Insert(0); tree.Insert(1); tree.Insert(2); tree.Insert(3);

  cQUAD_TREE::leaf_iterator currLeaf = tree.Begin();
  cQUAD_TREE::leaf_iterator lastLeaf = tree.End();

  for( ; currLeaf != lastLeaf ; currLeaf++ ) {
    printf("\nLeaf.....\n");
    cQUAD_TREE::leaf_element_iterator currElem = currLeaf->begin();
    cQUAD_TREE::leaf_element_iterator lastElem = currLeaf->end();

    for( ; currElem != lastElem ; currElem++ ) {
      printf("%d ", *currElem);
    }

    printf("\n");
  }

  tree.Delete(0);

  currLeaf = tree.Begin();
  lastLeaf = tree.End();

  for( ; currLeaf != lastLeaf ; currLeaf++ ) {
    printf("\nLeaf.....\n");
    cQUAD_TREE::leaf_element_iterator currElem = currLeaf->begin();
    cQUAD_TREE::leaf_element_iterator lastElem = currLeaf->end();

    for( ; currElem != lastElem ; currElem++ ) {
      printf("%d ", *currElem);
    }

    printf("\n");
  }

  tree.Delete(1);

  currLeaf = tree.Begin();
  lastLeaf = tree.End();

  for( ; currLeaf != lastLeaf ; currLeaf++ ) {
    printf("\nLeaf.....\n");
    cQUAD_TREE::leaf_element_iterator currElem = currLeaf->begin();
    cQUAD_TREE::leaf_element_iterator lastElem = currLeaf->end();

    for( ; currElem != lastElem ; currElem++ ) {
      printf("%d ", *currElem);
    }

    printf("\n");
  }

}

void quad_euler_operator()
{
  cSURFACE_MESH mesh;

  mesh.NewVertex(0, 0, 0);
  mesh.NewVertex(0, 0, 0);
  mesh.NewVertex(0, 0, 0);
  mesh.NewVertex(0, 0, 0);

  mesh.NewQuad(0, 1, 2, 3);

  mesh.PrintFacets();

  mesh.InsertDiagonal(0, 0, 2);

  mesh.PrintFacets();

  mesh.RemoveDiagonal(0, 2, 0, 1, 1);

  mesh.PrintFacets();

  mesh.NewVertex(0, 0, 0);
  mesh.SplitEdge(4, 3, 0);

  mesh.PrintFacets();

  mesh.MergeEdgesAtVertex(4);

  mesh.PrintFacets();
}
void add_triangle2()
{
  cSURFACE_MESH mesh;

  mesh.NewVertex(0, 0, 0);
  mesh.NewVertex(0, 0, 0);
  mesh.NewVertex(0, 0, 0);
  mesh.NewVertex(0, 0, 0);

  mesh.NewTriangle(0, 1, 2);

  mesh.NewTriangle(2, 1, 3);

  mesh.PrintFacets();
  std::cout<<mesh.NumVertices()<<std::endl;
  std::cout<<mesh.NumFacets()<<std::endl;
}

void print_manifolds(cSURFACE_MESH &mesh)
{
  cSURFACE_MESH::manifold_iterator currManifold = mesh.ManifoldsBegin();
  cSURFACE_MESH::manifold_iterator lastManifold = mesh.ManifoldsEnd();

  for ( ; currManifold != lastManifold ; currManifold++ ) {
    printf("Manifold %d : ", currManifold->Index());
    cSURFACE_MESH::cMANIFOLD::facet_iterator currFacet = currManifold->FacetsBegin();
    cSURFACE_MESH::cMANIFOLD::facet_iterator lastFacet = currManifold->FacetsEnd();

    for( ; currFacet != lastFacet ; currFacet++ )
      printf("f%d ", currFacet->Index());

    printf("\n");
  }

}

void build_shells1()
{
  cSURFACE_MESH mesh;

  mesh.NewVertex(0, 0, 0);
  mesh.NewVertex(0, 0, 0);
  mesh.NewVertex(0, 0, 0);
  mesh.NewVertex(0, 0, 0);

  mesh.NewTriangle(0, 1, 2);

  mesh.NewTriangle(2, 1, 3);

  mesh.ConstructManifolds();
  print_manifolds(mesh);
}

void build_shells2()
{
  cSURFACE_MESH mesh;

  mesh.NewVertex(0, 0, 0);
  mesh.NewVertex(0, 0, 0);
  mesh.NewVertex(0, 0, 0);

  mesh.NewVertex(0, 0, 0);
  mesh.NewVertex(0, 0, 0);
  mesh.NewVertex(0, 0, 0);

  mesh.NewTriangle(0, 1, 2);

  mesh.NewTriangle(3, 4, 5);

  mesh.ConstructManifolds();
  mesh.DeleteManifold(0);
  print_manifolds(mesh);
}

void build_shells3()
{
  cSURFACE_MESH mesh;

  mesh.NewVertex(0, 0, 0);
  mesh.NewVertex(1, 0, 0);
  mesh.NewVertex(1, 1, 0);
  mesh.NewVertex(0, 1, 0);

  mesh.NewVertex(0, 0, 1);

  mesh.NewTriangle(0, 1, 3);
  mesh.NewTriangle(1, 2, 3);
  mesh.NewTriangle(1, 3, 4);

  mesh.ConstructManifolds();
  print_manifolds(mesh);
}

void test_rect_grid()
{
  cPOINT3 p0(0,0,0);
  cPOINT3 p1(10.0,10.0,10.0);
  cBOX3 box(p0,p1);

  INT numCells[3] = {2, 2, 2};
  cRECTILINEAR_GRID grid(box, numCells);
  grid.Walk();

  for(INT i = 0; i < numCells[0] ; i++ ) {
    for(INT j = 0; j < numCells[1] ; j++ ) {
      for(INT k = 0; k < numCells[2] ; k++ ) {
	iCELL_INDEX index(i, j, k);
	index.Print();
	printf("Color = %d\n\n", grid.CellColor(index));
      }
    }
  }

  for(INT i = 0; i <= numCells[0] ; i++ ) {
    for(INT j = 0; j <= numCells[1] ; j++ ) {
      for(INT k = 0; k <= numCells[2] ; k++ ) {
	iCELL_INDEX index(i, j, k);
	index.Print();
	cPOINT3 point = grid.VertexPoint(index);
	point.Print();
      }
    }
  }
}

VOID test_octree()
{
//   cSURFACE_MESH boxMesh;
//   cBOX3 box(cPOINT3(0.0, 0.0, 0.0),
//             cPOINT3(1.0, 1.0, 1.0));

//   cBOX3 octreeBox(cPOINT3(-0.1, -0.1, -0.1),
//                   cPOINT3(1.1, 1.1, 1.1));

//   INT numCells[3] = {100, 100, 100};

//   cRECTILINEAR_GRID grid(box, numCells);


//   for(INT i = 0 ; i <= grid.NumCells(GEOM_X) ; i++ ) {
//     for(INT j = 0 ; j <= grid.NumCells(GEOM_Y) ; j++ ) {
//       for(INT k = 0 ; k <= grid.NumCells(GEOM_Z) ; k++ ) {
// 	iGRID_VERTEX index(i, j, k);
// 	boxMesh.NewVertex(grid.VertexPoint(index));
//       }
//     }
//   }

//   tMESH_VERTICES_OCTREE<cSURFACE_MESH> octree(octreeBox, &boxMesh);

//   time_t beforeInsertion = clock();
//   for(INT i = 0 ; i < boxMesh.NumVertices() ; i++ ) {
//     octree.Insert(i);
//   }
//   time_t afterInsertion = clock();

//   printf("Insertion time = %lf secs\n", ((double)(afterInsertion - beforeInsertion))/CLOCKS_PER_SEC);

//   std::vector<iVERTEX> matches;

//   time_t beforeSearch = clock();

//   for(INT j = 0; j < boxMesh.NumVertices(); j++) {
//     matches.clear();

//     octree.FindMatches(boxMesh.Vertex(j)->Point(), matches);

//     ASSERT(matches.size() == 1);
//     ASSERT(j == matches[0]);
//   }

//   time_t afterSearch = clock();

//   printf("Search time = %lf secs\n",((double)(afterSearch - beforeSearch))/CLOCKS_PER_SEC);
}

// int main() {
//   test_octree();
// }

VOID test_off_import(char *filename)
{
  cCUT_CELL m_cell;
  ImportOff<cCUT_CELL>(filename, m_cell);

  m_cell.Close();
  sFACET_FILTER filter;

  ExportToOff<cCUT_CELL, sFACET_FILTER>("cell_after_import.off", m_cell, filter);
}

// int main(int argc, char **argv)
// {
//   test_off_import(argv[1]);
// }

int export_to_openfoam(cVOLUMETRIC_GRID &grid, cSURFACE_MESH *bfacesMesh,
		char *filename, char *file_path, cBOB *bob = NULL)
{
  std::string filePath(file_path), fileName(filename);

  std::string cmd1 = "mkdir  " + filePath + "/system";
  system(cmd1.c_str());

  std::string controlFileName = filePath + "/system/controlDict";

  FILE *controlFile = fopen(controlFileName.c_str(), "wb+");
  fprintf(controlFile, "/*--------------------------------*- C++ -*----------------------------------*\\ \n");
  fprintf(controlFile, "| =========                 |                                                           |\n");
  fprintf(controlFile, "| \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox    |\n");
  fprintf(controlFile, "|  \\    /   O peration     | Version:  1.5.x                                            |\n");
  fprintf(controlFile, "|   \\  /    A nd           | Web:      http://www.OpenFOAM.org               |\n");
  fprintf(controlFile, "|    \\/     M anipulation  |                                                                  |\n");
  fprintf(controlFile, " \\*-----------------------------------------------------------------------------*/ \n");
  fprintf(controlFile, "FoamFile\n");
  fprintf(controlFile, "{\n");
  fprintf(controlFile, "version     2.0;\n");
  fprintf(controlFile, "format      ascii;\n");
  fprintf(controlFile, "class      dictionary;\n");
  fprintf(controlFile, "object      controlDict;\n");
  fprintf(controlFile,"}\n");
  fprintf(controlFile, "// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //\n");
  fprintf(controlFile, "application simpleFoam;\n\n");
  fprintf(controlFile, "startFrom       startTime;\n\n");
  fprintf(controlFile, "startTime       0;\n\n");
  fprintf(controlFile, "stopAt          endTime;\n\n");
  fprintf(controlFile, "endTime         200;\n\n");
  fprintf(controlFile, "deltaT          1;\n\n");
  fprintf(controlFile, "writeControl    timeStep;\n\n");
  fprintf(controlFile, "writeInterval   50;\n\n");
  fprintf(controlFile, "purgeWrite      0;\n\n");
  fprintf(controlFile, "writeFormat     ascii;\n\n");
  fprintf(controlFile, "writePrecision  6;\n\n");
  fprintf(controlFile, "writeCompression uncompressed;\n\n");
  fprintf(controlFile, "timeFormat      general;\n\n");
  fprintf(controlFile, "timePrecision   6;\n\n");
  fprintf(controlFile, "runTimeModifiable yes;\n\n");
  fclose(controlFile);

  std::string cmd2 = "mkdir  " + filePath + "/constant";
  system(cmd2.c_str());

  std::string cmd3 = cmd2 + "/polyMesh";
  system(cmd3.c_str());

  std::string cmd4 = cmd2 + "/triSurface";
  system(cmd4.c_str());

  std::string cmd5 = "cp " + fileName + "  " +  filePath + "/constant/triSurface/";
  system(cmd5.c_str());

  std::string cmd6 = "mkdir  " + filePath + "/0";
  system(cmd6.c_str());

  agrid_boolean::ExportGridToFoam(filePath, grid, bob);

  std::string cmd7 = "cp -R " + filePath+"/constant/polyMesh " + filePath + "/0";
  system(cmd7.c_str());

  printf("Exporting OpenFOAM files complete....\n");
  return 0;
}

cBOB generate_bob(cBOX3 &simVol, cBOX3 &meshBoundingBox, INT numCells[3])
{
	cPOINT3i externalMinPoint(0, 0, 0);
	cPOINT3i externalMaxPoint(numCells);
	cBOX3i externalBox(externalMinPoint, externalMaxPoint);
	INT internalMinPoint[3], internalMaxPoint[3];
	//now compute the internal box as the smallest integer box surrounding the mesh box
	DO_COORDS(coord){
	  REAL u = (meshBoundingBox.MinCoord(coord) - simVol.MinCoord(coord))/
					  (simVol.MaxCoord(coord) - simVol.MinCoord(coord))*numCells[coord];
	  internalMinPoint[coord] = (INT)(u - cLIMITS::Tolerance()) - 1;
	  if (internalMinPoint[coord] < externalMinPoint[coord])
		  internalMinPoint[coord] = externalMinPoint[coord];
	  REAL v = (meshBoundingBox.MaxCoord(coord) - simVol.MinCoord(coord))/
					  (simVol.MaxCoord(coord) - simVol.MinCoord(coord))*numCells[coord];
	  internalMaxPoint[coord] = (INT)(v + 1 + cLIMITS::Tolerance()) + 1;
	  if (internalMaxPoint[coord] > externalMaxPoint[coord])
		  internalMaxPoint[coord] = externalMaxPoint[coord];
	}
	cBOX3i internalBox(internalMinPoint, internalMaxPoint);
	cBOB bob(externalBox, internalBox,  tightSimVol);
	bob.RegisterSimVol(simVol);
	if (tightSimVol){
	  printf("Actual Tight SimVol: ");
	  bob.SimVol().Print();
	}
	return bob;
}

BOOL agrid_test(cBOX3 &simVol, INT numCells[3],
		std::vector<cSURFACE_MESH*> &meshes,
		cCELL_OFF_WRITER &cellWriter,
		char *file_name, char *file_path)
{
	cBOX3 meshBoundingBox = (*(meshes[0])).BoundingBox();
//	meshBoundingBox.Print();
	cBOB bob = generate_bob(simVol, meshBoundingBox, numCells);
	cBOX3 grayGridVol = bob.GrayGridVol();
	printf("grayGridVol: "); grayGridVol.Print();
	INT numGrayGridCells[3];
	bob.NumGrayGridCells(numGrayGridCells);
	printf("numGrayCells = %d %d %d\n", numGrayGridCells[0], numGrayGridCells[1], numGrayGridCells[2]);
	cCUT_CELLS_GENERATOR generator(grayGridVol, numGrayGridCells);

   for(INT i = 0; i < (INT)meshes.size(); i++) {
     FILE *file = fopen("mesh_before_clipping.off", "wb+");
     ExportToOff(file, *(meshes[i]));
     fclose(file);

       printf("Registering mesh %d \n", i);
     generator.Register(meshes[i]);
     cellWriter.ExportMeshInsideCellToOff(*(meshes[i]));
     cellWriter.MergeGridMeshFiles();
   }

   if (!cellWriter.IsEmpty())
	   return true; //this task of generating off files is completed.
   //   generator.Print();
   //   if ( printoutLevel < 2){
     printf("Before building cut-cells...\n");
     FILE *file = fopen("mesh1.off", "wb+");
     ExportToOff(file, *(meshes[0]));
     fclose(file);
     //   }
   generator.Generate();

	  file = fopen("mesh2.off", "wb+");
	 ExportToOff(file, *(meshes[0]));
	 fclose(file);
	 printf("After building cut-cells...\n");


   //generator.Export();
   cVOLUMETRIC_GRID grid(grayGridVol);

   printf("Building volumetric grid...\n");
   BOOL rv = generator.BuildGrid(&grid); //&bfacesMesh);

   sVOLUMETRIC_GRID_BOUNDARY_FILTER filter(&grid);
   tmesh::ExportToOff<cVOLUMETRIC_GRID,
    sVOLUMETRIC_GRID_BOUNDARY_FILTER>("bfacesMesh.off", grid, filter);

    ASSERT(grid.Verify());
#ifndef DONT_EXPORT_TO_OF
  export_to_openfoam(grid, NULL, file_name, file_path, &bob);
#endif //EXPORT_TO_OFF
  return rv;
}


// int agrid_test_box1()
// {
//   cSURFACE_MESH mesh;//, mesh1  ;
//   cBOX3 box(cPOINT3(1.5, 1.5, 1.5),
//             cPOINT3(8.5, 8.5, 8.5));

// //     box1(cPOINT3(11.5, 11.5, 11.5),
// // 	 cPOINT3(18.5, 18.5, 18.5));

//   BoxToMesh(box, mesh);
//   mesh.ConstructManifolds();


// //   BoxToMesh(box1, mesh1);
// //   mesh1.ConstructManifolds();

//   cBOX3 simVol(cPOINT3(0.0, 0.0, 0.0),
//                cPOINT3(10.0, 10.0, 10.0));

//   simVol.Print();

//   INT numCells[3] = {10, 10, 10};

//   std::vector<cSURFACE_MESH*> meshes;
//   meshes.push_back(&mesh);
//   //meshes.push_back(&mesh1);

//   agrid_test(simVol, numCells, meshes);

//   return 0;
// }

// int main()
// {
//   cSURFACE_MESH mesh;
//   cBOX3 simVol(cPOINT3(0, 0, 0), cPOINT3(3, 3, 3));

//   cBOX3 box1(cPOINT3(1, 0, 1.5), cPOINT3(2, 3, 2.5));
//   BoxToMesh(box1, mesh);

//   mesh.ConstructManifolds();

//   INT numCells[3] = {3, 1, 1};

//   std::vector<cSURFACE_MESH*> meshes;
//   meshes.push_back(&mesh);
//   //meshes.push_back(&mesh1);

//   agrid_test(simVol, numCells, meshes, "straightbox.stl", "prototest");

//   return 0;
// }


// int main()
// {
//   cSURFACE_MESH mesh;
// //   mesh.NewVertex(1, 0, 1.5);
// //   mesh.NewVertex(2, 0, 1.5);
// //   mesh.NewVertex(2, 3, 1.5);
// //   mesh.NewVertex(1, 3, 1.5);

// //   mesh.NewQuad(0, 1, 2, 3);
//   cBOX3 simVol(cPOINT3(0, 0, 0), cPOINT3(3, 3, 3));
//   //  BoxToMesh(simVol, mesh);

//   cBOX3 box1(cPOINT3(1, 0, 1.5), cPOINT3(2, 3, 2.5));
//   BoxToMesh(box1, mesh);

// //    sFACET_FILTER filter;
// //    tmesh::ExportToOff<cSURFACE_MESH,
// //     sFACET_FILTER>("boxes.off", mesh, filter);

//   mesh.ConstructManifolds();

//   INT numCells[3] = {3, 1, 1};

//   std::vector<cSURFACE_MESH*> meshes;
//   meshes.push_back(&mesh);
//   //meshes.push_back(&mesh1);

//   agrid_test(simVol, numCells, meshes, "straightbox.stl", "prototest");

//   return 0;
// }

FILE *fopenIndexed(char *c, char *extension, char *accessMode)
{
  char fileName[128];
  fileName[0] = '\0';
  sprintf(fileName, "%s.%s", c, extension);
  return fopen(fileName, accessMode);
}
VOID merge_off_map_files(char *c1, char *c2)
{
  FILE *offFile1 = fopenIndexed(c1, "grid.off", "rb");
  FILE *offFile2 = fopenIndexed(c2, "grid.off", "rb");
  FILE *mapFile1 = fopenIndexed(c1, "grid.map", "rb");
  FILE *mapFile2 = fopenIndexed(c2, "grid.map", "rb");

  cOFF_MERGER merger(offFile1, mapFile1);
  merger.Append(offFile2, mapFile2);
  merger.Import();
  fclose(offFile1);
  fclose(offFile2);
  fclose(mapFile1);
  fclose(mapFile2);

  char mergedName[128];
  sprintf(mergedName, "%s.%s", c1, c2);
  FILE *offExport = fopenIndexed(mergedName, "grid.off", "wb+");
  FILE *mapExport = fopenIndexed(mergedName, "grid.map", "wb+");
  merger.ExportFiles(offExport, mapExport);
  merger.Export();

  fclose(offExport);
  fclose(mapExport);
}

VOID CreateHole(cSURFACE_MESH &mesh)
{
//  FILE *mesh_file = fopen("mesh.txt", "wb");
//  mesh.PrintFacets(mesh_file);
//  fclose(mesh_file);
//  mesh_file = NULL;

  //using rand, remove a random triangle, and then triangles adjacent to the hole.
  printf("initial mesh contains %d facets\n",  mesh.NumFacets());
  INT numTriToRemove = (INT) mesh.NumFacets() /151.0;
//  printf("RAND_MAX = %ld\n", RAND_MAX);
  REAL c = ((REAL)mesh.NumFacets()) / ((REAL)RAND_MAX);

  srand(13);
  REAL r0 = (REAL)rand();

  iFACET f0 = (INT) (r0 * c);
//  printf("fo = %d\n", f0);
  cSURFACE_MESH::cFACET::half_edge_circulator currHe = mesh.Facet(f0)->HalfEdgesBegin();
  cSURFACE_MESH::cFACET:: half_edge_circulator lastHe = mesh.Facet(f0)->HalfEdgesEnd();

  std::vector<iFACET> borderFacets;
  for (; currHe != lastHe; currHe++){
	iFACET bf = currHe->Opp()->Facet()->Index();
	borderFacets.push_back(bf);
  }
  mesh.DeleteFacet(f0);
  INT numTriRemoved = 1;

  while (numTriRemoved < numTriToRemove){
//	printf("numTriRemoved = %d\n", numTriRemoved);
	REAL r = (REAL)rand();
	c =  ((REAL)borderFacets.size()-1) / ((REAL)RAND_MAX);
	INT index = (INT) ( r * c );
	if (index >= borderFacets.size())
	  index = borderFacets.size() - 1;
	iFACET f = borderFacets[index];
//	printf("index %d; f = %d\n", index, f);
	borderFacets.erase(borderFacets.begin()+index);

	if (!mesh.Facet(f)->IsDeleted()){

	  cSURFACE_MESH::cFACET::half_edge_circulator currHe = mesh.Facet(f)->HalfEdgesBegin();
	  cSURFACE_MESH::cFACET:: half_edge_circulator lastHe = mesh.Facet(f)->HalfEdgesEnd();

	  for (; currHe != lastHe; currHe++){
		cSURFACE_MESH::cFACET *f_opp = currHe->Opp()->Facet();
		if (f_opp != NULL){
		  iFACET bf = f_opp->Index();
		  borderFacets.push_back(bf);
		}
	  }
	  mesh.DeleteFacet(f);
	  numTriRemoved++;
//	  mesh.Verify();
	}
//	printf("bf %d: ", borderFacets.size());
//	for (int i=0; i < borderFacets.size(); i++)
//	  printf("%d ", borderFacets[i]);
//	printf("\n");
  }
  printf("mesh with holes contains %d facets\n",  mesh.NumFacets());
}


BOOL read_vertices(char* file_path)
{
  char *vertex_test_file_name = "vertex.test";

  FILE *vertex_test_file = fopen(vertex_test_file_name, "rb+");
  if (vertex_test_file == NULL)
	return false;

  cFOAM_READER foamReader(file_path);

  foamReader.ReadVertexTable(vertex_test_file);
  fclose(vertex_test_file);
  vertex_test_file = NULL;
  foamReader.ExportFacetsContainingVertices();

  return true;
}

BOOL read_facets(char* file_path)
{
  char *facet_test_file_name = "facet.test";

  FILE *facet_test_file = fopen(facet_test_file_name, "rb+");
  if (facet_test_file == NULL)
	return false;

  cFOAM_READER foamReader(file_path);

  foamReader.ReadFacetTable(facet_test_file);
  fclose(facet_test_file);
  facet_test_file = NULL;
  foamReader.ExportCellsContainingFacets();
  return true;
}

//generating and merging off and map files is described in a document "Generating off files"
//located in this directory
//int main(int argc, char **argv)
//{
//  REAL boxCoords[6];
//  INT nCells[3];
//
//  printoutLevel = 2;
//  char *file_name = NULL, *file_ops = NULL, *file_path = NULL;
//
////  file_name = argv[1]; file_ops = argv[2]; file_path = argv[3];
//
//  file_name = "data.stl"; //YG - temp
//  file_ops = "data.in"; //YG - temp
//  file_path = "output"; //YG - temp
//
//
///*
//  read_vertices(file_path);
//  read_facets(file_path);
//*/
//  char *cell_test_file_name = "cell.test";
//
//  FILE *cell_test_file = fopen(cell_test_file_name, "rb+");
//  cFOAM_READER foamReader("file_path");
//  cCELL_OFF_WRITER cellWriter(&foamReader);
//
//  if (cell_test_file != NULL){
//	foamReader.Import();
//
//	while (cell_test_file != NULL){
//	  INT cellIndex = -1;
//	  if (fscanf(cell_test_file, "%d", &cellIndex) == EOF){
//		fclose(cell_test_file);
//		cell_test_file = NULL;
//	  }
//	  else
//		cellWriter.AddCellIndex(cellIndex);
////	  io::GenerateCellOffFiles(cellIndex, file_path, &cellIndexVector, &cellBoxVector);
//	}
//	cellWriter.WriteOffFiles();
//  }
//
//  const char *merge_file_name = "merge.test";
//  FILE *merge_file = fopen(merge_file_name, "rb+");
//  if (merge_file != NULL){
//	char b1[128], b2[128];
//	fscanf(merge_file, "%s %s", b1, b2);
//	merge_off_map_files(b1, b2);
//	fclose(merge_file);
//	printf("Created Merged Files\n");
//	return 0;
//  }
//
//  cSURFACE_MESH mesh;
//  FILE *file =  fopen(file_name, "rb+");
//  ImportStl(file, mesh);
//  fclose(file);
//  file = NULL;
//  if (mesh.NumFacets() == 0){
//	printf("ERROR: Empty Mesh\n");
//	return 1;
//  }
//
//  printf("Input Mesh Bounding Box:\n");
//  mesh.BoundingBox().Print();
//  if (shadeAreaOnly){
//	DO_COORDS(c){
//	  REAL shadeArea = mesh.ShadeArea(c);
//	  printf("coord %c: shade area %lf\n", coordChar(c), shadeArea);
//	}
//	return 0;
//  }
//// testing for hole closure
///*  CreateHole(mesh);
//  std::cout<<"Filling hole...\n";
//  cHOLE_FILLER holeFiller(&mesh);
//  return holeFiller.CloseHoles();
//*/
//
//  FILE *mtfile = fopen("mesh_init.txt", "wb");
//  mesh.Print(mtfile);
//  fclose(mtfile);
//  mtfile = NULL;
//
//  FILE *off_file = fopen("mesh_init.off", "wb+");
//  ExportToOff(off_file, mesh);
//  fclose(off_file);
//
//  std::cout<<"Merging coplanar facets...\n";
//  cFACET_MERGER facetMerger(&mesh);
//  facetMerger.MergeCoplanarFacets();
//  if (!mesh.Verify()){
//	  printf("mesh verify failed\n");
//	  mesh.Verify();
//  }
//
//  off_file = fopen("mesh_merged_facets.off", "wb+");
//  ExportToOff(off_file, mesh);
//  fclose(off_file);
//  mtfile = fopen("mesh_merged_facets.txt", "wb");
//  mesh.Print(mtfile);
//  fclose(mtfile);
//  mtfile = NULL;
//
//  std::cout<<"Building manifolds...\n";
//  mesh.ConstructManifolds();
//
//  printf("Mesh contains %d manifolds\n", mesh.NumManifolds());
//  mtfile = fopen("mesh_init-Manifolds.txt", "wb");
//  mesh.Print(mtfile);
//  fclose(mtfile);
//  mtfile = NULL;
//
//  printf("mesh has %d border edges\n", mesh.NumBorderEdges());
//   mtfile = fopen("mesh_borderedges_before.txt", "wb");
//   mesh.PrintBorderEdges(mtfile);
//   fclose(mtfile);
//   mtfile = NULL;
///*
//   cCYLINDER_BUILDER cylinderBuilder(&mesh);
//   cylinderBuilder.RecomputeAll();
//   off_file = fopen("mesh_cylinders_rebuilt.off", "wb+");
//   ExportToOff(off_file, mesh);
//   fclose(off_file);
//*/
//
///* for oxy temporarily
//   cHOLE_CLOSER holeCLoser(&mesh, INVALID_IMANIFOLD);
//   holeCLoser.CloseHoles();
//   printf("after closing simple holes mesh has %d border edges\n", mesh.NumBorderEdges());
//*/
//   /*
//   cHOLE_FILLER holeFiller(&mesh);
//   holeFiller.Perform();
//*/
//   for (iMANIFOLD m = 0; m < mesh.NumManifolds(); m++){
//	 cSURFACE_MESH::cMANIFOLD *manifold = mesh.Manifold(m);
//	 char mfname[24];
//	 mfname[0] = '\0';
//	 if (manifold->IsOpen())
//	  sprintf(mfname, "manifold%dopen.off", m);
//	 else
//	   sprintf(mfname, "manifold%dclosed.off", m);
//	 ExportManifoldToOff(mfname, mesh, manifold->Index());
//	 printf("Manifold %d:\tNF %d\t", m, manifold->NumFacets());
//	 if (manifold->IsOpen()){
//      	mfname[0] = '\0';
//      	sprintf(mfname, "manifold%dborder-before.off", m);
//      	ExportManifoldBorderFacetsToOff(mfname, mesh, manifold->Index());
//        printf("Is Open. NBE %d\n", manifold->NumBorderEdges());
////        cHOLE_CLOSER holeCLoser(&mesh, manifold->Index());
////        holeCLoser.CloseHoles();
////      	mfname[0] = '\0';
////      	sprintf(mfname, "manifold%dborder-after.off", m);
////      	ExportManifoldBorderFacetsToOff(mfname, mesh, manifold->Index());
//
//
////        mtfile = fopen("mesh_borderedges_after.txt", "wb");
////         mesh.PrintBorderEdges(mtfile);
////         fclose(mtfile);
////         mtfile = NULL;
////         printf("Mesh contains %d manifolds\n", mesh.NumManifolds());
////         mtfile = fopen("mesh_after-Manifolds.txt", "wb");
////          mesh.Print(mtfile);
////          fclose(mtfile);
////          mtfile = NULL;
//      }
//      else{
//        printf("Closed; \n");
//  //      if (manifold->NumFacets() >= maxManifoldNF){
//  //    	maxManifoldNF = manifold->NumFacets();
//  //    	maxManifoldLabel = m;
//  //      }
//      }
//     }
//
//   //Called again to recompute the number of border edges
//    //Manifolds are constructed three times, it needs to be reduced to at least 2.
//    mesh.ConstructManifolds();
////    mesh.DeleteManifold(0);
///*  mesh.PrintBorderEdges();*/
//  //INT maxManifoldNF = 0;
////  iMANIFOLD maxManifoldLabel = -1;
//
//  for (iMANIFOLD m = 0; m < mesh.NumManifolds(); m++){
//    cSURFACE_MESH::cMANIFOLD *manifold = mesh.Manifold(m);
////    printf("Manifold %d:\tNF %d\t", m, manifold->NumFacets());
//    if (manifold->IsOpen()){
////      printf("Is Open. NBE %d\n", manifold->NumBorderEdges());
//      mesh.DeleteManifold(m);
//    }
//    else{
////      printf("Closed; \n");
////      if (manifold->NumFacets() >= maxManifoldNF){
////    	maxManifoldNF = manifold->NumFacets();
////    	maxManifoldLabel = m;
////      }
//    }
//   }
//  std::cout<<"Done building manifolds...\n";
//
//  //Called again so that the indexation of manifolds is
//  //from 0 - n-1.
//  mesh.ConstructManifolds();
//  printf("Remaining Mesh Bounding Box:\n");
//  mesh.BoundingBox().Print();
//
//  printf("Mesh has %d closed manifolds\n", mesh.NumManifolds());
//  //sort the manifolds by the increasing number of facets
//  std::vector <iMANIFOLD> manifoldIndex;
//  manifoldIndex.reserve(mesh.NumManifolds());
//  for (iMANIFOLD m = 0; m < mesh.NumManifolds(); m++){
//	  manifoldIndex[m] = m;
//  }
//
//  //sort manifolds by size
//  BOOL swapped = false;
//
//  do {
//    swapped = false;
//    for (INT i=1; i < mesh.NumManifolds(); i++){
//	  cSURFACE_MESH::cMANIFOLD *manifold0 = mesh.Manifold(manifoldIndex[i]);
//	  cSURFACE_MESH::cMANIFOLD *manifold1 = mesh.Manifold(manifoldIndex[i-1]);
//      if (manifold0->NumFacets() < manifold1->NumFacets()){
//        INT mm = manifoldIndex[i];
//        manifoldIndex[i] = manifoldIndex[i-1];
//        manifoldIndex[i-1] = mm;
//        swapped = true;
//      }
//    }
//  } while (swapped);
///*
//  for (INT i=0; i < mesh.NumManifolds(); i++){
//	cSURFACE_MESH::cMANIFOLD *manifold = mesh.Manifold(manifoldIndex[i]);
//	ASSERT(manifold->Index() == manifoldIndex[i]);
//	printf("%d\tm %d\tnf %d\n",
//			i, manifoldIndex[i], manifold->NumFacets());
//  }
//*/
//
//  FILE *datafile = fopen(file_ops, "rb");
//   if (datafile == NULL){
//     printf("no data file\n");
//     return (0);
//   }
//
//   fscanf(datafile, "%lf %lf %lf %lf %lf %lf %d %d %d",
// 	 boxCoords, boxCoords+1, boxCoords+2,
// 	 boxCoords+3, boxCoords+4, boxCoords+5,
// 	 nCells, nCells+1, nCells+2);
//
//   char buffer[20] = {'\0'};
//   fscanf(datafile, "%s", buffer);
//   fclose(datafile);
//   datafile = NULL;
//
//  cBOX3 simVol(cPOINT3(boxCoords[0], boxCoords[1], boxCoords[2]),
//	       cPOINT3(boxCoords[3], boxCoords[4], boxCoords[5]));
//
//  printf("SimVol:\n");
//  simVol.Print();
//
//  if (!simVol.ContainsBox(mesh.BoundingBox())){
//    printf("ERROR: SimVol does not contain the mesh\n");
//    return 1;
//  }
//
//  std::vector<cSURFACE_MESH*> meshes;
//  meshes.push_back(&mesh);
//  agrid_test(simVol, nCells, meshes, cellWriter, file_name, file_path);
//  printf("SUCCESSFUL EXIT\n");
//  return 0;
//
////  printf("nCells: %d %d %d\n", nCells[0], nCells[1], nCells[2]);
//  //try individual manifolds and pairs first
////  printf("generating Grid for Single Manifolds...\n");
//  std::vector <iMANIFOLD> manifolds;
///*
//  for (INT i = 0; i < mesh.NumManifolds(); i++){
////	  for (INT i = 65; i < 66; i++){
//	cSURFACE_MESH subMesh;
//	manifolds.clear();
//	manifolds.push_back(manifoldIndex[i]);
//	MeshFromManifolds(manifolds, mesh, subMesh);
//	INT NFInit = subMesh.NumFacets();
//	printf("count = %d\t; NF %d\n", i, NFInit);
//	std::vector<cSURFACE_MESH*> meshes;
//	meshes.push_back(&subMesh);
//	if (!agrid_test(simVol, nCells, meshes, cellWriter, file_name, file_path)){
//	  FILE *problems = fopen("problems.txt", "ab+");
//	  fprintf(problems, "count = %d\t; NF %d\n", i, NFInit);
//	  fclose(problems);
//	}
//  }
//*/
//  INT i0 = 1, j0 = 32;
//  for (INT i = i0; i < i0+1; i++){
//	INT jmin = j0;
//	for (INT j = jmin; j < jmin+1; j++){
//		  //these manifolds have problems by themselves
//		cSURFACE_MESH subMesh;
//		manifolds.clear();
//		manifolds.push_back(manifoldIndex[i]);
//		manifolds.push_back(manifoldIndex[j]);
//		MeshFromManifolds(manifolds, mesh, subMesh);
//		INT NFInit = subMesh.NumFacets();
//		printf("count = %d %d\t; NF %d\n", i, j, NFInit);
//		std::vector<cSURFACE_MESH*> meshes;
//		meshes.push_back(&subMesh);
//		if (!agrid_test(simVol, nCells, meshes, cellWriter, file_name, file_path)){
//		  FILE *problems = fopen("problems.txt", "ab+");
//		  fprintf(problems, "count = %d %d\t; NF %d\n", i, j, NFInit);
//		  fclose(problems);
//		}
//	}
//  }
//  printf("SUCCESSFUL EXIT\n");
//  return 0;
//}

int main(int argc, char **argv)
{	cAMOEBA_MODEL model;
	io::ImportAmoebaModel(argv[1], model);
}

