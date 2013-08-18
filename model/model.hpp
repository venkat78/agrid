/*
 * model.hpp
 *
 *  Created on: Apr 28, 2013
 *      Author: Bujji
 */

#ifndef _MODEL_HPP_
#define _MODEL_HPP_

#include "model_object_factory.hpp"

namespace model {
  template<typename _MODEL_OBJECT_ATTRIBUTES>
  class tMODEL {
     public:
      typedef typename _MODEL_OBJECT_ATTRIBUTES::cMESH_PROPS cMESH_PROPS;
      typedef typename _MODEL_OBJECT_ATTRIBUTES::cGROUP_PROPS cGROUP_PROPS;
      typedef tMODEL_MESH<cMESH_PROPS> cMESH;
      typedef tGROUP<cMESH, cGROUP_PROPS> cGROUP;
      typedef tMODEL_OBJECT_FACTORY<_MODEL_OBJECT_ATTRIBUTES> cFACTORY;
     public:
      typedef typename cFACTORY::mesh_iterator mesh_iterator;
      typedef typename cFACTORY::group_iterator group_iterator;

     public:
      /*
       * Individual object access methods.
       */
      cMESH* CreateMesh() {
        return m_factory.CreateMesh();
      }

      cGROUP* CreateGroup() {
        return m_factory.CreateGroup();
      }

      cGROUP *GetGroup(iGROUP index) {
        return m_factory.GetGroup(index);
      }

      cMESH* GetMesh(iMESH index) {
        return m_factory.GetMesh(index);
      }

     public:
      /*
       *  Iterator methods.
       */
      mesh_iterator MeshesBegin() {
        return m_factory.MeshesBegin();
      }

      mesh_iterator MeshesEnd() {
        return m_factory.MeshesEnd();
      }

      group_iterator GroupsBegin() {
        return m_factory.GroupsBegin();
      }

      group_iterator GroupsEnd() {
        return m_factory.GroupsEnd();
      }

     private:
      tMODEL_OBJECT_FACTORY<_MODEL_OBJECT_ATTRIBUTES> m_factory;
  };
}

#endif /* MODEL_HPP_ */
