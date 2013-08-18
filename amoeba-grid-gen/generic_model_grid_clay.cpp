/*
 * amoeba_grid_clay.cpp
 *
 *  Created on: Aug 4, 2013
 *      Author: venkat
 */

#include "defs.hpp"
#include "generic_model_grid_defs.hpp"
#include "base_grid.cpp"

typedef cDEFINE_GENERIC_MODEL_GRID::cGRID_CELL cGENERIC_MODEL_GRID_CELL;
typedef cDEFINE_GENERIC_MODEL_GRID::cMESH cGENERIC_MESH;
typedef cDEFINE_GENERIC_MODEL_GRID::cMANIFOLD_OBJ cMANIFOLD_OBJ;

template class tCUT_CELL_BUILDER<cMANIFOLD_OBJ, cGENERIC_MODEL_GRID_CELL>;
template class tGRID_CELL<cGENERIC_MESH>;
template class tGRID<cGENERIC_MODEL_GRID_CELL> ;
