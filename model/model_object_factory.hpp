/*
 * model_object_factory.hpp
 *
 *  Created on: Apr 28, 2013
 *      Author: Bujji
 */

#ifndef MODEL_OBJECT_FACTORY_HPP_
#define MODEL_OBJECT_FACTORY_HPP_

#include "mesh.hpp"
#include "group.hpp"

namespace model
{
	template<typename _MODEL_OBJECT_ATTRIBUTES>
	class tMODEL_OBJECT_FACTORY
	{
	public:
		typedef typename _MODEL_OBJECT_ATTRIBUTES::cMESH_PROPS cMESH_PROPS;
		typedef typename _MODEL_OBJECT_ATTRIBUTES::cGROUP_PROPS cGROUP_PROPS;
		typedef tMODEL_MESH<cMESH_PROPS> cMESH;
		typedef tGROUP<cMESH, cGROUP_PROPS> cGROUP;

	public:
		/*
		 * iterator definitions for individual objects.
		 */
		typedef typename tTABLE<cMESH>::iterator mesh_iterator;
		typedef typename tTABLE<cGROUP>::iterator group_iterator;

	public:
		tMODEL_OBJECT_FACTORY() {
		}

		/*
		 * Access methods for individual objects.
		 */
		cGROUP* CreateGroup() {
			cGROUP* group = m_groups.new_object();
			group->SetIndex(m_groups.size() - 1);
			return group;
		}

		cMESH* CreateMesh() {
			cMESH* mesh = m_meshes.new_object();
			mesh->SetIndex(m_meshes.size() - 1);
			return mesh;
		}

		cGROUP* GetGroup(iMESH index) {
			return m_groups.object_at(index);
		}

		cMESH* GetMesh(iGROUP index) {
			return m_meshes.object_at(index);
		}
	public:
		/*
		 * Iterators to walk entire class of objects.
		 */
		mesh_iterator MeshesBegin() {
			return m_meshes.begin();
		}

		mesh_iterator MeshesEnd() {
			return m_meshes.end();
		}

		group_iterator GroupsBegin() {
			return m_groups.begin();
		}

		group_iterator GroupsEnd() {
			return m_groups.end();
		}

	private:
		tTABLE<cMESH> m_meshes;
		tTABLE<cGROUP> m_groups;
	};
}

#endif /* MODEL_OBJECT_FACTORY_HPP_ */
