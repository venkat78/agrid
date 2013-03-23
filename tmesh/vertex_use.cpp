/*
 * vertex_use.cpp
 *
 *  Created on: Jun 6, 2009
 */

#include "vertex_use.hpp"

namespace tmesh {
  template <typename _Mesh_Entities>
  VOID tVERTEX_USE<_Mesh_Entities>::Print(FILE *stream) const
  {
    facet_circulator currFacet = FacetsBegin();
    facet_circulator lastFacet = FacetsEnd();

    for( ; currFacet != lastFacet ; currFacet++)
      fprintf(stream, "f%d ", currFacet->Index());

    fprintf(stream, "\n");

    half_edge_circulator currHalfEdge = HalfEdgesBegin();
    half_edge_circulator lastHalfEdge = HalfEdgesEnd();

     for( ; currHalfEdge != lastHalfEdge ; currHalfEdge++)
       fprintf(stream, "he: t%d h%d; ", currHalfEdge->Tail()->Index(), currHalfEdge->Head()->Index());

     fprintf(stream, "\n");

    fflush(stream);
  }

  template <typename _Mesh_Entities>
  BOOL tVERTEX_USE<_Mesh_Entities>::Verify() const
  {
    const cHALF_EDGE* incidentHe = m_pHe;

    ASSERT(incidentHe->IsFirstOnVertex());

    if(!incidentHe->IsFirstOnVertex())
      return false;

    INT countIndexHes = 0;

    do {
      if(incidentHe->IsDeleted()) {
	ASSERT(incidentHe->IsDeleted());
	return false;
      }

      if(incidentHe->IsFirstOnVertex()) {
	countIndexHes++;
      }

      if(countIndexHes > 1) {
	ASSERT(countIndexHes == 1);
	return false;
      }

      if(incidentHe->Vertex() != m_pHe->Vertex()) {
	ASSERT(incidentHe->Vertex() == m_pHe->Vertex());
	return false;
      }

      incidentHe = incidentHe->Opp()->Next();
    } while( incidentHe != m_pHe );

    return true;
  }

}
