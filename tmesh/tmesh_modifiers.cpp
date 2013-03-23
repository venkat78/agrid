/*
 * mesh_modifiers.cpp
 *
 */
#include <vector>
#include <algorithm>
#include "tmesh_def.hpp"
#include "tmesh_modifiers.hpp"

namespace tmesh {
  _TMESH_TMPL_TYPE
  VOID _TMESH_TMPL_DECL::Clear() 
  {
    m_factory.Clear();
    m_nmEdges.clear();
    m_numDeletedVertices = 0;
    m_numDeletedFacets = 0;
  }
}
