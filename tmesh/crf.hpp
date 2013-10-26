/*
 * crf.hpp
 *
 *  Created on: May 31, 2013
 *      Author: yury
 */

#ifndef _TMESH_CRF_HPP_
#define _TMESH_CRF_HPP_

/*
 * crf.hpp
 *
 *  Created on: May 31, 2013
 *      Author: yury
 */

#include "defs.hpp"

//crf - compressed row format is a format for representing sparse matrices.
//In agrid it is used for representing objects with multiple elements, like
//for representing facets and their vertices. CRF contains two arrays, for which
//we use traditional names from matrix theory, but they are meaningless for our purpose.
//rowArray contains accumulated number of elements in each object, so
//rowArray[0] = 0 and rowArray[i+1] - rowArray[i] = number of elements of object[i]
//and colArray contains indices of sequential elements of objects in their original order.

namespace tmesh {
  class cCRF {
  public:
	VOID ColPushBack(INT a) {m_colArray.push_back(a);}
	VOID RowPushBack(INT a) {m_rowArray.push_back(a);}
	VOID ObjectPushBack(INT a) {m_colArray.push_back(a);}
	VOID ElementPushBack(INT a) {m_rowArray.push_back(a);}

  private: //member fields
	std::vector<INT> m_colArray;
	std::vector<INT> m_rowArray;
  };

}//namespace


#endif // _TMESH_CRF_HPP_
