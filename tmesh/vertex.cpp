#include "defs.hpp"
#include "half_edge.hpp"
#include "vertex_use.hpp"
#include "vertex.hpp"
#include "facet.hpp"
//#include "debug_utils.hpp"

namespace tmesh{
  template <typename _Mesh_Entities>
  VOID tVERTEX<_Mesh_Entities>::AddVertexUse(const cHALF_EDGE *h)
  {
    //marking He as the first incident one on the split.
    cHALF_EDGE* newHe = const_cast<cHALF_EDGE*>(h);
    newHe->MarkFirstOnVertex();
    m_vertexUses.push_back(cVERTEX_USE(newHe));

    MarkDeleted(false); //m_isDeleted = false;
  }

  template <typename _Mesh_Entities>
  typename tVERTEX<_Mesh_Entities>::cVERTEX_USE
  tVERTEX<_Mesh_Entities>::FirstVertexUse()
  {
    typename tLIST<cVERTEX_USE>::iterator firstUse = m_vertexUses.begin();
    return *firstUse;
  }

  template <typename _Mesh_Entities>
  typename tVERTEX<_Mesh_Entities>::cVERTEX_USE
  tVERTEX<_Mesh_Entities>::Find(const cHALF_EDGE *h)
  {
    typename tLIST<cVERTEX_USE>::iterator firstUse = m_vertexUses.begin();
    typename tLIST<cVERTEX_USE>::iterator endUse = m_vertexUses.end();

    for( ; firstUse != endUse ; firstUse++) {
      cHALF_EDGE *vUseHe = const_cast<cHALF_EDGE*>(firstUse->HalfEdge());
      cHALF_EDGE *loopHe = vUseHe;

      do {
        if(loopHe == h)
          return *firstUse;

        loopHe = loopHe->Opp()->Next();
      }while(vUseHe != loopHe);
    }

    ASSERT(0); //failed.
    return cVERTEX_USE();
  }

  template <typename _Mesh_Entities>
  BOOL tVERTEX<_Mesh_Entities>::Find(const cVERTEX *v,
				     cHALF_EDGE **h_out,
				     cVERTEX_USE *vUse)
  {
    typename tLIST<cVERTEX_USE>::iterator firstUse = m_vertexUses.begin();
    typename tLIST<cVERTEX_USE>::iterator endUse = m_vertexUses.end();

    for( ; firstUse != endUse ; firstUse++) {
      cHALF_EDGE *vUseHe = const_cast<cHALF_EDGE*>(firstUse->HalfEdge());

      if(vUse)
    	vUse->HalfEdge(vUseHe);

      cHALF_EDGE *loopHe = vUseHe;
      do {
    	cHALF_EDGE *loopHeOpp = loopHe->Opp();

        if(loopHeOpp->Vertex() == v) {
          *h_out = loopHe;
          return true;
        }

        loopHe = loopHeOpp->Next();
      }while(vUseHe != loopHe);
    }

    return false;
  }

  template <typename _Mesh_Entities>
  void tVERTEX<_Mesh_Entities>::RemoveVertexUse(const cVERTEX_USE &vUse)
  {
    if(vUse.IsEmpty())
      return;

    typename tLIST<cVERTEX_USE>::iterator firstUse = m_vertexUses.begin();
    typename tLIST<cVERTEX_USE>::iterator endUse = m_vertexUses.end();

    for( ; firstUse != endUse ; firstUse++) {
      if(firstUse->m_pHe == vUse.m_pHe)
	break;
    }

    ASSERT(firstUse != endUse);
    firstUse->m_pHe->UnmarkFirstOnVertex();
    m_vertexUses.erase(firstUse);
  }

  template <typename _Mesh_Entities>
  void tVERTEX<_Mesh_Entities>::ReplaceHalfEdge(cHALF_EDGE* oldHe,
                                                cHALF_EDGE* newHe)
  {
    // ASSERT(m_vertexUses.size() > 1);

    typename tLIST<cVERTEX_USE>::iterator firstUse = m_vertexUses.begin();
    typename tLIST<cVERTEX_USE>::iterator endUse = m_vertexUses.end();

    for( ; firstUse != endUse ; firstUse++) {
      if(firstUse->m_pHe == oldHe)
	break;
    }

    ASSERT(firstUse != endUse);

    firstUse->HalfEdge(newHe);

    oldHe->UnmarkFirstOnVertex();
    newHe->MarkFirstOnVertex();
  }

  template <typename _Mesh_Entities>
  typename tVERTEX<_Mesh_Entities>::cVERTEX_USE
  tVERTEX<_Mesh_Entities>::FindVertexUse(const cHALF_EDGE* findHe)
  {
    //    ASSERT(m_vertexUses.size() > 1);

    typename tLIST<cVERTEX_USE>::iterator firstUse = m_vertexUses.begin();
    typename tLIST<cVERTEX_USE>::iterator endUse = m_vertexUses.end();

    for( ; firstUse != endUse ; firstUse++) {
      if(firstUse->m_pHe == findHe)
	return *firstUse;
    }

    //    ASSERT(0); //can't reach here.
    return cVERTEX_USE();
  }

  template <typename _Mesh_Entities>
  VOID tVERTEX<_Mesh_Entities>::AppendNormalData(cPOINT3 &vertexPoint,
						 cHALF_EDGE* he0,
						 cHALF_EDGE* he1,
						 cVECTOR3 *p_normalVector,
						 REAL *p_totalWeight)
  {
    cVECTOR3 vector0 = vertexPoint - he0->Head()->Point();
    cVECTOR3 vector1 = he1->Head()->Point() - vertexPoint;
#ifdef DEBUG_PRINT
    printf("he0.Opp: ");
    he0->Opp()->Print();
    printf("he1: ");
    he1->Print();
#endif //DEBUG_PRINT

    Normalize(vector0);
    Normalize(vector1);
    cVECTOR3 thisNormal = Cross(vector0, vector1);
    Normalize(thisNormal);
    REAL thisWeight = sqrt(1.0- vector0 *vector1);
    *p_totalWeight += thisWeight;
    *p_normalVector += (thisNormal*thisWeight);
  }
  //returns an average normal value at the vertex
  //The average normal is a weighted normalized sum of the incident facets normals
  //with weights being angles of the facet at this vertex
  template <typename _Mesh_Entities>
  cVECTOR3 tVERTEX<_Mesh_Entities>::AverageNormal()
  {
    cVECTOR3 normalVector;
    REAL totalWeight = 0.0; //sum of all the weights
    cPOINT3 vertexPoint = Point();
    cVERTEX_USE firstVertexUse = FirstVertexUse();
    cHALF_EDGE* first_he = NULL; //this is an outgoing he, and will be processed at the end
    cHALF_EDGE* prev_he = NULL;
    for (typename cVERTEX_USE::half_edge_circulator heCirculator = firstVertexUse.HalfEdgesBegin();
	 heCirculator != firstVertexUse.HalfEdgesEnd();
	 heCirculator++){
      cHALF_EDGE *he = heCirculator.operator->();
#ifdef DEBUG_PRINT
      he->Print();
      printf("tail:\t");
      he->Tail()->Print();
      printf("head:\t");
      he->Head()->Print();
#endif //DEBUG_PRINT
      if (first_he == NULL)
        first_he = he; //save for later use

      else { //this is an outgoing he, use it together with the previous incoming he
        AppendNormalData(vertexPoint, prev_he, he,
			 &normalVector, &totalWeight);
      }
      prev_he = he;
    }
    AppendNormalData(vertexPoint, prev_he, first_he,
		     &normalVector, &totalWeight);
    normalVector /= totalWeight;
    return normalVector;
  }

  template <typename _Mesh_Entities>
  VOID tVERTEX<_Mesh_Entities>::Print(FILE *stream)
  {
    fprintf(stream, "v[%d] : %lf %lf %lf \n", m_index,
	    m_point[0], m_point[1], m_point[2]);
//    return;
    vertex_use_iterator currVtxUse = VertexUsesBegin();
    vertex_use_iterator lastVtxUse = VertexUsesEnd();

    for( ; currVtxUse != lastVtxUse ; currVtxUse++)
      currVtxUse->Print(stream);
  }

  template <typename _Mesh_Entities>
  VOID tVERTEX<_Mesh_Entities>::IncidentFacets(std::vector<cFACET*> &facets)
  {
    vertex_use_iterator currVtxUse = VertexUsesBegin();
    vertex_use_iterator lastVtxUse = VertexUsesEnd();

    for( ; currVtxUse != lastVtxUse ; currVtxUse++) {
      typename cVERTEX_USE::facet_circulator currFacet = currVtxUse->FacetsBegin();
      typename cVERTEX_USE::facet_circulator lastFacet = currVtxUse->FacetsEnd();

      for ( ; currFacet != lastFacet ; currFacet++ )
	facets.push_back(currFacet.operator->());
    }
  }

  /*
   * There will be some duplication of vertices.
   */
  template <typename _Mesh_Entities>
  VOID tVERTEX<_Mesh_Entities>::IncidentVertices(std::vector<cVERTEX*> &vertices)
  {
    vertex_use_iterator currVtxUse = VertexUsesBegin();
    vertex_use_iterator lastVtxUse = VertexUsesEnd();

    for( ; currVtxUse != lastVtxUse ; currVtxUse++) {
      typename cVERTEX_USE::half_edge_circulator currHalfEdge = currVtxUse->HalfEdgesBegin();
      typename cVERTEX_USE::half_edge_circulator lastHalfEdge = currVtxUse->HalfEdgesEnd();

      for ( ; currHalfEdge != lastHalfEdge ; currHalfEdge++ )
	vertices.push_back(currHalfEdge->Head());
    }
  }

  template <typename _Mesh_Entities>
BOOL tVERTEX<_Mesh_Entities>::OutgoingBorderHE(cVECTOR3 &initVector, cHALF_EDGE **outgoingHE,  BOOL doPrint)
{
  *outgoingHE = NULL;
  vertex_use_iterator currVtxUse = VertexUsesBegin();
  vertex_use_iterator lastVtxUse = VertexUsesEnd();
  cHALF_EDGE *lastOutgoingHE = NULL;
  REAL dotMin = 2.0;

  for( ; currVtxUse != lastVtxUse ; currVtxUse++) {
//	if (doPrint)
//	  currVtxUse->Print();
    typename cVERTEX_USE::half_edge_circulator currHalfEdge = currVtxUse->HalfEdgesBegin();
    typename cVERTEX_USE::half_edge_circulator lastHalfEdge = currVtxUse->HalfEdgesEnd();

    for ( ; currHalfEdge != lastHalfEdge ; currHalfEdge++ ){
      if (currHalfEdge->IsBorder() && currHalfEdge->Tail() == this){
        if (doPrint)
          printf("currHE : t %d h %d\n", currHalfEdge->Tail()->Index(), currHalfEdge->Head()->Index());
    	cVECTOR3 currHEVector = currHalfEdge->Vector();
    	currHEVector = currHEVector.Normalize();
    	REAL currHEDot = currHEVector * initVector;
    	if (doPrint){
    		printf("dot = %g; currVec = ", currHEDot);
    		currHEVector.Print();
    	}
    	if (currHEDot < dotMin){  //this may not work for concave facets
    	  lastOutgoingHE = currHalfEdge.operator->();
    	  dotMin = currHEDot;
    	}
      }
    }
  }
  *outgoingHE = lastOutgoingHE;
  return (lastOutgoingHE != NULL);
}

  template <typename _Mesh_Entities>
  cVECTOR3 tVERTEX<_Mesh_Entities>::Normal()
  {
    std::vector<cFACET *> facets;

    IncidentFacets(facets);

    typename std::vector<cFACET*>::iterator currFacet = facets.begin();
    typename std::vector<cFACET*>::iterator lastFacet = facets.end();

    REAL totalArea = 0.0;
    cVECTOR3 normal;
    for( ; currFacet != lastFacet ; currFacet++ ) {
      REAL area = (*currFacet)->Area();
      totalArea += area;
      normal += ((*currFacet)->UnitNormal()*area);
    }

    return normal/totalArea;
  }

  template <typename _Mesh_Entities>
  BOOL tVERTEX<_Mesh_Entities>::Verify()
  {
    typename tLIST<cVERTEX_USE>::iterator firstUse = m_vertexUses.begin();
    typename tLIST<cVERTEX_USE>::iterator endUse = m_vertexUses.end();

    for( ; firstUse != endUse ; firstUse++) {
      if(!firstUse->Verify()) {
	ASSERT(firstUse->Verify());
	return false;
      }
    }

    return true;
  }

  template <typename _Mesh_Entities>
  INT tVERTEX<_Mesh_Entities>::NumIncidences(cFACET *facet)
  {
    typename tLIST<cVERTEX_USE>::iterator firstUse = m_vertexUses.begin();
    typename tLIST<cVERTEX_USE>::iterator endUse = m_vertexUses.end();

    INT numIncidences = 0;
    for( ; firstUse != endUse ; firstUse++) {
      typename cVERTEX_USE::facet_circulator currFacet = firstUse->FacetsBegin();
      typename cVERTEX_USE::facet_circulator lastFacet = firstUse->FacetsEnd();

      for( ; currFacet  != lastFacet ; currFacet++ )
	if(currFacet.operator->() == facet)
	  numIncidences++;
    }

    return numIncidences;
  }
}

