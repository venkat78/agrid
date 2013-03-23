#ifndef _GEOM_DO_INTERSECT_H_
#define _GEOM_DO_INTERSECT_H_

namespace geom {
  inline eRETURN_TYPE DoIntersect(const cLINE2 &line,
				  const cSEGMENT2 &segment);


 eRETURN_TYPE DoIntersect(const cBOX2 &box,
				const cSEGMENT2 &segment);

inline eRETURN_TYPE DoIntersect(const cBOX2 &box1,
				const cBOX2 &box2);


inline eRETURN_TYPE DoIntersect(const cBOX2 &box,
				const cRAY2 &ray);


inline eRETURN_TYPE DoIntersect(const cSEGMENT2 &segment1,
				const cSEGMENT2 &segment2);


eRETURN_TYPE DoIntersect(const cTRIANGLE2 &triangle,
			 const cSEGMENT2 &segment);

eRETURN_TYPE DoIntersect(const cTRIANGLE3 &triangle,
			 const cSEGMENT3 &lseg, BOOL doDebug = false);


eRETURN_TYPE DoIntersect(const cPLANE3 &plane,
			 const cSEGMENT3 &segment);


eRETURN_TYPE DoIntersect(const cBOX3 &box,
			 const cSEGMENT3 &segment);


eRETURN_TYPE DoIntersect(const cBOX3 &box,
			 const cTRIANGLE3 &triangle);


eRETURN_TYPE DoIntersect(const cBOX3 &box1,
			 const cBOX3 &box2);


eRETURN_TYPE DoIntersect(const cTRIANGLE3 &triangle1,
			 const cTRIANGLE3 &triangle2);

}

#endif //_GEOM_DO_INTERSECT_H_
