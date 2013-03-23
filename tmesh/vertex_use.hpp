#ifndef TMESH_VERTEX_USE_HPP_
#define TMESH_VERTEX_USE_HPP_

#include "defs.hpp"
#include "half_edge_circulators.hpp"

namespace tmesh {
  /*
   * Represents a 2-manifold incident on a vertex.
   */
  template <typename _Mesh_Entities>
  class tVERTEX_USE {
  public:
    _TMESH_TOPO_TYPES(_Mesh_Entities);
    
    typedef tHALF_EDGE_OPP_CIRCULATOR<_Mesh_Entities> cHALF_EDGE_OPP_CIRCULATOR;
    typedef tVERTEX_USE_FACET_CIRCULATOR<_Mesh_Entities> cVERTEX_USE_FACET_CIRCULATOR;
    
    typedef tCIRCULATOR< cHALF_EDGE_OPP_CIRCULATOR > half_edge_circulator;
    typedef tCIRCULATOR< cVERTEX_USE_FACET_CIRCULATOR > facet_circulator;
    
  public:
    tVERTEX_USE(cHALF_EDGE *h = NULL) : m_pHe(h)
    {}
  
    
  public:
    const cHALF_EDGE* HalfEdge() const {
      return m_pHe;
    }
    
    VOID HalfEdge(const cHALF_EDGE *h) {
      m_pHe = const_cast<cHALF_EDGE*>(h);
    }
    
    VOID Print(FILE *stream = stdout) const ;
    BOOL Verify() const ;
    BOOL IsEmpty() const { return m_pHe == NULL ? true : false; }
  public:
     /*
      * Methods for using circulators.
      */
     half_edge_circulator HalfEdgesBegin() const {
       return CirculatorBegin<cHALF_EDGE_OPP_CIRCULATOR>(m_pHe);
     }

     half_edge_circulator HalfEdgesEnd() const {
       return CirculatorEnd<cHALF_EDGE_OPP_CIRCULATOR>(m_pHe);
     }

     facet_circulator FacetsBegin() const {
       return CirculatorBegin<cVERTEX_USE_FACET_CIRCULATOR>(m_pHe);
     }

     facet_circulator FacetsEnd() const {
       return CirculatorEnd<cVERTEX_USE_FACET_CIRCULATOR>(m_pHe);
     }

  public:
    cHALF_EDGE *m_pHe;
  };
}
#endif /*TMESH_VERTEX_USE_HPP_*/
