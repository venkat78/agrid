/*
 * defs.hpp
 *
 *  Created on: Aug 19, 2010
 */

#ifndef _AGRID_MESH_DEFS_HPP_
#define _AGRID_MESH_DEFS_HPP_

#include <list>
#include <vector>
#include <ext/hash_map>

#include <cstring>

#include "utils.hpp"
using namespace utils;

#include "geom.hpp"
using namespace geom;

#include "tmesh.hpp"
using namespace tmesh;

#include "mesh_utils.hpp"
using namespace mesh_utils;

#include "surface_mesh.hpp"

#include "grid.hpp"
using namespace base_grid;

//enum eCELL_COLOR {
//  UNKNOWN=-1,
//  BLACK,
//  GRAY,
//  WHITE
//};
//
//struct sINDEX {
//  UINT64 x:16;
//  UINT64 y:16;
//  UINT64 z:16;
//  UINT64 padding : 16;
//  sINDEX(UINT64 _x = 65535, UINT64 _y =  65535, UINT64 _z = 65535) : x(_x) , y(_y), z(_z)
//  {}
//
//  UINT64 Value() const {
//    UINT64 val = 0;
//    val |= x;
//    val <<= 16;
//
//    val |= y;
//    val <<= 16;
//
//    val |= z;
//    return val;
//  }
//
//  VOID Print(FILE *fp = stdout) const {
//    fprintf(fp, "Grid index %u %u %u \n", (UINT)x, (UINT)y, (UINT)z);
//    fflush(fp);
//  }
//};
//
//typedef sINDEX iCELL_INDEX;
//typedef sINDEX iGRID_VERTEX;
//
//struct sINDEX_HASH_FCN
//{
//  UINT64 operator()(const sINDEX &i) const {
//    return i.Value();
//  }
//};
//
//inline BOOL operator < (const sINDEX &i1,
//			const sINDEX &i2)
//{
//  return i1.Value() < i2.Value();
//}
//
//inline BOOL operator == (const sINDEX &i1,
//			 const sINDEX &i2) {
//  return (i1.x == i2.x) && (i1.y == i2.y) && (i1.z == i2.z);
//}
//
//typedef __gnu_cxx::hash_map<sINDEX, eCELL_COLOR, sINDEX_HASH_FCN> cGRID_CELL_COLORS;
//typedef cGRID_CELL_COLORS cGRID_VERTEX_COLORS;
//
//typedef __gnu_cxx::hash_map<sINDEX, BOOL, sINDEX_HASH_FCN> cGRID_CELL_MARKS;
//typedef __gnu_cxx::hash_map<sINDEX, INT, sINDEX_HASH_FCN> cINDEX_TO_INT_MAP;



#endif /* DEFS_HPP_ */
