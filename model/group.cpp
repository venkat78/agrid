/*
 * group.cpp
 *
 *  Created on: Apr 27, 2013
 *      Author: Bujji
 */

#include "group.hpp"

namespace model
{
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
