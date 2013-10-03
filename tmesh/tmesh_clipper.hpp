#ifndef _TMESH_CLIPPER_HPP_
#define _TMESH_CLIPPER_HPP_

#include "defs.hpp"
#include "misc.hpp"

namespace tmesh {
  /*
   * Generic class to support geometric queries.
   * Will add other clipper methods of tMESH to this class.
   */
  template<typename _Mesh_Type>
  class tMESH_CLIPPER {
      _TMESH_ENTITIES(_Mesh_Type);

      struct sCLIP_RESULT {
        eRETURN_TYPE type;
        cPOINT3 point;
        cSEGMENT3 segment;
        cHALF_EDGE *he;
      };

      struct sSEGMENTS_TAG {};
      struct sPOINTS_TAG {};

      public:
      tMESH_CLIPPER(_Mesh_Type *mesh = NULL) : m_mesh(mesh)
      {}

      iFACET Clip(iFACET facetIndex, const cSEGMENT3 &segment);

      iFACET Clip(iFACET facetIndex, const cSEGMENT3 &segment,
          BOOL checkSegmentEndPoints);

      /*
       * Method to split a facet by a coordinate value.
       */
      iFACET Clip(iFACET facetIndex, eCOORD coord, REAL coordVal);

      private:
      iFACET Clip(iFACET facetIndex, const cSEGMENT3 &segment, sCLIP_RESULT &result);

      iFACET Clip(iFACET facetIndex, const cSEGMENT3 &segment,
          sCLIP_RESULT result[256], INT numClips, sSEGMENTS_TAG tag);

      iFACET Clip(iFACET facetIndex, const cSEGMENT3 &segment,
          sCLIP_RESULT results[256], INT numClips, sPOINTS_TAG tag);

      BOOL DoOverlap(const cSEGMENT3 &result, const cSEGMENT3 &segment);
      VOID GetFacetVertices(iFACET facetIndex, eCOORD coord, REAL coordVal, iVERTEX v[2]);

      private:
      _Mesh_Type *m_mesh;
    };}

#endif //_TMESH_CLIPPER_HPP_
