/*
 * mesh_geom_info.cpp
 *
 *  Created on: Aug 9, 2009
 */
#include "tmesh_def.hpp"

namespace tmesh {
//REAL ComputeMaskBits(INT *mask);

  _TMESH_TMPL_TYPE
    cBOX3 _TMESH_TMPL_DECL::BoundingBox()
    {
        cBOX3 box;

        vertex_iterator currVtx = VerticesBegin();
        vertex_iterator lastVtx = VerticesEnd();

        for( ; currVtx != lastVtx ; currVtx++)
          box += currVtx->Point();

        return box;
    }

  _TMESH_TMPL_TYPE
    REAL _TMESH_TMPL_DECL::ShadeArea(eCOORD coord)
  {
	cBOX2 meshBox2 = BoundingBox().DropCoord(coord);
	//The 2d projection of the bounding box is covered by 511*511 rectangles.
	//In the first implementation the grid has 511 rectangles in each of the two directions.
	//this can be improved by fitting the number of rectangles along each coordinate to
	//the bounding box dimention along this coordinate.

	REAL xStep = (meshBox2.XMax() - meshBox2.XMin())/511.0;
	REAL yStep = (meshBox2.YMax() - meshBox2.YMin())/511.0;

	BOOL bitMask[512][512]; //this ugly code will be changedf
	for (INT i=0; i < 512; i++)
	  for (INT j=0; j < 512; j++)
		bitMask[i][j] = false;

    facet_iterator currFacet = FacetsBegin();
    facet_iterator lastFacet = FacetsEnd();

    for( ; currFacet != lastFacet ; currFacet++) {
      typename cFACET::vertex_circulator currVertex = currFacet->VerticesBegin();
      typename cFACET::vertex_circulator lastVertex = currFacet->VerticesEnd();
      INT index = 0;
      cPOINT2 point[3];
      //temporarily assume that all the facets are triangular - we deal wtih an STL geometry
      //compute the 2d triangle
      for (; currVertex != lastVertex; currVertex++){
    	point[index++] = currVertex->Point().DropCoord(coord);
      }
      //Now define 3 lines which are triangle edges.
      cLINE2 line0(point[0], point[1]);
      line0.MakeSignedDistancePositive(point[2]);
      cLINE2 line1(point[1], point[2]);
      line1.MakeSignedDistancePositive(point[0]);
      cLINE2 line2(point[2], point[0]);
      line2.MakeSignedDistancePositive(point[1]);
      cBOX2 facetBox2(point[0], point[1]);
      facetBox2 += point[2];
      //find the facet bix limiting indices
      REAL u0 = (facetBox2.XMin() - meshBox2.XMin())/xStep;
      INT i0 = (INT)(u0 + cLIMITS::Tolerance());
      REAL u1 = (facetBox2.XMax() - meshBox2.XMin())/xStep;
      INT i1 = (INT)(u1 + 1.0 - cLIMITS::Tolerance());
      REAL v0 = (facetBox2.YMin() - meshBox2.YMin())/yStep;
      INT j0 = (INT)(v0 + cLIMITS::Tolerance());
      REAL v1 = (facetBox2.YMax() - meshBox2.YMin())/yStep;
      INT j1 = (INT)(v1 + 1.0 - cLIMITS::Tolerance());

      //this code is very inefficient but simple - it needs to be replaced
      for (INT i = i0; i <= i1; i++){
    	REAL x = meshBox2.XMin() + ((REAL)i)*xStep;
    	for (INT j = j0; j <= j1; j++){
    	  if (!bitMask[i][j]){
    		REAL y = meshBox2.YMin() + ((REAL)j)*yStep;
    		cPOINT2 gridPoint(x,y);
    		if (line0.SignedDistance(gridPoint) > 0.0 &&
    			line1.SignedDistance(gridPoint) > 0.0 &&
    			line2.SignedDistance(gridPoint) > 0.0)
    			bitMask[i][j] = true;
    	  }
    	}
      }
    }
    //for each vertex on the gord we know its status wrt the mesh projection.
    //Now compute the total of rectangle weights. A rectangle weight is computed as follows:
    //1, if all four of its vertices are true
    //0.5 if three of its vertices are true
    //0.25 if two of its vertices are true.
    //0 if 0 or 1 vertices are true.

    REAL shadedArea = 0.0;
    for (INT i = 0; i < 511; i++){
      for (INT j=0; j<511; j++){
    	INT n = 0;
    	if (bitMask[i][j])
    		n++;
    	if (bitMask[i+1][j])
    		n++;
    	if (bitMask[i][j+1])
    		n++;
    	if (bitMask[i+1][j+1])
    		n++;
    	REAL w = n == 4? 1 : n==3 ? 0.5 : n==2 ? 0.25 : 0;
    	shadedArea += w;
      }
    }

    shadedArea *= ((meshBox2.XMax() - meshBox2.XMin())*
							(meshBox2.YMax() - meshBox2.YMin()))/(511.0*511.0);
    return shadedArea;
  }

  _TMESH_TMPL_TYPE
  typename _TMESH_TMPL_DECL::cHALF_EDGE*
  _TMESH_TMPL_DECL::BorderHalfEdge(iMANIFOLD manifoldIndex)
   //returns an arbitrary border half edge
  {
	half_edge_iterator currHalfEdge = HalfEdgesBegin();
	half_edge_iterator lastHalfEdge = HalfEdgesEnd();

	for( ; currHalfEdge != lastHalfEdge; currHalfEdge++){
	  if (currHalfEdge->IsBorder()){
		if (manifoldIndex == INVALID_IMANIFOLD ||
				currHalfEdge->Opp()->Facet()->ManifoldIndex() == manifoldIndex)
		return currHalfEdge.operator->();
	  }
	}
	return NULL;
  }

  //generates vector with elements facet0_number_of_Vertices, Vertex[0],..., Vertex[n-1],
  //facet1_number_of_Vertices,...., etc. It's needed for amoeba grid export
  //for a deleted facet it puts the number of Vertices as 0, and it returns
  //the total number of facet Vertices, facet1_number_of_Vertices+facet0_number_of_Vertices+...
  _TMESH_TMPL_TYPE
      VOID _TMESH_TMPL_DECL::GenerateFacetsCRF(cCRF &crf)
  {
	INT totalNumVertices = 0;
	INT largestFacetIndex = LargestFacetIndex();
	crf.ElementPushBack(0);

	for (INT f = 0; f < largestFacetIndex; f++){
	  cFACET *facet = Facet(f);
	  if (facet != NULL && !facet->IsDeleted()){
		crf.ElementPushBack(totalNumVertices); //it cannot be ignored without re-indexation
		typename cFACET::vertex_circulator currVtx = facet->VerticesBegin();
		typename cFACET::vertex_circulator lastVtx = facet->VerticesEnd();

		for( ; currVtx != lastVtx ; currVtx++) {
		  crf.ElementPushBack(currVtx->Index());
		  totalNumVertices++;
		}
	  }
	  crf.ObjectPushBack(totalNumVertices);
	}
  }


 /* _TMESH_TMPL_TYPE
  BOOL _TMESH_TMPL_DECL::IsAlmostClosed() const
  {
	return true;
  }

  _TMESH_TMPL_TYPE
   REAL _TMESH_TMPL_DECL::SurfaceArea() const
   {
	  return 0;
   }

  _TMESH_TMPL_TYPE
   REAL _TMESH_TMPL_DECL::Volume() const
   {
	  return 0;
   }*/
}
