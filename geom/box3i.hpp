
#ifndef _GEOM_BOX3I_H_
#define _GEOM_BOX3I_H_

namespace geom {

class cBOX3i
{

public:
  cBOX3i() {}
  cBOX3i(const cPOINT3i &p, const cPOINT3i &q);

  typedef cPOINT3i cCellPos;
  typedef cPOINT3i cVertexPos;
  class cFacetPos
  {
  public:

	const INT& operator[] (int i) const
	{
	  return m_cornerPoint[i];
	}

	INT& operator[] (int i)
	{
	  return m_cornerPoint[i];
	}

	eCOORD Direction() { return m_direction;}
	VOID Direction(eCOORD direction) { m_direction = direction;}

  private:
	cPOINT3i m_cornerPoint;
	eCOORD m_direction;
  };

  cPOINT3i & Min()
  {
    return m_cornerPoints[0];
  }

  cPOINT3i & Max()
  {
    return m_cornerPoints[1];
  }

  const cPOINT3i & Min() const
  {
    return m_cornerPoints[0];
  }

  const cPOINT3i & Max() const
  {
    return m_cornerPoints[1];
  }

  cPOINT3i operator[](INT i) const;

  const INT &   XMin() const
  {
    return m_cornerPoints[0][0];
  }

  const INT &   YMin() const
  {
    return m_cornerPoints[0][1];
  }

  const INT &   ZMin() const
  {
    return m_cornerPoints[0][2];
  }

  const INT &   XMax() const
  {
    return m_cornerPoints[1][0];
  }

  const INT &   YMax() const
  {
    return m_cornerPoints[1][1];
  }

  const INT &   ZMax() const
  {
    return m_cornerPoints[1][2];
  }

  VOID XMin(int x) {m_cornerPoints[0][GEOM_X] = x;}
  VOID XMax(int x) {m_cornerPoints[1][GEOM_X] = x;}

  VOID YMin(int y) {m_cornerPoints[0][GEOM_Y] = y;}
  VOID YMax(int y) {m_cornerPoints[1][GEOM_Y] = y;}

  VOID ZMin(int z) {m_cornerPoints[0][GEOM_Z] = z;}
  VOID ZMax(int z) {m_cornerPoints[1][GEOM_Z] = z;}

  const INT &   MinCoord(int i) const
  {
    return m_cornerPoints[0][i];
  }

  const INT &   MaxCoord(int i) const
  {
    return m_cornerPoints[1][i];
  }

  INT           Volume() const
  {return XWidth() * YWidth() * ZWidth(); }

  INT SurfaceArea() const
  {return 2*(XWidth() * YWidth() + YWidth() * ZWidth() + ZWidth() * XWidth());}

  INT Thickness(int i) const {
    return m_cornerPoints[1][i] - m_cornerPoints[0][i];
  }

  INT NumCells() const;
  INT NumVertices() const;
  INT NumXFacets() const;
  INT NumYFacets() const;
  INT NumZFacets() const;
  INT NumFacets() const;

  cCellPos CellPosition(int index) const;
  VOID CellPosition(int index, cCellPos &cellPos) const;
  int CellIndex(cCellPos &cellPos) const;
  BOOL CellIsOnBorder(cCellPos &cellPos) const;
  INT BorderCellIndex(const cCellPos &cellPos, eCOORD coord) const;
  eCOORD BorderCellPos(INT index, cCellPos &cellPos) const;

  cVertexPos VertexPosition(INT index);
  VOID VertexPosition(INT index, cVertexPos &vertexPos);
  int VertexIndex(cVertexPos &vertexPos) const;

  cBOX3i::cFacetPos FacetPosition(int index) const;
  VOID FacetPosition(int index, cFacetPos &facetPos) const;
  int FacetIndex(cBOX3i::cFacetPos &facetPos) const;

  INT XWidth () const {return XMax() - XMin();}
  INT YWidth () const {return YMax() - YMin();}
  INT ZWidth () const {return ZMax() - ZMin();}

//width plus 1
  INT XWidth1 () const {return XWidth() + 1;}
  INT YWidth1 () const {return YWidth() + 1;}
  INT ZWidth1 () const {return ZWidth() + 1;}

  cPOINT3i Center() const {
    return cPOINT3i(static_cast<INT>((XMin() + XMax()) * 0.5),
		       static_cast<INT>((YMin() + YMax()) * 0.5),
		       static_cast<INT>((ZMin() + ZMax()) * 0.5));
  }

  VOID Print(FILE *stream = stdout) const
  {
    fprintf(stream,"Box min point %d %d %d; \t",
	    m_cornerPoints[0][0], m_cornerPoints[0][1],
	    m_cornerPoints[0][2]);

    fprintf(stream,"Box max point %d %d %d \n",
	    m_cornerPoints[1][0], m_cornerPoints[1][1],
	    m_cornerPoints[1][2]);
  }

  VOID operator += (const cPOINT3i &point);

  cBOX3i operator|( const cBOX3i& box ) const
  {
    cBOX3i unionBox = *this ;

    unionBox += box.Min() ;
    unionBox += box.Max() ;
    return unionBox ;
  }

  cBOX3i& operator |= ( const cBOX3i& box )
  {
    *this += box.Min() ;
    *this += box.Max() ;
    return *this ;
  }

  BOOL ContainsCell(const cCellPos& cellPos) const;

protected:
  tPAIR<cPOINT3i> m_cornerPoints;
};

}

#endif //_GEOM_BOX3I_H_
