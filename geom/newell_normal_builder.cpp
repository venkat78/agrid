/*
 * newell_normal_builder.cpp
 *
 *  Created on: Dec 31, 2010
 */

#include "geom_defs.hpp"
#include "common_internal3d.hpp"
#include "newell_normal_builder.hpp"

namespace geom{
  cNEWELL_NORMAL_BUILDER::cNEWELL_NORMAL_BUILDER()
  {
	m_pointsCounter = 0;
	m_normal = cVECTOR3(0.0,0.0,0.0);
	m_meanPoint = cPOINT3(0.0,0.0,0.0);
  }

  VOID cNEWELL_NORMAL_BUILDER::SegmentContribution(const cPOINT3 &point0,
			  const cPOINT3 &point1)
  {
	  INT index[5] = {0, 1, 2, 0, 1};
	  for (INT i = 0 ; i < 3; i++){
		INT i0 = index[i];
		INT i1 = index[i+1];
		INT i2 = index[i+2];
		m_normal[i0] +=	(point0[i1]-point1[i1])*(point0[i2]+point1[i2]);
	  }
  }

  VOID cNEWELL_NORMAL_BUILDER::AddPoint(const cPOINT3 &point)
  {
	if (m_pointsCounter)
		SegmentContribution(m_prevPoint, point);
	else
	  m_firstPoint = point;
	m_pointsCounter++;
	m_prevPoint = point;
	m_meanPoint += point;
  }

  BOOL cNEWELL_NORMAL_BUILDER::Complete()
  {
	SegmentContribution(m_prevPoint, m_firstPoint);
	m_meanPoint = m_meanPoint/m_pointsCounter;
	m_normal = m_normal.Normalize();
	return !m_normal.IsNull();
  }
} //namespace
