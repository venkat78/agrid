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
   VOID _TMESH_TMPL_DECL::PrintHoles(FILE *stream)
   {
	  ConstructManifolds();
	  PrintBorderEdges(stream);
#ifdef DONT_USE
	//  if (stream == NULL)
		  stream = stdout;
	  //dummy implementation
	  std::list<sVERTEX_PAIR> borderEdgeList;

	  //collect border edges
	  half_edge_iterator currHalfEdge = HalfEdgesBegin();
	  half_edge_iterator lastHalfEdge = HalfEdgesEnd();
	  INT numBorderEdges = 0;
	  PrintBorderEdges();

	  for( ; currHalfEdge != lastHalfEdge; currHalfEdge++){
		  if (currHalfEdge->IsBorder()){
			  sVERTEX_PAIR vtxPair(currHalfEdge->Tail()->Index(),
					  	  	  	   currHalfEdge->Head()->Index());
			  printf("border edge %d %d\n", vtxPair.V1(), vtxPair.V2());
			  borderEdgeList.push_back(vtxPair);
			  numBorderEdges++;
		  }
	  }

	  fprintf(stream, "Mesh contains %d border edges\n", numBorderEdges);
	  printf("YG: Mesh contains %d border edges\n", numBorderEdges);

	  while (!borderEdgeList.empty()){
		std::list<iVERTEX> holeVertices;

		iVERTEX firstVertexIndex = borderEdgeList.front().V1();
		printf("\tVertex %d\n",firstVertexIndex);
		holeVertices.push_back(firstVertexIndex);
		iVERTEX nextVertexIndex = borderEdgeList.front().V2();
		borderEdgeList.pop_front();
		BOOL holeIsCLosed = false;
		//traverse the edges until a closed hole is created and printed out
//		std::list<sVERTEX_PAIR>::iterator currVtxPair = borderEdgeList.begin();

		while (!holeIsCLosed){
			std::list<sVERTEX_PAIR>::iterator nextVtxPair = borderEdgeList.begin();
			BOOL nextFound = false;
			while (!nextFound){
				if (nextVtxPair->V1() == nextVertexIndex){
					borderEdgeList.begin();nextFound = true;
				}
				else
					nextVtxPair++;
			}
			fprintf(stream, "\tVertex %d\n", nextVertexIndex);
			nextVertexIndex = nextVtxPair->V2();
			borderEdgeList.erase(nextVtxPair);


		}

	  }
#endif //DONT_USE

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
    INT index = 0;

    for( ; currHalfEdge != lastHalfEdge; currHalfEdge++){
      if (currHalfEdge->IsBorder()){
    	fprintf(stream, "%d:\t", index++);
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
