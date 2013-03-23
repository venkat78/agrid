/*
 * mesh_euler_operators.cpp
 *
 *  Created on: Jun 7, 2009
 */
#include "tmesh_def.hpp"

namespace tmesh
{

  _TMESH_TMPL_TYPE
  iFACET _TMESH_TMPL_DECL::InsertDiagonal(iFACET facetIndex, iVERTEX diagEndPoint1,
			       iVERTEX diagEndPoint2, iFACET newFacetIndex)
  {
    return cTOPO_OPERATORS(this).InsertDiagonal(facetIndex, diagEndPoint1,
						diagEndPoint2, newFacetIndex);
  }
  
  _TMESH_TMPL_TYPE
  BOOL  _TMESH_TMPL_DECL::RemoveDiagonal(iVERTEX diagEndPoint1, iVERTEX diagEndPoint2,
      iFACET facetIndex1, iFACET facetIndex2, iFACET facetIndexToBeDeleted)
  {
    return cTOPO_OPERATORS(this).RemoveDiagonal(diagEndPoint1, diagEndPoint2,
						facetIndex1, facetIndex2, facetIndexToBeDeleted);
  }

  _TMESH_TMPL_TYPE
  BOOL _TMESH_TMPL_DECL::SplitEdge(iVERTEX vertexToBeInserted, iVERTEX edgeTailVtxIndex,
      iVERTEX edgeHeadVtxIndex)
  {
    return cTOPO_OPERATORS(this).SplitEdge(vertexToBeInserted,
					   edgeTailVtxIndex, edgeHeadVtxIndex);
  }

  //Inverse of split-edge.
  _TMESH_TMPL_TYPE
  BOOL _TMESH_TMPL_DECL::MergeEdgesAtVertex(iVERTEX vertexIndex)
  {
    return cTOPO_OPERATORS(this).MergeEdgesAtVertex(vertexIndex);
  }
  
  _TMESH_TMPL_TYPE
  BOOL _TMESH_TMPL_DECL::MergeVertices(iVERTEX v1Index, iVERTEX v2Index,
				       iVERTEX retainVtxIndex, const cPOINT3 &newVtxPoint,
				       sMV_TRANS *transaction)
  {
    cVERTEX* v1 = Vertex(v1Index);
    cVERTEX* v2 = Vertex(v2Index);
    
    if (v1->IsDeleted() || v2->IsDeleted())
      return false;
    
    if (v1Index != retainVtxIndex && v2Index != retainVtxIndex)
      return false;

    iVERTEX vanishingVtxIndex = (v1Index == retainVtxIndex ? v2Index : v1Index);
    if (transaction)
      {
        transaction->retainedVtxIndex = retainVtxIndex;
        cPOINT3 vtxPoint = Vertex(retainVtxIndex)->Point();

        transaction->x = vtxPoint[0];
        transaction->y = vtxPoint[1];
        transaction->z = vtxPoint[2];
      }

    cHALF_EDGE* v1ToV2He = NULL;
    v1->Find(v2, &v1ToV2He, NULL);

    iFACET facetsOnEdge[ARRAY_SIZE];

    INT numFacetsOnEdge = 0;

    if (v1ToV2He != NULL)
      {
        cHALF_EDGE* loopingHe = v1ToV2He;

        do
          {
            if (!loopingHe->IsBorder())
              facetsOnEdge[numFacetsOnEdge++] = loopingHe->Facet()->Index();

            loopingHe = loopingHe->NextInRing();
          }
        while (loopingHe != v1ToV2He);

        if (transaction)
          {
            for (INT i = 0; i < numFacetsOnEdge; i++)
              {
                sFACET_INFO facetInfo;
                cFACET* fOnEdge = Facet(facetsOnEdge[i]);
                facetInfo.index = facetsOnEdge[i];

                typename cFACET::vertex_circulator firstVtx = fOnEdge->VerticesBegin();
                typename cFACET::vertex_circulator lastVtx = fOnEdge->VerticesEnd();

                for (; firstVtx != lastVtx; firstVtx++)
                  facetInfo.vertices.push_back(firstVtx->Index());

                transaction->oldFacets.push_back(facetInfo);
              }
          }
      }

    std::vector<sFACET_INFO> facetsNewInfo;

    for (INT i = 0; i < numFacetsOnEdge; i++)
      {
        cFACET* fOnEdge = Facet(facetsOnEdge[i]);

        if (fOnEdge->NumVertices() > 3)
          {
            sFACET_INFO facetInfo;

            facetInfo.index = facetsOnEdge[i];
            typename cFACET::vertex_circulator firstVtx = fOnEdge->VerticesBegin();
            typename cFACET::vertex_circulator lastVtx = fOnEdge->VerticesEnd();
	    
            for (; firstVtx != lastVtx; firstVtx++) {
	      if(firstVtx->Index() != vanishingVtxIndex)
		facetInfo.vertices.push_back(firstVtx->Index());
	    }
	    
            facetsNewInfo.push_back(facetInfo);
          }
      }

    cVERTEX* vanishingVtx = Vertex(vanishingVtxIndex);

    std::vector<cFACET*> incidentFacets;
    vanishingVtx->IncidentFacets(incidentFacets);

      {
        typename std::vector<cFACET*>::iterator currFacet = incidentFacets.begin();
        typename std::vector<cFACET*>::iterator lastFacet = incidentFacets.end();

        for (; currFacet != lastFacet; currFacet++)
          {
            ASSERT(!(*currFacet)->IsDeleted());
            iFACET facetIndex = (*currFacet)->Index();
            if (!ValIsInTable<iFACET> (facetIndex, facetsOnEdge,
                numFacetsOnEdge))
              {
                sFACET_INFO facetInfo;
                facetInfo.index = facetIndex;

                sFACET_INFO undoFacetInfo;

                if (transaction)
                  undoFacetInfo.index = facetIndex;

                typename cFACET::vertex_circulator currVtx =
                    (*currFacet)->VerticesBegin();
                typename cFACET::vertex_circulator lastVtx = (*currFacet)->VerticesEnd();

                for (; currVtx != lastVtx; currVtx++)
                  {
                    iVERTEX vtxIndex = currVtx->Index();

                    if (vtxIndex == vanishingVtxIndex)
                      facetInfo.vertices.push_back(retainVtxIndex);
                    else
                      facetInfo.vertices.push_back(vtxIndex);

                    if (transaction)
                      undoFacetInfo.vertices.push_back(vtxIndex);
                  }

                if (transaction)
                  transaction->oldFacets.push_back(undoFacetInfo);

                facetsNewInfo.push_back(facetInfo);
              }
          }
      }
      {
        typename std::vector<cFACET*>::iterator currFacet = incidentFacets.begin();
        typename std::vector<cFACET*>::iterator lastFacet = incidentFacets.end();

        for (; currFacet != lastFacet; currFacet++) {
          DeleteFacet((*currFacet)->Index());
	}

        std::vector<sFACET_INFO>::iterator currFacetInfo =
            facetsNewInfo.begin();
        std::vector<sFACET_INFO>::iterator lastFacetInfo = facetsNewInfo.end();

        for (; currFacetInfo != lastFacetInfo; currFacetInfo++) {

            std::list<iVERTEX>::iterator currVtxIndex =
                currFacetInfo->vertices.begin();
            std::list<iVERTEX>::iterator lastVtxIndex =
                currFacetInfo->vertices.end();

            NewFacetStart(currFacetInfo->index);

            for (; currVtxIndex != lastVtxIndex; currVtxIndex++)
              NewFacetAddVertex(*currVtxIndex);

            NewFacetClose();
          }
      }
    Vertex(retainVtxIndex)->Point(newVtxPoint);

    return true;
  }

}
