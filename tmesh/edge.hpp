#ifndef TMESH_EDGE_HPP_
#define TMESH_EDGE_HPP_

#include "defs.hpp"
//#include "tmesh_def.hpp"

namespace tmesh {
  template <typename _Half_Edge>
  class tEDGE {
  public:
    iVERTEX v1, v2;
    tEDGE() : v1(INVALID_IVERTEX), v2(INVALID_IVERTEX)
    {}

    tEDGE(const _Half_Edge *h) : v1(h->Tail()->Index()),
                                 v2(h->Head()->Index())
    {}

    iVERTEX SmallerIndex() const {
      return v1 < v2 ? v1 : v2;
    }

    BOOL operator == (const tEDGE &e) const {
      return (v1 == e.v1 && v2 == e.v2) || (v1 == e.v2 && v2 == e.v1);
    }
  };

  /*
   * Function object for hash table that stores nm-edges.
   */
  template <typename _Edge>
  class tCMP_EDGE
  {
  public:
    const static size_t bucket_size = 6;
    const static size_t min_buckets = 1024;
    BOOL operator()(const _Edge &e1,
                    const _Edge &e2) const
    {
      return e1 == e2;
    }

    size_t operator()(const _Edge &e) const
    {
      return e.SmallerIndex();
    }
  };

  /*
   * Support functionality for determining radial order of an nm-edge.
   * 
   */

  template <typename _Half_Edge>
  struct tEDGE_RING_ENTRY {
    _Half_Edge* he;
    REAL angle;
    cPOINT2 point;
    cVECTOR2 vector;
    BOOL isValid;
  };

  struct sDUMMY_FILTER {};

  template <typename _Mesh_Type, 
	    typename _Half_Edge_Filter = sDUMMY_FILTER>
  class tEDGE_RING {
    _TMESH_ENTITIES(_Mesh_Type);

    typedef tEDGE_RING_ENTRY<cHALF_EDGE> sEDGE_RING_ENTRY;

  public:

  public:
    tEDGE_RING(cHALF_EDGE *he = NULL,
	       _Mesh_Type *mesh = NULL, 
	       _Half_Edge_Filter *filter = NULL) : numEntries(0), 
						   m_he(he),
						   m_filter(filter),
						   m_mesh(mesh),
						   m_topoOperators(mesh)
    {
      memset(entries, 0, sizeof(sEDGE_RING_ENTRY)*ARRAY_SIZE);
    }
    
  public:
    VOID MateHalfEdges();

  private:
    VOID FindRadialOrder();

    VOID MateHalfEdgesBreakTie(INT currIndex, INT prevIndex, INT nextIndex);

    VOID MateCoplanarHalfEdges();

    VOID MateHalfEdgesInRadialOrder();

  private:
    VOID Add(cHALF_EDGE *he, const cPOINT2 &point);
    VOID ComputeRadialOrder();
    VOID Center(const cPOINT2 &c) { center = c; }
    VOID FindPrevAndNext(INT ci, INT &pi, INT &ni);

  private:
    cPOINT2 center;
    INT numEntries;
    sEDGE_RING_ENTRY entries[ARRAY_SIZE];
    cHALF_EDGE *m_he;
    _Half_Edge_Filter *m_filter;
    _Mesh_Type *m_mesh;
    cTOPO_OPERATORS m_topoOperators;
  };
}

#endif /*TMESH_EDGE_HPP_*/
