#ifndef _TMESH_TO_VTK_H_
#define _TMESH_TO_VTK_H_

#include "defs.hpp"
#include "tmesh_to_off.hpp"

namespace tmesh {

  template<typename _Mesh_Type, typename _FACET_FILTER>
  VOID ExportToVTK(char *filename, _Mesh_Type &mesh, _FACET_FILTER &filter) {
    _TMESH_ENTITIES(_Mesh_Type);

    FILE *file = fopen(filename, "wb+");

    std::map<iVERTEX, INT> offVtxMap;

    INT *offFacetMap = ::new INT[mesh.LargestFacetIndex() + 1];
    memset(offFacetMap, -1, sizeof(INT) * (mesh.LargestFacetIndex() + 1));

    INT numVertices = 0, numFacets = 0, totalFacetVertices = 0;
    typename _Mesh_Type::facet_iterator currFacet = mesh.FacetsBegin();
    typename _Mesh_Type::facet_iterator lastFacet = mesh.FacetsEnd();

    for (; currFacet != lastFacet; currFacet++) {
      if (filter(currFacet->Index())) {
        typename cFACET::vertex_circulator currFacetVertex = currFacet->VerticesBegin();
        typename cFACET::vertex_circulator lastFacetVertex = currFacet->VerticesEnd();

        for (; currFacetVertex != lastFacetVertex; currFacetVertex++) {
          totalFacetVertices++;
          if (offVtxMap.find(currFacetVertex->Index()) == offVtxMap.end()) {
            offVtxMap[currFacetVertex->Index()] = numVertices++;
          }
        }

        offFacetMap[currFacet->Index()] = numFacets++;
      }
    }

    typename std::map<iVERTEX, INT>::iterator currFacetVtx = offVtxMap.begin();
    typename std::map<iVERTEX, INT>::iterator lastFacetVtx = offVtxMap.end();

    std::vector<iVERTEX> vIndices;
    vIndices.resize(numVertices);

    for (; currFacetVtx != lastFacetVtx; currFacetVtx++) {
      vIndices[currFacetVtx->second] = currFacetVtx->first;
    }

    //Writing vertices and facets to off file.
    fprintf(file, "# vtk DataFile Version 1.0\n");
    fprintf(file, "%s\n",filename);
    fprintf(file, "ASCII\n");
    fprintf(file, "DATASET POLYDATA\n");
    fprintf(file, "POINTS %d float\n", numVertices);
//
//    fprintf(file, "%d %d 0\n", numVertices, numFacets);
//
    for (INT i = 0; i < numVertices; i++) {
      iVERTEX vertexIndex = vIndices[i];
      cPOINT3 vtxPoint = mesh.Vertex(vertexIndex)->Point();
      fprintf(file, "%lf %lf %lf\n", vtxPoint[0], vtxPoint[1], vtxPoint[2]);
    }

    fprintf(file, "POLYGONS %d %d\n", numFacets, (totalFacetVertices + numFacets));
    //Writing facets.
    for (INT i = 0; i < mesh.LargestFacetIndex() + 1; i++) {
      if (offFacetMap[i] != -1) {
        cFACET *facet = mesh.Facet(i);

        INT numFacetVertices = facet->NumVertices();
        fprintf(file, "%d ", numFacetVertices);

        typename cFACET::vertex_circulator currFacetVertex = facet->VerticesBegin();
        typename cFACET::vertex_circulator lastFacetVertex = facet->VerticesEnd();

        for (; currFacetVertex != lastFacetVertex; currFacetVertex++) {
          fprintf(file, "%d ", offVtxMap[currFacetVertex->Index()]);
        }

        fprintf(file, "\n");
      }
    }

    fclose(file);
  }

  template<typename _Mesh_Type>
  VOID ExportToVTK(char *filename, iFACET facetIndex, _Mesh_Type &mesh) {
    sGENERIC_FACET_FILTER filter;
    filter.Add(facetIndex);

    ExportToVTK<_Mesh_Type, sGENERIC_FACET_FILTER>(filename, mesh, filter);
  }

  template<typename _Mesh_Type>
  VOID ExportManifoldToVTK(char *filename, _Mesh_Type &mesh, iMANIFOLD manifoldIndex) {
    _TMESH_ENTITIES(_Mesh_Type);

    sGENERIC_FACET_FILTER filter;
    typename _Mesh_Type::cMANIFOLD *manifold = mesh.Manifold(manifoldIndex);
    typename _Mesh_Type::cMANIFOLD::facet_iterator firstFacet = manifold->FacetsBegin();
    typename _Mesh_Type::cMANIFOLD::facet_iterator lastFacet = manifold->FacetsEnd();

    for (; firstFacet != lastFacet; firstFacet++)
      filter.Add(firstFacet->Index());

    ExportToVTK<_Mesh_Type, sGENERIC_FACET_FILTER>(filename, mesh, filter);
  }

  template<typename _Mesh_Type>
  VOID ExportManifoldBorderFacetsToVTK(char *filename, _Mesh_Type &mesh, iMANIFOLD manifoldIndex) {
    _TMESH_ENTITIES(_Mesh_Type);

    sGENERIC_FACET_FILTER filter;
    typename _Mesh_Type::cMANIFOLD *manifold = mesh.Manifold(manifoldIndex);
    typename _Mesh_Type::cMANIFOLD::facet_iterator firstFacet = manifold->FacetsBegin();
    typename _Mesh_Type::cMANIFOLD::facet_iterator lastFacet = manifold->FacetsEnd();

    for (; firstFacet != lastFacet; firstFacet++) {
      if (firstFacet->IsBorder())
        filter.Add(firstFacet->Index());
    }

    ExportToVTK<_Mesh_Type, sGENERIC_FACET_FILTER>(filename, mesh, filter);
  }
}

#endif// _MESH_TO_VTK_H_
