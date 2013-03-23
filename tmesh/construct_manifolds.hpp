#ifndef _TMESH_CONSTRUCT_MANIFOLDS_H_
#define _TMESH_CONSTRUCT_MANIFOLDS_H_

//#include "tmesh_def.hpp"

#include <stack>
#include <vector>
#include "defs.hpp"

namespace tmesh {


  template <typename _Mesh_Type>
  class tCONSTRUCT_MANIFOLDS {
    _TMESH_ENTITIES(_Mesh_Type);

  /*
   * Classes and methods which support construction of manifolds.
   */
  //Struct that supports stitching of manifold at a nm-edge.
  //Holds entry related to one index.
  struct sMANIFOLD_ZIPPER
  {
    cHALF_EDGE* m_halfEdges[SMALL_ARRAY_SIZE];
    iMANIFOLD m_index;
    INT m_numEntries;
  };
 
    
  public:
    tCONSTRUCT_MANIFOLDS(_Mesh_Type *mesh = NULL) : m_mesh(*mesh),
						    m_factory(m_mesh.Factory()),
						    m_nmEdges(m_mesh.NmEdges()),
						    m_topoOperators(mesh)
    {}

    INT Finish();

  private:
    //Static methods to support construction of manifolds.
    static INT AddToZipper(cHALF_EDGE *he, iMANIFOLD index,
			   sMANIFOLD_ZIPPER *entries,
			   INT numEntries);
    
  private:
    VOID BuildQueue(BOOL nmFacetsAtEnd);

    BOOL CanMergeManifolds();

    VOID WalkTwoManifoldEdge(cHALF_EDGE *facetHe, cFACET *facet,
			     std::vector<BOOL> &facetsToBeFlipped,
			     INT &numBorderEdges, cMANIFOLD *manifold);
    
    VOID WalkCodirectedEdge(cHALF_EDGE *facetHe, cFACET *facet,
			    std::vector<BOOL> &facetsToBeFlipped,
			    cMANIFOLD* manifold);
    
    eORIENTATION Construct(cFACET *facet, std::vector<BOOL> &facetsToBeFlipped,
			   sTRUE a);
    
    eORIENTATION Construct(cFACET *facet, 
			   std::vector<BOOL> &facetsToBeFlipped,
			   sFALSE a);
    INT RetVal();

    VOID OrientManifolds(std::vector<BOOL> &facetsToBeFlipped);

    VOID StitchManifoldsAtNmEdge(cHALF_EDGE *nmEdge);

    VOID StitchManifoldsAtNmEdges();

    VOID MateNmEdges();

    INT Construct(INT round);

  private:
    std::vector<cFACET*> m_queue;
    std::stack<cFACET*> m_stack;

    _Mesh_Type &m_mesh;
    cOBJECT_FACTORY &m_factory;
    cNM_EDGES &m_nmEdges;
    cTOPO_OPERATORS m_topoOperators;
  };
}
#endif //MESH_CONSTRUCT_MANIFOLDS_H_
