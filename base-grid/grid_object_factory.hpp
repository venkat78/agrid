#ifndef _BASE_GRID_GRID_OBJECT_FACTORY_HPP_
#define _BASE_GRID_GRID_OBJECT_FACTORY_HPP_

#include "defs.hpp"

namespace base_grid {
template<typename _ELT_TYPE, typename _GRID_STORAGE_POLICY>
class tGRID_OBJECT_FACTORY {
  typedef __gnu_cxx::hash_map<sINDEX, _ELT_TYPE*, sINDEX_HASH_FCN> cMAP;
 public:
  tGRID_OBJECT_FACTORY() : m_elements(_GRID_STORAGE_POLICY::numObjectsInPage,
                                      _GRID_STORAGE_POLICY::logOfObjectsInPage,
                                      _GRID_STORAGE_POLICY::numInitialPages, 0)
 {

 }
  //Constructor
 public:
  _ELT_TYPE* GetElement(INT index) {
    return m_elements.object_at(index);
  }

  /*
   * Elements for coarse map.
   */
  _ELT_TYPE* CoarseElement(sINDEX index) {
    typename cMAP::iterator itr = m_coarseMap.find(index);
    if (itr == m_coarseMap.end())
      return NULL;

    return itr->second;
  }

  _ELT_TYPE* NewElement(sINDEX index) {
    _ELT_TYPE *newElement = m_elements.new_object();
    INT localIndex = m_elements.size() - 1;
    m_coarseMap[index] = newElement;
    newElement->Index(localIndex);
    return newElement;
  }

  _ELT_TYPE* NewElement() {
    _ELT_TYPE *newElement = m_elements.new_object();
    INT localIndex = m_elements.size() - 1;
    newElement->Index(localIndex);
    return newElement;
  }

 private:
  cMAP m_coarseMap;
  tTABLE<_ELT_TYPE> m_elements;
};

}

#endif //_AGRID_BOOLEAN_GRID_OBJECT_FACTORY_HPP_
