#include "common_internal3d.hpp"

namespace geom {

cBOX3 cSEGMENT3::ComputeBbox() const
{
    return cBOX3(m_endPoints[0], m_endPoints[1]);
 }

BOOL cSEGMENT3::SegmentDistanceParams(const cSEGMENT3 &seg,
					    REAL & param1, REAL & param2) const
{
  cVECTOR3 seg1Vec = Target()-Source();
  REAL seg1MaxCoordLen = fabs(seg1Vec[seg1Vec.AbsMaxCoord()]);
  seg1Vec = seg1Vec / seg1MaxCoordLen;

  cVECTOR3 seg2Vec = seg.Target()-seg.Source();
  REAL seg2MaxCoordLen = fabs(seg2Vec[seg2Vec.AbsMaxCoord()]);;
  seg2Vec = seg2Vec / seg2MaxCoordLen;

  cVECTOR3 startVec = Source() - seg.Source();

  REAL dot11    = seg1Vec * seg1Vec;
  REAL dot12    = seg1Vec * seg2Vec;
  REAL dot22    = seg2Vec * seg2Vec;
  REAL dotSeg1  = seg1Vec * startVec;
  REAL dotSeg2  = seg2Vec * startVec;

  REAL dotMatrix[2][2];

  dotMatrix[0][0] = dot11;
  dotMatrix[0][1] = dot12;
  dotMatrix[1][0] = dot12;
  dotMatrix[1][1] = dot22;

  REAL det = Det2Val(dotMatrix);

  if (det < cLIMITS::Tolerance())  /* it cannot be negative */
    return false;

  dotMatrix[0][0] = dotSeg2;
  dotMatrix[0][1] = dotSeg1;
  dotMatrix[1][0] = dot22;
  dotMatrix[1][1] = dot12;

  param1 = (Det2Val(dotMatrix)/det)/seg1MaxCoordLen;

  dotMatrix[0][0] = dotSeg2;
  dotMatrix[0][1] = dotSeg1;
  dotMatrix[1][0] = dot12;
  dotMatrix[1][1] = dot11;

  param2 = (Det2Val(dotMatrix)/ det)/seg2MaxCoordLen;

  return true;
}

BOOL cSEGMENT3::HasOn(const cPOINT3 &point) const
{
  cPOINT3 projectionPoint = SupportingLine().Projection(point);
  if(!(projectionPoint == point))
    return false;

  return AreOrderedAlongLine(Source(), point, Target());

}

REAL cSEGMENT3::PointProjectionParam(const cPOINT3 &point)
{
	cVECTOR3 segVec = Target()-Source();
	cVECTOR3 pointVec = point - Source();
	REAL param = ( segVec * pointVec) / (segVec * segVec);
	if (param < 0.0)
		param = 0.0;
	else if (param > 1.0)
		param = 1.0;
	return param;
}

cPOINT3 cSEGMENT3::PointProjectionPoint(const cPOINT3 &point)
{
	REAL param = PointProjectionParam(point);
	return Evaluate(param);
}
}
