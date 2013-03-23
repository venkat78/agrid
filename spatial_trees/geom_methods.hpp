#ifndef _SPATIAL_TREES_GEOM_METHODS_HPP_
#define _SPATIAL_TREES_GEOM_METHODS_HPP_

//Provides octant and quadrant related methods to octree and quadtree.
#include "defs.hpp"

namespace spatial_trees {
  //Each octant is represented by a triple <x, y, z>
  //where x, y, z can take the values of the {0, 1}
  //x, y, z are used compute the minPoint of the octant.

  //octantIndex 0 = <0, 0, 0> 
  //octantIndex 1 = <0, 0, 1> 
  //octantIndex 2 = <0, 1, 0> 
  //octantIndex 3 = <0, 1, 1> 
  //octantIndex 4 = <1, 0, 0> 
  //octantIndex 5 = <1, 0, 1> 
  //octantIndex 6 = <1, 1, 0> 
  //octantIndex 7 = <1, 1, 1> 
  //Octants indexation
  //0 - octants with x less than octant center x coord.
  //1 - octants with y less than octant center y coord.
  //2 - octants with z less than octant center z coord.
  //3 - octants with x more than octant center x coord.
  //4 - octants with y more than octant center y coord.
  //5 - octants with z more than octant center z coord.

  const INT octantsRange[6][4] = {
    {0, 1, 2, 3},
    {0, 1, 4, 5},
    {0, 2, 4, 6},
    {4, 5, 6, 7},
    {2, 3, 6, 7},
    {1, 3, 5, 7}
  };

  cBOX3 OctantBox(const cBOX3 &octant, INT octantIndex);
  VOID IsInOctants(const cBOX3 &range, const cPOINT3 &point, BOOL partitions[8]);
  VOID IsInOctants(const cBOX3 &range, const cPOINT3 *points, INT numPoints, BOOL partitions[8]);

  template <typename FACET_TYPE>
  VOID IsInOctants(FACET_TYPE* facet, const cBOX3 &range, BOOL partitions[8])
  {
    BOOL isFound[8] = {true, true, true, true, true, true, true, true};

    cPOINT3 rangeCenter = range.Center();
    
    //Try to eliminate entire set of octants in one go.
    DO_COORDS(coord) {
      BOOL lowerHalf = false, upperHalf = false;
      
      typename FACET_TYPE::vertex_circulator currVtx = facet->VerticesBegin();
      typename FACET_TYPE::vertex_circulator lastVtx = facet->VerticesEnd();
      
      for(; currVtx != lastVtx ; currVtx++ ){
	const cPOINT3 &point = currVtx->Point();
	
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

#endif //_SPATIAL_TREES_GEOM_METHODS_HPP_
