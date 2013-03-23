#include "tmesh_object_factory.hpp"

namespace tmesh {
  template <typename _Mesh_Entities,
	    typename _Storage_Policy>
  VOID tMESH_OBJECT_FACTORY<_Mesh_Entities, _Storage_Policy>::Return(cHALF_EDGE *he)
  {
    ASSERT(he != NULL);
    he->~cHALF_EDGE();
    he->Next(m_freeHes);
    m_freeHes = he;
  }

  template <typename _Mesh_Entities,
	    typename _Storage_Policy>
  
  typename tMESH_OBJECT_FACTORY<_Mesh_Entities, 
			   _Storage_Policy>::cHALF_EDGE* 
  tMESH_OBJECT_FACTORY<_Mesh_Entities, _Storage_Policy>::HalfEdge()
  {
    if(m_freeHes == NULL)
      return NewHalfEdge();
    else {
      cHALF_EDGE* freeHe = m_freeHes;
      m_freeHes = freeHe->Next();
      
      ::new (freeHe)cHALF_EDGE;
      return freeHe;
    }
  }
  
  template <typename _Mesh_Entities,
	    typename _Storage_Policy>
  
  typename tMESH_OBJECT_FACTORY<_Mesh_Entities, 
			   _Storage_Policy>::cHALF_EDGE*
  tMESH_OBJECT_FACTORY<_Mesh_Entities, 
		  _Storage_Policy>::CreateAPair(const cVERTEX *v1,
						const cVERTEX *v2)
  {
    cHALF_EDGE* h1 = HalfEdge();
    cHALF_EDGE* h2 = HalfEdge();
    
    h1->Opp(h2);
    h2->Opp(h1);
    
    h1->Vertex(const_cast<cVERTEX*>(v1));
    h2->Vertex(const_cast<cVERTEX*>(v2));
    
    return h1;
  }
}
