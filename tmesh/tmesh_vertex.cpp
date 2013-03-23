#include "defs.hpp"
#include "tmesh_def.hpp"

namespace tmesh {
  _TMESH_TMPL_TYPE
  INT _TMESH_TMPL_DECL::NumVertices() const
  {
    return m_factory.NumVertices() - m_numDeletedVertices;
  }

  _TMESH_TMPL_TYPE
  iVERTEX _TMESH_TMPL_DECL::NewVertex(REAL x, REAL y, REAL z)
  {
    return NewVertex(cPOINT3(x, y, z));
  }
  
  _TMESH_TMPL_TYPE
  iVERTEX _TMESH_TMPL_DECL::NewVertex(const cPOINT3 &point)
  {
    cVERTEX* v = NULL;
    v = ::new (m_factory.NewVertex())cVERTEX(point);
    return v->Index();
  }
  
  _TMESH_TMPL_TYPE
  typename _TMESH_TMPL_DECL::cVERTEX* 
  _TMESH_TMPL_DECL::Vertex(const iVERTEX vtxIndex)
  {
    if(vtxIndex < m_factory.NumVertices())
      return m_factory.Vertex(vtxIndex);
    else
      return NULL;
  }
}
