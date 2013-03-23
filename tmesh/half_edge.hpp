#ifndef TMESH_HALF_EDGE_HPP_
#define TMESH_HALF_EDGE_HPP_

#include "defs.hpp"

namespace tmesh {
  template <typename _Mesh_Entities>
  class tHALF_EDGE
  {
    typedef tHALF_EDGE<_Mesh_Entities> cSELF;

    _TMESH_TOPO_TYPES(_Mesh_Entities);

    //Declaring friends of the class.
//    _FRIENDLY_MESH(_Mesh_Entities);
//    _FRIENDLY_VERTEX(_Mesh_Entities);

  public:
    tHALF_EDGE() : m_vertex(NULL), m_opposite(NULL),
		   m_next(NULL), m_facet(NULL)
    {
      m_isNonManifold = m_isARing =
	m_isFirstOnVertex = false;

      m_oppLocInRing = true;
    }

    ~tHALF_EDGE() {
      m_vertex = NULL;
      m_next = NULL;
      m_opposite = NULL;
      m_facet = NULL;

      m_isNonManifold = m_isARing =
	m_oppLocInRing	= m_isFirstOnVertex = false;
    }

    cFACET* Facet() const {
      return m_facet;
    }

    cHALF_EDGE* Next() const {
      return m_next;
    }

    /*
     * Returns true if he is in the two manifold of *this*
     * half-edge. Based on second parameter, tailVertex or
     * headVertex is checked.
     */
    BOOL IsReachable(cHALF_EDGE *he,
                     bool outgoing = true) const ;

    /*
     * Walks m_next pointers to get to previous half-edge.
     */
    cHALF_EDGE* Prev() const ;

    /*
     * Walks the two manifold to get to previous half-edge.
     */
    cHALF_EDGE* Prev2() const ;
    cHALF_EDGE* Opp() const ;

    cVERTEX *FurthestVertex(cPOINT3 *p_vertexProjection = NULL,
			    REAL *p_WidthSq = NULL) const;

    REAL SquaredLength() const;
    cVECTOR3 UniqueVector() ;
    cVERTEX* Vertex() const {
      return m_vertex;
    }

    cVERTEX* Tail() const {
      return m_vertex;
    }

    cVERTEX* Head() const {
      return m_next->Vertex();
    }

    cVECTOR3 Vector() const ;

    /* Method to be used inside object factory and mesh
     * iterators.
     */
    BOOL IsDeleted() const {
      return m_facet == NULL &&
      m_opposite == NULL &&
      m_vertex == NULL;
    }

    BOOL IsBorder() const {
      return m_facet == NULL;
    }

    BOOL IsARing() const {
      return m_isARing;
    }

    BOOL IsNonManifold() const {
      return m_isNonManifold;
    }

    cHALF_EDGE* NextInRing() const {
      return m_opposite;
    }

    BOOL OppIsNextInRing() const {
      return m_oppLocInRing;
    }

    BOOL IsFirstOnVertex() const {
       return m_isFirstOnVertex;
     }

    cHALF_EDGE* PrevInRing() const ;

    BOOL Verify() const ;
    BOOL Debug(FILE *stream = stderr) const ;

    VOID MarkFirstOnVertex() {
    	m_isFirstOnVertex = true;
    }

    VOID UnmarkFirstOnVertex() {
    	m_isFirstOnVertex = false;
    }

    BOOL HasIndex() const ;

    /*
     * Find a border edge in the two manifold of *this* half-edge.
     */
    cHALF_EDGE* BorderEdgeIntoVertex() const;

  public:
    VOID Facet(cFACET *f) {
      m_facet = f;
    }

    VOID Next(cHALF_EDGE *h) {
      m_next = h;
    }

    VOID Opp(cHALF_EDGE *h) {
      m_opposite = h;
      ClearOppFlags();
    }

    VOID Vertex(cVERTEX *v) {
      m_vertex = v;
    }

    VOID MarkOppLocInRing(BOOL flag) {
      m_oppLocInRing = flag;
    }

    VOID MarkAsNonManifold(BOOL flag = true) {
      m_isNonManifold = flag;
    }

    VOID MarkAsRing() {
      m_isARing = true;
    }

    VOID NextInRing(cHALF_EDGE* he) {
      m_opposite = he;
    }

    VOID ClearOppFlags() {
      m_isARing = m_oppLocInRing = m_isNonManifold = false;
    }

    //bisector normal is the difference of two plane normals
    cVECTOR3 BisectorPlaneNormal()
    {
      cVECTOR3 n0 = Facet()->UnitNormal();
      cFACET *oppFacet = Opp()->Facet();
      assert(oppFacet != NULL);
      cVECTOR3 n1 = oppFacet->UnitNormal();
      cVECTOR3 bisector = n0 - n1;
      bisector = bisector.Normalize();
      printf("dot = %lg\n", bisector*(n0+n1));
      return bisector;
    }



  public:
    cSEGMENT3 Segment() const ;
    cPOINT3 MidPoint() const {
    	return (Tail()->Point() + Head()->Point()) * 0.5;
    }

  public:
    VOID Print(FILE *stream = stdout) const;
 //   BOOL NextBorderHalfEdge(cHALF_EDGE **outgoingHE) const;

  private:
    cVERTEX *m_vertex;
    cHALF_EDGE *m_opposite;
    cHALF_EDGE *m_next;
    cFACET* m_facet;

    BOOL m_isARing;
    BOOL m_oppLocInRing;
    BOOL m_isFirstOnVertex;
    BOOL m_isNonManifold;
  };

  template <typename _Half_Edge>
  BOOL AreOpposite(const _Half_Edge *he1, const _Half_Edge *he2)
  {
    return he1->Tail() == he2->Head() && he1->Head() == he2->Tail();
  }

  template <typename _Half_Edge>
  BOOL AreCoDirected(const _Half_Edge *he1, const _Half_Edge *he2)
  {
    return he1->Tail() == he2->Tail() && he1->Head() == he2->Head();
  }
}


#endif //TMESH_HALF_EDGE_HPP_
