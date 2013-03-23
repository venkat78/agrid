#include "edge.hpp"

namespace tmesh {
  template <typename _Mesh_Type, typename _Half_Edge_Filter>
  VOID tEDGE_RING<_Mesh_Type, _Half_Edge_Filter>::Add(cHALF_EDGE *he, const cPOINT2 &point)
  {
    INT newIndex = numEntries;
    sEDGE_RING_ENTRY *newEntry = &entries[newIndex];
    
    newEntry->he = he;
    newEntry->point = point;
    newEntry->isValid = true;
    ASSERT(numEntries <= ARRAY_SIZE);
    numEntries++;
  }

  template <typename _Mesh_Type, typename _Half_Edge_Filter>
  VOID tEDGE_RING<_Mesh_Type, _Half_Edge_Filter>::ComputeRadialOrder()
  {
    if(numEntries == 0)
      return;

    REAL maxVal = cLIMITS::Min();
    INT pivot = 0;
    
    for(INT i = 0; i < numEntries ; i++) {
      entries[i].vector = (entries[i].point - center).Normalize();

      if((entries[i].vector)[0] > maxVal) {
	pivot = i;
	maxVal = (entries[i].vector)[0];
      }
    }
    
    entries[pivot].angle = 0.0;
    
    for(INT i = 1; i < numEntries ; i++) {
      entries[i].angle = Angle(entries[pivot].point, center, entries[i].point);
    }
    
    for (INT i = 0; i < numEntries ; i++) {
      for(INT j = i+1 ; j < numEntries ; j++) {
	if(entries[i].angle > entries[j].angle) {
	  sEDGE_RING_ENTRY tmp = entries[i];
	  entries[i] = entries[j];
	  entries[j] = tmp;
	}
      }
    }
  }
  
  template <typename _Mesh_Type, typename _Half_Edge_Filter>
  VOID tEDGE_RING<_Mesh_Type, _Half_Edge_Filter>::FindPrevAndNext(INT ci, INT &pi, INT &ni)
   {
     INT i = 0;
     INT nIndex;

     for( ; i < numEntries ; i++) {
       nIndex = (ci + i + 1) % numEntries;
       if(!Equal(entries[nIndex].angle,
		 entries[ci].angle))
	 break;
     }
     
     if(numEntries == i)
       ni = (ci + 1)%numEntries;
     else
       ni = nIndex;
     
     INT pIndex;
     i = 0;
     for( ; i < numEntries ; i++) {
       pIndex = (numEntries + ci - ( i + 1)) % numEntries;
       if(!Equal(entries[pIndex].angle,
		 entries[ci].angle))
	 break;
     }
     
     if(numEntries == i)
       pi = (numEntries + ci - 1)%numEntries;
     else
       pi = pIndex;
     
     
     //In case of overlapping geometry there is a possibility to find a matching 
     //half edge for prev with the same radial position.
     if(!AreOpposite(entries[pi].he,
		     entries[ci].he)) {
       i = 0;
       for( ; i < numEntries ; i++) {
	 pIndex = (numEntries + pi - ( i + 1)) % numEntries;
	 
	 if(pIndex == ni)
	   break;
	 
	 if(!Equal(entries[pIndex].angle,
		   entries[pi].angle))
	   break;
	 else {
	   if(AreOpposite(entries[pIndex].he,
			  entries[ci].he)) {
	     pi = pIndex;
	     break;
	   }
	 }
       }
     }

     //In case of overlapping geometry there is a possibility to find a matching 
     //half edge for next with the same radial position.
     if(!AreOpposite(entries[ni].he,
		     entries[ci].he)) {
       i = 0;
       for( ; i < numEntries ; i++) {
	 nIndex = (ni + i + 1) % numEntries;
	 
	 if(nIndex == pi)
	   break;
	 
	 if(!Equal(entries[ni].angle,
		   entries[nIndex].angle))
	   break;
	 else {
	   if(AreOpposite(entries[nIndex].he,
			  entries[ci].he)) {
	     ni = nIndex;
	     break;
	   }
	 }
       }
     }
     
     if(ni == pi) {
       ni = (ci + 1)%numEntries;
       pi = (numEntries+ci - 1)%numEntries;
     }
   }

  template <typename _Mesh_Type, typename _Half_Edge_Filter>
  VOID tEDGE_RING<_Mesh_Type, _Half_Edge_Filter>::FindRadialOrder()
  {
    cPOINT3 pointOnPlane = m_he->MidPoint();
    cVECTOR3 planeNormal = m_he->UniqueVector();

    cPLANE3 projectionPlane(pointOnPlane, planeNormal);
    
    eCOORD maxCoord = planeNormal.AbsMaxCoord();
    center = projectionPlane.Projection(pointOnPlane).DropCoord(maxCoord);
    
    cHALF_EDGE* loopingHe = m_he;
    
    do {
      if(!loopingHe->IsBorder() && 
	 loopingHe->Opp()->IsBorder())
	Add(loopingHe, projectionPlane.
	    Projection(loopingHe->Next()
		       ->Head()->Point())
	    .DropCoord(maxCoord));
      
      loopingHe = loopingHe->NextInRing();
    } while(loopingHe != m_he);
  
    ComputeRadialOrder();
  }

  template <typename _Mesh_Type, typename _Half_Edge_Filter>
  VOID tEDGE_RING<_Mesh_Type, _Half_Edge_Filter>::MateHalfEdgesBreakTie(INT currIndex, INT prevIndex, INT nextIndex)
  {
    cHALF_EDGE* prevHe = entries[prevIndex].he;
    cHALF_EDGE* currHe = entries[currIndex].he;
    cHALF_EDGE* nextHe = entries[nextIndex].he;
    
    INT mergeIndex = -1;
    cHALF_EDGE* potentialMate = NULL;
    BOOL foundMate = false;
    
    //Don't use angles, use normals.
    REAL prevDotProduct = currHe->Facet()->Normal() * prevHe->Facet()->Normal();
    
    REAL nextDotProduct = currHe->Facet()->Normal() * nextHe->Facet()->Normal();
    
    REAL val = prevDotProduct * nextDotProduct;
    if(val < 0.0) {
      //mate with one with positive dot product.
      if(prevDotProduct > 0.0) {
	mergeIndex = prevIndex;
	potentialMate = prevHe;
      }
      else {
	mergeIndex = nextIndex;
	potentialMate = nextHe;
      }
      foundMate = true;
    } //end of if.
    else if(fabs(val) < cLIMITS::Tolerance()){ 
      BOOL prevIsSmall = (fabs(prevDotProduct) < cLIMITS::Tolerance() ?
			  true : false);

      BOOL nextIsSmall = (fabs(nextDotProduct) < cLIMITS::Tolerance() ?
			  true : false);
	
      if(prevIsSmall && !nextIsSmall) {
	mergeIndex = nextIndex;
	potentialMate = nextHe;
	foundMate = true;
      }
      else if(!prevIsSmall && nextIsSmall) {
	mergeIndex = prevIndex;
	potentialMate = prevHe;
	foundMate = true;
      }
      else if(prevIsSmall && nextIsSmall) {
	iFACET prevFacetIndex = prevHe->Facet()->Index();
	iFACET nextFacetIndex = nextHe->Facet()->Index();
	  
	if(prevFacetIndex < nextFacetIndex) {
	  mergeIndex = prevIndex;
	  potentialMate = prevHe;
	}
	else {
	  mergeIndex = nextIndex;
	  potentialMate = nextHe;
	}

	foundMate = true;
      }
    }
    else {
      REAL dotProductDiff = fabs(prevDotProduct - nextDotProduct);
	
      //If choice is difficult to make, use facet indices to break
      //the tie. May be user provided info is much better.
      if(dotProductDiff <= cLIMITS::Tolerance()) {
	iFACET prevFacetIndex = prevHe->Facet()->Index();
	iFACET nextFacetIndex = nextHe->Facet()->Index();

	if(prevFacetIndex < nextFacetIndex) {
	  mergeIndex = prevIndex;
	  potentialMate = prevHe;
	}
	else {
	  mergeIndex = nextIndex;
	  potentialMate = nextHe;
	}

	foundMate = true;
      }

      else if(prevDotProduct > nextDotProduct) {
	mergeIndex = prevIndex;
	potentialMate = prevHe;
	foundMate = true;
      }
      else {
	mergeIndex = nextIndex;
	potentialMate = nextHe;
	foundMate = true;
      }
    } //End of else of prevDotProduct * nextDotProduct < 0.0
    
    if(foundMate) {
      m_topoOperators.MateHalfEdges(currHe, potentialMate);
      entries[currIndex].isValid = 
	entries[mergeIndex].isValid = false;
    }  
  }

  template <typename _Mesh_Type, typename _Half_Edge_Filter>
  VOID tEDGE_RING<_Mesh_Type, _Half_Edge_Filter>::MateCoplanarHalfEdges()
  {
    for(INT i = 0; i < numEntries ; i++) {
      if(!entries[i].isValid)
	continue;

      INT prevIndex, nextIndex; 
    
      FindPrevAndNext(i, prevIndex, nextIndex);
      
      cHALF_EDGE* prevHe = entries[prevIndex].he;
      cHALF_EDGE* currHe = entries[i].he;
      cHALF_EDGE* nextHe = entries[nextIndex].he;
      
      BOOL canMergeWithPrev = entries[prevIndex].isValid;
      
      //First checking coplanarity.
      if(canMergeWithPrev)
	canMergeWithPrev = (fabs(fabs(entries[i].angle - 
				      entries[prevIndex].angle) - M_PI) 
			    <= cLIMITS::Tolerance()) ? true : false ;
    
      BOOL canMergeWithNext = entries[nextIndex].isValid;

      if(canMergeWithNext)
	canMergeWithNext = (fabs(fabs(entries[i].angle - 
				      entries[nextIndex].angle) - M_PI) 
			    <= cLIMITS::Tolerance()) ? true : false ;
    
      if(!canMergeWithPrev && !canMergeWithNext)
	continue;
    
      //Checking mateability.
      if(canMergeWithPrev)
	canMergeWithPrev = AreOpposite(currHe, prevHe);
    
      if(canMergeWithNext)
	canMergeWithNext = AreOpposite(currHe, nextHe);
    
      if(!canMergeWithPrev && !canMergeWithNext)
	continue;

      if(!canMergeWithPrev && canMergeWithNext) {
	m_topoOperators.MateHalfEdges(currHe, nextHe);
	entries[i].isValid = 
	  entries[nextIndex].isValid = false;
      }
      else if(canMergeWithPrev && !canMergeWithNext) {
	m_topoOperators.MateHalfEdges(currHe, prevHe);
	entries[i].isValid = 
	  entries[prevIndex].isValid = false;
      }
      else if(canMergeWithPrev && canMergeWithNext)
	MateHalfEdgesBreakTie(i, prevIndex, nextIndex);
    }
  }

  template <typename _Mesh_Type, typename _Half_Edge_Filter>
  VOID tEDGE_RING<_Mesh_Type, _Half_Edge_Filter>::MateHalfEdgesInRadialOrder()
  {
    if(numEntries == 0)
      return;

    if(numEntries == 2) {
      cHALF_EDGE* firstHe = entries[0].he;
      cHALF_EDGE* secondHe = entries[1].he;
    
      if(AreOpposite(firstHe, secondHe)) {
	m_topoOperators.MateHalfEdges(firstHe, secondHe);
	return;
      }
    }

    MateCoplanarHalfEdges();
  
    for(INT i = 0; i < numEntries ; i++) {
      if(!entries[i].isValid)
	continue;

      INT prevIndex, nextIndex;

      FindPrevAndNext(i, prevIndex, nextIndex);

      cHALF_EDGE* prevHe = entries[prevIndex].he;
      cHALF_EDGE* currHe = entries[i].he;
      cHALF_EDGE* nextHe = entries[nextIndex].he;

      BOOL canMergeWithPrev = entries[prevIndex].isValid;
    
      if(canMergeWithPrev)
	canMergeWithPrev = AreOpposite(prevHe, currHe);
      
      BOOL canMergeWithNext = entries[nextIndex].isValid;

      if(canMergeWithNext)
	canMergeWithNext = AreOpposite(nextHe, currHe);
    
      if(!canMergeWithPrev && canMergeWithNext) {
	m_topoOperators.MateHalfEdges(currHe, nextHe);
	entries[i].isValid = 
	  entries[nextIndex].isValid = false;
      }
      else if(canMergeWithPrev && !canMergeWithNext) {
	m_topoOperators.MateHalfEdges(currHe, prevHe);
	entries[i].isValid = 
	  entries[prevIndex].isValid = false;
      }
      else if(canMergeWithPrev && canMergeWithNext) {
	MateHalfEdgesBreakTie(i, prevIndex, nextIndex);
      }
    }
  }

  template <typename _Mesh_Type, typename _Half_Edge_Filter>
  VOID tEDGE_RING<_Mesh_Type, _Half_Edge_Filter>::MateHalfEdges()
  {
    if(m_he->Vector().IsNull())
      return;
    
    FindRadialOrder();
    MateHalfEdgesInRadialOrder();
  }

}
