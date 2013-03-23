#ifndef _TMESH_TREES_HPP_
#define _TMESH_TREES_HPP_

#include "defs.hpp"
#include "tmesh_def.hpp"

namespace tmesh {
  template <typename _Mesh_Type>
  class tMESH_VERTICES_MGR
  {
  public:
    tMESH_VERTICES_MGR(_Mesh_Type *mesh = NULL) : m_mesh(mesh)
    {}

    cBOX3 PartitionBound(const cBOX3 &box, INT i)
    {
      return OctantBox(box, i);
    }

    VOID Contains(iVERTEX vertexIndex, const cBOX3 &box, BOOL partitions[8])
    {
      const cPOINT3& point = m_mesh->Vertex(vertexIndex)->Point();
      //Is a faster method but needs more debugging.
      IsInOctants(box, point, partitions);
    }

    VOID Crosses(const cPOINT3 &point, const cBOX3 &box, BOOL partitions[8])
    {
      IsInOctants(box, point, partitions);
    } 

  private:
    _Mesh_Type *m_mesh;
		  
  };

  template <typename _Mesh_Type>
  class tMESH_VERTICES_OCTREE
  {
  public:
    typedef tMESH_VERTICES_MGR<_Mesh_Type> cVERTICES_MGR;
    typedef tOCTREE<cGEOM_3D_KERNEL, cVERTICES_MGR, iVERTEX > cOCTREE;
    typedef typename cOCTREE::cLEAF cLEAF;
    typedef typename cOCTREE::leaf_iterator leaf_iterator;
    typedef typename cOCTREE::leaf_element_iterator leaf_element_iterator;

  public:
    tMESH_VERTICES_OCTREE(const cBOX3 &bounds = cBOX3(),
			  _Mesh_Type *mesh = NULL, INT height = 12, 
			  INT leafElementsLim = 50) : m_mesh(mesh),
                                                      m_verticesMgr(mesh),
						      m_octree(bounds, &m_verticesMgr,
							       height, leafElementsLim)
    {}

    /*
     *
     */
    VOID Insert(iVERTEX vertexIndex) {
      m_octree.Insert(vertexIndex);
    }

    VOID FindMatches(const cPOINT3 &point, std::vector<iVERTEX> &matches)
    {
      std::vector<cLEAF*> leaves;
      m_octree.Crosses(point, leaves);
      
      if(leaves.size() == 0)
	return;
      
      INT localMatches[1024];
      INT numLocalMatches = 0;

      typename std::vector<cLEAF*>::iterator currLeaf = leaves.begin();
      typename std::vector<cLEAF*>::iterator lastLeaf = leaves.end();

      for( ; currLeaf != lastLeaf ; currLeaf++ ) {
	cLEAF* leaf = currLeaf.operator*();

	typename cLEAF::element_iterator currElem = leaf->begin();
	typename cLEAF::element_iterator lastElem = leaf->end();

	for( ; currElem != lastElem ; currElem++ ){
	  iVERTEX currVertex = *currElem;

	  if(point == m_mesh->Vertex(currVertex)->Point()) {
	    if(!ValIsInTable<iVERTEX>(currVertex, localMatches, numLocalMatches)) {
	      localMatches[numLocalMatches++] = currVertex;
	      matches.push_back(currVertex);
	    }
	  }
	}
      }
    }

  private:
    _Mesh_Type *m_mesh;
    cVERTICES_MGR m_verticesMgr;
    cOCTREE m_octree;
  
  };

  /*
   *  Facets octree.
   */
  template <typename _Mesh_Type>
  class tMESH_FACETS_MGR
  {
  public:
    tMESH_FACETS_MGR(_Mesh_Type *mesh = NULL) : m_mesh(mesh)
    {}

    cBOX3 PartitionBound(const cBOX3 &box, INT i)
    {
      return OctantBox(box, i).Grow(1);
    }

    BOOL FacetCrossesBox(iFACET facetIndex, const cBOX3 &box)
    {
      typedef typename _Mesh_Type::cFACET cFACET;
      typedef typename _Mesh_Type::cHALF_EDGE cHALF_EDGE;

      cFACET* facet = m_mesh->Facet(facetIndex);
      
      ASSERT(!facet->IsDeleted());
      
      //Research about more optimized methods.
      cHALF_EDGE *firstHe = facet->HalfEdge();
      cHALF_EDGE *secondHe = firstHe->Next();
      cHALF_EDGE *thirdHe = secondHe->Next();
      
      do {
	cTRIANGLE3 triangle(firstHe->Tail()->Point(),
			    secondHe->Tail()->Point(),
			    thirdHe->Tail()->Point());
	
	if(DoIntersect(box, triangle) == SUCCESS)
	  return true;
	
	secondHe = thirdHe;
	thirdHe = thirdHe->Next();
      } while (thirdHe != firstHe);
      
      return false;
    }
    
    VOID Contains(iFACET facetIndex, const cBOX3 &box, BOOL partitions[8])
    {
      typedef typename _Mesh_Type::cFACET cFACET;
      cFACET *facet = m_mesh->Facet(facetIndex);
      IsInOctants<cFACET>(facet, box, partitions);
    }

    VOID Crosses(const cSEGMENT3 &segment, const cBOX3 &box, BOOL partitions[8])
    {
      cPOINT3 segmentPoints[2];
      INT numPoints = 2;

      segmentPoints[0] = segment.Source();
      segmentPoints[1] = segment.Target();

      IsInOctants(box, segmentPoints, 2, partitions);

      for(INT i = 0; i < 8 ; i++) {
	if(partitions[i] && (DoIntersect(PartitionBound(box,i), segment) != SUCCESS))
	  partitions[i] = false;
      }
    }
		 
  private:
    _Mesh_Type *m_mesh;
		  
  };

  template <typename _Mesh_Type>
  class tMESH_FACETS_OCTREE
  {
  public:
    typedef tMESH_FACETS_MGR<_Mesh_Type> cFACETS_MGR;
    typedef tOCTREE<cGEOM_3D_KERNEL, cFACETS_MGR, iFACET > cOCTREE;
    typedef typename cOCTREE::cLEAF cLEAF;
    typedef typename cOCTREE::leaf_iterator leaf_iterator;
    typedef typename cOCTREE::leaf_element_iterator leaf_element_iterator;

  public:
    tMESH_FACETS_OCTREE(const cBOX3 &bounds = cBOX3(),
			  _Mesh_Type *mesh = NULL, INT height = 12, 
			  INT leafElementsLim = 50) : m_mesh(mesh),
                                                      m_facetsMgr(mesh),
						      m_octree(bounds, &m_facetsMgr,
							       height, leafElementsLim)
    {}
    
    /*
     *  Methods to insert and delete facets.
     */
    VOID Insert(iFACET facetIndex) {
      m_octree.Insert(facetIndex);
    }

    VOID Delete(iFACET facetIndex) {
      m_octree.Delete(facetIndex);
    }

    VOID DeleteAll() {
      m_octree.DeleteAll();
    }

    leaf_iterator Begin() {
      return m_octree.Begin();
    }

    leaf_iterator End() {
      return m_octree.End();
    }

    VOID Crosses(const cSEGMENT3 &segment, std::vector<cLEAF*> &leaves)
    {
      m_octree.Crosses(segment, leaves);
    }

    BOOL Verify() {
      leaf_iterator currLeaf = Begin();
      leaf_iterator lastLeaf = End();

      for( ; currLeaf != lastLeaf ; currLeaf++ ) {
	leaf_element_iterator currElem = currLeaf->begin();
	leaf_element_iterator lastElem = currLeaf->end();

	for( ; currElem != lastElem ; currElem++ ) {
	  if(m_mesh->Facet(*currElem)->IsDeleted()) {
	    ASSERT(!m_mesh->Facet(*currElem)->IsDeleted());
	    return false;
	  }
	}
      }
      
      return true;
    }
private:
    _Mesh_Type *m_mesh;
    cFACETS_MGR m_facetsMgr;
    cOCTREE m_octree;
  
  };

}

#endif //_TMESH_TREES_HPP_
