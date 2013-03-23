/*
 * hole_closer.cpp
 *
 *  Created on: Feb 18, 2013
 */

#include "hole_closer.hpp"
#include "cleanup.hpp"

cHOLE_CLOSER::cHOLE_CLOSER(cSURFACE_MESH *mesh, iMANIFOLD manifoldIndex)
{
  m_mesh = mesh;
  m_manifoldIndex = manifoldIndex;
  m_maxHoleSize = 12;
  m_debugPrint = false;
}

//returns true if there are no border edges left
BOOL cHOLE_CLOSER::CloseHoles()
{
  //traverse border half-edges, and for each one check if a closed loop can be constructed. If so, build this closed loop,
  //  and build a facet covering this loop.
  cSURFACE_MESH::half_edge_iterator currHalfEdge = m_mesh->HalfEdgesBegin();
  cSURFACE_MESH::half_edge_iterator lastHalfEdge = m_mesh->HalfEdgesEnd();
  BOOL hasBorderEdges = false;
  m_debugCount = 0;
  m_debugPrint = false;

  for( ; currHalfEdge != lastHalfEdge; currHalfEdge++){

    if (currHalfEdge->IsBorder()){
      m_debugCount++;
      if (m_debugPrint){
        FILE *mtfile = fopen("meshbeforeclosing.txt", "wb+");
        m_mesh->Print(mtfile);
        fclose(mtfile);
        mtfile = NULL;
      }

      if (!(m_debugCount%100))
        printf("m_debugCount %d\n", m_debugCount);
      if (m_debugPrint){
        printf("border edge: t%d; h%d; OppF %d\n",
          currHalfEdge->Tail()->Index(), currHalfEdge->Head()->Index(),
          currHalfEdge->Opp()->Facet()->Index());
      }
      if (m_manifoldIndex == INVALID_IMANIFOLD ||
           currHalfEdge->Opp()->Facet()->ManifoldIndex() == m_manifoldIndex){
        if (FormLoop(currHalfEdge))
          CloseHole(currHalfEdge);
        else
          printf("cHOLE_CLOSER cannot form loop: m_debugCount = %d\n", m_debugCount);
      }
      if (!m_mesh->Verify()){
        printf("mesh verify failed: m_debugCount %d\n", m_debugCount);
      }
    }
  }
}

BOOL cHOLE_CLOSER::FormLoop(cSURFACE_MESH::half_edge_iterator startHalfEdge)
{
  m_holeSize = 1;
  cSURFACE_MESH::cVERTEX *initVertex = startHalfEdge->Tail();
  cSURFACE_MESH::cHALF_EDGE *currHalfEdge = startHalfEdge.operator->();
  m_loopVertexIndex[0] = initVertex->Index();
  if (m_debugPrint)
    printf("initVertex %d\n", initVertex->Index());

  while (m_holeSize <= m_maxHoleSize){
    cSURFACE_MESH::cVERTEX *nextVertex = currHalfEdge->Head();
    if (m_debugPrint)
      printf("nextVertex %d\n", nextVertex->Index());
    if (nextVertex == initVertex)
      return true;
  //  currHalfEdge->NextBorderHalfEdge(&currHalfEdge);
    cVECTOR3 heVector = currHalfEdge->Vector();
    heVector = heVector.Normalize();
    if (m_debugPrint){
      printf("heVector = ");
      heVector.Print();
    }
    nextVertex->OutgoingBorderHE(heVector, &currHalfEdge, m_debugPrint);
    if (m_debugPrint)
      printf("currHalfEdge: t%d h%d\n", currHalfEdge->Tail()->Index(), currHalfEdge->Head()->Index());
    m_loopVertexIndex[m_holeSize] = nextVertex->Index();
    m_holeSize++;
  }
  return false;
}

 VOID cHOLE_CLOSER::CloseHole(cSURFACE_MESH::half_edge_iterator startHalfEdge)
 {
   //build a new facet, and if it is degenerate, immediately delete it

   m_mesh->NewFacetStart();
   if (m_holeSize > 12)
     printf("holeSize %d; debugCOunt %d\n", m_holeSize, m_debugCount);
   for (INT i = 0; i < m_holeSize; i++)
     m_mesh->NewFacetAddVertex(m_loopVertexIndex[i]);
   iFACET iFacetNew = m_mesh->NewFacetClose();
   if (m_holeSize <= 4){
     cSURFACE_MESH_CLEANUP cleanup(m_mesh);
     cleanup.DeleteFacetIfDegenerate(iFacetNew);
     m_mesh->Facet(iFacetNew)->ManifoldIndex(m_manifoldIndex);
   }
   //How to decrease the number of manifold Border edges by  m_holeSize??
   }


 /*
    cSURFACE_MESH::cVERTEX* nextVertex = currBorderHE->Head();
  cSURFACE_MESH::cHALF_EDGE*  nextBorderHE = NULL;
  nextVertex->OutgoingBorderHE(&nextBorderHE);
  */

#ifdef NW_CODE - ths is borrowed from cut_cell code
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

#endif //NEW_CODE
