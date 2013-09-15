/*
 * amoeba_grid_clay.cpp
 *
 *  Created on: Aug 4, 2013
 *      Author: venkat
 */

#include "defs.hpp"
#include "amoeba_grid_defs.hpp"
#include "base_grid.cpp"
#include "grid_gen.cpp"

typedef cDEFINE_AMOEBA_MODEL_GRID::cGRID_CELL cAMOEBA_GRID_CELL;
typedef cDEFINE_AMOEBA_MODEL_GRID::cMESH cAMOEBA_MESH;
typedef cDEFINE_AMOEBA_MODEL_GRID::cMANIFOLD_OBJ cMANIFOLD_OBJ;

template class tCUT_CELL_BUILDER<cMANIFOLD_OBJ, cAMOEBA_GRID_CELL>;
template class tGRID_CELL<cAMOEBA_MESH>;
template class tGRID<cAMOEBA_GRID_CELL> ;
