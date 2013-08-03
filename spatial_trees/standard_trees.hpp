#ifndef _SPATIAL_TREES_STANDARD_TREES_HPP_
#define _SPATIAL_TREES_STANDARD_TREES_HPP_

#include "tree.hpp"
#include "geom_methods.hpp"

namespace spatial_trees {
template<typename GEOM_KERNEL, typename GEOM_MANAGER, typename ELEMENT_ID>
class tOCTREE : public tTREE<GEOM_KERNEL, GEOM_MANAGER, ELEMENT_ID, 8> {
 public:
  typedef tTREE<GEOM_KERNEL, GEOM_MANAGER, ELEMENT_ID, 8> cBASE;
  typedef typename cBASE::cLEAF cLEAF;
  typedef typename cBASE::leaf_iterator leaf_iterator;
  typedef typename cBASE::leaf_element_iterator leaf_element_iterator;

  typedef typename GEOM_KERNEL::cBOX cBOX;
  typedef typename GEOM_KERNEL::cPOINT cPOINT;
  typedef typename GEOM_KERNEL::cSEGMENT cSEGMENT;
  typedef typename GEOM_KERNEL::cLINE cLINE;
  typedef typename GEOM_KERNEL::cRAY cRAY;
  typedef typename GEOM_KERNEL::cTRIANGLE cTRIANGLE;

 public:
  tOCTREE(const cBOX &box = cBOX(), GEOM_MANAGER *mgr = NULL, INT height = 8,
          INT leafElementsLim = 50)
      : cBASE(box, mgr, height, leafElementsLim) {
  }
};

template<typename GEOM_KERNEL, typename GEOM_MANAGER, typename ELEMENT_ID>
class tQUAD_TREE : public tTREE<GEOM_KERNEL, GEOM_MANAGER, ELEMENT_ID, 4> {
 public:
  typedef tTREE<GEOM_KERNEL, GEOM_MANAGER, ELEMENT_ID, 4> cBASE;
  typedef typename cBASE::cLEAF cLEAF;
  typedef typename cBASE::leaf_iterator leaf_iterator;
  typedef typename cBASE::leaf_element_iterator leaf_element_iterator;

  typedef typename GEOM_KERNEL::cBOX cBOX;
  typedef typename GEOM_KERNEL::cPOINT cPOINT;
  typedef typename GEOM_KERNEL::cSEGMENT cSEGMENT;
  typedef typename GEOM_KERNEL::cLINE cLINE;
  typedef typename GEOM_KERNEL::cRAY cRAY;
  typedef typename GEOM_KERNEL::cTRIANGLE cTRIANGLE;

 public:
  tQUAD_TREE(const cBOX &box = cBOX(), GEOM_MANAGER *mgr = NULL, INT height = 8,
             INT leafElementsLim = 50)
      : cBASE(box, mgr, height, leafElementsLim) {
  }
};

template<typename GEOM_KERNEL, typename GEOM_MANAGER, typename ELEMENT_ID>
class tBINARY_TREE : public tTREE<GEOM_KERNEL, GEOM_MANAGER, ELEMENT_ID, 2> {
 public:
  typedef tTREE<GEOM_KERNEL, GEOM_MANAGER, ELEMENT_ID, 2> cBASE;
  typedef typename cBASE::cLEAF cLEAF;
  typedef typename cBASE::leaf_iterator leaf_iterator;
  typedef typename cBASE::leaf_element_iterator leaf_element_iterator;

  typedef typename GEOM_KERNEL::cBOX cBOX;
  typedef typename GEOM_KERNEL::cPOINT cPOINT;
  typedef typename GEOM_KERNEL::cSEGMENT cSEGMENT;
  typedef typename GEOM_KERNEL::cLINE cLINE;
  typedef typename GEOM_KERNEL::cRAY cRAY;
  typedef typename GEOM_KERNEL::cTRIANGLE cTRIANGLE;

 public:
  tBINARY_TREE(const cBOX &box = cBOX(), GEOM_MANAGER *mgr = NULL, INT height = 8,
               INT leafElementsLim = 50)
      : cBASE(box, mgr, height, leafElementsLim) {
  }
};

}

#endif //_SPATIAL_TREES_STANDARD_TREES_HPP_
