#include "manifold.hpp"

namespace tmesh {
  template <typename _MESH_TYPE>
  tMANIFOLD<_MESH_TYPE>::tMANIFOLD()
  {
    m_isDeleted=false;
    m_orientation = ORIENTED;
    m_numFacets = m_numBorderEdges = 0;
    m_mesh = NULL;
  }

  template <typename _MESH_TYPE>  
  tMANIFOLD<_MESH_TYPE>::~tMANIFOLD()
  {
    m_mesh = NULL;
  }

  template <typename _MESH_TYPE>  
  typename tMANIFOLD<_MESH_TYPE>::facet_iterator
  tMANIFOLD<_MESH_TYPE>::FacetsBegin()
  {
    typename _MESH_TYPE::facet_iterator currFacet = m_mesh->FacetsBegin();
    typename _MESH_TYPE::facet_iterator lastFacet = m_mesh->FacetsEnd();

    for( ; currFacet != lastFacet ; currFacet++ ) {
      if(currFacet->ManifoldIndex() == m_index)
	break;
    }

    return facet_iterator(m_mesh, currFacet->Index(), m_index);
  }

  template <typename _MESH_TYPE>  
  typename tMANIFOLD<_MESH_TYPE>::facet_iterator
  tMANIFOLD<_MESH_TYPE>::FacetsEnd()
  {
    return facet_iterator(m_mesh, m_mesh->LargestFacetIndex() + 1, m_index);
  }
}
