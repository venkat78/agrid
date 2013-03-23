#ifndef _AGRID_BOOLEAN_FOAM_EXPORT_HPP_
#define _AGRID_BOOLEAN_FOAM_EXPORT_HPP_

#include "cut_cells_adhesive.hpp"
#include "bob.hpp"

namespace agrid_boolean {
  BOOL PrintDebugFiles();
  VOID PrintDebugFiles(BOOL doPrint);

  VOID ExportGridToFoam(const std::string &filePath, cVOLUMETRIC_GRID &cutCellGrid,
				  cBOB *bob = NULL);

}

#endif //_AGRID_BOOLEAN_FOAM_EXPORT_HPP_
