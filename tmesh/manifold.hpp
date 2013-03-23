#ifndef _TMESH_MANIFOLD_HPP_
#define _TMESH_MANIFOLD_HPP_

#include "defs.hpp"
#include "manifold_facet_iterator.hpp"

namespace tmesh {
  enum eORIENTATION { ORIENTABLE = -1, ORIENTED, NON_ORIENTABLE } ;

  template <typename _Mesh_Type>
  class tCONSTRUCT_MANIFOLDS;

  template <typename _MESH_TYPE>
  class tMANIFOLD
  {
    typedef tMANIFOLD<_MESH_TYPE> cSELF;
    _TMESH_ENTITIES(_MESH_TYPE);
    friend class tCONSTRUCT_MANIFOLDS<_MESH_TYPE>;

  public:
    typedef tMANIFOLD_FACET_ITERATOR<_MESH_TYPE> facet_iterator;


    tMANIFOLD();
    ~tMANIFOLD();

    eORIENTATION Orientation() const { return m_orientation; }
    _MESH_TYPE* Mesh() const { return m_mesh; }

    INT NumFacets() const { return m_numFacets; }
    INT NumBorderEdges() const { return m_numBorderEdges; }
    INT Index() const { return m_index; }

    BOOL IsOriented() const { return m_orientation == ORIENTED; }
    BOOL IsNonOrientable() const { return m_orientation == NON_ORIENTABLE; }
    BOOL IsOpen() const { return m_numBorderEdges !=0; }
    BOOL IsAlmostClosed() const;
    REAL SurfaceArea() const ;
    REAL Volume() const ;
    cBOX3 BoundingBox() const ;

    facet_iterator FacetsBegin();
    facet_iterator FacetsEnd();
    VOID Index(INT index) { m_index = index; }
    VOID Mesh(_MESH_TYPE *mesh) { m_mesh = mesh; }

    VOID MarkDeleted() {
      m_isDeleted = true;
      m_numFacets = m_numBorderEdges = 0;
    }
    BOOL IsDeleted() { return m_isDeleted; }
  private:
    VOID NumFacets(INT numFacets) { m_numFacets = numFacets; }
    VOID NumBorderEdges(INT numBorderEdges) { m_numBorderEdges = numBorderEdges; }
    VOID Orientation(eORIENTATION orientation) { m_orientation = orientation; }

  private:
    BOOL m_isDeleted;
    iMANIFOLD m_index;
    INT m_numFacets;
    INT m_numBorderEdges;
    eORIENTATION m_orientation;
    _MESH_TYPE *m_mesh;
  };
}

#endif //_MESH_MANIFOLD_HPP_
