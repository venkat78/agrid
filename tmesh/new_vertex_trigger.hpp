/*
 * new_vertex_trigger.hpp
 *
 *  Created on: Aug 4, 2011
 */

#ifndef NEW_VERTEX_TRIGGER_HPP_
#define NEW_VERTEX_TRIGGER_HPP_

#include "defs.hpp"

namespace tmesh {

struct sVERTEX_INFO {
	  iVERTEX meshVertexIndex;
	  INT outputIndex; //e.g., index in the OpenFOAM output
	  cPOINT3 vertexPoint;
};

class cNEW_VERTEX_TRIGGER
{
public:
	cNEW_VERTEX_TRIGGER () {m_currVertexInfoIndex = 0;}

	INT ReadVertexIndices(const char *filename);
	INT ReadVertexInfos(const char *filename);
	BOOL DoTrigger(iVERTEX newVertexIndex);

private:
	std::vector<iVERTEX> m_vertexIndices;
	std::vector<sVERTEX_INFO> m_vertexInfoTable;
	INT m_currVertexInfoIndex;
};

}//namespace

#endif /* NEW_VERTEX_TRIGGER_HPP_ */
