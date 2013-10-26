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
	int m_color[3];
	int m_cindex;
	int m_meshing_priority;
	BOOL m_is_hollow;
	cVECTOR3 m_mesh_max_size;
	cVECTOR3 m_mesh_min_size;
};

BOOL MeshProcessingPriority(sAMOEBA_MESH_ATTRIBUTES *attr1, sAMOEBA_MESH_ATTRIBUTES *attr2);

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


	VOID MaxMeshSize(cVECTOR3 &size){m_max_mesh_size = size;}

	cVECTOR3 MaxMeshSize(){return m_max_mesh_size;}

	VOID MinMeshSize(cVECTOR3 &size){m_min_mesh_size = size;}

	cVECTOR3 MinMeshSize(){return m_min_mesh_size;}

	VOID AngleTolerance(REAL tol){m_angleTolerance = tol;}

	REAL AngleTolerance(){return m_angleTolerance;}


private:
	cGROUP *m_rootGroup;
	cVECTOR3 m_max_mesh_size;
	cVECTOR3 m_min_mesh_size;
	REAL m_angleTolerance;

};
/*
class cAMOEBA_SUBDIVIDE_OP
{
	eCOORD operator () (cGRID_CELL);
};
*/
#endif //_AMOEBA_MODEL_HPP
