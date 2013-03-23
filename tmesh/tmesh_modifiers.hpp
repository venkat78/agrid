/*
 * mesh_modifiers.hpp
 *
 */

#ifndef TMESH_MODIFIERS_HPP_
#define TMESH_MODIFIERS_HPP_

namespace tmesh {
  template <typename _Mesh_Type>
  BOOL MergeIdenticalVertices(_Mesh_Type &mesh, REAL tolerance=1e-8)
{
  _TMESH_ENTITIES(_Mesh_Type);

  /*
   * Using octree is much better than the sort used below.
   */
  std::vector<iVERTEX> vIndices;
  tTABLE<cVERTEX> &meshVertices = mesh.Factory().Vertices();
  SortedIndices<cVERTEX>(meshVertices, vIndices);

  INT numVertices = vIndices.size();
  INT baseIndex = vIndices[0];

  for (INT i = 1 ; i < numVertices ; i++) {
    INT currIndex = vIndices[i];

    if(meshVertices[baseIndex] == meshVertices[currIndex]) {
	mesh.MergeVertices(baseIndex, currIndex, baseIndex, mesh.Vertex(baseIndex)->Point());
    }
    else
	baseIndex = currIndex;
  }

  return true;
}
}

#endif /* MESH_MODIFIERS_HPP_ */
