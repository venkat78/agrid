#include "defs.hpp"
#include "half_edge.hpp"
#include "vertex_use.hpp"
#include "vertex.hpp"
#include "facet.hpp"

namespace tmesh {
template <typename _Mesh_Entities>
typename tHALF_EDGE<_Mesh_Entities>::cHALF_EDGE*
tHALF_EDGE<_Mesh_Entities>::Prev() const
{
  const cHALF_EDGE* prev = this;

  while(prev->Next() != this)
    prev = prev->Next();

  return const_cast<cHALF_EDGE*>(prev);
}

template <typename _Mesh_Entities>
typename tHALF_EDGE<_Mesh_Entities>::cHALF_EDGE*
tHALF_EDGE<_Mesh_Entities>::Prev2() const
{
  const cHALF_EDGE* thisHe = this;
  cHALF_EDGE* prev = NULL;

  do {
    cHALF_EDGE* thisHeOpp = thisHe->Opp();
    prev = thisHeOpp;
    thisHe = thisHeOpp->Next();
  }while(thisHe != this);

  return prev;
}

template <typename _Mesh_Entities>
cVECTOR3 tHALF_EDGE<_Mesh_Entities>::Vector() const {
  return Head()->Point() - Tail()->Point();
}

template <typename _Mesh_Entities>
typename tHALF_EDGE<_Mesh_Entities>::cHALF_EDGE*
tHALF_EDGE<_Mesh_Entities>::Opp() const
{
  if(!IsARing()) {
    return m_opposite;
  }
  else{
    if(OppIsNextInRing())
      return NextInRing();
    else {
      const cHALF_EDGE *h = this;

      while(h->NextInRing() != this)
	h = h->NextInRing();

      return const_cast<cHALF_EDGE*>(h);
    }
  }
}

template <typename _Mesh_Entities>
typename tHALF_EDGE<_Mesh_Entities>::cHALF_EDGE*
tHALF_EDGE<_Mesh_Entities>::PrevInRing() const
{
  if(IsARing()) {
    const cHALF_EDGE* prev = this;

    while(prev->NextInRing() != this)
      prev = prev->NextInRing();

    return const_cast<cHALF_EDGE*>(prev);
  }

  //For a oriented 2-manifold case, return
  //the opposite pointer.
  return m_opposite;
}

template <typename _Mesh_Entities>
BOOL tHALF_EDGE<_Mesh_Entities>::HasIndex() const
{
  const cHALF_EDGE* nextHe = this;

  do {
    if(nextHe->IsFirstOnVertex())
      return true;

    nextHe = nextHe->Opp()->Next();
  } while(nextHe != this);

  return false;

}

#ifdef NEW_CODE
//assumes that this is a border half edge. Computes the halfedge following this one
//within the sallest loop
//Angles are used, but perhaps there is a purely topological way to find it
template <typename _Mesh_Entities>
BOOL tHALF_EDGE<_Mesh_Entities>::NextBorderHalfEdge(cHALF_EDGE **outgoingHE) const
{
  printf("this he: t%d h%d f%d\n", Tail()->Index(), Head()->Index(), Facet()->Index);
  cHALF_EDGE *lastOutgoingHE = NULL;
  cVERTEX *vertexInit = Tail();
  vertex_use_iterator currVtxUse = vertexInit->VertexUsesBegin();
  vertex_use_iterator lastVtxUse = vertexInit->VertexUsesEnd();

  for( ; currVtxUse != lastVtxUse ; currVtxUse++) {
        currVtxUse->Print();
    typename cVERTEX_USE::half_edge_circulator currHalfEdge = currVtxUse->HalfEdgesBegin();
    typename cVERTEX_USE::half_edge_circulator lastHalfEdge = currVtxUse->HalfEdgesEnd();

    for ( ; currHalfEdge != lastHalfEdge ; currHalfEdge++ ){
      printf("currHalfEdge: t%d h%d\n", currHalfEdge->Tail()->Index(), currHalfEdge->Head()->Index());
      if (currHalfEdge->IsBorder() && currHalfEdge->Tail() == this)
                lastOutgoingHE = currHalfEdge.operator->();
    }
  }
  *outgoingHE = lastOutgoingHE;
  return (lastOutgoingHE != NULL);
}

#endif //NW_CODE

template <typename _Mesh_Entities>
typename tHALF_EDGE<_Mesh_Entities>::cHALF_EDGE*
tHALF_EDGE<_Mesh_Entities>::BorderEdgeIntoVertex() const
{
  cHALF_EDGE* currHe = const_cast<cHALF_EDGE*>(this);

  do {
    cHALF_EDGE* heIntoVertex = currHe->Opp();

    if(heIntoVertex->IsBorder())
      return heIntoVertex;

    currHe = heIntoVertex->Next();
  }while(currHe != this);

  return NULL;
}

//returns facet vertex which is the furthest from the edge,
//and also computes its projection point onto the edge, and the squared distance
//between the furthest vertex and its projection point.
//If the vertex orthogonal projection is outside of the edge, the closest
//edge endpoint is used as the projection
template <typename _Mesh_Entities>
typename tHALF_EDGE<_Mesh_Entities>::cVERTEX *
tHALF_EDGE<_Mesh_Entities>::FurthestVertex(cPOINT3 *p_vertexProjection,
						    REAL *p_WidthSq) const
{
  cVERTEX *furthestVertex = NULL;
  cPOINT3 vertexProjection;
  REAL widthSq = -1.0;
  cSEGMENT3 thisHeSegment = Segment();
  cHALF_EDGE* currHe = (const_cast<cHALF_EDGE*>(this))->Next()->Next();
  do {
    cVERTEX *vertex = currHe->Vertex();
    cPOINT3 vertexPoint = vertex->Point();
    cPOINT3 projection = thisHeSegment.PointProjectionPoint(vertexPoint);
    REAL distSq = SquaredDistance(vertex->Point(), projection);
    if (distSq > widthSq){
      widthSq = distSq;
      vertexProjection = projection;
      furthestVertex = vertex;
    }
    currHe = currHe->Next();
  }while(currHe != this);

  if (p_vertexProjection != NULL)
    *p_vertexProjection = vertexProjection;
  if (p_WidthSq != NULL)
    *p_WidthSq = widthSq;

  return furthestVertex;
}

template <typename _Mesh_Entities>
REAL tHALF_EDGE<_Mesh_Entities>::SquaredLength() const
{
  const cPOINT3& point0 = Tail()->Point();
  const cPOINT3& point1 = Head()->Point();
  return SquaredDistance(point0, point1);
}

//---------------------------------------------------------
// IsIncomingHeReachable()
//   -- Returns TRUE if incomingHe belongs to the 2-manifold
//      represented by *this*
//---------------------------------------------------------
template <typename _Mesh_Entities>
BOOL tHALF_EDGE<_Mesh_Entities>::IsReachable(cHALF_EDGE *he,
                             bool outgoing) const
{
  cHALF_EDGE* outHe = const_cast<cHALF_EDGE*>(this);

  do {
    if(outgoing && outHe == he)
      return true;

    cHALF_EDGE* inHe = outHe->Opp();

    if(!outgoing && inHe == he)
      return true;

    outHe = inHe->Next();
  } while(outHe != this);

  return false;
}

template <typename _Mesh_Entities>
cSEGMENT3 tHALF_EDGE<_Mesh_Entities>::Segment() const
{
  return cSEGMENT3(Tail()->Point(), Head()->Point());
}

template <typename _Mesh_Entities>
cVECTOR3 tHALF_EDGE<_Mesh_Entities>::UniqueVector()
{
  cPOINT3 tailPoint = Tail()->Point();
  cPOINT3 headPoint = Head()->Point();

  cVECTOR3 edgeVec = headPoint - tailPoint;
  edgeVec /= edgeVec.Length(); //.Normalize();

  INT sign[3] = {0, 0, 0};

  sign[0] = edgeVec[0] > 0.0 ? 1 : (edgeVec[0] < 0.0 ? -1 : 0);
  sign[1] = edgeVec[1] > 0.0 ? 1 : (edgeVec[1] < 0.0 ? -1 : 0);
  sign[2] = edgeVec[2] > 0.0 ? 1 : (edgeVec[2] < 0.0 ? -1 : 0);

  BOOL invertVector = false;
  for(INT i = 0; i < 3; i++)  {
    if(sign[i] > 0) {
      invertVector = false;
      break;
    }

    if(sign[i] < 0) {
      invertVector = true;
      break;
    }
  }

  //  cPOINT3 pointOnPlane = (tailPoint + headPoint) * 0.5;

  if(invertVector)
    edgeVec *= -1.0;

  return edgeVec; //cPLANE3(pointOnPlane, edgeVec);
}

template <typename _Mesh_Entities>
VOID tHALF_EDGE<_Mesh_Entities>::Print(FILE *stream) const
{
  INT fIndex = m_facet == NULL ? -1 : m_facet->Index() ;

  fprintf(stream, "halfedge f%d: tv%d hv%d \n", fIndex, Tail()->Index(), Head()->Index());
}

template <typename _Mesh_Entities>
BOOL tHALF_EDGE<_Mesh_Entities>::Verify() const
{
  if(Opp() == NULL || Vertex() == NULL) {
    ASSERT(Opp() != NULL);
    ASSERT(Vertex() != NULL);
  }

  if(Opp() == this) {
    ASSERT(Opp() == this);
    return false;
  }

  //Also verify that opposites are true opposites.
  cHALF_EDGE* opp = Opp();
  ASSERT(AreOpposite(this, opp));

  if(!AreOpposite(this, opp))
    return false;

  //degenerate half-edge
  if(Tail() == Head()) {
    ASSERT(Tail() != Head());
    return false;
  }

  //Invariant 1.
  if(Prev() != Prev2()) {
    ASSERT(Prev() == Prev2());
    return false;
  }

  //Invariant 2.
  if(IsBorder()) {
    if(Opp()->IsBorder()) {
      ASSERT(!Opp()->IsBorder());
      return false;
    }
  }

  //Invariant 3.
  if(Opp()->Opp() != this) {
    ASSERT(!(Opp()->Opp() != this));
    return false;
  }

  //Invariant 4a.
  if(!IsARing()) {
    if(NextInRing() != Opp()) {
      ASSERT(!(NextInRing() != Opp()));
      return false;
    }
  }

  //Invariant 4b.
  if(!IsARing()) {
    if(PrevInRing() != Opp()) {
      ASSERT(!(PrevInRing() != Opp()));
      return false;
    }
  }

  return true;
}
}

