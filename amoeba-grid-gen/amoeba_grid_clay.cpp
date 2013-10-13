/*
 * amoeba_grid_clay.cpp
 *
 *  Created on: Aug 4, 2013
 *      Author: venkat
 */

#include "defs.hpp"
#include "amoeba_grid_defs.hpp"
#include "tmesh.cpp"
#include "base_grid.cpp"
#include "grid_gen.cpp"

typedef typename cDEFINE_AMOEBA_MODEL_GRID::cGRID_CELL cAMOEBA_GRID_CELL;
typedef typename cDEFINE_AMOEBA_MODEL_GRID::cMESH cAMOEBA_MESH;
typedef typename cDEFINE_AMOEBA_MODEL_GRID::cMANIFOLD_OBJ cMANIFOLD_OBJ;

GEN_MESH_CLIPPER(cAMOEBA_MESH);
template class tCUT_CELL_BUILDER<cMANIFOLD_OBJ, cAMOEBA_GRID_CELL>;
template class tGRID_CELL<cAMOEBA_MESH>;
template class tGRID<cAMOEBA_GRID_CELL> ;
