#include "tmesh_def.hpp"

namespace tmesh {
  _TMESH_TMPL_TYPE
  VOID _TMESH_TMPL_DECL::Print(FILE *stream, BOOL longPrint)
  {
    PrintVertices(stream);
    PrintHalfEdges(stream);
    PrintFacets(stream, longPrint);
  }

  _TMESH_TMPL_TYPE
  VOID _TMESH_TMPL_DECL::PrintVertices(FILE *stream)
  {
    vertex_iterator currVtx = VerticesBegin();
    vertex_iterator lastVtx = VerticesEnd();

    for ( ; currVtx != lastVtx ; currVtx++)
      currVtx->Print(stream);
  }

  _TMESH_TMPL_TYPE
  VOID _TMESH_TMPL_DECL::PrintHalfEdges(FILE *stream)
  {
    half_edge_iterator currHalfEdge = HalfEdgesBegin();
    half_edge_iterator lastHalfEdge = HalfEdgesEnd();

    for( ; currHalfEdge != lastHalfEdge; currHalfEdge++)
      currHalfEdge->Print(stream);
  }

  _TMESH_TMPL_TYPE
  VOID _TMESH_TMPL_DECL::PrintBorderEdges(FILE *stream)
  {
    half_edge_iterator currHalfEdge = HalfEdgesBegin();
    half_edge_iterator lastHalfEdge = HalfEdgesEnd();
    BOOL hasBorderEdges = false;

    for( ; currHalfEdge != lastHalfEdge; currHalfEdge++){
      if (currHalfEdge->IsBorder()){
        currHalfEdge->Print(stream);
        hasBorderEdges = true;
      }
    }
    if (!hasBorderEdges){
      if(stream == NULL)
    	stream = stdout;
      fprintf(stream, "Mesh contains no Border Edges\n");
    }
  }

  _TMESH_TMPL_TYPE
  VOID _TMESH_TMPL_DECL::PrintFacets(FILE *stream,   BOOL longPrint)
  {
    facet_iterator currFacet = FacetsBegin();
    facet_iterator lastFacet = FacetsEnd();

    for( ; currFacet != lastFacet ; currFacet++)
      currFacet->Print(stream, longPrint);
  }
}
