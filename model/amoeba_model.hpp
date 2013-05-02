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

typedef tMODEL<sAMOEBA_MODEL_ATTRIBUTES> cAMOEBA_MODEL;
#endif //_AMOEBA_MODEL_HPP
