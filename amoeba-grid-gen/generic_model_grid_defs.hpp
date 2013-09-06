/*
 * amoeba_grid_defs.hpp
 *
 *  Created on: Aug 4, 2013
 *      Author: venkat
 */

#ifndef _GENERIC_MODEL_GRID_DEFS_HPP_
#define _GENERIC_MODEL_GRID_DEFS_HPP_

#include "defs.hpp"

typedef tDEFINE_MODEL_GRID<cGENERIC_MODEL> cDEFINE_GENERIC_MODEL_GRID;
typedef typename cDEFINE_GENERIC_MODEL_GRID::cGRID_CELL cGENERIC_MODEL_GRID_CELL;
typedef typename cDEFINE_GENERIC_MODEL_GRID::cMANIFOLD_OBJ cGENERIC_MODEL_MANIFOLD_OBJ;
typedef tCUT_CELL_BUILDER<cGENERIC_MODEL_MANIFOLD_OBJ, cGENERIC_MODEL_GRID_CELL> cGENERIC_MODEL_CUT_CELL_BUILDER;
typedef tGRID_GENERATOR<cGENERIC_MODEL> cGENERIC_MODEL_GRID_GENERATOR;

#endif /* _GENERIC_MODEL_GRID_DEFS_HPP_ */
