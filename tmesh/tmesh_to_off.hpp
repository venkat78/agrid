#ifndef _TMESH_TO_OFF_H_
#define _TMESH_TO_OFF_H_

#include "defs.hpp"

namespace tmesh {
  struct sFACET_FILTER_POLICY {
    BOOL operator () (iFACET facetIndex) const;
    sCOLOR Color(iFACET facetIndex) const ;
  };

  struct sFACET_FILTER {
    BOOL operator() ( iFACET facetIndex ) const {
      return true;
    }

    sCOLOR Color(iFACET facetIndex) const {
      return utils::sCOLOR();
    }
  };

  template <typename _Mesh_Type>
  struct sFACET_NEIGHBORHOOD_FILTER {
    _TMESH_ENTITIES(_Mesh_Type);

    sFACET_NEIGHBORHOOD_FILTER(iFACET facetIndex, INT numRings = 0, _Mesh_Type *mesh = NULL)
    {
      neighbors[0] = facetIndex ; numNeighbors = 1;
      if(mesh == NULL || numRings == 0) return;

      CollectNeighbors(numRings, mesh);
    }

  public:
    BOOL operator()(iFACET facetIndex)  {
      return utils::ValIsInTable<iFACET>(facetIndex, neighbors, numNeighbors);
    }

    sCOLOR Color(iFACET facetIndex) {
      return sCOLOR();
    }

  private:
    VOID CollectNeighbors(INT numRings, _Mesh_Type *mesh) {
      for(INT i = 0 ; i < numRings ; i++ ) {
	INT numOldNeighbors = numNeighbors;
	for( INT j = 0 ; j < numOldNeighbors ; j++ ) {
	  cFACET *facet = mesh->Facet(neighbors[j]);
	  typename cFACET::half_edge_circulator currHe = facet->HalfEdgesBegin();
	  typename cFACET::half_edge_circulator lastHe = facet->HalfEdgesEnd();

	  for( ; currHe != lastHe ; currHe++ ) {
	    cHALF_EDGE *ringHe = (*currHe);
	    cHALF_EDGE *loopingHe = ringHe;
	    do {
	      if(!loopingHe->IsBorder()) {
		if(!ValIsInTable<iFACET>(loopingHe->Facet()->Index(), neighbors, numNeighbors)) {
		  neighbors[numNeighbors++] = loopingHe->Facet()->Index();
		}
	      }

	      loopingHe = loopingHe->NextInRing();
	    } while( loopingHe != ringHe );
	  }
	}
      }
    }


  private:
    iFACET neighbors[1024];
    INT numNeighbors;
  };

  template <typename _Mesh_Type>
  struct sVERTICES_NEIGHBORHOOD_FILTER
  {
    _TMESH_ENTITIES(_Mesh_Type);
  public:
    sVERTICES_NEIGHBORHOOD_FILTER(iVERTEX v1 = INVALID_IVERTEX,
				  iVERTEX v2 = INVALID_IVERTEX,
				  _Mesh_Type *mesh = NULL) {
      std::vector<cFACET*> facets;
      numNeighbors = 0;
      if(v1 != INVALID_IVERTEX) {
	mesh->Vertex(v1)->IncidentFacets(facets);
      }

      if(v2 != INVALID_IVERTEX) {
	mesh->Vertex(v2)->IncidentFacets(facets);
      }

      INT numFacets = facets.size();

      for(INT i = 0 ; i < numFacets ; i++ ) {
	if(!ValIsInTable<iFACET>(facets[i]->Index(), neighbors, numNeighbors)) {
	  neighbors[numNeighbors++] = facets[i]->Index();
	}
      }
    }

    BOOL operator()(iFACET facetIndex)  {
      return utils::ValIsInTable<iFACET>(facetIndex, neighbors, numNeighbors);
    }

    sCOLOR Color(iFACET facetIndex) {
      return sCOLOR();
    }

  private:
    iFACET neighbors[1024];
    INT numNeighbors;
  };

  struct sGENERIC_FACET_FILTER
  {
  public:
    VOID Add(iFACET facetIndex) {
      m_facets.insert(facetIndex);
    }

    BOOL operator() (iFACET facetIndex) {
      return IsPresent(facetIndex);
    }

    sCOLOR Color(iFACET facetIndex) {
      return sCOLOR();
    }
  private:
    BOOL IsPresent(iFACET facetIndex) {
      if(m_facets.find(facetIndex) == m_facets.end())
	return false;

      return true;
    }

  private:
    std::set<iFACET> m_facets;
  };


  template <typename _Mesh_Type, typename _FACET_FILTER>
  VOID ExportToOff(char *filename, _Mesh_Type &mesh,  _FACET_FILTER &filter)
  {
    _TMESH_ENTITIES(_Mesh_Type);

    FILE *file = fopen(filename, "wb+");

    std::map<iVERTEX, INT> offVtxMap;

    INT *offFacetMap = ::new INT[mesh.LargestFacetIndex() + 1];
    memset(offFacetMap, -1, sizeof(INT)*(mesh.LargestFacetIndex() + 1));

    INT numVertices = 0, numFacets = 0;
    typename _Mesh_Type::facet_iterator currFacet = mesh.FacetsBegin();
    typename _Mesh_Type::facet_iterator lastFacet = mesh.FacetsEnd();

    for( ; currFacet != lastFacet ; currFacet++ ) {
      if(filter(currFacet->Index())) {
	typename cFACET::vertex_circulator currFacetVertex = currFacet->VerticesBegin();
	typename cFACET::vertex_circulator lastFacetVertex = currFacet->VerticesEnd();

	for( ; currFacetVertex != lastFacetVertex ; currFacetVertex++ ) {
	  if( offVtxMap.find(currFacetVertex->Index()) == offVtxMap.end()) {
	    offVtxMap[ currFacetVertex->Index() ] = numVertices++;
	  }
	}

	offFacetMap[currFacet->Index()] = numFacets++;
      }
    }

    typename std::map<iVERTEX, INT>::iterator currFacetVtx = offVtxMap.begin();
    typename std::map<iVERTEX, INT>::iterator lastFacetVtx = offVtxMap.end();

    std::vector<iVERTEX> vIndices; vIndices.resize(numVertices);

    for( ; currFacetVtx != lastFacetVtx ; currFacetVtx++ ) {
      vIndices[currFacetVtx->second] = currFacetVtx->first;
    }

    //Writing vertices and facets to off file.
    fprintf(file, "OFF\n");
    fprintf(file, "%d %d 0\n", numVertices, numFacets);

    for(INT i = 0 ; i < numVertices ; i++ ) {
      iVERTEX vertexIndex = vIndices[i];
      cPOINT3 vtxPoint = mesh.Vertex(vertexIndex)->Point();
      fprintf(file, "%lf %lf %lf \n", vtxPoint[0], vtxPoint[1], vtxPoint[2]);
    }

    //Writing facets.
    for(INT i = 0 ; i < mesh.LargestFacetIndex()+1 ; i++ ) {
      if(offFacetMap[i] != -1) {
	cFACET *facet = mesh.Facet(i);

	INT numFacetVertices = facet->NumVertices();
	fprintf(file, "%d ", numFacetVertices);

	typename cFACET::vertex_circulator currFacetVertex = facet->VerticesBegin();
	typename cFACET::vertex_circulator lastFacetVertex = facet->VerticesEnd();

	for( ; currFacetVertex != lastFacetVertex ; currFacetVertex++ ) {
	  fprintf(file, "%d ", offVtxMap[currFacetVertex->Index()]);
	}

	sCOLOR color = filter.Color(i);
	fprintf(file, "%lf %lf %lf %lf \n", color.r, color.g, color.b, color.alpha);
      }
    }

    fclose(file);
  }

  template <typename _Mesh_Type>
   VOID ExportToOff(char *filename, iFACET facetIndex, _Mesh_Type &mesh)
   {
     sGENERIC_FACET_FILTER filter;
     filter.Add(facetIndex);

     ExportToOff<_Mesh_Type, sGENERIC_FACET_FILTER>(filename, mesh, filter);
   }

   template <typename _Mesh_Type>
   VOID ExportManifoldToOff(char *filename, _Mesh_Type &mesh, iMANIFOLD manifoldIndex)
   {
	   _TMESH_ENTITIES(_Mesh_Type);

	   sGENERIC_FACET_FILTER filter;
	   typename _Mesh_Type::cMANIFOLD *manifold = mesh.Manifold(manifoldIndex);
	   typename _Mesh_Type::cMANIFOLD::facet_iterator firstFacet = manifold->FacetsBegin();
	   typename _Mesh_Type::cMANIFOLD::facet_iterator lastFacet = manifold->FacetsEnd();

	   for( ; firstFacet != lastFacet ; firstFacet++)
		 filter.Add(firstFacet->Index());

	   ExportToOff<_Mesh_Type, sGENERIC_FACET_FILTER>(filename, mesh,filter);
   }

   template <typename _Mesh_Type>
   VOID ExportManifoldBorderFacetsToOff(char *filename, _Mesh_Type &mesh, iMANIFOLD manifoldIndex)
   {
	   _TMESH_ENTITIES(_Mesh_Type);

	   sGENERIC_FACET_FILTER filter;
	   typename _Mesh_Type::cMANIFOLD *manifold = mesh.Manifold(manifoldIndex);
	   typename _Mesh_Type::cMANIFOLD::facet_iterator firstFacet = manifold->FacetsBegin();
	   typename _Mesh_Type::cMANIFOLD::facet_iterator lastFacet = manifold->FacetsEnd();

	   for( ; firstFacet != lastFacet ; firstFacet++){
		 if (firstFacet->IsBorder())
		   filter.Add(firstFacet->Index());
	   }

	   ExportToOff<_Mesh_Type, sGENERIC_FACET_FILTER>(filename, mesh,filter);
   }
}

#endif// _MESH_TO_OFF_H_
