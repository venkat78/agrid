/*
 * facet_group.hpp
 *
 *  Created on: Jul 23, 2009
 */

#ifndef _MESH_UTILS_FACET_GROUP_HPP_
#define _MESH_UTILS_FACET_GROUP_HPP_

#include "defs.hpp"
#include <string>
#include <vector>

/*
 * class defines mesh facet_group.
 */
namespace mesh_utils {
  class cFACET_GROUP {
  public:
    cFACET_GROUP() { }
    cFACET_GROUP(const std::string &name) : m_name(name)
    {}

  public:
    std::vector<iFACET> m_facets;
    std::string m_name;
  };
}
#endif /* MESH_FACET_GROUP_HPP_ */
