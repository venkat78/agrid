#ifndef _GEOM_INTERSECTION_H_
#define _GEOM_INTERSECTION_H_

namespace geom {

  eRETURN_TYPE Intersection(const cPLANE3 &plane,
			    const cSEGMENT3 &segment,
			    cPOINT3 &resultingPoint,
			    cSEGMENT3 &resultingSegment);
  
  
  
  eRETURN_TYPE Intersection(const cPLANE3 &plane,
			    const cLINE3 &line,
			    cPOINT3 &resultingPoint,
			    cLINE3 &resultingLine);
  

  
  eRETURN_TYPE Intersection(const cPLANE3 &plane,
			    const cSEGMENT3 &segment,
			    cPOINT3 &resultingPoint,
			    cSEGMENT3 &resultingSegment);
  
  
  eRETURN_TYPE Intersection(const cPLANE3 &plane,
			    const cRAY3 &ray,
			    cPOINT3 &resultingPoint,
			    cRAY3 &resultingRay);


  eRETURN_TYPE Intersection(const cSEGMENT2 &segment1,
			    const cSEGMENT2 &segment2,
			    cSEGMENT2 &resultingSegment,
			    cPOINT2 &resultingPoint);
  
  
  eRETURN_TYPE Intersection(const cRAY2 &ray,
			    const cSEGMENT2 &segment,
			    cSEGMENT2 &resultingSegment,
			    cPOINT2 &resultingPoint);
  
  
  eRETURN_TYPE Intersection(const cLINE2 &line1,
			    const cLINE2 &line2,
			    cPOINT2 &resultPoint,
			    cLINE2 &resultLine);
  
}
#endif //_GEOM_INTERSECTION_H_
