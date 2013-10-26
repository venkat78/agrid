#ifndef TMESH_FACET_HPP_
#define TMESH_FACET_HPP_

#include "defs.hpp"
#include "half_edge_circulators.hpp"

namespace tmesh {

  template <typename _Mesh_Entities>
  class tFACET {
    typedef tFACET<_Mesh_Entities> cSELF;
    _TMESH_TOPO_TYPES(_Mesh_Entities);

  public:
    /*
     * Circulators for walking a facet.
     */
    typedef tFACET_HALF_EDGE_CIRCULATOR<_Mesh_Entities> cFACET_HALF_EDGE_CIRCULATOR;
    typedef tFACET_VERTEX_CIRCULATOR <_Mesh_Entities> cFACET_VERTEX_CIRCULATOR;

    typedef tCIRCULATOR<cFACET_HALF_EDGE_CIRCULATOR> half_edge_circulator;
    typedef tCIRCULATOR<cFACET_VERTEX_CIRCULATOR> vertex_circulator;

  public:
    tFACET() : m_pHe(NULL), m_manifoldIndex(INVALID_IMANIFOLD)
    {}

    tFACET(const cSELF &f) {
      m_pHe = NULL;

      m_manifoldIndex = f.m_manifoldIndex;

    }

    ~tFACET() {}

    iFACET Index() const { return m_index; }
    iMANIFOLD ManifoldIndex() const { return m_manifoldIndex; }
    const cHALF_EDGE * HalfEdge() const { return m_pHe; }
    cHALF_EDGE* HalfEdge() { return m_pHe; }

    BOOL IsDeleted() const {
      return m_pHe == NULL ? true : false;
    }

    INT NumVertices() const ;
    VOID PushVerticesToVector(std::vector<INT> *facetVertexVector) const;
    BOOL ContainsVertex(cVERTEX *vertex) const;
    BOOL Contains(const cPOINT3 &point);
    BOOL IsBorder() const;

    cVECTOR3 Normal() const;
    cVECTOR3 UnitNormal() const;
    REAL Area() const;
    cVECTOR3 AreaVector() const;
    cPOINT3 MeanPoint() const;
    cBOX3 BoundingBox() const;
    eBOX_FACE IsOnBoxBoundary(const cBOX3 &box, eBOX_FACE boxFacePos = BOX_FACE_NONE) const;

    cHALF_EDGE* FindMinHalfEdge(eCOORD coord);

    VOID Print(FILE *stream = stdout,  BOOL longPrint = true) const ;
    BOOL Verify() const ;
    BOOL Debug(FILE *stream = stderr) const ;
    BOOL HasDuplicateVertices() const;
    BOOL HasDuplicateVertex(const cHALF_EDGE* heStart) const;
    const cHALF_EDGE *HasDegenerateEdge(REAL tol) const;

    BOOL IsNarrow(REAL tol, cHALF_EDGE **p_narrowWidthHE,
                                cVERTEX **p_furthestVertex,
                                cPOINT3 *p_vertexProjectionOntoEdge) const;

    BOOL HasNonManifoldEdge() const ;

  public:

     half_edge_circulator HalfEdgesBegin() const {
       return CirculatorBegin<cFACET_HALF_EDGE_CIRCULATOR>(m_pHe);
     }

     half_edge_circulator HalfEdgesEnd() const {
       return CirculatorEnd<cFACET_HALF_EDGE_CIRCULATOR>(m_pHe);
     }

     vertex_circulator VerticesBegin() const {
       return CirculatorBegin<cFACET_VERTEX_CIRCULATOR>(m_pHe);
     }

     vertex_circulator VerticesEnd() const {
       return CirculatorEnd<cFACET_VERTEX_CIRCULATOR>(m_pHe);
     }

    VOID Index(iFACET index) {
      m_index = index;
    }

    VOID ManifoldIndex(iMANIFOLD index) { m_manifoldIndex = index; }

    VOID HalfEdge(const cHALF_EDGE *h) {
      m_pHe = const_cast<cHALF_EDGE*>(h);
    }

    BOOL IsSimple() const ;

  protected:
    cHALF_EDGE *m_pHe;
    INT m_manifoldIndex;
    INT m_index;
  };

  template <typename _Mesh_Entities>
  cVECTOR3 tFACET<_Mesh_Entities>::Normal() const
  {
  /* BB * I don't see any division by zero. */

    cVECTOR3 planeNormal(0.0, 0.0, 0.0);
    const cHALF_EDGE* heInit = HalfEdge();
    const cHALF_EDGE* heCurr = heInit;
    const cHALF_EDGE* heNext = heCurr->Next();

    do {
  	const cPOINT3& point0 = heCurr->Vertex()->Point();
  	const cPOINT3& point1 = heNext->Vertex()->Point();

    	for (INT i = 0 ; i < 3; i++){
    	  INT i1 = i + 1;
    	  if (i1 > 2) i1 -= 3;
    	  INT i2 = i + 2;
    	  if (i2 > 2) i2 -= 3;
    	  planeNormal[i] += (point0[i1]-point1[i1])*(point0[i2]+point1[i2]);
    	}

    	heCurr = heNext;
    	heNext = heNext->Next();
    } while(heCurr != heInit);

    return planeNormal;
  }

  template <typename _Mesh_Entities>
  cBOX3 tFACET<_Mesh_Entities>::BoundingBox() const
  {
    cBOX3 boundingBox;
    const cHALF_EDGE* currHe = HalfEdge();
    const cHALF_EDGE* facetHe = currHe;

    do {
      boundingBox += currHe->Vertex()->Point();
      currHe = currHe->Next();
    } while(currHe != facetHe);

    return boundingBox;
  }

  template <typename _Mesh_Entities>
  eBOX_FACE tFACET<_Mesh_Entities>::IsOnBoxBoundary(const cBOX3 &box, eBOX_FACE boxFacePos) const
  {
	vertex_circulator currVertex = VerticesBegin();
	vertex_circulator lastVertex = VerticesEnd();
	BOOL vertexOnBoxBoundary[3][2] = {{true, true}, {true, true}, {true, true}};
	if (boxFacePos != BOX_FACE_NONE){
	  INT f = 0;
	  DO_COORDS(c){
		for (INT j = 0; j < 2; j++){
		  if (f != (INT)boxFacePos)
			  vertexOnBoxBoundary[c][j] = false;
		  f++;
		}
	  }
	}

	for( ; currVertex != lastVertex ; currVertex++ ) {
	  cPOINT3 vertexPoint = currVertex->Point();
	  if (!box.PointIsOnBoundary(vertexPoint, vertexOnBoxBoundary))
		return BOX_FACE_NONE;
	}
	return BoxFace(vertexOnBoxBoundary);
  }

  //returns the arithmetic point of the facet vertices.
  //For a convex facet, this point is located inside of the facet
  template <typename _Mesh_Entities>
  cPOINT3 tFACET<_Mesh_Entities>::MeanPoint() const
  {
    cPOINT3 meanPoint = cPOINT3();
    INT numVertices = 0;
    const cHALF_EDGE* currHe = HalfEdge();
    const cHALF_EDGE* facetHe = currHe;

    do {
      meanPoint += currHe->Vertex()->Point();
      numVertices++;
      currHe = currHe->Next();
    } while(currHe != facetHe);

    return meanPoint /((REAL)numVertices);
  }

}

#endif /*SMESH_FACET_HPP_*/
