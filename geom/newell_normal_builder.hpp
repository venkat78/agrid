/*
 * newell_normal_builder.hpp
 *
 *  Created on: Dec 31, 2010
 */

#ifndef NEWELL_NORMAL_BUILDER_HPP_
#define NEWELL_NORMAL_BUILDER_HPP_

namespace geom{

  class cNEWELL_NORMAL_BUILDER {

  public:
	  cNEWELL_NORMAL_BUILDER();
	  VOID AddPoint(const cPOINT3 &point);
	  BOOL Complete();
	  cVECTOR3 const &Normal() { return m_normal;}
	  cPOINT3 const &MeanPoint() { return m_meanPoint;}
	  INT const PointsCount() { return m_pointsCounter;}
	  cPLANE3 const Plane() {return cPLANE3(m_meanPoint, m_normal);}
	  //this is not the best plane, because the greatest positive and negative deviations may
	  //differ for the case of more than 4 points

  private: //methods;
	  VOID SegmentContribution(const cPOINT3 &point0, const cPOINT3 &point1);

  private: //members
	  INT m_pointsCounter;
	  cPOINT3 m_firstPoint;
	  cPOINT3 m_prevPoint;
	  cVECTOR3 m_normal;
	  cPOINT3 m_meanPoint;
  };
} //namespace

#endif /* NEWELL_NORMAL_BUILDER_HPP_ */
