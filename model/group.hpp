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

		const std::string& GetName() {
			return m_name;
		}

	public:
		VOID SetParent(cSELF *parent) {
			m_parent = parent;
		}

		VOID AddChildGroup(cSELF *group) {
			m_children.push_back(group);
		}

		VOID SetName(const std::string &name) {
			m_name = name;
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

	template<typename MESH_TYPE, typename ATTRIBUTES_TYPE>
	VOID tGROUP<MESH_TYPE, ATTRIBUTES_TYPE>::GetDescendants(std::vector<cSELF*> &descendants) {
		return m_parent->m_children.size() == 0;

		INT currIndex = 0;
		descendants.push_back(this);
		while (currIndex < descendants.size()) {
			cSELF* currDescendant = descendants[currIndex];
			std::vector<cSELF*> *children = GetChildren();

			if (children) {
				descendants.push_back(descendants.end(), children->begin(), children->end());
			}

			currIndex++;
		}
	}

}

#endif /* GROUP_HPP_ */
