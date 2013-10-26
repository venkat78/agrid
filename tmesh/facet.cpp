#include "defs.hpp"
#include "half_edge.hpp"
#include "facet.hpp"
#include "half_edge_circulators.hpp"
#include "vertex_use.hpp"
#include "vertex.hpp"

namespace tmesh {

template <typename _Mesh_Entities>
INT tFACET<_Mesh_Entities>::NumVertices() const
{
  INT numVertices = 0;

  half_edge_circulator currHe = HalfEdgesBegin();
  half_edge_circulator lastHe = HalfEdgesEnd();

  for( ; currHe != lastHe ; currHe++ )
    numVertices++;

  return numVertices;
}

template <typename _Mesh_Entities>
VOID tFACET<_Mesh_Entities>::PushVerticesToVector(std::vector<INT> *facetVertexVector) const
{
  vertex_circulator currVtx = VerticesBegin();
  vertex_circulator lastVtx = VerticesEnd();

  for( ; currVtx != lastVtx ; currVtx++) {
	facetVertexVector->push_back(currVtx->Index());
  }
}

template <typename _Mesh_Entities>
BOOL tFACET<_Mesh_Entities>::ContainsVertex(cVERTEX *vertex) const
{
  const cHALF_EDGE* heInit = HalfEdge();
  const cHALF_EDGE* heCurr = heInit;
  do {
    if (heCurr->Vertex() == vertex)
      return true;
    heCurr = heCurr->Next();
  } while(heCurr != heInit);
  return false;
}

template <typename _Mesh_Entities>
BOOL tFACET<_Mesh_Entities>::Contains(const cPOINT3 &point)
{
  cHALF_EDGE *facetHe = HalfEdge();
  cHALF_EDGE* nextHe = facetHe->Next();
  cHALF_EDGE* nextToNextHe = nextHe->Next();

  cPOINT3 basePoint = facetHe->Vertex()->Point();
  cPOINT3 vertex2Point = nextHe->Vertex()->Point();

  do {
    cPOINT3 vertex3Point = nextToNextHe->Vertex()->Point();

    cTRIANGLE3 facetTriangle(basePoint, vertex2Point, vertex3Point);

    if(facetTriangle.IsPointInside(point, true))
      return true;

    nextHe = nextToNextHe;
    vertex2Point = vertex3Point;
    nextToNextHe = nextToNextHe->Next();
  } while (nextToNextHe != facetHe);

  return false;
}


template <typename _Mesh_Entities>
BOOL tFACET<_Mesh_Entities>::IsBorder() const
{
  half_edge_circulator currHe = HalfEdgesBegin();
  half_edge_circulator lastHe = HalfEdgesEnd();

  for( ; currHe != lastHe ; currHe++ ){
	if (currHe->Opp()->IsBorder())
	  return true;
  }

  return false;
}

template <typename _Mesh_Entities>
VOID tFACET<_Mesh_Entities>::Print(FILE *stream, BOOL longPrint) const
{
  if (longPrint){
    fprintf(stream, "f [%d]  : \n { \n", m_index);

    if (IsDeleted()){
      fprintf(stream, "is deleted\n");
      return;
    }
    vertex_circulator currVtx = VerticesBegin();
    vertex_circulator lastVtx = VerticesEnd();
    cPOINT3 pt = currVtx->Point();

    for( ; currVtx != lastVtx ; currVtx++) {
      fprintf(stream, "v%d :  ", currVtx->Index());
      currVtx->Point().Print(stream);
    }

    cVECTOR3 n = Normal().Normalize();
    fprintf(stream, "} N %lg %lg %lg; ", n[0], n[1], n[2]);
    if (fabs(n[1]) <= 1e-8 && fabs(n[2]) <= 1e-8)
      fprintf(stream, "x=%lg", pt[0]);
    else if (fabs(n[2]) <= 1e-8 && fabs(n[0]) <= 1e-8)
      fprintf(stream, "y=%lg", pt[1]);
    else if (fabs(n[0]) <= 1e-8 && fabs(n[1]) <= 1e-8)
      fprintf(stream, "z=%lg", pt[2]);
    fprintf(stream,"\n");
  }

  else {
    fprintf(stream, "f[%d] {", m_index);
    if (IsDeleted()){
      fprintf(stream, "is deleted\n");
      return;
    }
    vertex_circulator currVtx = VerticesBegin();
    vertex_circulator lastVtx = VerticesEnd();
    for( ; currVtx != lastVtx ; currVtx++) {
      fprintf(stream, "v%d  ", currVtx->Index());
    }
    fprintf(stream,"}\n");
  }
  fflush(stream);
}

template <typename _Mesh_Entities>
BOOL tFACET<_Mesh_Entities>::HasDuplicateVertex(const cHALF_EDGE* heStart) const
{
   const cHALF_EDGE* heInit = heStart;
   const cHALF_EDGE* heCurr = heStart->Next();
   const cVERTEX *vtxStart = heStart->Vertex();
   do {
     if (heCurr == NULL)
       return true;
     if (heCurr->Vertex() == vtxStart)
       return true;
     heCurr = heCurr->Next();
   } while(heCurr != heInit);
   return false;
}

template <typename _Mesh_Entities>
BOOL tFACET<_Mesh_Entities>::HasDuplicateVertices() const
{
   const cHALF_EDGE* heInit = HalfEdge();
   const cHALF_EDGE* heCurr = HalfEdge();

   do {
     if (HasDuplicateVertex(heCurr))
       return true;
     heCurr = heCurr->Next();
  } while(heCurr != heInit);
   return false;
}

//Newell's method works well for convex polygons, but for non-convex ones
//it may return a zero vector, causing devision by zero at its normalization,
//or return a meaningless result

template <typename _Mesh_Entities>
cVECTOR3 tFACET<_Mesh_Entities>::UnitNormal() const
{
  cVECTOR3 normal(Normal());

  if(normal.IsNull())
    return cVECTOR3();
  else {
    return normal.Normalize();
  }
}

template <typename _Mesh_Entities>
REAL tFACET<_Mesh_Entities>::Area() const
{
  cVECTOR3 areaVector = AreaVector();
  REAL area =  sqrt(areaVector * areaVector)*0.5;
  return area;
}

#ifdef UNUSED_CODE //- there is a bug in processing
//this method is based on the Allen Van Gelder formula
//from Graphics Gems V. See
//http://softsurfer.com/Archive/algorithm_0101/algorithm_0101.htm#3D%20Polygons
//the formula slightly differs depending on whether the number of edges is even or odd.
//since we have no direct access to this number, the last term of the formula depends on it
//the word even or odd in the names of half-edges and vertices refer to their
//index in the edge sequence
template <typename _Mesh_Entities>
cVECTOR3 tFACET<_Mesh_Entities>::AreaVector() const
{
  cVECTOR3 areaVector = cVECTOR3();

  const cHALF_EDGE* heInit = HalfEdge();
  const cHALF_EDGE* heEven = heInit;
  const cHALF_EDGE* heOdd = heEven->Next();
  const cHALF_EDGE* heEvenNext = heOdd->Next();

  if (heEvenNext == heEven) //two edges
  	return cVECTOR3();

  const cHALF_EDGE* heOddNext = heEvenNext->Next();
  //it needs to be verified for degenerate facets
  //and triangles
  const cVERTEX* vertexInit = heInit->Tail();
  //const cVERTEX* vertexEvenPrev = vertexInit;
  const cVERTEX* vertexOddPrev = heOdd->Tail();
  const cVERTEX* vertexEvenNext = heEvenNext->Tail();
  const cVERTEX* vertexOddNext = heOddNext->Tail();

  do {
  	if (heOddNext != heInit){ //skipping num_edges==3
  	  cVECTOR3 vectorEven = vertexEvenNext->Point() - vertexInit->Point();
  	  cVECTOR3 vectorOdd  = vertexOddNext->Point()  - vertexOddPrev->Point();
  	  areaVector += Cross(vectorEven, vectorOdd);

  	  heEven = heEvenNext;
  	  heOdd  = heOddNext;
  	  heEvenNext = heOdd->Next();
	  heOddNext = heEvenNext->Next();
  	}
  	if (heOddNext == heInit){ //odd number of edges - add triangle area
  	  cVECTOR3 vectorEven = vertexEvenNext->Point() - vertexInit->Point();
  	  cVECTOR3 vectorOdd  = vertexInit->Point() - vertexOddPrev->Point();
  	  areaVector += Cross(vectorEven, vectorOdd);
  	}
  } while(heEven != heInit && heOddNext != heInit);

  return areaVector;
}
#endif //Unused_code

//this is a simple but slower version based on splitting the polygon into triangles
template <typename _Mesh_Entities>
cVECTOR3 tFACET<_Mesh_Entities>::AreaVector() const
{
  cVECTOR3 areaVector = cVECTOR3();
  half_edge_circulator currHe = HalfEdgesBegin();
  cVERTEX *vertexInit = currHe->Tail(); //init vertex
  currHe++;
  half_edge_circulator lastHe = HalfEdgesEnd();
  half_edge_circulator nextHe = currHe;
  nextHe++;

	do {
	cVECTOR3 vectorInit = currHe->Tail()->Point() - vertexInit->Point();
	cVECTOR3 vectorEdge = currHe->Vector();
	areaVector += Cross(vectorInit, vectorEdge);
	currHe = nextHe;
	nextHe++;
  } while (nextHe != lastHe);
  return areaVector;
}

//if the facets shortest edge is shorter than tolerance, it's returned, otherwise NULL
template <typename _Mesh_Entities>
const typename tFACET<_Mesh_Entities>::cHALF_EDGE *
tFACET<_Mesh_Entities>::HasDegenerateEdge(REAL tol) const
{
  const cHALF_EDGE* heShortest = HalfEdge();
  REAL shortestHELengthSq = cLIMITS::Max();
  const cHALF_EDGE* heInit = HalfEdge();
  const cHALF_EDGE* heCurr = heInit;
  const cHALF_EDGE* heNext = heCurr->Next();

  do {
        REAL heLengthSq = heCurr->SquaredLength();
        if (heLengthSq < shortestHELengthSq){
          shortestHELengthSq = heLengthSq;
          heShortest = heCurr;
        }
        heCurr = heNext;
        heNext = heNext->Next();
  } while(heCurr != heInit);
  return (shortestHELengthSq < tol*tol)? heShortest : NULL;
}


template <typename _Mesh_Entities>
typename tFACET<_Mesh_Entities>::cHALF_EDGE*
tFACET<_Mesh_Entities>::FindMinHalfEdge(eCOORD coord)
{
  cHALF_EDGE *facetHe = HalfEdge();
  cHALF_EDGE *loopingHe = facetHe;
  cHALF_EDGE *minHe = facetHe;

  REAL minCoordVal = (loopingHe->Tail()->Point())[coord];
  loopingHe = loopingHe->Next();

  do {
    REAL coordVal = (loopingHe->Tail()->Point())[coord];
    if(coordVal < minCoordVal)  {
      minCoordVal = coordVal;
      minHe = loopingHe;
    }

    loopingHe = loopingHe->Next();
  } while(loopingHe != facetHe);

  return minHe;
}

//facet is narrow, if it has at least one edge such that all the vertices of the facet are
//located within tol from this edge
//This method optionally computes the narrow edge, its furthest vertex, and projection of
//this vertex onto this edge
template <typename _Mesh_Entities>
BOOL tFACET<_Mesh_Entities>::IsNarrow(REAL tol, cHALF_EDGE **p_narrowWidthHE,
		      cVERTEX **p_furthestVertex,
		      cPOINT3 *p_vertexProjectionOntoEdge) const
  {
    const cHALF_EDGE* heNarrowWidth = NULL;
    cVERTEX *furthestVertex = NULL;
    cPOINT3 furthestVertexProjectionPoint;
    REAL narrowWidthSq = cLIMITS::Max();;

    const cHALF_EDGE* heInit = HalfEdge();
    const cHALF_EDGE* heCurr = heInit;

    do {
      cPOINT3 vertexProjection;
      REAL heWidthSq = 0;
      cVERTEX *vertex = heCurr->FurthestVertex(&vertexProjection, &heWidthSq);
//        printf("heCurr = %d %d; v = %d; heWidthSq = %g\n",
//            heCurr->Tail()->Index(), heCurr->Head()->Index(), vertex->Index(), heWidthSq);
       if (heWidthSq < narrowWidthSq){
         narrowWidthSq = heWidthSq;
         furthestVertex = vertex;
         furthestVertexProjectionPoint = vertexProjection;
         heNarrowWidth = heCurr;
       }
       heCurr = heCurr->Next();
    } while(heCurr != heInit);
    if (p_narrowWidthHE != NULL)
      *p_narrowWidthHE= (cHALF_EDGE*) heNarrowWidth;
    if (p_furthestVertex !=NULL)
      *p_furthestVertex = furthestVertex;
    if (p_vertexProjectionOntoEdge != NULL)
      *p_vertexProjectionOntoEdge = furthestVertexProjectionPoint;
    return (narrowWidthSq< tol*tol);
  }


template <typename _Mesh_Entities>
BOOL tFACET<_Mesh_Entities>::HasNonManifoldEdge() const
{
  half_edge_circulator currHe = HalfEdgesBegin();
  half_edge_circulator lastHe = HalfEdgesEnd();

  for( ; currHe != lastHe ; currHe++ ) {
    if(currHe->IsNonManifold())
      return true;
  }

  return false;
}

template <typename _Mesh_Entities>
BOOL tFACET<_Mesh_Entities>::IsSimple() const
{
  vertex_circulator currVertex = VerticesBegin();
  vertex_circulator lastVertex = VerticesEnd();

  for( ; currVertex != lastVertex ; currVertex++ ) {
    if(currVertex->NumIncidences((cFACET*)this) != 1) {
      printf("facet %d %d\n", m_index, currVertex->Index());
      fflush(stdout);
      ASSERT(currVertex->NumIncidences((cFACET*)this) == 1);
      return false;
    }
  }

  return true;
}

template <typename _Mesh_Entities>
BOOL tFACET<_Mesh_Entities>::Verify() const
{
  if (IsDeleted())
	return true;
  half_edge_circulator currHe = HalfEdgesBegin();
  half_edge_circulator lastHe = HalfEdgesEnd();

  for( ; currHe != lastHe ; currHe++ ) {
    ASSERT(currHe.operator->() != NULL);
    if(currHe->IsDeleted())
      return true;
  }

  //Invariant 1.
  if(NumVertices() < 3) {
    ASSERT(!(NumVertices() < 3));
    return false;
  }

  if(!IsSimple()) {
    ASSERT(IsSimple());
    return false;
  }

  currHe = HalfEdgesBegin();
  lastHe = HalfEdgesEnd();

  std::vector<cPOINT3> facetPoints;
  for( ; currHe != lastHe ; currHe++ )
    facetPoints.push_back(currHe->Tail()->Point());

  std::sort(facetPoints.begin(), facetPoints.end());

  INT i = 0;
  INT n = facetPoints.size();

  for( ; i < n-1 ; i++ ) {
    if(facetPoints[i] == facetPoints[i+1]) {
      Print();
      ASSERT(facetPoints[i] != facetPoints[i+1]);
      return false;
    }
  }

  return true;
}

}
