#ifndef TMESH_VERTEX_HPP_
#define TMESH_VERTEX_HPP_

#include "defs.hpp"
#include "vertex_use.hpp"

namespace tmesh {

 /* template <typename _Mesh_Entities>
  class tTOPO_OPERATORS;
*/
  template <typename _Mesh_Entities>
  class tVERTEX {

    _TMESH_TOPO_TYPES(_Mesh_Entities);

    //Declaring friends of the class
//    _FRIENDLY_MESH(_Mesh_Entities);

  public:
    typedef tVERTEX_USE<_Mesh_Entities> cVERTEX_USE;


    //Iterator definition.
    typedef typename tLIST<cVERTEX_USE>::iterator vertex_use_iterator;
  public:
    tVERTEX() : m_isDeleted(0), m_isVisited(0)
    {}

    tVERTEX(const cPOINT3 &point) : m_point(point),
				    m_isDeleted(0),
				    m_isVisited(0)
    {}

    ~tVERTEX() {}

  public:
    bool IsNonManifold() const {
      return m_vertexUses.size() > 1;
    }

    iVERTEX Index() const { return m_index; }
    iVERTEX Label() const { return m_index; }
    BOOL IsDeleted() const {
      return m_isDeleted ? true : false;
    }
    INT NumFacets() const;
    const cPOINT3& Point() const { return m_point; }
    INT NumVertexUses() const { return m_vertexUses.size(); }
    BOOL HasIncidentFacets() const { return m_vertexUses.size() > 0; }
    BOOL OutgoingBorderHE(cVECTOR3 &initVector, cHALF_EDGE **outgoingHE, BOOL doPrint = false);
    cVECTOR3 Normal();
    cVECTOR3 AverageNormal() ;
    VOID AppendNormalData(cPOINT3 &vertexPoint,
    						 cHALF_EDGE* he0,
    						 cHALF_EDGE* he1,
    						 cVECTOR3 *p_normalVector,
    						 REAL *p_totalWeight);


    VOID Print(FILE *stream = stdout)  ;

    VOID IncidentFacets(std::vector<cFACET*> &facets)  ;
    cVERTEX_USE FirstVertexUse();

    VOID IncidentVertices(std::vector<cVERTEX*> &vertices) ;

  public:
    VOID Point(const cPOINT3 &point) { m_point = point; }
    VOID AddVertexUse(const cHALF_EDGE* halfEdge);
    VOID MarkVisited() {
      m_isVisited = 1;
    }

    VOID UnmarkVisited() {
      m_isVisited = 0;
    }

    BOOL IsVisited() const {
      return m_isVisited ? true : false;
    }

    /*
     * Removes vertexUse from vertex uses of the vertex. Method
     * used while inserting facets in a mesh.
     */
    VOID RemoveVertexUse(const cVERTEX_USE &vUse);

    /*
     * Finds vertex use represented by findHe. And also findHe
     * must be an index half-edge.
     */
    cVERTEX_USE FindVertexUse(const cHALF_EDGE* findHe) ;

    /*
     * Replaces oldHe in a vertex use with newHe.
     */

    VOID ReplaceHalfEdge(cHALF_EDGE *oldHe,
                         cHALF_EDGE *newHe);


    /*
     *  Finds if v can be located in the manifolds incident on *this*
     *  If v can be found, half-edge is returned in *h_out and vertexUse
     *  to which h_out belongs is returned in vUse.
     */
    BOOL Find(const cVERTEX *v, /* in */
              cHALF_EDGE **h_out, /* out */
              cVERTEX_USE *vUse /* out */
              ) ;

    /*
     * Finds the vertexUse to which he belongs. *this* vertex
     * should be tail vertex of he. he doesn't need to be an index
     * half-edge.
     */
    cVERTEX_USE Find(const cHALF_EDGE *he) ;


  public:
    /*
     * Reports num incidences of a facet on a vertex.
     */
    INT NumIncidences(cFACET *facet) ;

  public:
    VOID Index(iVERTEX index) {
      m_index = index;
    }

    VOID label(iVERTEX label) {
      m_index = label;
    }

    VOID MarkDeleted(BOOL flag = true) {
      m_isDeleted = flag ? 1 : 0;

      if(flag)
        m_vertexUses.clear();
    }

    BOOL Verify() ;

  public:
    vertex_use_iterator VertexUsesBegin() {
      return m_vertexUses.begin();
    }

    vertex_use_iterator VertexUsesEnd() {
      return m_vertexUses.end();
    }

  protected:
    cPOINT3 m_point;
    iVERTEX m_index : 30;
    iVERTEX m_isDeleted : 1;
    iVERTEX m_isVisited : 1;

    tLIST<cVERTEX_USE> m_vertexUses;

  };

  //Geometric operators used to sorts nodes by raster order.
  template <typename _Vertex>
  inline BOOL operator < (const _Vertex &v1, const _Vertex &v2)
  {
    return v1.Point() < v2.Point();
  }

  template <typename _Vertex>
  inline BOOL operator == (const _Vertex &v1, const _Vertex &v2)
  {
    return v1.Point() == v2.Point();
  }

}


#endif //TMESH_VERTEX_HPP_
