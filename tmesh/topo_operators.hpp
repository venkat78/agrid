#ifndef TMESH_TOPO_OPERATORS_HPP_
#define TMESH_TOPO_OPERATORS_HPP_

#include "defs.hpp"

namespace tmesh {
  template <typename _MESH_TYPE>
  class tTOPO_OPERATORS
  {
    _TMESH_ENTITIES(_MESH_TYPE);
  public:
    tTOPO_OPERATORS(_MESH_TYPE *mesh = NULL) : m_mesh(*mesh),
					       m_factory(m_mesh.Factory()),
					       m_nmEdges(m_mesh.NmEdges())
    {}
    
    /*
     * Inserts h2 and h2Opp into h1 edge-ring.
     */
    VOID MakeEdgeRing(cHALF_EDGE* h1,
		      cHALF_EDGE* h2,
		      BOOL markAsNonManifold);

    /*
     * Splits a half-edge and mate by introducing border edges.
     * Called while adding a facet and a new nm-edge is created.
     */
    VOID SplitManifoldEdge(cHALF_EDGE* he);

    /*
     * Removes half-edge and its mate from ring. Used mainly in
     * delete facets.
     */
    VOID RemoveFromEdgeRing(cHALF_EDGE *he);

    /*
     * Opposite of SplitManifoldEdge() removes border edges and
     * mates facet half-edges in the ring. Called while deleting
     * a facet.
     */
    VOID CollapseEdgeRing(cHALF_EDGE *he);

    /*
     * Introduces border edges as opposites of facet half-edges.
     * Used in cDELETE_FACET to break nm-edges before deleting
     * a facet.
     */
    VOID BreakNmEdge(cHALF_EDGE *he);

    VOID BreakNmEdges();

    VOID MateHalfEdges(cHALF_EDGE *he1, cHALF_EDGE *he2);

  public:
    /*
     * Euler operators to use outside cMESH class.
     */

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
    
  private:

    /*
     * Used in BreakNmEdge, splits two facet half-edges that are mates.
     */
    VOID BreakHePair(cHALF_EDGE *hePairToBreak,
		     cHALF_EDGE *nextHeInRing);

  public:
    //Methods which verify topology.
    VOID VerifyVertex(iVERTEX vertexIndex) ;
    VOID VerifyFacet(iFACET facetIndex);
    BOOL VerifyNmEdge(cHALF_EDGE *nmEdge, BOOL checkNmBit = true) ;
    VOID VerifyHalfEdge(cHALF_EDGE *halfEdge) ;

  private:
    _MESH_TYPE &m_mesh;
    cOBJECT_FACTORY &m_factory;
    cNM_EDGES &m_nmEdges;
  };
}

#endif /*SMESH_MESH_TOPO_OPERATORS_HPP_*/
