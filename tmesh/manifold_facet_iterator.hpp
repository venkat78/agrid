#ifndef _TMESH_MANIFOLD_FACET_ITERATOR_HPP_
#define _TMESH_MANIFOLD_FACET_ITERATOR_HPP_

#include "defs.hpp"

namespace tmesh {
  template <typename _MESH_TYPE>
  class tMANIFOLD_FACET_ITERATOR 
  {
    _TMESH_ENTITIES(_MESH_TYPE);
  
  public:
    typedef tMANIFOLD_FACET_ITERATOR<_MESH_TYPE> cSELF;
  
	
    typedef cFACET value_type; 
    typedef cFACET& reference;
    typedef cFACET* pointer;

    tMANIFOLD_FACET_ITERATOR(_MESH_TYPE *mesh = NULL,
			     iFACET facetIndex = INVALID_IFACET,
			     iMANIFOLD index = INVALID_IMANIFOLD) : m_mesh(mesh),
								    m_facetIndex(facetIndex),
								    m_manifoldIndex(index)
    {}	
    
    BOOL operator==(const cSELF &obj) const
    {
      return (m_mesh == obj.m_mesh && 
	      m_facetIndex == obj.m_facetIndex && 
	      m_manifoldIndex == obj.m_manifoldIndex);
    }

    BOOL operator!=(const cSELF &obj) const
    {
      return !(*this == obj);
    }

    reference operator*()
    {
      return *(m_mesh->Facet(m_facetIndex));
    }

    pointer operator->()
    {
      return m_mesh->Facet(m_facetIndex);
    }

    const reference operator*() const
    {
      return  *(m_mesh->Facet(m_facetIndex));
    }
    
    const pointer operator->() const
    {
      return m_mesh->Facet(m_facetIndex);
    }

    cSELF& operator++()
    {
      do {
	m_facetIndex++;
      }while(m_facetIndex <= m_mesh->LargestFacetIndex() &&
	      (*this)->ManifoldIndex() != m_manifoldIndex);
      
      return *this;
    }

    cSELF&
    operator++(int)
    {
      return ++*this;
    }

  private:
    _MESH_TYPE *m_mesh;
    iFACET m_facetIndex;
    iMANIFOLD m_manifoldIndex;
  };

}


#endif //_MANIFOLD_FACET_ITERATOR_HPP_
