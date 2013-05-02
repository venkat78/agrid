/*
 * mesh.hpp
 *
 *  Created on: Apr 28, 2013
 *      Author: Bujji
 */

#ifndef _MODEL_MESH_HPP_
#define _MODEL_MESH_HPP_

#include "defs.hpp"

namespace model
{
	template<typename _MESH_ATTRIBUTES>
	class tMODEL_MESH: public cSURFACE_MESH
	{
	public:
		tMODEL_MESH() :
				cSURFACE_MESH()
		{
			m_index = INVALID_MESH_INDEX;
			m_attributes = new _MESH_ATTRIBUTES;
		}

		~tMODEL_MESH() {
			delete m_attributes;
		}

		_MESH_ATTRIBUTES* GetAttributes() {
			return m_attributes;
		}

		const std::string& GetName() {
			return m_name;
		}

		VOID SetName(const std::string &name) {
			m_name = name;
		}

		iMESH GetIndex() {
			return m_index;
		}

		VOID SetIndex(iMESH index) {
			m_index = index;
		}
	private:
		_MESH_ATTRIBUTES *m_attributes;
		std::string m_name;
		iMESH m_index;
	};
}

#endif /* MESH_HPP_ */
