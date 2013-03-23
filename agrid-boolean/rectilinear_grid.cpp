#include "defs.hpp"
#include "rectilinear_grid.hpp"

namespace agrid_boolean {
  //Constructors
  cRECTILINEAR_GRID::cRECTILINEAR_GRID()
  {
  }

  cRECTILINEAR_GRID::cRECTILINEAR_GRID(const cBOX3 &simVol,
				       INT numCells[3]) 
  {
    m_simVol = simVol;

    INT nCells = (REAL)(numCells[0]*numCells[1]*numCells[2]);
    m_cellColors.resize(nCells);
    m_vertexColors.resize(nCells);

    DO_COORDS(coord){
      m_numCells[coord] = numCells[coord];
      m_cellSize[coord] = simVol.Thickness(coord)/((REAL)numCells[coord]);
    }
  }

  cRECTILINEAR_GRID::cRECTILINEAR_GRID(const cBOX3 &simVol,
				       INT numCells)
  {
    INT numGridCells[3];
    for (INT i=0; i<3; i++)
      numGridCells[i] = numCells;
    cRECTILINEAR_GRID(simVol, numGridCells);
  }
  
  iCELL_INDEX cRECTILINEAR_GRID::CellIndex(const cPOINT3 &point)
  {
    UINT index[3];
    DO_COORDS(coord){
      REAL u = ((point[coord] - m_simVol.Min()[coord])/m_cellSize[coord]);
      index[coord] = (INT)u;
    }

    return iCELL_INDEX(index[0], index[1], index[2]);
  }

  //normal is used to resolve borderline cases
  INT cRECTILINEAR_GRID::ModifiedCellIndex(const cPOINT3 &point,
				   cVECTOR3 normal, iCELL_INDEX indices[2])
  {
    INT index[3];
    
    BOOL onGridPlane = false;
    eCOORD gridPlaneCoord;
    DO_COORDS(coord){
      REAL u = ((point[coord] - m_simVol.Min()[coord])/m_cellSize[coord]);
      index[coord] = (INT)(u+cLIMITS::Epsilon());
      if (fabs(u-index[coord]) <= cLIMITS::Epsilon()){
// 	onGridPlane = true;
// 	gridPlaneCoord = coord;
// 	printf("Raju I am here...\n");

	if (normal[coord] > 0.0) {
	  index[coord]--;
	}
      }
    }

    INT numIndices = 1;

    indices[0] = iCELL_INDEX(index[0], index[1], index[2]);

//     if(onGridPlane) {
//       index[gridPlaneCoord]--;
//       indices[1] = iCELL_INDEX(index[0], index[1], index[2]);
//       numIndices = 2;
//     }

    return numIndices;
  }

  iCELL_INDEX cRECTILINEAR_GRID::CellIndex(REAL x, REAL y, REAL z)
  {
    return CellIndex(cPOINT3(x, y, z));
  }

  cBOX3 cRECTILINEAR_GRID::CellBox(iCELL_INDEX cellIndex)
  {
    INT coordIndex[3];
    
    coordIndex[0] = cellIndex.x; coordIndex[1] = cellIndex.y; coordIndex[2] = cellIndex.z;
    
    cPOINT3 cellBoxMin, cellBoxMax;
    DO_COORDS(i){
      REAL h = m_simVol.Thickness(i) / ((REAL)m_numCells[i]);
      cellBoxMin[i] = m_simVol.MinCoord(i) + coordIndex[i] * h;
      cellBoxMax[i] = cellBoxMin[i] + h;
    }

    cBOX3 cellBox(cellBoxMin, cellBoxMax);
    return cellBox;
  }

  REAL cRECTILINEAR_GRID::CellUpperBound(REAL val, eCOORD coord)
  {
    REAL h = m_simVol.Thickness(coord)/((REAL)m_numCells[coord]);
 
    INT index = (INT)((val - m_simVol.MinCoord(coord))/h) + 1;
  
    return m_simVol.MinCoord(coord) + (index*h);
  }

  REAL cRECTILINEAR_GRID::CellLowerBound(REAL val, eCOORD coord)
  {
    REAL h = m_simVol.Thickness(coord)/((REAL)m_numCells[coord]);
 
    INT index = (INT)((val - m_simVol.MinCoord(coord))/h);
  
    return m_simVol.MinCoord(coord) + (index*h);
  }

  eCELL_COLOR cRECTILINEAR_GRID::CellColor(iCELL_INDEX index)  
  {
    cGRID_CELL_COLORS::iterator pos = m_cellColors.find(index);

    if(pos == m_cellColors.end())
      return UNKNOWN;

    return pos->second;
  }

  eCELL_COLOR cRECTILINEAR_GRID::VertexColor(iGRID_VERTEX index)
  {
    cGRID_VERTEX_COLORS::iterator pos = m_vertexColors.find(index);
    
    if(pos == m_vertexColors.end())
      return UNKNOWN;

    return pos->second;
  }

  VOID cRECTILINEAR_GRID::CollectWhiteVertices()
  {
    cGRID_CELL_COLORS::iterator currPos = m_cellColors.begin();
    cGRID_CELL_COLORS::iterator lastPos = m_cellColors.end();

    for( ; currPos != lastPos ; currPos++ ) {
      if(currPos->second == WHITE) {
        iCELL_INDEX cellIndex = currPos->first;
        
        for(INT i = 0; i < 2 ; i++ ) {
          for(INT j = 0; j < 2 ; j++ ) {
            for(INT k = 0; k < 2 ; k++ ) {
              iGRID_VERTEX vIndex(cellIndex.x+i, cellIndex.y+j, cellIndex.z+k);
              m_vertexColors[vIndex] = WHITE;
            }
          }
        }
      }
    }
  }

  VOID cRECTILINEAR_GRID::AddToFront(eCELL_COLOR sourceCellColor, 
				     iCELL_INDEX index, std::vector<iCELL_INDEX> &front,
				     cGRID_CELL_MARKS &alreadyVisited)
  {
    if(!IsValid(index))
      return;

    cGRID_CELL_COLORS::iterator indexColor = m_cellColors.find(index);

    if(sourceCellColor == GRAY) {
      if(indexColor != m_cellColors.end() && 
	 indexColor->second == GRAY) {

	cGRID_CELL_MARKS::iterator pos = alreadyVisited.find(index);

	if(pos == alreadyVisited.end()) {
	  alreadyVisited[index] = true;
	  front.push_back(index);
	}

	return;
      }
    }
    else {
      cGRID_CELL_MARKS::iterator pos = alreadyVisited.find(index);
      if(pos == alreadyVisited.end()) {
	alreadyVisited[index] = true;
	front.push_back(index);

	if(indexColor == m_cellColors.end())
	  CellColor(index, WHITE);
      }
    }
  }

  VOID cRECTILINEAR_GRID::AdvancingFront(std::vector<iCELL_INDEX> &front,
					 cGRID_CELL_MARKS &alreadyVisited)
  {
        //Z-min, max cells.
    INT i = 0, j = 0;
    for( ; i < NumCells(GEOM_X) ; i++) {
      for(; j < NumCells(GEOM_Y) ; j++ ) {
	AddToFront(WHITE, iCELL_INDEX(i, j, 0), front, alreadyVisited);
	AddToFront(WHITE, iCELL_INDEX(i, j, NumCells(GEOM_Z)-1), front, alreadyVisited);
      }
    }

    //Y-min, max cells.
    i = 0, j = 0;
    for( ; i < NumCells(GEOM_X) ; i++) {
      for(; j < NumCells(GEOM_Z) ; j++ ) {
	AddToFront(WHITE, iCELL_INDEX(i, 0, j), front, alreadyVisited);
	AddToFront(WHITE, iCELL_INDEX(i, NumCells(GEOM_Y)-1, j), front, alreadyVisited);
      }
    }
    
    //X-min, max cells
    i = 0, j = 0;
    for( ; i < NumCells(GEOM_Y) ; i++) {
      for(; j < NumCells(GEOM_Z) ; j++ ) {
	AddToFront(WHITE, iCELL_INDEX(0,i,j), front, alreadyVisited);
	AddToFront(WHITE, iCELL_INDEX(NumCells(GEOM_X)-1, i, j), front, alreadyVisited);
      }
    }
  }
	
  VOID cRECTILINEAR_GRID::MarkBlackCells(cGRID_CELL_MARKS &visitedCells)
  {
    for(INT i = 0; i < m_numCells[0] ; i++ ) {
      for(INT j = 0; j < m_numCells[1] ; j++ ) {
	for(INT k = 0; k < m_numCells[2] ; k++ ) {
	  iCELL_INDEX index(i, j, k);
	  cGRID_CELL_MARKS::iterator pos = visitedCells.find(iCELL_INDEX(i, j, k));
	  if(pos == visitedCells.end()) {
	    CellColor(index, BLACK);
	  }
	}  
      }  
    }  
  }

  VOID cRECTILINEAR_GRID::Print() 
  {
    for(INT i = 0; i < m_numCells[0] ; i++ ) {
      for(INT j = 0; j < m_numCells[1] ; j++ ) {
	for(INT k = 0; k < m_numCells[2] ; k++ ) {
	  iCELL_INDEX index(i, j, k);
	  index.Print();
	  printf("%d\n", CellColor(index));
	}  
      }  
    }  
  }
  
  /*
   *  1. Before calling this method all gray cells need to be marked.
   *  2. The outer layer of cells are assumed to be WHITE.
   *  3. Keep walking until all white cells are visited.
   */
  VOID cRECTILINEAR_GRID::Walk(BOOL markBlackCells)
  {
    std::vector<iCELL_INDEX> front;
    cGRID_CELL_MARKS alreadyVisited;

    AdvancingFront(front, alreadyVisited);

    INT oldFrontSize, newFrontSize;
    oldFrontSize = newFrontSize = front.size();

    do {
      INT i = 0;
      oldFrontSize = front.size();
      while(i < oldFrontSize) {
	iCELL_INDEX cellIndex = front[i];
	cGRID_CELL_COLORS::iterator cellColorItr = m_cellColors.find(cellIndex);
	eCELL_COLOR cellColor = cellColorItr != m_cellColors.end() ? cellColorItr->second : UNKNOWN;

	//X direction neighbors.
	AddToFront(cellColor, iCELL_INDEX(cellIndex.x-1, cellIndex.y, cellIndex.z),
		   front, alreadyVisited);
	  
	AddToFront(cellColor, iCELL_INDEX(cellIndex.x+1, cellIndex.y, cellIndex.z),
		   front, alreadyVisited);
	
	//Y direction neighbors.
	AddToFront(cellColor, iCELL_INDEX(cellIndex.x, cellIndex.y-1, cellIndex.z),
		   front, alreadyVisited);
	
	AddToFront(cellColor, iCELL_INDEX(cellIndex.x, cellIndex.y+1, cellIndex.z),
		   front, alreadyVisited);
	
	//Z direction neighbors.
	AddToFront(cellColor, iCELL_INDEX(cellIndex.x, cellIndex.y, cellIndex.z-1),
		   front, alreadyVisited);
	
	AddToFront(cellColor, iCELL_INDEX(cellIndex.x, cellIndex.y, cellIndex.z+1),
		   front, alreadyVisited);
	
	i++;
      }

      newFrontSize = front.size();
    
    } while (newFrontSize > oldFrontSize);
    
    if(markBlackCells)
      MarkBlackCells(alreadyVisited);
    
    CollectWhiteVertices();
  }
}
