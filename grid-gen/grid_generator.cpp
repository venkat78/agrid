#include <vector>
#include "grid_generator.hpp"

namespace grid_gen {
/*
 * Methods to support registration of a model in coarse grid.
 */
template<typename _MODEL_TYPE>
VOID tGRID_GENERATOR<_MODEL_TYPE>::Register(_MODEL_TYPE *model) {
  typename _MODEL_TYPE::mesh_iterator currMesh = model->MeshesBegin();
  typename _MODEL_TYPE::mesh_iterator lastMesh = model->MeshesEnd();

  for (; currMesh != lastMesh; currMesh++) {
    Register(currMesh.operator->());
  }
}

template<typename _MODEL_TYPE>
VOID tGRID_GENERATOR<_MODEL_TYPE>::Register(cMESH *mesh) {
  Slice(mesh);

  INT numManifoldRecordsOld = m_manifolds.size();

  //Create record for each manifold.
  typename cMESH::manifold_iterator currManifold = mesh->ManifoldsBegin();
  typename cMESH::manifold_iterator lastManifold = mesh->ManifoldsEnd();

  for (; currManifold != lastManifold; currManifold++) {
    RegisterManifold(currManifold.operator->(), mesh);
  }

  INT numManifoldRecordsNew = m_manifolds.size();

  /*
   * Method colors the global grid and individual grid for
   * each manifold.
   */
//  PaintCurrManifoldGrids(mesh, numManifoldRecordsOld);
}

template<typename _MODEL_TYPE>
VOID tGRID_GENERATOR<_MODEL_TYPE>::Slice(cMESH *mesh) {
  std::vector<iFACET> facetIndices;

  typename cMESH::facet_iterator currFacet = mesh->FacetsBegin();
  typename cMESH::facet_iterator lastFacet = mesh->FacetsEnd();

  for (; currFacet != lastFacet; currFacet++)
    facetIndices.push_back(currFacet->Index());

  std::vector<iFACET>::iterator currIndex = facetIndices.begin();
  std::vector<iFACET>::iterator lastIndex = facetIndices.end();

  for (; currIndex != lastIndex; currIndex++) {
    cBOX3 facetBox = mesh->Facet(*currIndex)->BoundingBox();
    std::vector<iFACET> facetsToBeClipped;
    facetsToBeClipped.push_back(*currIndex);

    DO_COORDS(coord)
    {
      iFACET oldLargestFacetIndex = mesh->LargestFacetIndex();
      std::vector<iFACET>::iterator currFacetToBeClipped = facetsToBeClipped
          .begin();
      std::vector<iFACET>::iterator lastFacetToBeClipped =
          facetsToBeClipped.end();

      REAL step = m_grid.CellLength(coord);
      for (; currFacetToBeClipped != lastFacetToBeClipped;
          currFacetToBeClipped++) {
        //Clips facet along the lines parallel to coordinate axis.
        REAL minVal = m_grid.CellUpperBound(facetBox.MinCoord(coord), coord);
        REAL maxVal = m_grid.CellUpperBound(facetBox.MaxCoord(coord), coord);

        mesh->Clip(*currFacetToBeClipped, coord, minVal, maxVal, step);
      }

      iFACET newLargestFacetIndex = mesh->LargestFacetIndex();
      for (INT facetIndex = oldLargestFacetIndex + 1;
          facetIndex <= newLargestFacetIndex; facetIndex++)
        facetsToBeClipped.push_back(facetIndex);
    }
  }
}

template<typename _MODEL_TYPE>
VOID tGRID_GENERATOR<_MODEL_TYPE>::RegisterManifold(cMANIFOLD *manifold,
                                                    cMESH *mesh) {
  cMANIFOLD_RECORD *record = m_manifolds.new_object();
  ::new (record) cMANIFOLD_RECORD(manifold, mesh);
  record->Index(m_manifolds.size() - 1);

}

}
