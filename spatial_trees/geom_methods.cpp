#include "geom_methods.hpp"

namespace spatial_trees {
  cBOX3 OctantBox(const cBOX3 &octant, INT octantIndex)
  {
    INT dims[3] = {0, 0, 0};
    dims[2] = octantIndex % 2;
    octantIndex /= 2;
    
    dims[1] = octantIndex % 2;
    octantIndex /= 2;
    
    dims[0] = octantIndex % 2;

    REAL octantDims[3];
    DO_COORDS(coord) {
      octantDims[coord] = octant.Thickness(coord) * 0.5;
    }
    
    const cPOINT3 &minPoint = octant.Min();
    cPOINT3 startPoint(minPoint[0] + dims[0]*octantDims[0],
		       minPoint[1] + dims[1]*octantDims[1],
		       minPoint[2] + dims[2]*octantDims[2]);

    cPOINT3 endPoint(startPoint[0] + octantDims[0],
		     startPoint[1] + octantDims[1],
		     startPoint[2] + octantDims[2]);

    return cBOX3(startPoint, endPoint);
  }

  VOID IsInOctants(const cBOX3 &range, const cPOINT3& point, BOOL partitions[8])
  {
    BOOL isFound[8] = {true, true, true, true, true, true, true, true};

    cPOINT3 rangeCenter = range.Center();

    //Try to eliminate entire set of octants in one go.
    DO_COORDS(coord) {
      BOOL lowerHalf = false, upperHalf = false;
      if(point[coord] < rangeCenter[coord]) {
	lowerHalf = true;
      }
      else if(point[coord] > rangeCenter[coord]) {
	upperHalf = true;
      }

      if(!lowerHalf && upperHalf) {
	INT j = coord;
	for(INT k = 0 ; k < 4; k++)
	  isFound[octantsRange[j][k]] = false;
      }
      
      if(!upperHalf && lowerHalf) {
	INT j = coord + 3;
	for(INT k = 0 ; k < 4; k++)
	  isFound[octantsRange[j][k]] = false;
      }
    }

    for(INT i = 0; i < 8 ; i++) {
      partitions[i] = isFound[i];
      
      if(partitions[i]) {
	if(OctantBox(range, i).BoundedSide(point) == ON_UNBOUNDED_SIDE)
	  partitions[i] = false;
      }
    }
  }
  
  // template <typename FACET_TYPE>
//   VOID IsInOctants(FACET_TYPE *facet, const cBOX3 &rane
  VOID IsInOctants(const cBOX3 &range, const cPOINT3 *points, 
		   INT numPoints, BOOL partitions[8])
  {
    BOOL isFound[8] = {true, true, true, true, true, true, true, true};

    cPOINT3 rangeCenter = range.Center();

    //Try to eliminate entire set of octants in one go.
    DO_COORDS(coord) {
      BOOL lowerHalf = false, upperHalf = false;

      for(INT i = 0; i < numPoints ; i++ ){
	const cPOINT3 &point = points[i];

	if(point[coord] < rangeCenter[coord]) {
	  lowerHalf = true;
	}
	else if(point[coord] > rangeCenter[coord]) {
	  upperHalf = true;
	}
      }

      if(!lowerHalf && upperHalf) {
	INT j = coord;
	for(INT k = 0 ; k < 4; k++)
	  isFound[octantsRange[j][k]] = false;
      }
      
      if(!upperHalf && lowerHalf) {
	INT j = coord + 3;
	for(INT k = 0 ; k < 4; k++)
	  isFound[octantsRange[j][k]] = false;
      }
    }

    for(INT i = 0; i < 8 ; i++) {
      partitions[i] = isFound[i];
    }
  }
}
