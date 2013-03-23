/*
 * half_edge_circulators.hpp
 *
 *  Created on: Jun 6, 2009
 */

#ifndef THALF_EDGE_CIRCULATORS_HPP_
#define THALF_EDGE_CIRCULATORS_HPP_

#include "defs.hpp"

namespace tmesh {
  template <typename _Mesh_Entities>
  class tHALF_EDGE_NEXT_CIRCULATOR
  {
 public:
    _TMESH_TOPO_TYPES(_Mesh_Entities);

    typedef tHALF_EDGE_NEXT_CIRCULATOR<_Mesh_Entities> cSELF;
    typedef cHALF_EDGE value_type;
    typedef value_type& reference;
    typedef cHALF_EDGE* pointer;

  public:
    /*
     * Should never be called. Just to make the
     * compiler happy.
     */
    tHALF_EDGE_NEXT_CIRCULATOR(cHALF_EDGE *halfEdge = NULL) : m_halfEdge(halfEdge)
    {}

    cSELF& operator++() {
      m_halfEdge = m_halfEdge->Next();
      return *this;
    }

    cSELF& operator++(int) {
      return ++*this;
    }

    BOOL operator==(const cSELF& circulator) const
    {
      return m_halfEdge == circulator.m_halfEdge;
    }

    BOOL operator!=(const cSELF &circulator) const
    {
      return !(*this == circulator);
    }

    reference operator*() { return *m_halfEdge; }
    pointer operator->() { return m_halfEdge; }

    const reference operator*() const { return reference(*m_halfEdge); }
    const pointer operator->() const { return (cHALF_EDGE*)(m_halfEdge); }

  protected:
    cHALF_EDGE* m_halfEdge;
  };

  template <typename _Mesh_Entities>
  class tHALF_EDGE_OPP_CIRCULATOR : public tHALF_EDGE_NEXT_CIRCULATOR<_Mesh_Entities>
  {
	  _TMESH_TOPO_TYPES(_Mesh_Entities);
  public:
    typedef tHALF_EDGE_OPP_CIRCULATOR<_Mesh_Entities> cSELF;
    typedef tHALF_EDGE_NEXT_CIRCULATOR<_Mesh_Entities> cBASE;
    
    typedef typename cBASE::value_type value_type;
    typedef typename cBASE::reference reference;
    typedef typename cBASE::pointer pointer;
    
    public:
    tHALF_EDGE_OPP_CIRCULATOR(cHALF_EDGE* halfEdge = NULL) : cBASE(halfEdge)
    {}
    
    cSELF& operator++() {
      cBASE::m_halfEdge = (cBASE::m_halfEdge)->Opp()->Next();
      return *this;     
    }

    cSELF& operator++(int) {
      return ++*this;
    }
  };

  template< typename _Mesh_Entities>
  class tHALF_EDGE_RING_CIRCULATOR : public tHALF_EDGE_NEXT_CIRCULATOR<_Mesh_Entities>
  {
	  _TMESH_TOPO_TYPES(_Mesh_Entities);
    public:
      typedef tHALF_EDGE_RING_CIRCULATOR<_Mesh_Entities> cSELF;
      typedef tHALF_EDGE_NEXT_CIRCULATOR<_Mesh_Entities> cBASE;

      typedef typename cBASE::value_type value_type;
      typedef typename cBASE::reference reference;
      typedef typename cBASE::pointer pointer;

    public:
    tHALF_EDGE_RING_CIRCULATOR(cHALF_EDGE* halfEdge = NULL) : cBASE(halfEdge)
    {}

    cSELF& operator++() {
      (cBASE::m_halfEdge) = (cBASE::m_halfEdge)->NextInRing();
      return *this;
    }

    cSELF& operator++(int) {
      return ++*this;
    }
  };

  //typedef cHALF_EDGE_NEXT_CIRCULATOR cFACET_HALF_EDGE_CIRCULATOR;
#define tFACET_HALF_EDGE_CIRCULATOR tHALF_EDGE_NEXT_CIRCULATOR

  template <typename _Mesh_Entities>
  class tFACET_VERTEX_CIRCULATOR : public tFACET_HALF_EDGE_CIRCULATOR<_Mesh_Entities>
  {
	  _TMESH_TOPO_TYPES(_Mesh_Entities);
  public:
    typedef tFACET_VERTEX_CIRCULATOR<_Mesh_Entities> cSELF;
    typedef tFACET_HALF_EDGE_CIRCULATOR<_Mesh_Entities> cBASE;

    typedef cVERTEX value_type;
    typedef value_type& reference;
    typedef cVERTEX* pointer;

  public:
    tFACET_VERTEX_CIRCULATOR(cHALF_EDGE *halfEdge = NULL) : cBASE(halfEdge)
    {}

    reference operator*() {
      m_vertex = (cBASE::m_halfEdge)->Tail();
      return *m_vertex;
    }

    pointer operator->() {
      m_vertex = (cBASE::m_halfEdge)->Tail();
      return m_vertex;
    }

    const reference operator*() const {
      m_vertex = (cBASE::m_halfEdge)->Tail();
      return reference(m_vertex);
    }

    const pointer operator->() const {
      m_vertex = (cBASE::m_halfEdge)->Tail();
      return pointer(&m_vertex);
    }

  private:
    mutable cVERTEX *m_vertex;
  };

  template <typename _Mesh_Entities>
  class tVERTEX_USE_FACET_CIRCULATOR : public tHALF_EDGE_OPP_CIRCULATOR<_Mesh_Entities>
  {
	  _TMESH_TOPO_TYPES(_Mesh_Entities);
  public:
    typedef tVERTEX_USE_FACET_CIRCULATOR<_Mesh_Entities> cSELF;
    typedef tHALF_EDGE_OPP_CIRCULATOR<_Mesh_Entities> cBASE;

    typedef cFACET value_type;
    typedef value_type& reference;
    typedef cFACET* pointer;

  public:
    tVERTEX_USE_FACET_CIRCULATOR(cHALF_EDGE *halfEdge = NULL) {
      ASSERT(halfEdge != NULL);
      (cBASE::m_halfEdge) = halfEdge;
      m_facet = cBASE::m_halfEdge->Facet();
      ASSERT(m_facet);
    }

    reference operator*() { return *m_facet; }
    pointer operator->() { return m_facet; }

    const reference operator*() const { return reference(m_facet); }
    const pointer operator->() const { return  pointer(m_facet); }

    cSELF& operator++() {
      do {
	cBASE::operator++();
	m_facet = cBASE::m_halfEdge->Facet();
      } while(m_facet == NULL);
      
      return *this;
    }

    cSELF& operator++(int) {
      return ++*this;
    }

  public:
    mutable cFACET *m_facet;
  };
}

#endif /* THALF_EDGE_CIRCULATORS_HPP_ */
