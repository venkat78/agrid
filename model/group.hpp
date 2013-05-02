/*
 * collection.hpp
 *
 *  Created on: Apr 27, 2013
 *      Author: Bujji
 */

#ifndef _MODEL_GROUP_HPP_
#define _MODEL_GROUP_HPP_

#include "defs.hpp"

#include <string>
#include <vector>

namespace model
{
	template<typename MESH_TYPE, typename ATTRIBUTES_TYPE>
	struct tGROUP
	{
		typedef tGROUP<MESH_TYPE, ATTRIBUTES_TYPE> cSELF;

	public:

		iGROUP GetIndex() {
			return m_index;
		}

		INT NumChildren() {
			return m_children.size();
		}

		cSELF*
		GetParent()
		{
			return m_parent;
		}

		cSELF*
		GetChildGroup(const std::string &name);

		VOID SetIndex(iGROUP index) {
			m_index = index;
		}

		VOID SetMesh(MESH_TYPE *mesh) {
			m_mesh = mesh;
		}

		MESH_TYPE*
		GetMesh()
		{
			return m_mesh;
		}

		ATTRIBUTES_TYPE*
		GetAttributes()
		{
			return m_attributes;
		}

		BOOL IsMeshRoot() {
			return (m_mesh != NULL) && ((m_parent == NULL) || (m_parent->m_mesh == NULL));
		}

		std::vector<cSELF*>*
		GetChildren()
		{
			return &m_children;
		}

	public:
		VOID SetParent(cSELF *parent) {
			m_parent = parent;
		}

		VOID AddChildGroup(cSELF *group) {
			m_children.push_back(group);
		}

	public:
		tGROUP() {
			m_index = INVALID_GROUP_INDEX;
			m_attributes = new ATTRIBUTES_TYPE;
			m_mesh = NULL;
			m_parent = NULL;
		}

		~tGROUP() {
		}

	private:
		VOID GetDescendants(std::vector<cSELF*> &descendants);

	protected:
		cSELF* m_parent;
		MESH_TYPE *m_mesh;
		std::string m_type;
		std::string m_name;
		std::vector<cSELF*> m_children;
		iGROUP m_index;
		ATTRIBUTES_TYPE *m_attributes;
	};

}

#endif /* GROUP_HPP_ */
