/*
 * mesh_gen.hpp
 *
 *  Created on: Aug 28, 2009
 */

#ifndef TMESH_GEN_HPP_
#define TMESH_GEN_HPP_

#include <list>
#include <vector>
#include <map>

#include "defs.hpp"

namespace tmesh {
  /*
   * Populate the mesh with box.
   */
  template<typename _Mesh_Type>
  VOID BoxToMesh(const cBOX3 &box, _Mesh_Type &mesh) {
    iVERTEX boxVertices[8];

    for (INT i = 0; i < 8; i++)
      boxVertices[i] = mesh.NewVertex(box.CornerVertex(i));

    mesh.NewQuad(boxVertices[0], boxVertices[1], boxVertices[3], boxVertices[2]);  //xmin

    mesh.NewQuad(boxVertices[0], boxVertices[4], boxVertices[5], boxVertices[1]);  //ymin

    mesh.NewQuad(boxVertices[0], boxVertices[2], boxVertices[6], boxVertices[4]);  //zmin

    mesh.NewQuad(boxVertices[6], boxVertices[7], boxVertices[5], boxVertices[4]);  //xmax

    mesh.NewQuad(boxVertices[2], boxVertices[3], boxVertices[7], boxVertices[6]);  //ymax

    mesh.NewQuad(boxVertices[1], boxVertices[5], boxVertices[7], boxVertices[3]);  //zmax

  }

  /*
   * Makes new mesh from current facet list.
   */
  template<typename _Mesh_Type_A, typename _Mesh_Type_B>
  VOID MeshFromFacets(std::list<iFACET> &facetLabels, _Mesh_Type_A &oldMesh, _Mesh_Type_B &newMesh) {
    std::map<iVERTEX, iVERTEX> oldToNewVertexMap;

    //Add vertices of facet to new mesh and add facets to new mesh.
    typename std::list<iFACET>::iterator currFacetLabel = facetLabels.begin();
    typename std::list<iFACET>::iterator lastFacetLabel = facetLabels.end();

    //Adding vertices of facet to new mesh.
    for (; currFacetLabel != lastFacetLabel; currFacetLabel++) {
      typename _Mesh_Type_A::cFACET *facet = oldMesh.Facet(*currFacetLabel);
      if (!facet->IsDeleted()) {
        typename _Mesh_Type_A::cFACET::vertex_circulator currVtx = facet->VerticesBegin();
        typename _Mesh_Type_A::cFACET::vertex_circulator lastVtx = facet->VerticesEnd();

        for (; currVtx != lastVtx; currVtx++) {
          iVERTEX currVtxLabel = currVtx->Index();
          typename std::map<iVERTEX, iVERTEX>::iterator newVtxIndex = oldToNewVertexMap.find(currVtxLabel);
          if (newVtxIndex == oldToNewVertexMap.end()) {
            oldToNewVertexMap[currVtxLabel] = newMesh.NewVertex(oldMesh.Vertex(currVtxLabel)->Point());
          }
        }
      }
    }

    //Adding old facets to new mesh.
    currFacetLabel = facetLabels.begin();

    for (; currFacetLabel != lastFacetLabel; currFacetLabel++) {
      typename _Mesh_Type_A::cFACET *facet = oldMesh.Facet(*currFacetLabel);
      if (!facet->IsDeleted()) {

        typename _Mesh_Type_A::cFACET::vertex_circulator currVtx = facet->VerticesBegin();
        typename _Mesh_Type_A::cFACET::vertex_circulator lastVtx = facet->VerticesEnd();

        newMesh.NewFacetStart();

        for (; currVtx != lastVtx; currVtx++)
          newMesh.NewFacetAddVertex(oldToNewVertexMap.find(currVtx->Index())->second);

        newMesh.NewFacetClose();
      }
    }  //End of adding facets to new mesh.
  }

  /*
   * Makes new mesh from the manifold list.
   */
  template<typename _Mesh_Type_A, typename _Mesh_Type_B>
  VOID MeshFromManifolds(std::vector<iMANIFOLD> &manifoldLabels, _Mesh_Type_A &oldMesh, _Mesh_Type_B &newMesh) {
    std::list<iFACET> facetLabels;
    //Collect manifold facets .
    typename std::vector<iMANIFOLD>::iterator currManifoldLabel = manifoldLabels.begin();
    typename std::vector<iMANIFOLD>::iterator lastManifoldLabel = manifoldLabels.end();

    for (; currManifoldLabel != lastManifoldLabel; currManifoldLabel++) {
      typename _Mesh_Type_A::cMANIFOLD *manifold = oldMesh.Manifold(*currManifoldLabel);
      typename _Mesh_Type_A::cMANIFOLD::facet_iterator currFacet = manifold->FacetsBegin();
      typename _Mesh_Type_A::cMANIFOLD::facet_iterator lastFacet = manifold->FacetsEnd();

      for (; currFacet != lastFacet; currFacet++) {
        facetLabels.push_back(currFacet->Index());
      }
    }

    MeshFromFacets(facetLabels, oldMesh, newMesh);
    newMesh.ConstructManifolds();

  }
  /*
   * Makes new mesh from current vector list.
   */
  template<typename _Mesh_Type_A, typename _Mesh_Type_B>
  inline VOID MeshFromFacets(std::vector<iFACET> &facetLabels, _Mesh_Type_A &oldMesh, _Mesh_Type_B &newMesh) {
    std::list<iFACET> facets;

    for (INT i = 0; i < facetLabels.size(); i++) {
      facets.push_back(facetLabels[i]);
    }

    MeshFromFacets(facets, oldMesh, newMesh);
  }

  template<typename _Mesh_Type_A, typename _Mesh_Type_B>
  inline VOID MeshFromManifold(iMANIFOLD manifoldIndex, _Mesh_Type_A &oldMesh, _Mesh_Type_B &newMesh) {
    std::list<iFACET> facets;

    typename _Mesh_Type_A::cMANIFOLD *manifold = oldMesh.Manifold(manifoldIndex);
    typename _Mesh_Type_A::cMANIFOLD::facet_iterator currFacet = manifold->FacetsBegin();
    typename _Mesh_Type_A::cMANIFOLD::facet_iterator lastFacet = manifold->FacetsEnd();

    for (; currFacet != lastFacet; currFacet++) {
      facets.push_back(currFacet->GetIndex());
    }

    MeshFromFacets(facets, oldMesh, newMesh);
  }

  template<typename _Mesh_Type_A, typename _Mesh_Type_B>
  VOID Append(_Mesh_Type_A &srcMesh, _Mesh_Type_B &dstMesh) {
    std::map<iVERTEX, iVERTEX> oldToNewVertexMap;

    typename _Mesh_Type_A::vertex_iterator currVertex = srcMesh.VerticesBegin();
    typename _Mesh_Type_A::vertex_iterator lastVertex = srcMesh.VerticesEnd();

    for (; currVertex != lastVertex; currVertex++)
      oldToNewVertexMap[currVertex->Index()] = dstMesh.NewVertex(currVertex->Point());

    typename _Mesh_Type_A::facet_iterator currFacet = srcMesh.FacetsBegin();
    typename _Mesh_Type_A::facet_iterator lastFacet = srcMesh.FacetsEnd();

    for (; currFacet != lastFacet; currFacet++) {
      dstMesh.NewFacetStart();

      typename _Mesh_Type_A::cFACET::vertex_circulator currFacetVertex = currFacet->VerticesBegin();
      typename _Mesh_Type_A::cFACET::vertex_circulator lastFacetVertex = currFacet->VerticesEnd();

      for (; currFacetVertex != lastFacetVertex; currFacetVertex++) {
        dstMesh.NewFacetAddVertex(oldToNewVertexMap.find(currFacetVertex->Index())->second);
      }

      dstMesh.NewFacetClose();
    }
  }
}

#endif /* TMESH_GEN_HPP_ */
