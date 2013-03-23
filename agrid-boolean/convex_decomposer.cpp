/*`	`			`
 * convex_decomposer.cpp
 *
 *  Created on: Dec 31, 2010
 */

#include "convex_decomposer.hpp"
#include "planar_cutter.hpp"

//#ifdef CONVEX_DECOMP
namespace agrid_boolean {

  cCONVEX_DECOMPOSER::cCONVEX_DECOMPOSER()
  {
    m_mesh = NULL;
    m_mostConcaveHE = NULL;
    m_debug = false;
  }

  cCONVEX_DECOMPOSER::cCONVEX_DECOMPOSER(std::vector<cCUT_CELL*> *cutCells)
  {
    m_cutCells = cutCells;
    m_currCutCell = m_cutCells->begin(); 
    m_mesh = NULL;
    m_mostConcaveHE = NULL;
    m_debug = false;
  }

  VOID cCONVEX_DECOMPOSER::Decompose()
  {
//    cGRAY_CELL_ENTRY::cut_cell_iterator lastCutCell = m_cellEntry->CutCellsEnd(); //CutCells().end();
    
    while(m_currCutCell != m_cutCells->end()) {
      cCUT_CELL *mesh = *m_currCutCell;
      if (DecomposeMesh(mesh)){
	if (Debug()){}
	//ExportToOff(*m_mesh, "splitmesh.off");
	//split the mesh into two by moving the facets on one side of the plane
	//into a new cut cell
      }
      else
	m_currCutCell++;
    }
  }

  //returns true if the mesh gas been decomposed
  BOOL cCONVEX_DECOMPOSER::DecomposeMesh(cCUT_CELL *mesh)
  {
	m_mesh = mesh;
	if (!FindConcaveEdge())
	  return false;
	BuildSplittingPlane();
	ClipMeshByPlane();
	return true;
  }

  BOOL cCONVEX_DECOMPOSER::FindConcaveEdge()
  {
	m_mostConcaveHE = m_mesh->MostConcaveEdge();
	if (Debug()){
	  printf("most concave he = %d %d\n",
			  m_mostConcaveHE->Tail()->Index(),
			  m_mostConcaveHE->Head()->Index());
	}
	return m_mostConcaveHE != NULL;
  }

  //clipping plane passes through the most concave half edge and is perpendicular to
  //one of the lattice planes.
  VOID cCONVEX_DECOMPOSER::BuildSplittingPlane()
  {
	assert(m_mostConcaveHE != NULL);
	cVECTOR3 planeNormal = m_mostConcaveHE->BisectorPlaneNormal();
	eCOORD minCoord = planeNormal.AbsMinCoord();
	cVECTOR3 heVector = m_mostConcaveHE->Vector();
	if (Debug())
		heVector.Print();
	//modify the plane normal so that it's still perpendicular to the edge, but
	//is also perpendicular to one of the coordinate planes.
	planeNormal[minCoord] = 0.0;
	planeNormal[CoordPlusOne(minCoord)] = heVector[CoordPlusTwo(minCoord)];
	planeNormal[CoordPlusTwo(minCoord)] = -heVector[CoordPlusOne(minCoord)];
	planeNormal = planeNormal.Normalize();

	cPOINT3 edgeMidPoint = m_mostConcaveHE->Segment().MidPoint();
	cPLANE3 splittingPlane(edgeMidPoint, planeNormal);
	m_splittingPlane = splittingPlane;
	if (Debug()){
		planeNormal.Print();
		cPOINT3 pt = m_mostConcaveHE->Tail()->Point();
		cPOINT3 ph = m_mostConcaveHE->Head()->Point();
		printf("distTail = %lg\n", m_splittingPlane.SignedDistance(pt));
		printf("dist Head = %lg\n", m_splittingPlane.SignedDistance(ph));
		printf("dot = %lg\n", m_splittingPlane.Normal()*m_mostConcaveHE->Segment().Vector());
	}
  }

  //compute closed loop starting at the concave he
  VOID cCONVEX_DECOMPOSER::ClipMeshByPlane()
  {
	cPLANAR_CUTTER cutter(m_mesh, m_mostConcaveHE, m_splittingPlane,  m_cutCells);
	cutter.Debug(false);
	cutter.SplitMesh();
  }

  VOID GenerateConvexPartitions(std::vector<cCUT_CELL*> &cutCells)
  {
	  cCONVEX_DECOMPOSER decomposer(&cutCells);
	  decomposer.Decompose();
  }

  VOID GenerateConvexPartitions(cGRAY_CELL_ENTRY *entry)
  {
	  std::vector<cCUT_CELL*> &cutCells = entry->CutCells();
	  GenerateConvexPartitions(cutCells);
  }

  VOID GenerateConvexPartitions(cGRAY_CELL *cell)
  {
	  if(cell->NumEntries() > 1) {
		  std::vector<cCUT_CELL*> &cutCells = cell->CutCells();
		  GenerateConvexPartitions(cutCells);
	  }
	  else {
		  cGRAY_CELL_ENTRY *entry = cell->begin().operator*();
		  GenerateConvexPartitions(entry);
	  }
  }

} //namespace
//#endif //CONVEX_DECOMP
