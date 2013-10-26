#include "defs.hpp"
#include "tmesh_def.hpp"
#include "add_facet.hpp"
#include "delete_facet.hpp"

namespace tmesh {
  _TMESH_TMPL_TYPE
  INT _TMESH_TMPL_DECL::NumFacets() const
  {
    return m_factory.NumFacets() - m_numDeletedFacets;
  }

  _TMESH_TMPL_TYPE
  typename _TMESH_TMPL_DECL::cFACET*
  _TMESH_TMPL_DECL::Facet(iFACET facetIndex)
  {
#ifdef DEBUG
    VerifyFacetIndex(facetIndex);
#endif //DEBUG

    if(facetIndex >=0 && facetIndex < m_factory.NumFacets())
      return m_factory.Facet(facetIndex);
    else
      return NULL;
  }

  _TMESH_TMPL_TYPE
  VOID _TMESH_TMPL_DECL::NewFacetStart(const iFACET facetIndex)
  {
    m_facetIndex = facetIndex;
    m_facetVertices.clear();
  }

  _TMESH_TMPL_TYPE
  VOID _TMESH_TMPL_DECL::NewFacetAddVertex(const iVERTEX vertexIndex)
  {
#ifdef DEBUG
    VerifyVertexIndex(vertexIndex);
#endif //DEBUG

    m_facetVertices.push_back(vertexIndex);
  }

  _TMESH_TMPL_TYPE
  iFACET _TMESH_TMPL_DECL::NewFacetClose()
  {
    BOOL validFacet = true;
    INT numVertices = m_facetVertices.size();

    for(INT i = 0; i < numVertices ; i++) {
      cVERTEX *vertex = Vertex(m_facetVertices[i]);

      if(vertex->IsVisited()) {
	validFacet = false;
	break;
      }else
	vertex->MarkVisited();
    }

    for(INT i = 0; i < numVertices ; i++ )
      Vertex(m_facetVertices[i])->UnmarkVisited();

    if(!validFacet)
      return INVALID_IFACET;

    tADD_FACET<cSELF> addFacetOp(this);
    iFACET rv = addFacetOp.Finish();
//#ifdef DEBUG
    cFACET *facet = Facet(rv);
    REAL area = facet->Area();
    if (area <= 1e-20)
      printf("f %d; nv %d; small area %lg\n", rv, numVertices, area);
//#endif
    return rv;
  }

  _TMESH_TMPL_TYPE
  iFACET _TMESH_TMPL_DECL::NewTriangle(const iVERTEX v1,
                            const iVERTEX v2,
                            const iVERTEX v3,
                            const iFACET facetIndex)
  {
    NewFacetStart(facetIndex);

    NewFacetAddVertex(v1);
    NewFacetAddVertex(v2);
    NewFacetAddVertex(v3);

    return NewFacetClose();
  }

  _TMESH_TMPL_TYPE
  iFACET _TMESH_TMPL_DECL::NewQuad(const iVERTEX v1,
                        const iVERTEX v2,
                        const iVERTEX v3,
                        const iVERTEX v4,
                        const iFACET facetIndex)
  {
    NewFacetStart(facetIndex);

    NewFacetAddVertex(v1);
    NewFacetAddVertex(v2);
    NewFacetAddVertex(v3);
    NewFacetAddVertex(v4);

    return NewFacetClose();
  }

  _TMESH_TMPL_TYPE
  BOOL _TMESH_TMPL_DECL::DeleteFacet(iFACET facetIndex)
  {
    if(Facet(facetIndex)->IsDeleted())
      return false;

    tDELETE_FACET<cSELF> deleteFacetOp(this, facetIndex);
    deleteFacetOp.Finish();

    return true;
  }

  //tessellates a cylinder/cone with a mesh with n strips. The top and bottom are
  //constructed as polygons with n vertices. Returns index of the first vertex

  _TMESH_TMPL_TYPE
    iVERTEX _TMESH_TMPL_DECL::AppendTessellatedCylinder(cPOINT3 &c1, cPOINT3 &c2,
    		REAL r1, REAL r2, INT n)
  {
	  cVECTOR3 z_axis = c2 - c1;
	  cVECTOR3 x_axis = z_axis.PerpVector();
	  cVECTOR3 y_axis= Cross(z_axis, x_axis);
	  y_axis = y_axis.Normalize();

	  //create vertices
	  iVERTEX firstVertexIndex = INVALID_IVERTEX;
	  REAL angleIncrement = 2.0*M_PI/((REAL)n);
	  REAL angle = 0.0;

	  for (INT k = 0; k < n; k++){
		  cVECTOR3 vertexVector = x_axis*cos(angle) + y_axis*sin(angle);
		  cPOINT3 point1 = c1 + vertexVector*r1;
		  cPOINT3 point2 = c2 + vertexVector*r2;
		  iVERTEX ivertex1 = NewVertex(point1);
		  if (firstVertexIndex == INVALID_IVERTEX)
			  firstVertexIndex = ivertex1;
		  iVERTEX ivertex2 = NewVertex(point2);
		  angle += angleIncrement;
	  }

	  //create bottom and top polygons
	  NewFacetStart();
	  for (INT k=n-1; k>=0; k--){
		  NewFacetAddVertex(firstVertexIndex+2*k);
	  }
	  NewFacetClose();

	  NewFacetStart();
	  for (INT k=0; k<n; k++){
		  NewFacetAddVertex(firstVertexIndex+1+2*k);
	  }
	  NewFacetClose();

	  for(INT k=1; k < n; k++){
		  NewQuad(firstVertexIndex+2*k-2, firstVertexIndex+2*k,
				  firstVertexIndex+2*k+1, firstVertexIndex+2*k-1);
	  }
	  NewQuad(firstVertexIndex+2*n-2, firstVertexIndex,
	  				  firstVertexIndex+1, firstVertexIndex+2*n-1);

	  return firstVertexIndex;
  }

  _TMESH_TMPL_TYPE
  BOOL _TMESH_TMPL_DECL::FlipFacet(iFACET facetIndex)
  {
    iVERTEX vertices[ARRAY_SIZE];
    INT numVertices = 0;

    cFACET *facet = Facet(facetIndex);

    typename cFACET::vertex_circulator currVtx = facet->VerticesBegin();
    typename cFACET::vertex_circulator lastVtx = facet->VerticesEnd();

    for( ; currVtx != lastVtx ; currVtx++ )
      vertices[numVertices++] = currVtx->Index();

    DeleteFacet(facetIndex);

    NewFacetStart(facetIndex);

    for(INT i = numVertices - 1; i >= 0 ; i-- ) {
      NewFacetAddVertex(vertices[i]);
    }

    iFACET index = NewFacetClose();

    if(index == facetIndex)
      return true;

    return false;
  }

}
