#ifndef _TMESH_ADD_FACET_HPP_
#define _TMESH_ADD_FACET_HPP_

#include "defs.hpp"

namespace tmesh {
  template < typename _Mesh_Type >
  class tADD_FACET {
    _TMESH_ENTITIES(_Mesh_Type);

    //struct to support adding of facets.
    class sEDGE_INFO
    {
    public:
      bool isFirstHeOnV;

      //flags that represent status of halfedge
      //emanating from v.
      bool isANewPair;

      cHALF_EDGE* facetHe;
      cHALF_EDGE* borderHe;
      cVERTEX_USE vertexUse;

      sEDGE_INFO():isFirstHeOnV(false), isANewPair(false),
		   facetHe(NULL), borderHe(NULL)
      {}
    };

  public:
    tADD_FACET(_Mesh_Type *mesh = NULL) : m_mesh(*mesh),
					  m_facet(NULL),
					  m_facetIndex(mesh->CurrFacetIndex()),
					  m_facetVertices(mesh->CurrFacetVertices()),
					  m_numVertices(m_facetVertices.size()),
					  m_factory(m_mesh.Factory()),
					  m_topoOperators(mesh)
    {
    }

    ~tADD_FACET() {}

    iFACET Finish();

    VOID FindEdge(const iVERTEX vertexIndex1,
                  const iVERTEX vertexIndex2,
                  sEDGE_INFO *edgeInfo);

    VOID StitchEdgesAtVertex(sEDGE_INFO *edgeInfoOutOfV,
                             sEDGE_INFO *edgeInfoIntoV);
    
    VOID Construct();
    VOID CreateHalfEdgePair(sEDGE_INFO *edgeInfo,
                            cVERTEX *vertex1,
                            cVERTEX *vertex2);
    
  private:
    sEDGE_INFO m_edgeInfo[ARRAY_SIZE];
    _Mesh_Type &m_mesh;
    cFACET *m_facet;
    iFACET m_facetIndex;
    std::vector<iFACET> &m_facetVertices;
    INT m_numVertices;
    cOBJECT_FACTORY &m_factory;
    cTOPO_OPERATORS m_topoOperators;
  };
}

#endif /*MESH_ADD_FACET_HPP_*/
