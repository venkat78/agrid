/*
 * tmesh_def.hpp
 *
 *  Created on: Jul 26, 2010
 */

#ifndef TMESH_DEF_HPP_
#define TMESH_DEF_HPP_

#include "edge.hpp"
#include "vertex_use.hpp"
#include "iterators.hpp"
#include "manifold.hpp"
#include "misc.hpp"

namespace tmesh {

  template<typename _Mesh>
  class tTOPO_OPERATORS;

  template<typename _Mesh_Entities, typename _Storage_Policy,

  template<typename _ME, typename _SP> class _Object_Factory>
  class tMESH {
     public:
      typedef tMESH<_Mesh_Entities, _Storage_Policy, _Object_Factory> cSELF;
      /*
       * Typedefs for external consumption.
       */
      _TMESH_TOPO_TYPES(_Mesh_Entities);

      typedef tTOPO_OPERATORS<cSELF> cTOPO_OPERATORS;
      _TMESH_EXPORTED_TYPES;

      public:
      /*
       * External API.
       */
      tMESH() : m_numDeletedVertices(0), m_numDeletedFacets(0),
      m_numDeletedManifolds(0),
      m_manifolds(_Storage_Policy::numObjectsInPage,
          _Storage_Policy::logOfObjectsInPage,
          _Storage_Policy::numInitialPages, 0)
      {}

      virtual ~tMESH()
      {}

      iVERTEX NewVertex(REAL x, REAL y, REAL z);
      iVERTEX NewVertex(const cPOINT3& point);

      cVERTEX* Vertex(iVERTEX iVertex);
      const cVERTEX* Vertex(iVERTEX iVertex) const;

      INT NumVertices() const;  //Skips deleted vertices.
      INT LargestFacetIndex() {
        return m_factory.NumFacets() - 1;
      }

      INT NumBorderEdges() {
        half_edge_iterator currHalfEdge = HalfEdgesBegin();
        half_edge_iterator lastHalfEdge = HalfEdgesEnd();
        INT numBorderEdges = 0;;

        for(; currHalfEdge != lastHalfEdge; currHalfEdge++) {
          if (currHalfEdge->IsBorder())
          numBorderEdges++;
        }
        return numBorderEdges;
      }

      INT LargestVertexIndex() {
        return m_factory.NumVertices() - 1;
      }

      cHALF_EDGE* BorderHalfEdge(iMANIFOLD manifoldIndex = INVALID_IMANIFOLD);  //returns a border half edge

      //Methods for adding facets.
      VOID NewFacetStart(iFACET facetIndex = INVALID_IFACET);
      VOID NewFacetAddVertex(iVERTEX vertexIndex);
      iFACET NewFacetClose();

      cFACET* Facet(iFACET facetIndex);
      const cFACET* Facet(iFACET facetIndex) const;

      INT NumFacets() const;

      iFACET NewTriangle(iVERTEX v1, iVERTEX v2, iVERTEX v3,
          iFACET facetIndex = INVALID_IFACET);

      iFACET NewQuad(const iVERTEX v1, const iVERTEX v2,
          const iVERTEX v3, const iVERTEX v4,
          const iFACET facetIndex = INVALID_IFACET);
      VOID MergeCoplanarfacets();

      BOOL DeleteFacet(iFACET facetIndex);
      BOOL FlipFacet(iFACET facetIndex);

      INT ConstructManifolds();
      BOOL DeleteManifold(iMANIFOLD index);
      //Methods related to manifolds.
      cMANIFOLD *NewManifold() {
        cMANIFOLD* manifold = m_manifolds.new_object();
        manifold->Index(m_manifolds.size() - 1);
        manifold->Mesh(this);
        return manifold;
      }

      cMANIFOLD* Manifold(iMANIFOLD index) {
        ASSERT(index >= 0 && index < m_manifolds.size());
        return m_manifolds.object_at(index);
      }

      INT NumManifolds() const {
        return m_manifolds.size() - m_numDeletedManifolds;
      }

      INT LargestManifoldIndex() const {
        return m_manifolds.size() - 1;
      }
      public:
      //Euler operators.
      /*
       * Splits an existing facet by inserting a diagonal.
       */
      iFACET InsertDiagonal(iFACET facetIndex,
          iVERTEX diagEndPoint1,
          iVERTEX diagEndPoint2,
          iFACET newFacetIndex = INVALID_IFACET);

      /*
       * Removes a diagonal and joins two adjacent facets.
       */
      BOOL RemoveDiagonal(iVERTEX diagEndPoint1, iVERTEX diagEndPoint2,
          iFACET facetIndex1, iFACET facetIndex2,
          iFACET facetIndexToBeDeleted);

      BOOL SplitEdge(iVERTEX vertexToBeInserted,
          iVERTEX edgeTailVtxIndex,
          iVERTEX edgeHeadVtxIndex);

      BOOL MergeEdgesAtVertex(iVERTEX vertexIndex);

      BOOL MergeVertices(iVERTEX v1Index, iVERTEX v2Index,
          iVERTEX retainVtxIndex, const cPOINT3 &newVtxPoint,
          sMV_TRANS *transaction = NULL);

      BOOL MergeVertices(iVERTEX v1Index, iVERTEX v2Index);

      public:
      //More complex geometry operators.
      VOID Clip(iFACET facetIndex, eCOORD coord,
          REAL minVal, REAL maxVal, REAL step);

      iFACET Clip(iFACET facetIndex, eCOORD coord, REAL coordVal);

      VOID GetFacetVertices(iFACET facetIndex, eCOORD coord, REAL coordVal, iVERTEX v[2]);

      /*
       * Clips facet by the segment. The segment endpoints are assumed to be
       * on the boundary of the facet.
       */
      iFACET Clip(iFACET facetIndex, const cSEGMENT3 &segment);

      public:
      /*
       *  Geometry Info..
       */
      cBOX3 BoundingBox();
      REAL ShadeArea(eCOORD coord);
      cHALF_EDGE* MostConcaveEdge();

      public:
      /*
       * Iterators to walk objects in a mesh.
       */
      typedef tOBJECTS_ITERATOR<typename cOBJECT_FACTORY::vertex_iterator> vertex_iterator;
      typedef tOBJECTS_ITERATOR<typename cOBJECT_FACTORY::half_edge_iterator> half_edge_iterator;
      typedef tOBJECTS_ITERATOR<typename cOBJECT_FACTORY::facet_iterator> facet_iterator;
      typedef tOBJECTS_ITERATOR<typename tTABLE<cMANIFOLD>::iterator> manifold_iterator;
      //    typedef cOBJECT_FACTORY::manifold_iterator manifold_iterator;

      vertex_iterator VerticesBegin() {
        vertex_iterator firstVtx(m_factory.VerticesBegin(), m_factory.NumVertices());
        vertex_iterator lastVtx = VerticesEnd();

        while(firstVtx != lastVtx && firstVtx->IsDeleted() )
        firstVtx++;

        return firstVtx;
      }

      vertex_iterator VerticesEnd() {
        return vertex_iterator(m_factory.VerticesEnd(), m_factory.NumVertices());
      }

      half_edge_iterator HalfEdgesBegin() {
        half_edge_iterator firstHe(m_factory.HalfEdgesBegin(), m_factory.NumHalfEdges());
        half_edge_iterator lastHe = HalfEdgesEnd();

        while(firstHe != lastHe && firstHe->IsDeleted() )
        firstHe++;

        return firstHe;
      }

      half_edge_iterator HalfEdgesEnd() {
        return half_edge_iterator(m_factory.HalfEdgesEnd(), m_factory.NumHalfEdges());
      }

      facet_iterator FacetsBegin() {
        facet_iterator firstFacet(m_factory.FacetsBegin(), m_factory.NumFacets());
        facet_iterator lastFacet = FacetsEnd();

        while(firstFacet != lastFacet && firstFacet->IsDeleted() )
        firstFacet++;

        return firstFacet;
      }

      facet_iterator FacetsEnd() {
        return facet_iterator(m_factory.FacetsEnd(), m_factory.NumFacets());
      }

      manifold_iterator ManifoldsBegin() {
        manifold_iterator firstManifold(m_manifolds.begin(), m_manifolds.size());
        manifold_iterator lastManifold = ManifoldsEnd();

        while(firstManifold != lastManifold && firstManifold->IsDeleted() )
        firstManifold++;

        return firstManifold;
      }

      manifold_iterator ManifoldsEnd() {
        return manifold_iterator(m_manifolds.end(), m_manifolds.size());
      }

      public:
      //Dump methods.
      VOID Print(FILE *stream=stdout, BOOL longPrint = false);

      VOID PrintVertices(FILE *stream = stdout);
      VOID PrintHalfEdges(FILE *stream = stdout);
      VOID PrintBorderEdges(FILE *stream = stdout);
      VOID PrintFacets(FILE *stream = stdout, BOOL longPrint = false);

      BOOL Verify();

      BOOL VerifyVertexIndex(iVERTEX vertexIndex) const {
        ASSERT(vertexIndex >= 0 && vertexIndex < m_factory.NumVertices());
        return vertexIndex >= 0 && vertexIndex < m_factory.NumVertices();
      }

      BOOL VerifyFacetIndex(iFACET facetIndex) const {
        ASSERT(facetIndex >= 0 && facetIndex < m_factory.NumFacets());
        return facetIndex >= 0 && facetIndex < m_factory.NumFacets();
      }

      BOOL VerifyVertices();
      BOOL VerifyFacets();

      public:
      //Support methods.
      cOBJECT_FACTORY& Factory() {
        return m_factory;
      }

      cNM_EDGES& NmEdges() {
        return m_nmEdges;
      }

      std::vector<iVERTEX> & CurrFacetVertices() {
        return m_facetVertices;
      }

      iFACET CurrFacetIndex() {
        return m_facetIndex;
      }

      VOID IncrementNumDeletedFacets() {
        m_numDeletedFacets++;
      }

      VOID IncrementNumDeletedVertices() {
        m_numDeletedVertices++;
      }

      VOID DecrementNumDeletedFacets() {
        m_numDeletedFacets--;
      }

      VOID DecrementNumDeletedVertices() {
        m_numDeletedVertices--;
      }

      //Empties the mesh but retains the storage.
      VOID Clear();

      VOID ClearManifolds();

      private:
      /*
       * Friend classes which support functionality in MESH.
       */
      friend class tTOPO_OPERATORS<cSELF>;

      /*
       * Private methods used in support classes for tMESH.
       */
      private:

      public:
      /*
       * Member variables.
       */
      cOBJECT_FACTORY m_factory;
      iFACET m_facetIndex;
      std::vector<iFACET> m_facetVertices;
      INT m_numDeletedVertices;
      INT m_numDeletedFacets;
      INT m_numDeletedManifolds;
      cNM_EDGES m_nmEdges;

      public:
      //Container for manifolds.
      tTABLE<cMANIFOLD> m_manifolds;
    };

#define _TMESH_TMPL_TYPE			\
  template <typename _Mesh_Entities,		\
	    typename _Storage_Policy, \
		template <typename _ME, typename _SP> \
		class _Object_Factory >

#define _TMESH_TMPL_DECL			\
  tMESH<_Mesh_Entities, _Storage_Policy, _Object_Factory>
  }

#endif /* TMESH_DEF_HPP_ */
