#ifndef _GEOM_BOX2_H_
#define _GEOM_BOX2_H_

namespace geom {
class cBOX2
{

public:
  cBOX2() {}
  cBOX2(const cPOINT2 &p, const cPOINT2 &q);

  cPOINT2 & Min()
  {
    return m_cornerPoints[0];
  }

  cPOINT2 & Max()
  {
    return m_cornerPoints[1];
  }

  const cPOINT2 & Min() const
  {
    return m_cornerPoints[0];
  }

  const cPOINT2 & Max() const
  {
    return m_cornerPoints[1];
  }

  const REAL &   XMin() const
  {
    return m_cornerPoints[0][0];
  }

  const REAL &   YMin() const
  {
    return m_cornerPoints[0][1];
  }

  const REAL &   XMax() const
  {
    return m_cornerPoints[1][0];
  }

  const REAL &  YMax() const
  {
    return m_cornerPoints[1][1];
  }

  cPOINT2 Vertex(INT i) const;
  cPOINT2 operator[](INT i) const {
	  return Vertex(i);
  }

  VOID operator += (const cPOINT2 &point);

  cPOINT2 Center() const {
    return cPOINT2((XMin() + XMax()) * 0.5,
                   (YMin() + YMax()) * 0.5);
  }

  eBOUNDED_SIDE BoundedSide(const cPOINT2 &q) const;
  BOOL HasOnBoundary(const cPOINT2& p) const
  {
    return BoundedSide(p) == ON_BOUNDARY ? true : false;
  }

  BOOL HasOnBoundedSide(const cPOINT2& p) const
  {
    return BoundedSide(p) == ON_BOUNDED_SIDE ? true : false;
  }

  BOOL HasOnUnboundedSide(const cPOINT2& p) const
  {
    return BoundedSide(p) == ON_UNBOUNDED_SIDE ? true : false;
  }

protected:
  tPAIR<cPOINT2> m_cornerPoints;
};
}

#endif //_GEOM_BOX2_H_
