/*
 * cut_cell.hpp
 *
 *  Created on: Aug 19, 2010
 */

#ifndef _AGRID_MESH_CUT_CELL_HPP_
#define _AGRID_MESH_CUT_CELL_HPP_

#include "defs.hpp"

/*
 * Defining basic types for cut cell clay mesh.
 */
template <typename _Mesh_Entities>
class tCUT_CELL_VERTEX : public tmesh::tVERTEX<_Mesh_Entities> {
  typedef tmesh::tVERTEX<_Mesh_Entities> cBASE;

public:
  tCUT_CELL_VERTEX() : cBASE()
  {}

  tCUT_CELL_VERTEX(const cPOINT3 &point) : cBASE(point)
  {}

  ~tCUT_CELL_VERTEX() {}

  //For grid vertex mesh = NULL;
  VOID Source(INT srcIndex, cSURFACE_MESH *mesh = NULL) {
    m_srcIndex = srcIndex;
    m_srcMesh = mesh;
  }

  INT SourceIndex() {
    return m_srcIndex;
  }

  cSURFACE_MESH *SourceMesh() {
    return m_srcMesh;
  }

protected:
  INT m_srcIndex;
  cSURFACE_MESH *m_srcMesh; //For a grid vertex m_srcMesh = NULL;
};


template <typename _Mesh_Entities>
class tCUT_CELL_FACET : public tmesh::tFACET<_Mesh_Entities> {
  typedef tmesh::tFACET<_Mesh_Entities> cBASE;
public:
  tCUT_CELL_FACET() : cBASE(), m_srcFace(NULL),
		      m_srcMesh(NULL),
		      m_isFromInputSurface(false)
  {}

  ~tCUT_CELL_FACET()
  {}

  VOID SourceFacetGroup(cFACET_GROUP *face) {
    m_srcFace = face;
  }

  cFACET_GROUP *SourceFacetGroup() {
    return m_srcFace;
  }

  VOID SourceMesh(cSURFACE_MESH *mesh) {
    m_srcMesh = mesh;
  }

  VOID IsFromInputSurface(BOOL flag) {
    m_isFromInputSurface = flag;
  }

  cSURFACE_MESH *SourceMesh() {
    return m_srcMesh;
  }

  BOOL IsFromInputSurface() {
    return m_isFromInputSurface;
  }

protected:
  cFACET_GROUP* m_srcFace;

  //For grid faces this will be NULL.
  cSURFACE_MESH *m_srcMesh;

  BOOL m_isFromInputSurface;
};


template <typename _Mesh_Entities,
	  typename _Storage_Policy,
	  template <typename _ME, typename _SP> class _Object_Factory >
class tCUT_CELL : public tMESH < _Mesh_Entities,
				 _Storage_Policy,
				 _Object_Factory >
{
public:
  typedef tMESH < _Mesh_Entities, _Storage_Policy, _Object_Factory > cBASE;
  typedef tCUT_CELL< _Mesh_Entities, _Storage_Policy, _Object_Factory > cSELF;

  _TMESH_ENTITIES(cBASE);

public:
  tCUT_CELL() : cBASE()
  { }

  ~tCUT_CELL() { }

  cHALF_EDGE* BoundaryEdge() {
    typename cBASE::half_edge_iterator currHalfEdge = cBASE::HalfEdgesBegin();
    typename cBASE::half_edge_iterator lastHalfEdge = cBASE::HalfEdgesEnd();

    for( ; currHalfEdge != lastHalfEdge ; currHalfEdge++ ) {
      if(currHalfEdge->IsBorder())
	return currHalfEdge.operator->();
    }

    return NULL;
  }

  INT BoundaryLength() {
    cHALF_EDGE *boundaryEdge = BoundaryEdge();

    INT length = 0;
    if(boundaryEdge) {
      cHALF_EDGE *loopingHe = boundaryEdge;

      do {
	length++;
	loopingHe = loopingHe->Next();
      } while(boundaryEdge != loopingHe);
    }

    return length;
  }

  //Checks if the point is on one of the
  //facets of the cut-cell..
  BOOL IsOnBoundary(const cPOINT3 &point) {
    typename cBASE::facet_iterator currFacet = cBASE::FacetsBegin();
    typename cBASE::facet_iterator lastFacet = cBASE::FacetsEnd();

    for( ; currFacet != lastFacet ; currFacet++ )
      if(currFacet->Contains(point))
	return true;

    return false;
  }

  VOID DeleteAll() {
    INT i = 0, nFacets = cBASE::LargestFacetIndex();

    for( ; i < nFacets + 1 ; i++ )
      cBASE::DeleteFacet(i);
  }

  BOOL CloseHole(std::vector<iVERTEX> &cycleVertices) {
    //The orientation in the vector is reverse.
    //Close hole in the opposite order of vertices in cycleVertices.
    INT numCycleVertices = cycleVertices.size();
    
    cBASE::NewFacetStart();
    
    for( INT i = numCycleVertices - 1; i >= 0 ; i-- )
      cBASE::NewFacetAddVertex(cycleVertices[i]);
    
    iFACET newIndex = cBASE::NewFacetClose();
    
    if(newIndex == INVALID_IFACET)
      return false;
      
    return true;
  }

  VOID CollectCycleVertices(std::stack<cHALF_EDGE*> &boundaryHes,
			    iVERTEX terminalVtx,
			    BOOL checkTerminalVtx,
			    std::vector<iFACET> &cycleVertices)
  {
    do {
      cHALF_EDGE *topHe = boundaryHes.top();
      cycleVertices.push_back(topHe->Tail()->Index());
      
      if(checkTerminalVtx && topHe->Tail()->Index() == terminalVtx) {
	boundaryHes.pop();
	break;
      }

      boundaryHes.pop();
    } while(!boundaryHes.empty());
    
  }
  
  BOOL CloseHole(cHALF_EDGE *initHe)
  {
    std::stack<cHALF_EDGE*> boundaryHes;
    cHALF_EDGE* firstHe = initHe;
    BOOL visited[256];
    memset(visited, 0, sizeof(BOOL)*256);

    BOOL cyclesLeft = false;

    //Implementing cycle detection algorithm.
    do {
      cHALF_EDGE* loopHe = firstHe;

      BOOL foundCycle = false;
      iVERTEX cycleAtV = INVALID_IVERTEX;
      do {
	ASSERT(loopHe->IsBorder());
	iVERTEX v = loopHe->Tail()->Index();
	if(visited[v] == true) {
	  foundCycle = true;
	  cycleAtV = v;
	  break;
	}
	
	visited[v] = true;
	boundaryHes.push(loopHe);
	loopHe = loopHe->Next();
      } while(loopHe != firstHe);
      
      std::vector<iVERTEX> cycleVertices;      
      if(foundCycle) {
	CollectCycleVertices(boundaryHes, cycleAtV, 
			     foundCycle, cycleVertices);
	if(!CloseHole(cycleVertices))
	  return false;

	if(!boundaryHes.empty()) {
	  cyclesLeft = true;
	  firstHe = boundaryHes.top();
	  memset(visited, 0, sizeof(BOOL)*256);
	  while(!boundaryHes.empty()) boundaryHes.pop();
	}
	else
	  cyclesLeft = false;
      } //foundCycle.
      else {
	//Hole is a simple loop.
	CollectCycleVertices(boundaryHes, INVALID_IVERTEX, 
			     foundCycle, cycleVertices);
	if(!CloseHole(cycleVertices))
	  return false;

	cyclesLeft = false;
      }

    } while (cyclesLeft);

    return true;
  }

//   BOOL CloseHole(cHALF_EDGE *initBoundaryHE)
//   {
//     std::vector<iVERTEX> loopVertices;
//     std::vector<iVERTEX> bfaceVertices;
    
//     cHALF_EDGE *loopingHe = initBoundaryHE;
    
//     do {
//       loopVertices.push_back(loopingHe->Tail()->Index());
//       loopingHe = loopingHe->Next();
//     } while(loopingHe != initBoundaryHE);

//     ASSERT(loopVertices.size() >= 3);

//     cBASE::NewFacetStart();

//     for(INT i = 0; i < loopVertices.size() ; i++) {
//       cBASE::NewFacetAddVertex(loopVertices[i]);
//     }

//     iFACET newFacetIndex = cBASE::NewFacetClose();
    
//     if(newFacetIndex == INVALID_IFACET)
//       return false;

//     cFACET *newFacet = cBASE::Facet(newFacetIndex);
//     newFacet->IsFromInputSurface(true);
//     return true;
//   }

  BOOL Close()
  {
    while (true){
      //Find an open boundary and close it by a new facet.
      cHALF_EDGE *boundaryHalfEdge = BoundaryEdge();
      
      if(boundaryHalfEdge == NULL)
	break;
      
      if (!CloseHole(boundaryHalfEdge))
	return false;
    }

    return true;
  }

  BOOL IsClosed() {
    return BoundaryEdge() ? false : true ;
  }

  BOOL HasInputSurface() {
    typename cBASE::facet_iterator currFacet = cBASE::FacetsBegin();
    typename cBASE::facet_iterator lastFacet = cBASE::FacetsEnd();
    
    BOOL hasInputSurface = false;
    for( ; currFacet != lastFacet ; currFacet++ ) {
      if(currFacet->IsFromInputSurface())
	hasInputSurface = true;
    }

    return hasInputSurface;
  }

};

struct sCUT_CELL_STORAGE_POLICY {
  static const INT numObjectsInPage = 16;
  static const INT logOfObjectsInPage = 4;
  static const INT numInitialPages = 4;
};


TYPEDEF_SUBMESH(cCUT_CELL, tCUT_CELL, sCUT_CELL_STORAGE_POLICY, tMESH_OBJECT_FACTORY,
		tCUT_CELL_VERTEX, tHALF_EDGE, tCUT_CELL_FACET);



#endif /* CUT_CELL_HPP_ */
