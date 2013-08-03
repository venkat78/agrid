#ifndef _BASE_GRID_MANIFOLD_RECORD_HPP_
#define _BASE_GRID_MANIFOLD_RECORD_HPP_

#include "defs.hpp"

namespace base_grid {
/*
 * Class stores the details of manifolds being processed.
 */
template<typename _MESH_TYPE>
class tMANIFOLD_RECORD {
 public:
  tMANIFOLD_RECORD()
      : m_mesh(NULL),
        m_manifold(NULL) {
  }

  tMANIFOLD_RECORD(_MESH_TYPE::cMANIFOLD *manifold, _MESH_TYPE *mesh)
      : m_mesh(mesh),
        m_manifold(manifold) {
  }

  _MESH_TYPE* Mesh() {
    return m_mesh;
  }

  _MESH_TYPE::cMANIFOLD* Manifold() {
    return m_manifold;
  }

  VOID Index(INT index) {
    m_index = index;
  }

  INT Index() const {
    return m_index;
  }

  BOOL IsOpen() const {
    return m_manifold->IsOpen();
  }

 private:
  /*
   * m_grid records gray/white/black cells with respect to the
   * manifold
   */
  _MESH_TYPE *m_mesh;
  _MESH_TYPE::cMANIFOLD *m_manifold;
  INT m_index;
};

}

#endif //_BASE_GRID_MANIFOLD_RECORD_HPP_
