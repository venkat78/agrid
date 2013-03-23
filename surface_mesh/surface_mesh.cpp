/*
 * surface_mesh.cpp
 *
 *  Created on: Aug 11, 2010
 */

#include "surface_mesh_class.hpp"
#include "tmesh.cpp"

GEN_AUX_CLASS(cSURFACE_MESH);

GEN_SUB_MESH(tSURFACE_MESH, sSURFACE_MESH_STORAGE_POLICY, tMESH_OBJECT_FACTORY, tVERTEX, tHALF_EDGE, tFACET);

