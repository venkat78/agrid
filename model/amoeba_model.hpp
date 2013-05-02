/*
 * amoeba_model.hpp
 *
 *  Created on: May 1, 2013
 *      Author: Bujji
 */

#ifndef _AMOEBA_MODEL_HPP_
#define _AMOEBA_MODEL_HPP_

#include "model.hpp"
using namespace model;

struct sAMOEBA_GROUP_ATTRIBUTES
{
};

struct sAMOEBA_MESH_ATTRIBUTES
{

};

struct sAMOEBA_MODEL_ATTRIBUTES
{
	typedef sAMOEBA_MESH_ATTRIBUTES cMESH_PROPS;
	typedef sAMOEBA_GROUP_ATTRIBUTES cGROUP_PROPS;
};

typedef tMODEL<sAMOEBA_MODEL_ATTRIBUTES> cAMOEBA_MODEL_BASE;

#define MODEL_TYPENAMES(MODEL) \
typedef typename MODEL::cGROUP cGROUP; \
typedef typename MODEL::cMESH cMESH;\
typedef typename MODEL::mesh_iterator mesh_iterator;\
typedef typename MODEL::group_iterator group_iterator

class cAMOEBA_MODEL: public cAMOEBA_MODEL_BASE
{
public:
	cAMOEBA_MODEL() :
			cAMOEBA_MODEL_BASE()
	{
		m_rootGroup = NULL;
	}

	MODEL_TYPENAMES(cAMOEBA_MODEL_BASE);

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
#endif //_AMOEBA_MODEL_HPP
