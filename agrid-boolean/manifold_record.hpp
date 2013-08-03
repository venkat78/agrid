#ifndef _AGRID_BOOLEAN_MANIFOLD_RECORD_HPP_
#define _AGRID_BOOLEAN_MANIFOLD_RECORD_HPP_

#include "rectilinear_grid.hpp"

namespace agrid_boolean {
/*
 * Class stores the details of manifolds being processed.
 */
class cMANIFOLD_RECORD {
 public:
  cMANIFOLD_RECORD()
      : m_mesh(NULL),
        m_manifold(NULL) {
  }

  cMANIFOLD_RECORD(cSURFACE_MESH::cMANIFOLD *manifold, cSURFACE_MESH *mesh,
                   cBOX3 bbox, INT numCells[3])
      : m_grid(bbox, numCells),
        m_mesh(mesh),
        m_manifold(manifold) {
  }

  cSURFACE_MESH* Mesh() {
    return m_mesh;
  }
  cSURFACE_MESH::cMANIFOLD* Manifold() {
    return m_manifold;
  }
  cRECTILINEAR_GRID* Grid() {
    return &m_grid;
  }

  VOID Index(INT index) {
    m_index = index;
  }
  INT Index() const {
    return m_index;
  }

  VOID Print() {
    m_grid.Print();
  }

  BOOL IsOpen() const {
    return m_manifold->IsOpen();
  }

  VOID ClearGridMarkers() {
    m_grid.ClearColorMarkers();
  }

 private:
  /*
   * m_grid records gray/white/black cells with respect to the
   * manifold
   */
  cRECTILINEAR_GRID m_grid;
  cSURFACE_MESH *m_mesh;
  cSURFACE_MESH::cMANIFOLD *m_manifold;
  INT m_index;
};

struct sMANIFOLD_RECORD_HASH_FCN {
  INT operator()(const cMANIFOLD_RECORD &rec) const {
    return rec.Index();
  }
};

struct sMANIFOLD_RECORD_HASH_FCN1 {
  INT operator()(const cMANIFOLD_RECORD *rec) const {
    return rec->Index();
  }

};

inline BOOL operator <(const cMANIFOLD_RECORD &rec1,
                       const cMANIFOLD_RECORD &rec2) {
  return rec1.Index() < rec2.Index();
}

//   inline BOOL operator < (const cMANIFOLD_RECORD *rec1,
// 			  const cMANIFOLD_RECORD *rec2) {
//     return rec1->Index() < rec2->Index();
//   }

inline BOOL operator ==(const cMANIFOLD_RECORD &rec1,
                        const cMANIFOLD_RECORD &rec2) {
  cMANIFOLD_RECORD &r1 = (cMANIFOLD_RECORD &) rec1;
  cMANIFOLD_RECORD &r2 = (cMANIFOLD_RECORD &) rec2;

  return r1.Mesh() == r2.Mesh() && r1.Manifold() == r2.Manifold()
      && r1.Index() == r2.Index();
}

}

#endif //_AGRID_BOOLEAN_MANIFOLD_RECORD_HPP_
