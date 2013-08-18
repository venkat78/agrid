/*
 * generic_model.hpp
 *
 *  Created on: May 1, 2013
 *      Author: Venkat
 */

#ifndef _GENERIC_MODEL_HPP_
#define _GENERIC_MODEL_HPP_

#include "model.hpp"
using namespace model;

struct sGENERIC_GROUP_ATTRIBUTES {
};

struct sGENERIC_MESH_ATTRIBUTES {

};

struct sGENERIC_MODEL_ATTRIBUTES {
    typedef sGENERIC_MESH_ATTRIBUTES cMESH_PROPS;
    typedef sGENERIC_GROUP_ATTRIBUTES cGROUP_PROPS;
};

typedef tMODEL<sGENERIC_MODEL_ATTRIBUTES> cGENERIC_MODEL_BASE;

#define MODEL_TYPENAMES(MODEL) \
typedef typename MODEL::cGROUP cGROUP; \
typedef typename MODEL::cMESH cMESH;\
typedef typename MODEL::mesh_iterator mesh_iterator;\
typedef typename MODEL::group_iterator group_iterator

class cGENERIC_MODEL : public cGENERIC_MODEL_BASE {
   public:
    cGENERIC_MODEL()
        : cGENERIC_MODEL_BASE() {
      m_rootGroup = NULL;
    }

    MODEL_TYPENAMES(cGENERIC_MODEL_BASE);

    public:
    cGROUP* GetRootGroup() {
      return m_rootGroup;
    }

    VOID SetRootGroup(cGROUP *g) {
      m_rootGroup = g;
    }

    private:
    cGROUP *m_rootGroup;
  };
#endif //_GENERIC_MODEL_HPP
