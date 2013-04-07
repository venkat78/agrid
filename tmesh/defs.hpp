/*
 * defs.hpp
 *
 *  Created on: Aug 1, 2010
 */

#ifndef TMESH_DEFS_HPP_
#define TMESH_DEFS_HPP_

#include <cstdio>
#include <cstring>
#include <set>
#include <ext/hash_map>

#include "utils.hpp"
using namespace utils;

#include "geom.hpp"
using namespace geom;

#include "spatial_trees.hpp"
using namespace spatial_trees;

namespace tmesh {
  //Forward declarations
  template <typename _Mesh_Entities,
		  typename _Storage_Policy,
		  template <typename _ME, typename _SP>
		  class _Object_Factory >
  class tMESH;

//  template <typename _Mesh_type>
//  class tTOPO_OPERATORS;
  
  template <typename _Mesh_Entities,
	    typename _Storage_Policy>
  class tMESH_OBJECT_FACTORY;

  //Forward declarations of basic mesh entities.
  template <typename _Mesh_Entities>
  class tVERTEX;

  template <typename _Entities>
  class tMESH_ENTITIES {
  public:
    typedef typename _Entities::template tWRAPPER<_Entities>::cVERTEX cVERTEX;
    typedef typename _Entities::template tWRAPPER<_Entities>::cHALF_EDGE cHALF_EDGE;
    typedef typename _Entities::template tWRAPPER<_Entities>::cFACET cFACET;
 };

#define _TMESH_TOPO_TYPES(_Mesh_Entities)			\
  typedef tMESH_ENTITIES<_Mesh_Entities> cMESH_TOPO_TYPES;	\
  typedef typename cMESH_TOPO_TYPES::cVERTEX cVERTEX;		\
  typedef typename cMESH_TOPO_TYPES::cHALF_EDGE cHALF_EDGE;	\
  typedef typename cMESH_TOPO_TYPES::cFACET cFACET

#define _TMESH_EXPORTED_TYPES						\
  typedef tEDGE<cHALF_EDGE> cEDGE;					\
  typedef tVERTEX_USE<_Mesh_Entities> cVERTEX_USE;			\
  typedef _Storage_Policy cSTORAGE_POLICY;				\
  typedef _Mesh_Entities cMESH_ENTITIES;				\
  typedef _Object_Factory <_Mesh_Entities, _Storage_Policy > cOBJECT_FACTORY; \
  typedef tMANIFOLD< cSELF > cMANIFOLD;					\
  typedef __gnu_cxx::hash_multimap< cEDGE,				\
				    cHALF_EDGE*,			\
				    tCMP_EDGE<cEDGE> > cNM_EDGES

#define _TOBJECT_FACTORY_ENTITIES(_Mesh_Entities)	\
  _TMESH_TOPO_TYPES(_Mesh_Entities) 

#define _TMESH_HALF_EDGE_ENTITIES(_Mesh) \
  typedef typename _Mesh::cVERTEX cVERTEX;			\
  typedef typename _Mesh::cHALF_EDGE cHALF_EDGE;		\
  typedef typename _Mesh::cFACET cFACET;			\
  typedef typename _Mesh::cEDGE cEDGE;				\
  typedef typename _Mesh::cVERTEX_USE cVERTEX_USE

#define _TMESH_UTILITY_ENTITIES(_Mesh) \
  typedef typename _Mesh::cTOPO_OPERATORS cTOPO_OPERATORS; \
  typedef typename _Mesh::cNM_EDGES cNM_EDGES

#define _TMESH_TEMPLATE_PARAMETERS(_Mesh) \
  typedef typename _Mesh::cSTORAGE_POLICY cSTORAGE_POLICY;	\
  typedef typename _Mesh::cMESH_ENTITIES cMESH_ENTITIES;	\
  typedef typename _Mesh::cOBJECT_FACTORY cOBJECT_FACTORY

#define _TMESH_MANIFOLD(_Mesh) \
  typedef typename _Mesh::cMANIFOLD cMANIFOLD

#define _TMESH_BASIC_ENTITIES(_Mesh) \
  _TMESH_TEMPLATE_PARAMETERS(_Mesh); \
  _TMESH_HALF_EDGE_ENTITIES(_Mesh); \
  _TMESH_UTILITY_ENTITIES(_Mesh)

#define _TMESH_ENTITIES(_Mesh) \
	_TMESH_BASIC_ENTITIES(_Mesh); \
	_TMESH_MANIFOLD(_Mesh)

  //Constants and label definitions.
  typedef INT iVERTEX;
  typedef INT iFACET;
  typedef INT iMANIFOLD;

  const iVERTEX INVALID_IVERTEX=-10;
  const iFACET INVALID_IFACET=-11;
  const iMANIFOLD INVALID_IMANIFOLD=-12;
  const INT ORIENTED_MANIFOLDS=0;
  const INT NON_ORIENTABLE_MANIFOLDS=-1;
  const INT ARRAY_SIZE=1024;
  const INT SMALL_ARRAY_SIZE=64;
  const INT TMP_ARRAY_SIZE = 1024;

}
#endif /* TMESH_DEFS_HPP_ */
