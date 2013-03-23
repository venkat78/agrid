#ifndef _SURFACE_MESH_DEF_HPP_
#define _SURFACE_MESH_DEF_HPP_

#include "defs.hpp"
//namespace surface_mesh {

  template <typename _Mesh_Entities,
	    typename _Storage_Policy,
	    template <typename _ME, typename _SP> class _Object_Factory >

  class tSURFACE_MESH : public tMESH < _Mesh_Entities, _Storage_Policy, _Object_Factory >
  {
  public:
    typedef tMESH < _Mesh_Entities, _Storage_Policy, _Object_Factory > cBASE;
    typedef tSURFACE_MESH< _Mesh_Entities, _Storage_Policy, _Object_Factory > cSELF;

    _TMESH_ENTITIES(cBASE);

//     _TMESH_TOPO_TYPES(_Mesh_Entities);
//     _TMESH_EXPORTED_TYPES;

  public:
    tSURFACE_MESH() : cBASE()
    { }

    ~tSURFACE_MESH() { }

    /*
     * Methods that support faces.
     */
    VOID NewFacetGroup(const std::string& name) {
        m_faces.push_back(cFACET_GROUP(name));
    }

    cFACET_GROUP* GetFacetGroup(const std::string& name) {
        for(INT i = 0; i < (INT)m_faces.size(); i++) {
          if(m_faces[i].m_name.compare(name) == 0) {
            return &m_faces[i];
          }
        }

        return NULL;

    }

  private:
    std::vector<cFACET_GROUP> m_faces;
  };

#define _TSURFACE_MESH_TMPL  _TMESH_TMPL_TYPE

#define _TSURFACE_MESH_TMPL_DECL                       \
    tSURFACE_MESH<_Mesh_Entities, _Storage_Policy, _Object_Factory>
//}
#endif /*SMESH_MESH_DEF_HPP_*/
