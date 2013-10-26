/*
 * hole_filler.cpp
 *
 *  Created on: Nov 28, 2011
 */

#include "hole_filler.hpp"

cHOLE_VERTEX_RECORD::cHOLE_VERTEX_RECORD()
{

}

REAL cHOLE_VERTEX_RECORD::ComputeAngleCosine(cSURFACE_MESH *m_mesh)
{
//  printf("pt0: "); m_in_half_edge->Tail()->Point().Print();
//  printf("pt1: "); m_in_half_edge->Head()->Point().Print();
//  printf("pt2: "); m_out_half_edge->Head()->Point().Print();
  cVECTOR3 inVector = m_in_half_edge->Vector();
  cVECTOR3 outVector = m_out_half_edge->Vector();
  inVector = - inVector;
  inVector = inVector.Normalize();
//  printf("InV: "); inVector.Print();
  outVector = outVector.Normalize();
//  printf("outV: "); outVector.Print();
  m_angle_cosine = inVector * outVector;
  //m_is_deleted = false;
//  printf("a_cos = %lf\n", m_angle_cosine);
  return m_angle_cosine;
}

cHOLE_FILLER::cHOLE_FILLER(cSURFACE_MESH *mesh)
{
  m_mesh = mesh;
  m_first_hole_facet_index = mesh->NumFacets();
  m_first_hole_vertex_index = mesh->NumVertices();
  m_cosine_0 = 0.5*sqrt(3.0); //acos(M_PI/6.0);
  m_cosine_1 = 0.5; //acos(M_PI/3.0);
  m_cosine_2 = -0.5; //acos(2.0*M_PI/3.0);
  m_small_edge_length_squared = 1e-8; //0.01 * mesh bounding box size
  m_comparable_edge_size_coeff = 0.75;
  m_max_half_edge_squared_length = 0.0;
  m_min_half_edge_squared_length = cLIMITS:: Max();

}

BOOL cHOLE_FILLER::Perform()
{
  while (CloseHole1());
  /*

  PrepareVetexRecords();
  while (m_vertex_records.size() > 0)
    FillAtSmallestAngleVertex();
*/
  return true;

}

//returns true if a hole has been filled
BOOL cHOLE_FILLER::CloseHole1()
{
  if (!FindCycle())
    return false;
  if (!FillHole())
    return false;
  return true;
}

BOOL cHOLE_FILLER::FindCycle()
{
#ifdef NEW_CODE
  //Find an open boundary and start tracing a hole
  cSURFACE_MESH::HALF_EDGE *firstHe = m_mesh->BorderHalfEdge();
  if(firstHe == NULL)
    return false;
  m_vertexVisited.assign(m_mesh->NumVertices(), false);

  BOOL cyclesLeft = false;

   //Implementing cycle detection algorithm.
   do {
     cHALF_EDGE* loopHe = firstHe;

     BOOL foundCycle = false;
     iVERTEX cycleAtV = INVALID_IVERTEX;
     do {
       ASSERT(loopHe->IsBorder());
       iVERTEX v = loopHe->Tail()->Index();
       if(m_vertexVisited[v] == true) {
         foundCycle = true;
         cycleAtV = v;
         break;
       }

       visited[v] = true;
       m_boundaryHalfEdges.push(loopHe);
       loopHe = loopHe->Next();
     } while(loopHe != firstHe);

     m_cycleVertices.empty();

     if(foundCycle) {
       CollectCycleVertices(m_boundaryHalfEdges, cycleAtV,
                            foundCycle, cycleVertices);
       if(!CloseHole(cycleVertices))
         return false;

       if(!m_boundaryHalfEdges.empty()) {
         cyclesLeft = true;
         firstHe = m_boundaryHalfEdges.top();
         memset(visited, 0, sizeof(BOOL)*256);
         while(!m_boundaryHalfEdges.empty()) m_boundaryHalfEdges.pop();
       }
       else
         cyclesLeft = false;
     } //foundCycle.
     else {
       //Hole is a simple loop.
       CollectCycleVertices(INVALID_IVERTEX, foundCycle);
       if(!CloseHole())
         return false;

       cyclesLeft = false;
     }

   } while (cyclesLeft);
#endif
  return false;
}

VOID cHOLE_FILLER::CollectCycleVertices(iVERTEX terminalVtx,
                          BOOL checkTerminalVtx)
{
  do {
    cSURFACE_MESH::cHALF_EDGE *topHe = NULL;//m_boundaryHalfEdges.top();
    m_cycleVertices.push_back(topHe->Tail()->Index());

    if(checkTerminalVtx && topHe->Tail()->Index() == terminalVtx) {
      m_boundaryHalfEdges.pop();
      break;
    }

    m_boundaryHalfEdges.pop();
  } while(!m_boundaryHalfEdges.empty());

}


BOOL cHOLE_FILLER::FillHole()
{
  return false;
}

VOID cHOLE_FILLER::PrepareVetexRecords()
{
  cSURFACE_MESH::cHALF_EDGE* firstBorderHE = m_mesh->BorderHalfEdge();
  printf("firstBorderHE: oppF%d: t%d; h%d\n",
    firstBorderHE->Opp()->Facet()->Index(), firstBorderHE->Tail()->Index(), firstBorderHE->Head()->Index());
  cSURFACE_MESH::cHALF_EDGE* currBorderHE = firstBorderHE;
  FILE *vfile = fopen("vertices", "wb+");
  INT vcount = 0;
  fprintf(vfile, "%d\t:\tvertex %d\n", vcount++, firstBorderHE->Tail()->Index());

  do {
    REAL l = currBorderHE->SquaredLength();
    if (l < m_small_edge_length_squared){ //remove this half-edge
      cPOINT3 midPoint = (currBorderHE->Tail()->Point() + currBorderHE->Head()->Point())*.05;
      m_mesh->MergeVertices(currBorderHE->Tail()->Index(), currBorderHE->Head()->Index(),
          currBorderHE ->Tail()->Index(),  midPoint);
    }
    else {
      if (l < m_min_half_edge_squared_length)
        m_min_half_edge_squared_length = l;
      if (l > m_max_half_edge_squared_length)
        m_max_half_edge_squared_length = l;
      currBorderHE = RegisterVertexRecord(currBorderHE);
      printf("%d:\tcurrBorderHE: oppF%d: t%d; h%d\n",
          vcount, currBorderHE->Opp()->Facet()->Index(), currBorderHE->Tail()->Index(), currBorderHE->Head()->Index());
      fprintf(vfile, "%d\t:\tvertex %d\n", vcount++, currBorderHE->Tail()->Index());

    }
  } while (currBorderHE != firstBorderHE);

  printf("size = %d; min_edge_length %lg; max_edge_length %lg\n",
		  m_vertex_records.size(),
		  sqrt(m_min_half_edge_squared_length), sqrt(m_max_half_edge_squared_length));
  fclose(vfile);

//temporary code for testing the pr_q
/*
  while (!m_vertex_records.empty()){
	cHOLE_VERTEX_RECORD vr = m_vertex_records.top();
    printf("v%d %d\t:cos %lf\n", vr.VertexIndex(), vr.AngleCosine());
    m_vertex_records.pop();
  }
  */
/*
    for (std::list<cHOLE_VERTEX_RECORD>::iterator currVR = m_vertex_records.begin();
        currVR != m_vertex_records.end(); currVR++ )
    printf("v%d %d\t:cos %lf\n",
        (*currVR).VertexIndex(),
        (*currVR).AngleCosine());
        */
}

INT cHOLE_FILLER::FillAtSmallestAngleVertex()
{
  cHOLE_VERTEX_RECORD SmallestAngleVertexRecord = m_vertex_records.top();
  REAL angleCosine = SmallestAngleVertexRecord.AngleCosine();
  printf("smallest: v %d; cos %lf; angle = %lf\n",
      SmallestAngleVertexRecord.VertexIndex(), angleCosine, acos(angleCosine)*180.0/M_PI);
  if (angleCosine <= m_cosine_0) //temp to test
    return FillAtVertexZeroTriangles(SmallestAngleVertexRecord);
  else if (angleCosine >= m_cosine_1)
    return FillAtVertexOneTriangle(SmallestAngleVertexRecord);
  else if (angleCosine >= m_cosine_2)
    return FillAtVertexTwoTriangles(SmallestAngleVertexRecord);
  else
    return FillAtVertexThreeTriangles(SmallestAngleVertexRecord);
}

/*
std::list<cHOLE_VERTEX_RECORD>::iterator cHOLE_FILLER::FindSmallestAngleVertexRecord()
{
  REAL smallestAngleCosine = -2.0;
  iVERTEX smallestAngleVertexIndex = INVALID_IVERTEX;

  while (!m_vertex_records.empty()){
//  for (std::list<cHOLE_VERTEX_RECORD>::iterator currVR = m_vertex_records.begin();
//      currVR != m_vertex_records.end(); currVR++ ){
    const cHOLE_VERTEX_RECORD currVR =   m_vertex_records.top();
    REAL currAngleCosine = currVR.AngleCosine();
    iVERTEX currVertexIndex = currVR.VertexIndex();
//    printf("curr: v %d c %lf; smallest: v %d c %lf\n",
//        currVertexIndex, currAngleCosine,
//        smallestAngleVertexIndex, smallestAngleCosine);
    if ((fabs(currAngleCosine - smallestAngleCosine) <= cLIMITS::Epsilon() &&
      currVertexIndex < smallestAngleVertexIndex) ||
      (currAngleCosine > smallestAngleCosine)){
        smallestAngleVertexIndex = currVertexIndex;
        smallestAngleCosine = currAngleCosine;
        smallestAngleVR = currVR;
    }
  }
  return smallestAngleVR;
}
*/

INT cHOLE_FILLER::FillAtVertexZeroTriangles(cHOLE_VERTEX_RECORD &vertexRecord)
{
//  cHOLE_VERTEX_RECORD vertexRecord = SmallestAngleVertexRecord.operator*();
  cSURFACE_MESH::cHALF_EDGE *inHE = vertexRecord.IncomingHE();
  cSURFACE_MESH::cHALF_EDGE *outHE = vertexRecord.OutgoingHE();
  REAL l1 = inHE->SquaredLength();
  REAL l2 = outHE->SquaredLength();
  if (l1 < l2){
    REAL t = sqrt(l1/l2);
 //   cPOINT3 newPoint = outHE->ParamPoint(t);
 //   m_mesh->MergeVertices(inHE->Tail()->Index(), outHE->Head()->Index(),
  //       inHE->Tail()->Index(),  newPoint);
//    if ()

  }
  printf("l1 = %lf; l2 = %lf\n", l1, l2);
  if (l1 >= l2*m_comparable_edge_size_coeff && l2 >= l1*m_comparable_edge_size_coeff){
    //merge the two vertices
    cPOINT3 midPoint = (inHE->Tail()->Point() + outHE->Head()->Point())*.05;
    m_mesh->MergeVertices(inHE->Tail()->Index(), outHE->Head()->Index(),
        inHE->Tail()->Index(),  midPoint);
    m_vertex_records.pop();
  }
  else if (l1 < l2){

  }
  else {

  }
  return 0;
}

INT cHOLE_FILLER::FillAtVertexOneTriangle(cHOLE_VERTEX_RECORD &vertexRecord)
{
  return 1;
}

INT cHOLE_FILLER::FillAtVertexTwoTriangles(cHOLE_VERTEX_RECORD &vertexRecord)
{
  return 2;
}

INT cHOLE_FILLER::FillAtVertexThreeTriangles(cHOLE_VERTEX_RECORD &vertexRecord)
{
  return 3;
}

//creates border vertex records
cSURFACE_MESH::cHALF_EDGE*
cHOLE_FILLER::RegisterVertexRecord(cSURFACE_MESH::cHALF_EDGE* currBorderHE)
{
  cSURFACE_MESH::cVERTEX* nextVertex = currBorderHE->Head();
  cSURFACE_MESH::cHALF_EDGE*  nextBorderHE = NULL;
  cVECTOR3 currHEVector = currBorderHE->Vector();
  currHEVector = currHEVector.Normalize();
  nextVertex->OutgoingBorderHE(currHEVector, &nextBorderHE);
  cHOLE_VERTEX_RECORD vertexRecord;
  vertexRecord.VertexIndex(nextVertex);
  vertexRecord.IncomingHE(currBorderHE);
  vertexRecord.OutgoingHE(nextBorderHE);
  vertexRecord.ComputeAngleCosine(m_mesh);
  m_vertex_records.push(vertexRecord);
  return nextBorderHE;
}

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
