#ifndef MESH_DELETE_FACET_HPP_
#define MESH_DELETE_FACET_HPP_

#include "defs.hpp"

namespace tmesh {
  template <typename _Mesh_Type>
  class tDELETE_FACET {
    _TMESH_ENTITIES(_Mesh_Type);

    enum eEDGE_TYPE {
      BOUNDARY,
      TWO_MANIFOLD,
      NON_MANIFOLD,
      MISORIENTED
    };
    
    struct sEDGE_INFO {
      cHALF_EDGE* edge;
      eEDGE_TYPE edgeType;
    };

  public:
    tDELETE_FACET(_Mesh_Type *mesh = NULL,
		  iFACET facetIndex = INVALID_IFACET) : m_mesh(*mesh),
							m_factory(m_mesh.Factory()),
							m_nmEdges(m_mesh.NmEdges()),
							m_facetIndex(facetIndex),
							m_topoOperators(mesh)

    {}

    VOID Finish();

  private:
    VOID ModifyTopology();
    VOID ModifyVertexUses(sEDGE_INFO &edgeOut,
                          sEDGE_INFO &edgeIn);
    VOID ModifyBorderEdgesAtVertex(sEDGE_INFO &edgeOut,
                                   sEDGE_INFO &edgeIn);
    VOID ModifyEdgeRings();
    VOID ReturnFreeEdges();

  private:
    _Mesh_Type &m_mesh;
    cOBJECT_FACTORY &m_factory;
    cNM_EDGES &m_nmEdges;
    iFACET m_facetIndex;
    std::vector<sEDGE_INFO> m_edgeInfo;
    cTOPO_OPERATORS m_topoOperators;
  };
}

#endif /*MESH_DELETE_FACET_HPP_*/
