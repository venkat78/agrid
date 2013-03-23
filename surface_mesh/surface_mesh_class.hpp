#ifndef _SURFACE_MESH_CLASS_HPP_
#define _SURFACE_MESH_CLASS_HPP_

#include "defs.hpp"
#include "surface_mesh_def.hpp"

struct sSURFACE_MESH_STORAGE_POLICY {
  static const INT numObjectsInPage = 512;
  static const INT logOfObjectsInPage = 9;
  static const INT numInitialPages = 512;
};

TYPEDEF_SUBMESH(cSURFACE_MESH, tSURFACE_MESH, sSURFACE_MESH_STORAGE_POLICY,
		tMESH_OBJECT_FACTORY, tVERTEX, tHALF_EDGE, tFACET);

VOID  CopyManifolds(std::vector <iMANIFOLD> &manifoldIndices,  cSURFACE_MESH &meshPortion);

#endif /*MESH_HPP_*/
