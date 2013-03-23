#ifndef TMESH_OBJECT_FACTORY_HPP_
#define TMESH_OBJECT_FACTORY_HPP_

#include "defs.hpp"

namespace tmesh {
  template <typename _Mesh_Entities,
	    typename _Storage_Policy>
  class tMESH_OBJECT_FACTORY {
    typedef tMESH_OBJECT_FACTORY<_Mesh_Entities, _Storage_Policy> cSELF;

  public:
    _TOBJECT_FACTORY_ENTITIES(_Mesh_Entities);

    //Defining iterators to walk meshes defined.
    typedef typename tTABLE<cVERTEX>::iterator vertex_iterator;
    typedef typename tTABLE<cHALF_EDGE>::iterator half_edge_iterator;
    typedef typename  tTABLE<cFACET>::iterator facet_iterator;
    //typedef tTABLE<cMANIFOLD>::iterator manifold_iterator;

  public:
    tMESH_OBJECT_FACTORY() : m_vertices(_Storage_Policy::numObjectsInPage, 
				   _Storage_Policy::logOfObjectsInPage, 
				   _Storage_Policy::numInitialPages, 0),
			m_halfEdges(_Storage_Policy::numObjectsInPage, 
				    _Storage_Policy::logOfObjectsInPage, 
				    _Storage_Policy::numInitialPages, 0),
			m_facets(_Storage_Policy::numObjectsInPage, 
				 _Storage_Policy::logOfObjectsInPage, 
				 _Storage_Policy::numInitialPages, 0),
			m_freeHes(NULL)
    {}

    ~tMESH_OBJECT_FACTORY() {}
    /*
     * Creates a new vertex.
     */
    cVERTEX* NewVertex() {
      cVERTEX* vtx = m_vertices.new_object();
      vtx->Index(m_vertices.size() - 1);
      return vtx;
    }

    /*
     * Provides access to a vertex object.
     */
    cVERTEX* Vertex(iVERTEX vtxIndex) {
      ASSERT(vtxIndex >= 0 && vtxIndex < m_vertices.size());
      return m_vertices.object_at(vtxIndex);
    }

    INT NumVertices() const {
      return m_vertices.size();
    }

    /*
     * Creates a new facet.
     */
    cFACET* NewFacet() {
      cFACET* facet = m_facets.new_object();
      facet->Index(m_facets.size() - 1);
      return facet;
    }

    /*
     * Copies a facet to a new facet.
     */
    cFACET* NewFacet(const cFACET &f)
    {
      m_facets.push_back(f);
      cFACET* facet = m_facets.object_at(m_facets.size() - 1);
      facet->Index(m_facets.size() - 1);
      facet->HalfEdge(NULL);
      return facet;
    }

    /*
     * Returns access to a facet object.
     */
    cFACET* Facet(iFACET facetIndex) {
      ASSERT(facetIndex >= 0 && facetIndex < m_facets.size());
      return m_facets.object_at(facetIndex);
    }

    INT NumFacets() const {
      return m_facets.size();
    }


    INT NumHalfEdges() const {
      return m_halfEdges.size();
    }

    /*
     * Returns a half-edge for consumption.
     */
    cHALF_EDGE* HalfEdge();

    /*
     * Return half-edge to factory to recycle.
     */
    VOID Return(cHALF_EDGE* he);

    /*
     *  Creates a half-edge and its mate.
     */
    cHALF_EDGE* CreateAPair(const cVERTEX* v1,
                            const cVERTEX* v2);

  private:
    cHALF_EDGE* NewHalfEdge() {
      return m_halfEdges.new_object();
    }

  public:
    /*
     * Methods related to iterators.
     */
    vertex_iterator VerticesBegin() {
      return m_vertices.begin();
    }

    vertex_iterator VerticesEnd() {
      return m_vertices.end();
    }

    half_edge_iterator HalfEdgesBegin() {
      return m_halfEdges.begin();
    }

    half_edge_iterator HalfEdgesEnd() {
      return m_halfEdges.end();
    }

    facet_iterator FacetsBegin() {
      return m_facets.begin();
    }

    facet_iterator FacetsEnd() {
      return m_facets.end();
    }

    //Methods shouldn't be exposed but overriding for prototype.
  public:
    tTABLE<cVERTEX>& Vertices() { return m_vertices; }

    VOID Clear() {
      m_vertices.flush();
      m_halfEdges.flush();
      m_facets.flush();
      m_freeHes = NULL;
    }

  protected:
    tTABLE<cVERTEX> m_vertices;
    tTABLE<cHALF_EDGE> m_halfEdges;
    tTABLE<cFACET> m_facets;
    cHALF_EDGE *m_freeHes;
  };
}

#endif /*TMESH_OBJECT_FACTORY_HPP_*/
