#ifndef _SPATIAL_TREES_TREE_HPP_
#define _SPATIAL_TREES_TREE_HPP_

#include "defs.hpp"

namespace spatial_trees {
  /*
   * Can be modified later to store bounding boxes in internal and 
   * leaf nodes.
   */
  template <typename GEOM_KERNEL,
	    typename GEOM_MANAGER,
	    typename ELEMENT_ID,
	    INT N>
  class tTREE {
  public:
    typedef typename GEOM_KERNEL::cBOX cBOX;
    typedef typename GEOM_KERNEL::cPOINT cPOINT;
    typedef typename GEOM_KERNEL::cSEGMENT cSEGMENT;
    typedef typename GEOM_KERNEL::cLINE cLINE;
    typedef typename GEOM_KERNEL::cRAY cRAY;
    typedef typename GEOM_KERNEL::cTRIANGLE cTRIANGLE;

  private:
    static const INT INVALID_INDEX = -100;

  public:
    tTREE(const cBOX &box = cBOX(), GEOM_MANAGER *mgr = NULL,
	  INT height = 8, INT leafElementsLim = 50) : m_bounds(box), m_height(height), 
						      m_leafElementsLim(leafElementsLim),
						      m_geomMgr(mgr)
    {
      m_root = FreeInternalNode();
    }

    VOID Insert(const ELEMENT_ID &elem);

    VOID Delete(const ELEMENT_ID &elem);

    VOID DeleteAll();

    VOID SetGeomMgr(GEOM_MANAGER *mgr) {
      m_geomMgr = mgr;
    }

  public:

    class cINDEX {
    public:
      INT m_id : 30;
      INT m_isLeaf : 2;
    public:
      cINDEX() {
	m_id = INVALID_INDEX;
	m_isLeaf = false;
      }

      ~cINDEX() {
	m_id = INVALID_INDEX;
	m_isLeaf = false;
      }

    public:
      BOOL IsLeaf() { return m_isLeaf; }
      BOOL IsValid() { return m_id != INVALID_INDEX; }
    };

    class cLEAF {
    public:
      typedef typename tLIST<ELEMENT_ID>::iterator element_iterator;
      
      element_iterator begin() {
	return m_elements.begin();
      }

      element_iterator end() {
	return m_elements.end();
      }

    public:
      cLEAF() {}
      ~cLEAF() { m_elements.clear(); }

      tLIST<ELEMENT_ID> &Elements() {
	return m_elements;
      }
    private:
      tLIST<ELEMENT_ID> m_elements;
    };

    class cNON_LEAF_NODE {
    public:
      cINDEX m_children[N];
    };

  private:
    template <typename GEOM_OBJECT>
    VOID Crosses(cINDEX index, const cBOX &bound,
		 const GEOM_OBJECT &geomObj, std::vector<cLEAF*> &leaves) ;
    

    template <typename GEOM_OBJECT>
    VOID Crosses(const GEOM_OBJECT &geomObj, std::vector<cLEAF*> &leaves) ;
      
  public:
    /*
     * Methods that walk the tree.
     */
    VOID Crosses(const cPOINT &point,
		 std::vector<cLEAF*> &leaves) {
      Crosses<cPOINT>(point, leaves);
    }
    
    VOID Crosses(const cBOX &box,
		 std::vector<cLEAF*> &leaves) {
      Crosses<cBOX>(box, leaves);
    }

    VOID Crosses(const cSEGMENT &segment,
		 std::vector<cLEAF*> &leaves) {
      Crosses<cSEGMENT>(segment, leaves);
    }

    VOID Crosses(const cLINE &line,
		 std::vector<cLEAF*> &leaves) {
      Crosses<cLINE>(line, leaves);
    }
    
    VOID Crosses(const cRAY &ray,
		 std::vector<cLEAF*> &leaves) {
      Crosses<cRAY>(ray, leaves);
    }

  public:
    typedef typename tTABLE<cLEAF>::iterator leaf_iterator;
    typedef typename cLEAF::element_iterator leaf_element_iterator;

    leaf_iterator Begin() { return m_leaves.begin(); }
    leaf_iterator End() { return m_leaves.end(); }

  private:
    //Method to support construct the tree.
 
    cINDEX FreeInternalNode();
    cINDEX FreeLeafNode();
    VOID ReturnFreeLeaf(cINDEX index);
    VOID ReturnFreeNonLeaf(cINDEX index);
    VOID AddNewLeaf(cINDEX parent, INT partition, const ELEMENT_ID &id);
    VOID InsertIntoNonLeafNode(const cINDEX parentIndex, const cBOX &bound,
			       const ELEMENT_ID &id, const INT height);
    VOID InsertIntoLeafNode(const cINDEX parentIndex, const cINDEX leafIndex,
			    const INT partition, const cBOX &box,
			    const ELEMENT_ID id,const INT height);


    //Method to support deletion of elements.
    VOID Delete(cINDEX parentIndex, 
		const cBOX &bound, 
		const ELEMENT_ID &id);    
  private:
    cBOX m_bounds;
    INT m_height;
    INT m_leafElementsLim;  //Max elements in a leaf.
    GEOM_MANAGER *m_geomMgr;
    
    tLIST<INT> m_freeLeafNodes;
    tLIST<INT> m_freeNonLeafNodes;
    
    tTABLE<cLEAF> m_leaves;
    tTABLE<cNON_LEAF_NODE> m_nonLeafNodes;

    cINDEX m_root;
  };

  //Implementation follows.
#define TREE_TMPL_DECL						\
  template <typename GEOM_KERNEL, typename GEOM_MANAGER,	\
	    typename ELEMENT_ID, INT N>

#define TREE_TMPL_DEFN				\
  tTREE<GEOM_KERNEL, GEOM_MANAGER,		\
	ELEMENT_ID, N> 

  TREE_TMPL_DECL
  typename TREE_TMPL_DEFN::cINDEX 
  TREE_TMPL_DEFN::FreeInternalNode()
  {
    cINDEX index;
    if(m_freeNonLeafNodes.size() == 0) {
      m_nonLeafNodes.new_object();
      index.m_id = m_nonLeafNodes.size()-1;
      index.m_isLeaf = false;
      return index;
    }

    INT freeIndex = m_freeNonLeafNodes.front();

    cNON_LEAF_NODE *newNode = m_nonLeafNodes.object_at(freeIndex);
    ::new (newNode)cNON_LEAF_NODE();

    m_freeNonLeafNodes.pop_front();

    index.m_id = freeIndex;
    index.m_isLeaf = false;
  
    return index;
  }

  TREE_TMPL_DECL
  typename TREE_TMPL_DEFN::cINDEX 
  TREE_TMPL_DEFN::FreeLeafNode()
  {
    cINDEX index;
    if(m_freeLeafNodes.size() == 0) {
      m_leaves.new_object();
      index.m_id = m_leaves.size()-1;
      index.m_isLeaf = true;

      return index;
    }

    INT freeIndex = m_freeLeafNodes.front();
    cLEAF *newNode = m_leaves.object_at(freeIndex);
    ::new (newNode)cLEAF();
  
    m_freeLeafNodes.pop_front();
  
    index.m_id = freeIndex;
    index.m_isLeaf = true;

    return index;
  }

  TREE_TMPL_DECL
  VOID TREE_TMPL_DEFN::ReturnFreeLeaf(cINDEX index)
  {
    cLEAF *leaf = m_leaves.object_at(index.m_id);
    leaf->~cLEAF();

    m_freeLeafNodes.push_front((INT)index.m_id);
  }

  TREE_TMPL_DECL
  VOID TREE_TMPL_DEFN::ReturnFreeNonLeaf(cINDEX index)
  {
    cNON_LEAF_NODE *nonLeaf = m_nonLeafNodes.object_at(index.m_id);
    nonLeaf->~cNON_LEAF_NODE();
  
    m_freeNonLeafNodes.push_front(index.m_id);
  }

  TREE_TMPL_DECL
  VOID TREE_TMPL_DEFN::Insert(const ELEMENT_ID &id)
  {
    InsertIntoNonLeafNode(m_root, m_bounds, id, 0);
  }

  TREE_TMPL_DECL
  VOID TREE_TMPL_DEFN::Delete(const ELEMENT_ID &id)
  {
    Delete(m_root, m_bounds, id);
  }

  TREE_TMPL_DECL
  VOID TREE_TMPL_DEFN::DeleteAll()
  {
    m_freeLeafNodes.clear();
    m_freeNonLeafNodes.clear();
    m_leaves.flush();
    m_nonLeafNodes.flush();

    m_root = FreeInternalNode();
  }

  TREE_TMPL_DECL
  VOID TREE_TMPL_DEFN::Delete(cINDEX parentIndex, 
			      const cBOX &bound,
			      const ELEMENT_ID &id)
  {
    if(!parentIndex.IsValid())
      return;

    BOOL partitionSet[N];
    cBOX partitionBound[N];
    
    for(INT i = 0; i < N ; i++ )
      partitionSet[i] = false;
    

    m_geomMgr->Contains(id, bound, partitionSet);
    
    for(INT i = 0; i < N ; i++) {
      if(partitionSet[i]) {
	cINDEX index = m_nonLeafNodes.object_at(parentIndex.m_id)->m_children[i];

	if(index.IsLeaf()) {
	  cLEAF *leaf = m_leaves.object_at(index.m_id);
	  leaf->Elements().remove(id);
	}
	else {
	  cBOX partitionBound = m_geomMgr->PartitionBound(bound, i);
	  Delete(index, partitionBound, id);
	}
      }
    }
  }
  

  TREE_TMPL_DECL
  VOID TREE_TMPL_DEFN::AddNewLeaf(cINDEX parentIndex,
				  INT partition,
				  const ELEMENT_ID &id)
  {
    cINDEX leafIndex = FreeLeafNode();
  
    cLEAF *leaf = m_leaves.object_at(leafIndex.m_id);
    leaf->Elements().push_back(id);
    m_nonLeafNodes.object_at(parentIndex.m_id)->m_children[partition] = leafIndex;
  }

  TREE_TMPL_DECL
  VOID TREE_TMPL_DEFN::InsertIntoNonLeafNode(const cINDEX parentIndex,
					     const cBOX &bound,
					     const ELEMENT_ID &id,
					     const INT height)
  {
    BOOL partitionSet[N];
    cBOX partitionBound[N];

    for(INT i = 0; i < N ; i++)
      partitionSet[i] = false;
  
    ASSERT(height <= m_height - 2);
  
    m_geomMgr->Contains(id, bound, partitionSet);

    for(INT i = 0;i < N; i++) {
      if(partitionSet[i])
	partitionBound[i] = m_geomMgr->PartitionBound(bound, i);
    }
  
    if(height == m_height - 2) {
      for(INT i = 0; i < N; i++ ) {
	if(partitionSet[i]) { 
	  cINDEX index = m_nonLeafNodes.object_at(parentIndex.m_id)->m_children[i];
	
	  if(!index.IsValid())
	    AddNewLeaf(parentIndex, i, id);
	  else {
	    ASSERT(index.IsLeaf());
	    cLEAF *leaf = m_leaves.object_at(index.m_id);
	    leaf->Elements().push_back(id);
	  }
	}
      }
    
      return;
    }
  
  
    for(INT i = 0; i < N; i++ ) {
      if(partitionSet[i]) { 
	cINDEX index = m_nonLeafNodes.object_at(parentIndex.m_id)->m_children[i];
	if(!index.IsValid())
	  AddNewLeaf(parentIndex, i, id);
	else if(!index.IsLeaf())
	  InsertIntoNonLeafNode(index, partitionBound[i], id, height+1);
	else
	  InsertIntoLeafNode(parentIndex, index, i, partitionBound[i], id, height+1);
      }
    }
  }

  TREE_TMPL_DECL
  VOID TREE_TMPL_DEFN::InsertIntoLeafNode(const cINDEX parentIndex,
					  const cINDEX leafIndex,
					  const INT partition,
					  const cBOX &box,
					  const ELEMENT_ID id,
					  const INT height)
  {
    ASSERT(height <= m_height);

    //Depth of tree gets priority 
    if(height == m_height - 1) {
      cLEAF *leaf = m_leaves.object_at(leafIndex.m_id);
      leaf->Elements().push_back(id);
      return;
    }
  
    cLEAF *leaf = m_leaves.object_at(leafIndex.m_id);
    if(leaf->Elements().size() <  m_leafElementsLim) {
      leaf->Elements().push_back(id);
      return;
    }
  
    tLIST<ELEMENT_ID> copiedList(leaf->Elements());
    copiedList.push_back(id);

    //bumping the tree down.
    cINDEX newInternalNodeIndex = FreeInternalNode();
    m_nonLeafNodes.object_at(parentIndex.m_id)->m_children[partition] = newInternalNodeIndex;
  
    ReturnFreeLeaf(leafIndex);

    typename tLIST<ELEMENT_ID>::iterator firstElement = copiedList.begin();
    typename tLIST<ELEMENT_ID>::iterator lastElement = copiedList.end();
  
    for( ; firstElement != lastElement ; firstElement++)
      InsertIntoNonLeafNode(newInternalNodeIndex, box, *firstElement, height);
  
  }

  TREE_TMPL_DECL
  template <typename GEOM_OBJECT>
  VOID TREE_TMPL_DEFN::Crosses(cINDEX index,
			       const cBOX &bound,
			       const GEOM_OBJECT &geomObj,
			       std::vector<cLEAF*> &leaves) 
  {
    if(!index.IsValid())
      return;

    cNON_LEAF_NODE *nonLeaf = m_nonLeafNodes.object_at(index.m_id);
    BOOL partitionSet[N];
    cBOX partitionBound[N];

    for(INT i = 0; i < N ; i++) {
      partitionSet[i] = false;
    }

    m_geomMgr->Crosses(geomObj, bound, partitionSet);

    for(INT i = 0; i < N ; i++) {
      if(partitionSet[i]) {
	cINDEX childIndex = nonLeaf->m_children[i];

	if(childIndex.IsLeaf())
	  leaves.push_back(m_leaves.object_at(childIndex.m_id));
	else
	  Crosses<GEOM_OBJECT>(childIndex, m_geomMgr->PartitionBound(bound, i),
			       geomObj, leaves);
      }
    }
  }
  
  TREE_TMPL_DECL
  template <typename GEOM_OBJECT>
  VOID TREE_TMPL_DEFN::Crosses(const GEOM_OBJECT &geomObj,
			       std::vector<cLEAF*> &leaves) 
  {
    Crosses(m_root, m_bounds, geomObj, leaves);
  }

#undef TREE_TMPL_DECL
#undef TREE_TMPL_DEFN

}

#endif //_SPATIAL_TREES_TREE_HPP_
