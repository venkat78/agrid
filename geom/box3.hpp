
#ifndef _GEOM_BOX3_H_
#define _GEOM_BOX3_H_


namespace geom {
  class cBOX3
  {

  public:
    cBOX3();
    cBOX3(const cPOINT3 &p);
    cBOX3(const cPOINT3 &p, const cPOINT3 &q);
    cBOX3(const cBOX3 &box) : m_cornerPoints(box.Min(), box.Max())
    {}

    cBOX3(const cPOINT3 &left, const cPOINT3 &right,
	  const cPOINT3 &bottom, const cPOINT3 &top,
	  const cPOINT3 &far, const cPOINT3 &close)
      : m_cornerPoints(cPOINT3(left.X(),  bottom.Y(), far.Z()),
		       cPOINT3(right.X(), top.Y(), close.Z()))
    {
      ASSERT(left.X() <= right.X());
      ASSERT(bottom.Y() <= top.Y());
      ASSERT(far.Z() <= close.Z());
    }

    cBOX3(const REAL& min_x, const REAL& min_y, const REAL& min_z,
	  const REAL& max_x, const REAL& max_y, const REAL& max_z)
      : m_cornerPoints(cPOINT3(min_x, min_y, min_z),
		       cPOINT3(max_x, max_y, max_z))
    {
      ASSERT(min_x <= max_x);
      ASSERT(min_y <= max_y);
      ASSERT(min_z <= max_z);
    }

    cPOINT3 & Min()
    {
      return m_cornerPoints[0];
    }

    cPOINT3 & Max()
    {
      return m_cornerPoints[1];
    }

    const cPOINT3 & Min() const
    {
      return m_cornerPoints[0];
    }

    const cPOINT3 & Max() const
    {
      return m_cornerPoints[1];
    }

    INT IsOnEdge(const cPOINT3 &p) const ;

    cPOINT3 Vertex(INT i) const;
    cPOINT3 CornerVertex(INT index) const;
    REAL FaceCoord(INT face_index);
    REAL FaceCoord(eCOORD face_coord, eBOX_POSITION face_position);
    cBOX2 DropCoord(const eCOORD coord) const ;

    cPOINT3 operator[](INT i) const {
      return Vertex(i);
    }

    eBOUNDED_SIDE BoundedSide(const cPOINT3& p) const;

    BOOL HasOnBoundary(const cPOINT3& p) const
    {
      return BoundedSide(p) == ON_BOUNDARY ? true : false;
    }

    BOOL HasOnBoundedSide(const cPOINT3& p) const
    {
      return BoundedSide(p) == ON_BOUNDED_SIDE ? true : false;
    }

    BOOL HasOnUnboundedSide(const cPOINT3& p) const
    {
      return BoundedSide(p) == ON_UNBOUNDED_SIDE ? true : false;
    }

    BOOL TwoPointsOnSameFace(const cPOINT3 &p1, const cPOINT3 &p2) const;
    const REAL &   XMin() const
    {
      return m_cornerPoints[0][0];
    }

    const REAL &   YMin() const
    {
      return m_cornerPoints[0][1];
    }

    const REAL &   ZMin() const
    {
      return m_cornerPoints[0][2];
    }

    const REAL &   XMax() const
    {
      return m_cornerPoints[1][0];
    }

    const REAL &   YMax() const
    {
      return m_cornerPoints[1][1];
    }

    const REAL &   ZMax() const
    {
      return m_cornerPoints[1][2];
    }

    const REAL &   MinCoord(int i) const
    {
      return m_cornerPoints[0][i];
    }

    const REAL &   MaxCoord(int i) const
    {
      return m_cornerPoints[1][i];
    }

    REAL           Volume() const
    {
      return ((XMax() - XMin()) *
	      (YMax() - YMin()) *
	      (ZMax() - ZMin()));
    }

    REAL Thickness(int i) const {
      return m_cornerPoints[1][i] - m_cornerPoints[0][i];
    }

    cPOINT3 Center() const {
      return cPOINT3((XMin() + XMax()) * 0.5,
		     (YMin() + YMax()) * 0.5,
		     (ZMin() + ZMax()) * 0.5);
    }

    VOID Print(FILE *stream = stdout) const
    {
      fprintf(stream,"Box min point %lf %lf %lf \n",
	      m_cornerPoints[0][0], m_cornerPoints[0][1],
	      m_cornerPoints[0][2]);

      fprintf(stream,"Box max point %lf %lf %lf \n",
	      m_cornerPoints[1][0], m_cornerPoints[1][1],
	      m_cornerPoints[1][2]);
    }

    VOID operator += (const cPOINT3 &point);

    cBOX3 operator|( const cBOX3& box ) const
    {
      cBOX3 unionBox = *this ;

      unionBox += box.Min() ;
      unionBox += box.Max() ;
      return unionBox ;
    }

    cBOX3& operator |= ( const cBOX3& box )
    {
      *this += box.Min() ;
      *this += box.Max() ;
      return *this ;
    }

    cBOX3 operator*(const REAL scale) const
    {
      cBOX3 scaledBox = *this ;

      scaledBox.Min() *= scale;
      scaledBox.Max() *= scale;
      return scaledBox ;
    }

    cBOX3& operator*=(const REAL scale) {
      Min() *= scale;
      Max() *= scale;

      return *this ;
    }

    /*
     * Assumes that point is on the boundary and encodes
     * position as a vector.
     *
     * If vector squared length == 1, point is interior to a face.
     * If vector squared length == 2, point is on edge.
     * If vector squared length == 3, point is a vertex.
     *
     */
    cVECTOR3 PositionVector(const cPOINT3 &point) const ;

    REAL PointDistSquared(const cPOINT3& point) const;
    BOOL ContainsPoint(const cPOINT3& point) const;
    BOOL ContainsBox(const cBOX3& box) const;
    BOOL PointIsOnBoundary(cPOINT3 &point, BOOL pointOnBoundary[3][2]) const;

    BOOL Intersects(const cRAY3& ray, REAL tolerance) const;
    cPLANE3 BoundingPlane(INT i) const ;
    cSEGMENT3 Diagonal(INT i) const;
    cSEGMENT3 Edge(INT i) const;
    cSEGMENT3 EdgeOrdered(INT i) const;
    INT VertexPointIndex(const cPOINT3& point) const;
    cBOX3 &Grow(REAL factor);
    cBOX3 GrowBy(INT percentage = 5) const ;
  protected:
    tPAIR<cPOINT3> m_cornerPoints;
  };
}

#endif //_GEOM_BOX3_H_
