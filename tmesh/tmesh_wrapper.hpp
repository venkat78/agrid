/*
 * tmesh_wrapper.hpp
 *
 *  Created on: Aug 4, 2011
 */

#ifndef TMESH_WRAPPER_HPP_
#define TMESH_WRAPPER_HPP_

#include "defs.hpp"
#include "new_vertex_trigger.hpp"

namespace tmesh {

template <typename MESH_TYPE>
class tMESH_WRAPPER
{
public:
   tMESH_WRAPPER (MESH_TYPE *mesh = NULL) : m_mesh(mesh)
   {}

   VOID NewVertexTrigger(cNEW_VERTEX_TRIGGER *newVertexTrigger)
		   {m_newVertexTrigger = newVertexTrigger;}

   iVERTEX NewVertex(const cPOINT3 &point) {
	   iVERTEX newVertexIndex = m_mesh->NewVertex(point);
	   if (m_newVertexTrigger->DoTrigger(newVertexIndex))
		 NewVertexAction(newVertexIndex);
      return newVertexIndex;
   }

   iVERTEX NewVertex(REAL x, REAL y, REAL z){
	 return m_mesh->NewVertex(x, y, z);
   }
#ifdef WRONG
   cVERTEX* Vertex(iVERTEX iVertex) {return m_mesh->Vertex(iVertex);}
   const cVERTEX* Vertex(iVERTEX iVertex) const {return m_mesh->Vertex(iVertex);}

   INT NumVertices() const {return m_mesh->NumVertices();}
   INT LargestFacetIndex() {return m_mesh->LargestFacetIndex();}

   INT LargestVertexIndex() {return m_mesh->LargestVertexIndex();}

   //Methods for adding facets.
   VOID NewFacetStart(iFACET facetIndex = INVALID_IFACET)
   {return m_mesh->NewFacetStart(facetIndex);}
   VOID NewFacetAddVertex(iVERTEX vertexIndex)
	   {return m_mesh->NewFacetAddVertex(vertexIndex);}
   iFACET NewFacetClose() {return m_mesh->NewFacetClose() ;}

   cFACET* Facet(iFACET facetIndex); {return m_mesh->Facet(facetIndex);}
   const cFACET* Facet(iFACET facetIndex) const {return m_mesh->->Facet(facetIndex);}

   INT NumFacets() const  {return m_mesh->NumFacets();}

   iFACET NewTriangle(iVERTEX v1, iVERTEX v2, iVERTEX v3,
	      	       iFACET facetIndex = INVALID_IFACET)
   {return m_mesh->NewTriangle(v1, v2, v3, facetIndex);}

   iFACET NewQuad(const iVERTEX v1, const iVERTEX v2,
		   const iVERTEX v3, const iVERTEX v4,
		   const iFACET facetIndex = INVALID_IFACET)
   {return m_mesh->NewQuad(v1, v2, v3, v4, facetIndex);}
#endif //WRONG
   BOOL DeleteFacet(iFACET facetIndex) {return m_mesh->DeleteFacet(facetIndex);}
   BOOL FlipFacet(iFACET facetIndex) {return m_mesh->FlipFacet(facetIndex);}
//#endif //WRONG

   VOID NewVertexAction(iVERTEX newVertexIndex) {;}
   //Wrap other methods of tMESH like this.

   MESH_TYPE *m_mesh;
   cNEW_VERTEX_TRIGGER *m_newVertexTrigger;

};
} //namespace
#endif /* TMESH_WRAPPER_HPP_ */
