/*
 * stl.hpp
 *
 *  Created on: Jul 23, 2009
 */

#include "defs.hpp"

namespace io {
  bool ImportStl(FILE *file, cSURFACE_MESH &mesh);
  bool ImportStl(FILE *file, cGENERIC_MODEL &model);
}
