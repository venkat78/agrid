/*
 * tmesh.hpp
 *
 *  Created on: Jul 26, 2010
 */

#ifndef TMESH_HPP_
#define TMESH_HPP_

#include "vertex.hpp"
#include "half_edge.hpp"
#include "facet.hpp"
#include "tmesh_object_factory.hpp"
#include "manifold.hpp"
#include "tmesh_def.hpp"
#include "edge.hpp"
#include "add_facet.hpp"
#include "delete_facet.hpp"
#include "construct_manifolds.hpp"
#include "tmesh_modifiers.hpp"
#include "tmesh_gen.hpp"
#include "tmesh_to_off.hpp"
#include "tmesh_trees.hpp"
#include "tmesh_geom_queries.hpp"
#include "tmesh_clipper.hpp"
#include "tmesh_facets_ops.hpp"
#include "new_vertex_trigger.hpp"
#include "tmesh_wrapper.hpp"

#define _TMESH_POLICY(_SUB_MESH, STORAGE_POLICY) _SUB_MESH ## STORAGE_POLICY
#define GEN_MESH_POLICY(SUB_MESH, STORAGE_POLICY, SUB_VERTEX, SUB_HALF_EDGE, SUB_FACET) \
class _TMESH_POLICY(SUB_MESH, STORAGE_POLICY) {					\
public:									\
  template <typename _Mesh_Entities>					\
  struct tWRAPPER {							\
    typedef SUB_VERTEX<_Mesh_Entities> cVERTEX;				\
    typedef SUB_HALF_EDGE<_Mesh_Entities> cHALF_EDGE;			\
    typedef SUB_FACET<_Mesh_Entities> cFACET;				\
  };									\
}

#define TYPEDEF_SUBMESH(_NEW_NAME, SUB_MESH, STORAGE_POLICY, SUB_OBJECT_FACTORY, SUB_VERTEX, SUB_HALF_EDGE, SUB_FACET ) \
GEN_MESH_POLICY(SUB_MESH, STORAGE_POLICY, SUB_VERTEX, SUB_HALF_EDGE, SUB_FACET);	\
typedef SUB_MESH< _TMESH_POLICY(SUB_MESH, STORAGE_POLICY), STORAGE_POLICY,  SUB_OBJECT_FACTORY  > _NEW_NAME


#define GEN_AUX_CLASS(_MESH_TYPE)		\
template class tTOPO_OPERATORS< _MESH_TYPE >;	\
template class tADD_FACET< _MESH_TYPE >;	\
template class tDELETE_FACET< _MESH_TYPE >

#define GEN_SUB_MESH(SUB_MESH, STORAGE_POLICY, SUB_OBJECT_FACTORY, SUB_VERTEX, SUB_HALF_EDGE, SUB_FACET ) \
template class SUB_VERTEX< _TMESH_POLICY(SUB_MESH, STORAGE_POLICY) >;			\
template class SUB_HALF_EDGE< _TMESH_POLICY(SUB_MESH, STORAGE_POLICY) >;		\
template class SUB_FACET< _TMESH_POLICY(SUB_MESH, STORAGE_POLICY) >;			\
									\
template class SUB_OBJECT_FACTORY < _TMESH_POLICY(SUB_MESH, STORAGE_POLICY), STORAGE_POLICY >; \
template class tMANIFOLD< tMESH < _TMESH_POLICY(SUB_MESH, STORAGE_POLICY), STORAGE_POLICY,  SUB_OBJECT_FACTORY  > >; \
template class tMESH < _TMESH_POLICY(SUB_MESH, STORAGE_POLICY), STORAGE_POLICY,  SUB_OBJECT_FACTORY  >; \
template class SUB_MESH < _TMESH_POLICY(SUB_MESH, STORAGE_POLICY), STORAGE_POLICY,  SUB_OBJECT_FACTORY  >

#define GEN_MESH(STORAGE_POLICY, SUB_OBJECT_FACTORY, SUB_VERTEX, SUB_HALF_EDGE, SUB_FACET ) \
template class tVERTEX< _TMESH_POLICY(tMESH, STORAGE_POLICY) >; \
template class SUB_VERTEX< _TMESH_POLICY(tMESH, STORAGE_POLICY) >;			\
template class SUB_HALF_EDGE< _TMESH_POLICY(tMESH, STORAGE_POLICY) >;		\
template class SUB_FACET< _TMESH_POLICY(tMESH, STORAGE_POLICY) >;			\
									\
template class SUB_OBJECT_FACTORY < _TMESH_POLICY(tMESH, STORAGE_POLICY), STORAGE_POLICY >; \
template class tMANIFOLD< tMESH < _TMESH_POLICY(tMESH, STORAGE_POLICY), STORAGE_POLICY,  SUB_OBJECT_FACTORY  > >; \
template class tMESH < _TMESH_POLICY(tMESH, STORAGE_POLICY), STORAGE_POLICY,  SUB_OBJECT_FACTORY  >

#define GEN_MESH_QUERIES(_MESH_TYPE)		\
template class tMESH_GEOM_QUERIES< _MESH_TYPE >

#define GEN_MESH_CLIPPER(_MESH_TYPE) \
template class tMESH_CLIPPER< _MESH_TYPE >

#define GEN_MESH_FACETS_OPS(_MESH_TYPE ) \
template class tMESH_FACETS_OPS< _MESH_TYPE >

#endif /* TMESH_HPP_ */
