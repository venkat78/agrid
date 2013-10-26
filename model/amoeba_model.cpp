/*
 * amoeba_model.cpp
 *
 *  Created on: May 1, 2013
 *      Author: Bujji
 */

#include "amoeba_model.hpp"

template class tMODEL<sAMOEBA_MODEL_ATTRIBUTES>;


BOOL MeshProcessingPriority(sAMOEBA_MESH_ATTRIBUTES *attr1, sAMOEBA_MESH_ATTRIBUTES *attr2)
{
	return attr1->m_cindex > attr2->m_cindex;
}
