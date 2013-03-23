/*
 * foam_cell_to_off.hpp
 *
 *  Created on: Jan 28, 2011
 */

#ifndef FOAM_CELL_TO_OFF_HPP_
#define FOAM_CELL_TO_OFF_HPP_


#include "defs.hpp"

namespace io {
  BOOL GenerateCellOffFiles(INT cellIndex, char *file_path,
		  std::vector<INT> *p_cellIndexVector = NULL,
		  std::vector<cBOX3> *p_cellBoxVector = NULL);
}

#endif /* FOAM_CELL_TO_OFF_HPP_ */
