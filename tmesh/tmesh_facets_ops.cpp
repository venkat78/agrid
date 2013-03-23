#include "tmesh_facets_ops.hpp"

namespace tmesh {
  template <typename _Mesh_Type>
  INT tMESH_FACETS_OPS<_Mesh_Type>::CollectVertices(iFACET facetIndex, sVERTEX vertices[256])
  {
    INT numVertices = 0;

    cFACET *f = m_mesh->Facet(facetIndex);
    typename cFACET::vertex_circulator currVertex = f->VerticesBegin();
    typename cFACET::vertex_circulator lastVertex = f->VerticesEnd();

    for( ; currVertex != lastVertex ; currVertex++ ) {
      vertices[numVertices].index = currVertex->Index();
      vertices[numVertices].point = currVertex->Point();

      numVertices++;
    }

    return numVertices;
  }

  template <typename _Mesh_Type>
  BOOL tMESH_FACETS_OPS<_Mesh_Type>::CheckVertices(sVERTEX f1Vertices[256],
						   sVERTEX f2Vertices[256],
						   INT numVertices)
  {
    for(INT i = 0 ; i < numVertices ; i++ ) {
      if(f1Vertices[i].point != f2Vertices[i].point)
	return false;
    }

    return true;
  }

  template <typename _Mesh_Type>
  BOOL tMESH_FACETS_OPS<_Mesh_Type>::MergeFacets(iFACET f1, iFACET f2)
  {
    INT f1NumVertices = 0, f2NumVertices = 0;
    sVERTEX f1Vertices[256], f2Vertices[256];

    f1NumVertices = CollectVertices(f1, f1Vertices);
    f2NumVertices = CollectVertices(f2, f2Vertices);

    if(f1NumVertices != f2NumVertices)
      return false;

    std::sort(f1Vertices, f1Vertices + f1NumVertices);
    std::sort(f2Vertices, f2Vertices + f2NumVertices);

    if(!CheckVertices(f1Vertices, f2Vertices, f1NumVertices))
      return false;

    for(INT i = 0 ; i < f1NumVertices ; i++ ) {
      if(f1Vertices[i].index != f2Vertices[i].index) {
	m_mesh->MergeVertices(f1Vertices[i].index, f2Vertices[i].index,
			      f1Vertices[i].index, f1Vertices[i].point);
      }
    }

    return true;
  }

}
