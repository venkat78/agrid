/*
 * cut_cell_clay.hpp
 *
 *  Created on: Aug 19, 2010
 */

#ifndef _AGRID_MESH_CUT_CELL_CLAY_HPP_
#define _AGRID_MESH_CUT_CELL_CLAY_HPP_

#include "defs.hpp"

/*
 * Defining basic types for cut cell clay mesh.
 */
template<typename _Mesh_Entities>
class tCUT_CELL_CLAY_VERTEX : public tmesh::tVERTEX<_Mesh_Entities> {
    typedef tmesh::tVERTEX<_Mesh_Entities> cBASE;

   public:
    tCUT_CELL_CLAY_VERTEX()
        : cBASE(),
//          m_isReflection(false),
//          m_reflection(INVALID_IVERTEX),
//          m_srcIndex(INVALID_IVERTEX),
          m_color(UNKNOWN) {
    }

    tCUT_CELL_CLAY_VERTEX(const cPOINT3 &point)
        : cBASE(point),
//          m_isReflection(false),
//          m_reflection(INVALID_IVERTEX),
//          m_srcIndex(INVALID_IVERTEX),
//          m_srcMesh(NULL),
          m_color(UNKNOWN) {
    }

    ~tCUT_CELL_CLAY_VERTEX() {
    }

    VOID Color(eCELL_COLOR color) {
      m_color = color;
    }

    eCELL_COLOR Color() {
      return m_color;
    }

//  //For grid vertex mesh = NULL;
//  VOID Source(INT srcIndex, cSURFACE_MESH *mesh = NULL) {
//    m_srcIndex = srcIndex;
//    m_srcMesh = mesh;
//  }
//
//  iVERTEX SourceIndex() {
//    return m_srcIndex;
//  }
//
//  cSURFACE_MESH *SourceMesh() {
//    return m_srcMesh;
//  }
//
//  VOID IsReflection(BOOL flag) { m_isReflection = flag; }
//
//  BOOL IsReflection() { return m_isReflection; }
//
//  VOID Reflection(iVERTEX vertexIndex) {
//    m_reflection = vertexIndex;
//  }
//
//  iVERTEX Reflection() { return m_reflection; }

   protected:
//  BOOL m_isReflection;
//  iVERTEX m_reflection;
//  iVERTEX m_srcIndex;
//  cSURFACE_MESH *m_srcMesh; //For a grid vertex m_srcMesh = NULL;
    eCELL_COLOR m_color;
};

template<typename _Mesh_Entities>
class tCUT_CELL_CLAY_FACET : public tmesh::tFACET<_Mesh_Entities> {
    typedef tCUT_CELL_CLAY_FACET<_Mesh_Entities> cSELF;
    typedef tmesh::tFACET<_Mesh_Entities> cBASE;

   public:
    tCUT_CELL_CLAY_FACET()
        : cBASE(),
          m_color(UNKNOWN) {
    }

    tCUT_CELL_CLAY_FACET(const cSELF &f)
        : cBASE(f),
          m_color(f.m_color) {
    }

    ~tCUT_CELL_CLAY_FACET() {
    }

    VOID Color(eCELL_COLOR color) {
      m_color = color;
    }

    eCELL_COLOR Color() {
      return m_color;
    }

   protected:
    eCELL_COLOR m_color;
};

struct sCUT_CELL_CLAY_STORAGE_POLICY {
    static const INT numObjectsInPage = 16;
    static const INT logOfObjectsInPage = 4;
    static const INT numInitialPages = 4;
};

TYPEDEF_SUBMESH(cCUT_CELL_CLAY, tMESH, sCUT_CELL_CLAY_STORAGE_POLICY, tMESH_OBJECT_FACTORY, tCUT_CELL_CLAY_VERTEX, tHALF_EDGE, tCUT_CELL_CLAY_FACET);

struct sCUT_CELL_CLAY_FILTER {
   public:
    sCUT_CELL_CLAY_FILTER(cCUT_CELL_CLAY *clay = NULL)
        : m_clay(clay) {
    }

    BOOL operator ()(iFACET faceIndex) const {
      return true;
    }
    sCOLOR Color(iFACET facetIndex) {
      eCELL_COLOR facetColor = m_clay->Facet(facetIndex)->Color();

      if (facetColor == WHITE)
        return sCOLOR(1.0, 1.0, 0.0, 1.0);
      else
        return sCOLOR(0.0, 0.0, 0.0, 1.0);
    }

   private:
    cCUT_CELL_CLAY *m_clay;
};

#endif /* CUT_CELL_CLAY_HPP_ */
