#include "tmesh_clipper.hpp"

namespace tmesh {
  template<typename _Mesh_Type>
  BOOL tMESH_CLIPPER<_Mesh_Type>::DoOverlap(const cSEGMENT3 &result, const cSEGMENT3 &segment) {
    REAL u1 = segment.Param(result.Source());
    REAL u2 = segment.Param(result.Target());

    REAL u = u1 < u2 ? u1 : u2;
    REAL v = u1 < u2 ? u2 : u1;

    if (fabs(u - 1.0) <= cLIMITS::Tolerance())
      return false;

    if (u > 1.0)
      return false;

    if (fabs(v) <= cLIMITS::Tolerance())
      return false;

    //result and segment are identical.
    if (fabs(u) < cLIMITS::Tolerance() && fabs(1.0 - v) < cLIMITS::Tolerance())
      return false;

    if (u > 0.0 && v < 1.0)
      return false;

    return true;
  }

  template<typename _Mesh_Type>
  iFACET tMESH_CLIPPER<_Mesh_Type>::Clip(iFACET facetIndex, const cSEGMENT3 &segment, sCLIP_RESULT &result) {
    cSEGMENT3 heSeg = result.he->Segment();

    BOOL sourcePresent = heSeg.HasOn(segment.Source());
    BOOL targetPresent = heSeg.HasOn(segment.Target());

    if (!sourcePresent && !targetPresent) {
      return INVALID_IFACET;
    } else if (!sourcePresent && targetPresent) {
      if (segment.Target() == heSeg.Target() || segment.Target() == heSeg.Source())
        return INVALID_IFACET;

      iVERTEX v = m_mesh->NewVertex(segment.Target());
      m_mesh->SplitEdge(v, result.he->Tail()->Index(), result.he->Head()->Index());
    } else if (sourcePresent && !targetPresent) {
      if (segment.Source() == heSeg.Target() || segment.Source() == heSeg.Source())
        return INVALID_IFACET;

      iVERTEX v = m_mesh->NewVertex(segment.Source());
      m_mesh->SplitEdge(v, result.he->Tail()->Index(), result.he->Head()->Index());
    } else if (sourcePresent && targetPresent) {
      iVERTEX t = result.he->Tail()->Index(), h = result.he->Head()->Index();

      REAL heSegLen = SquaredDistance(heSeg.Source(), heSeg.Target());

      REAL sourceLen = sqrt(SquaredDistance(heSeg.Source(), segment.Source()) / heSegLen);
      REAL targetLen = sqrt(SquaredDistance(heSeg.Source(), segment.Target()) / heSegLen);

      REAL v1Len = sourceLen < targetLen ? sourceLen : targetLen;
      REAL v2Len = sourceLen < targetLen ? targetLen : sourceLen;

      const cPOINT3 &v1Point = sourceLen < targetLen ? segment.Source() : segment.Target();
      const cPOINT3 &v2Point = sourceLen < targetLen ? segment.Target() : segment.Source();

      ASSERT(fabs(sourceLen - targetLen) > cLIMITS::Tolerance());

      iVERTEX v1 = INVALID_IVERTEX;

      if (v1Len > cLIMITS::Tolerance())
        v1 = m_mesh->NewVertex(v1Point);

      iVERTEX v2 = INVALID_IVERTEX;

      if (1.0 - v2Len > cLIMITS::Tolerance())
        v2 = m_mesh->NewVertex(v2Point);

      if (v1 != INVALID_IVERTEX) {
        m_mesh->SplitEdge(v1, t, h);
      } else
        v1 = t;

      if (v2 != INVALID_IVERTEX) {
        m_mesh->SplitEdge(v2, v1, h);
      }
    }

    return INVALID_IFACET;
  }

  template<typename _Mesh_Type>
  iFACET tMESH_CLIPPER<_Mesh_Type>::Clip(iFACET facetIndex, const cSEGMENT3 &segment, sCLIP_RESULT results[256], INT numClips, sSEGMENTS_TAG tag) {
    INT i = 0;

    for (; i < numClips; i++) {
      if (results[i].type == RT_SEGMENT3)
        Clip(facetIndex, segment, results[i]);
    }

    return INVALID_IFACET;
  }

  template<typename _Mesh_Type>
  iFACET tMESH_CLIPPER<_Mesh_Type>::Clip(iFACET facetIndex, const cSEGMENT3 &segment, sCLIP_RESULT results[256], INT numClips, sPOINTS_TAG tag) {
    //    ASSERT(numClips == 2);
    if (numClips == 0)
      return INVALID_IFACET;

    iVERTEX v1 = INVALID_IVERTEX, v2 = INVALID_IVERTEX;

    cVERTEX* result0Tail = results[0].he->Tail();
    cVERTEX* result0Head = results[0].he->Head();

    if (results[0].point == result0Head->Point())
      v1 = result0Head->Index();
    else {
      v1 = m_mesh->NewVertex(results[0].point);
      m_mesh->SplitEdge(v1, result0Tail->Index(), result0Head->Index());
    }

    if (numClips > 1) {
      cVERTEX* result1Tail = results[1].he->Tail();
      cVERTEX* result1Head = results[1].he->Head();

      if (results[1].point == result1Head->Point())
        v2 = result1Head->Index();
      else {
        v2 = m_mesh->NewVertex(results[1].point);
        m_mesh->SplitEdge(v2, result1Tail->Index(), result1Head->Index());
      }
      return m_mesh->InsertDiagonal(facetIndex, v1, v2);
    } else
      return INVALID_IFACET;
  }

  template<typename _Mesh_Type>
  iFACET tMESH_CLIPPER<_Mesh_Type>::Clip(iFACET facetIndex, const cSEGMENT3 &segment) {
    /*
     * The assumption is that the facet is a convex polygon.
     */
    //YG comments:
    //this code clips facets located on a lattice plane by a segment located on the same plane. If so, it should be
    //simplified by dropping the plane coordinate and reducing it to 2d.
    //I added a check for numClips != 1 at the end to resolve a case of segment passing through the facet vertex
    //whose endpoints are outside of the facet
    cFACET* facet = m_mesh->Facet(facetIndex);
    cVECTOR3 facetNormal = facet->UnitNormal();

    sCLIP_RESULT results[16];
    INT numPotentialClips = 0;

    ASSERT(!facet->IsDeleted());
    typename cFACET::half_edge_circulator currHe = facet->HalfEdgesBegin();
    typename cFACET::half_edge_circulator lastHe = facet->HalfEdgesEnd();

    for (; currHe != lastHe; currHe++) {
      cVECTOR3 normal = Cross(facetNormal, currHe->UniqueVector());
      cPLANE3 verticalPlane(currHe->MidPoint(), normal);

      cSEGMENT3 currHeSeg = currHe->Segment();
      cSEGMENT3 resultSeg;
      cPOINT3 resultPoint;
      eRETURN_TYPE retVal = Intersection(verticalPlane, segment, resultPoint, resultSeg);

      if (retVal != FAILURE) {
        if (retVal == RT_POINT3 && currHe->Tail()->Point() != resultPoint && currHeSeg.HasOn(resultPoint)) {

          INT i = numPotentialClips;

          results[i].point = resultPoint;
          results[i].he = currHe.operator->();
          results[i].type = RT_POINT3;

          numPotentialClips++;
        }

        if (retVal == RT_SEGMENT3) {
          INT i = numPotentialClips;

          results[i].segment = resultSeg;
          results[i].he = currHe.operator->();
          results[i].type = RT_SEGMENT3;

          numPotentialClips++;
          break;
        }
      }
    }

    if (results[numPotentialClips - 1].type == RT_SEGMENT3)
      return Clip(facetIndex, segment, results, numPotentialClips, sSEGMENTS_TAG());  //[numPotentialClips-1]);
    else {
      return Clip(facetIndex, segment, results, numPotentialClips, sPOINTS_TAG());
    }
  }

  template<typename _Mesh_Type>
  iFACET tMESH_CLIPPER<_Mesh_Type>::Clip(iFACET facetIndex, const cSEGMENT3 &segment, BOOL checkSegmentEndPoints) {
    if (!checkSegmentEndPoints)
      return Clip(facetIndex, segment);

    cFACET* facet = m_mesh->Facet(facetIndex);
    cVECTOR3 facetNormal = facet->UnitNormal();

    sCLIP_RESULT results[256];
    INT numPotentialClips = 0;

    ASSERT(!facet->IsDeleted());

    cPOINT3 thirdPoint = segment.Source() + facetNormal;
    cPLANE3 segPlane(segment.Source(), segment.Target(), thirdPoint);

    typename cFACET::half_edge_circulator currHe = facet->HalfEdgesBegin();
    typename cFACET::half_edge_circulator lastHe = facet->HalfEdgesEnd();

    BOOL segmentAndEdgeOverlap = false;

    for (; currHe != lastHe; currHe++) {
      cSEGMENT3 currHeSeg = currHe->Segment();
      cSEGMENT3 resultSeg;
      cPOINT3 resultPoint;
      eRETURN_TYPE retVal = Intersection(segPlane, currHeSeg, resultPoint, resultSeg);

      if (retVal != FAILURE) {
        if (retVal == RT_POINT3 && currHe->Tail()->Point() != resultPoint && segment.HasOn(resultPoint)) {

          INT i = numPotentialClips;

          results[i].point = resultPoint;
          results[i].he = currHe.operator->();
          results[i].type = RT_POINT3;

          numPotentialClips++;
        }

        if (retVal == RT_SEGMENT3) {
          INT i = numPotentialClips;

          if (DoOverlap(resultSeg, segment)) {
            results[i].segment = resultSeg;
            results[i].he = currHe.operator->();
            results[i].type = RT_SEGMENT3;

            numPotentialClips++;
            segmentAndEdgeOverlap = true;
          }
        }
      }
    }

    if (segmentAndEdgeOverlap) {
      return Clip(facetIndex, segment, results, numPotentialClips, sSEGMENTS_TAG());
    } else {
      return Clip(facetIndex, segment, results, numPotentialClips, sPOINTS_TAG());
    }
  }

  template<typename _Mesh_Type>
  VOID tMESH_CLIPPER<_Mesh_Type>::GetFacetVertices(iFACET facetIndex, eCOORD coord, REAL coordVal, iVERTEX v[2]) {

    cHALF_EDGE *facetHes[2] = { NULL, NULL };
    BOOL verticesAreNew[2] = { false, false };

    cHALF_EDGE* startingHe = m_mesh->Facet(facetIndex)->HalfEdge();
    cHALF_EDGE* currHe = startingHe;

    INT i = 0;
    do {
      cPOINT3 tailPoint = m_mesh->Vertex(currHe->Tail()->Index())->Point();
      cPOINT3 headPoint = m_mesh->Vertex(currHe->Head()->Index())->Point();

      REAL heTailCoord = tailPoint[coord];
      REAL heHeadCoord = headPoint[coord];

      REAL diff1 = coordVal - heTailCoord;
      REAL diff2 = heHeadCoord - coordVal;

      if (fabs(diff2) < cLIMITS::Tolerance()) {
        currHe = currHe->Next();
        continue;
      }

      if (fabs(diff1) < cLIMITS::Tolerance())
        v[i++] = currHe->Tail()->Index();
      else if (diff1 * diff2 > 0.0) {
        cPOINT3 newPoint = ((tailPoint * diff2) + (headPoint * diff1)) / (diff1 + diff2);
        v[i++] = m_mesh->NewVertex(newPoint);
        facetHes[i - 1] = currHe;
        verticesAreNew[i - 1] = true;
      }

      if (i == 2)
        break;

      currHe = currHe->Next();
    } while (currHe != startingHe);

    if (verticesAreNew[0])
      m_mesh->SplitEdge(v[0], facetHes[0]->Tail()->Index(), facetHes[0]->Head()->Index());

    if (verticesAreNew[1])
      m_mesh->SplitEdge(v[1], facetHes[1]->Tail()->Index(), facetHes[1]->Head()->Index());
  }

  template<typename _Mesh_Type>
  iFACET tMESH_CLIPPER<_Mesh_Type>::Clip(iFACET facetIndex, eCOORD coord, REAL coordVal) {
    iVERTEX v[2] = { INVALID_IVERTEX, INVALID_IVERTEX };

    GetFacetVertices(facetIndex, coord, coordVal, v);

    if (v[0] == v[1] || v[0] == INVALID_IVERTEX || v[1] == INVALID_IVERTEX)
      return INVALID_IFACET;

    return m_mesh->InsertDiagonal(facetIndex, v[1], v[0]);
  }

}
