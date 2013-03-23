/*
 * volume_mesh.hpp
 *
 *  Created on: Aug 19, 2010
 */

#ifndef _AGRID_MESH_VOLUME_GRID_HPP_
#define _AGRID_MESH_VOLUME_GRID_HPP_

#include "cut_cell.hpp"

typedef INT iCELL;
typedef iFACET iFACE;
const iCELL INVALID_ICELL=-100;

struct sFACE_CELL_PAIR {
  INT faceIndex;
  INT cellIndex;
};

//Used for openfoam output.
struct sFACE_CELL_PAIR_CMP {
  BOOL operator() ( const sFACE_CELL_PAIR &pair1, const sFACE_CELL_PAIR &pair2 ) {
    if (pair1.cellIndex < 0) //pairs corresponding to boundary facets are moved to the end of the container
      return (pair2.cellIndex < 0);//false; //YG-0321
    if(pair2.cellIndex < 0 || pair1.cellIndex < pair2.cellIndex)
      return true;

    return false;
  }
};

template <typename _Face_Type>
class tGRID_CELL
{
public:
  tGRID_CELL() : m_index(INVALID_ICELL)
  {}

  ~tGRID_CELL()
  {}

  VOID AppendFace(_Face_Type* face) {
    m_faces.push_back(face);
  }

  VOID RemoveFace(_Face_Type *face) {
    m_faces.remove(face);
  }

  VOID Index(iCELL index) {
    m_index = index;
  }

  iCELL Index() const {
    return m_index;
  }

  VOID GridIndex(iCELL_INDEX index) {
    m_gridIndex = index;
  }

  iCELL_INDEX GridIndex() {
    return m_gridIndex;
  }

  BOOL Verify() {
    INT numFaces = m_faces.size();

    if(numFaces < 4) {
      //printf("Num faces in cell %d < 4\n", m_index);
      return false;
    }

    std::vector<sFACE_CELL_PAIR> neighbours;
    Neighbours(neighbours);
    std::sort(neighbours.begin(), neighbours.end(), sFACE_CELL_PAIR_CMP());

    INT n = neighbours.size();

    for(INT i = 0 ; i < n - 1; i++ ) {
      if(neighbours[i].cellIndex >= 0) {
	if(neighbours[i].cellIndex == neighbours[i+1].cellIndex && m_index < neighbours[i].cellIndex) {
// 	  printf("Cells %d %d share more than one face\n", m_index, neighbours[i].cellIndex);
// 	  fflush(stdout);
// 	  ASSERT(neighbours[i].cellIndex != neighbours[i+1].cellIndex);
	  return true;
	}
      }
    }

    return true;
  }

public:
  //iterators to walk incident faces.
  typedef typename tLIST<_Face_Type*>::iterator face_iterator;

  face_iterator FacesBegin() const {
    return m_faces.begin();
  }

  face_iterator FacesEnd() const {
    return m_faces.end();
  }

  //returns true if some of the neighbours are on the boundary
  //it's used for further checking
  BOOL Neighbours(std::vector<sFACE_CELL_PAIR> &neighbours)
  {
    BOOL rv = false;
    face_iterator currFace = m_faces.begin();
    face_iterator lastFace = m_faces.end();
    for( ; currFace != lastFace ; currFace++ ){
      typename _Face_Type::cell_iterator currCell = (*currFace)->CellsBegin();
      typename _Face_Type::cell_iterator lastCell = (*currFace)->CellsEnd();
      INT numFacetCells = 0;

      for( ; currCell != lastCell ; currCell++ ) {
        if((*currCell)->Index() != m_index) {
          sFACE_CELL_PAIR pair;
          pair.faceIndex = (*currFace)->Index();
          pair.cellIndex = (*currCell)->Index();
          neighbours.push_back(pair);
        }
        numFacetCells++;
      }
      //if facet is boundary
      if (numFacetCells == 1){
    	sFACE_CELL_PAIR pair;
    	pair.faceIndex = (*currFace)->Index();
    	pair.cellIndex = -1;
        neighbours.push_back(pair);
        rv = true;
      }
    }
    return rv;
  }

private:
  iCELL_INDEX m_gridIndex;
  iCELL m_index;
  tLIST<_Face_Type*> m_faces;
};

template <typename _Mesh_Entities>
class tVOLUME_GRID_FACE : public tmesh::tFACET<_Mesh_Entities> {
public:
  typedef tmesh::tFACET<_Mesh_Entities> cBASE;
  typedef tVOLUME_GRID_FACE<_Mesh_Entities> cSELF;

  _TMESH_TOPO_TYPES(_Mesh_Entities);
  typedef tGRID_CELL<cFACET> cCELL;
public:
  tVOLUME_GRID_FACE() : cBASE(),
			m_isFromInputSurface(false)
  {}

  tVOLUME_GRID_FACE(const cSELF &f) : cBASE(f),
				      m_cells(f.m_cells),
				      m_isFromInputSurface(f.m_isFromInputSurface)
  {}

  ~tVOLUME_GRID_FACE()
  {}

  VOID AppendCell(cCELL *cell) {
    m_cells.push_back(cell);
  }

  VOID IsFromInputSurface(BOOL flag) {
    m_isFromInputSurface = flag;
  }

  VOID RemoveCells() {
    m_cells.clear();
  }

public:
  //Query methods.
  BOOL IsInterior() {
    return m_cells.size() > 1;
  }

  BOOL IsFromInputSurface() {
    //    ASSERT(!IsInterior());
    return m_isFromInputSurface;
  }

  BOOL IsBoundary() {
    ASSERT(!cBASE::IsDeleted());
    return m_cells.size() == 1;
  }

  /*
   * The methods below assume
   */
  iCELL LeftCellIndex() {
    return m_cells.front()->Index();
  }

  iCELL RightCellIndex() {
    return m_cells.size() > 1 ? m_cells.back()->Index() : INVALID_ICELL;
  }

  VOID Print(FILE *file = stdout, BOOL logP = false) {
    cBASE::Print();
    ASSERT(m_cells.size() > 0);

    cell_iterator currCell = m_cells.begin();
    cell_iterator lastCell = m_cells.end();
    for(; currCell != lastCell ; currCell++){
      printf("c%d ", (*currCell)->Index());
    }

    m_isFromInputSurface ? printf("isFromInputSurface = true\n") : 
      printf("isFromInputSurface = false\n");

    printf("\n\n");
  }

  BOOL Verify() {
    BOOL retVal = cBASE::Verify();
    ASSERT(m_cells.size() <= 2);
    ASSERT(m_cells.size() > 0);

    if(m_cells.size() > 1) {
      retVal = (m_cells.front()->Index() < m_cells.back()->Index()) && retVal;
      ASSERT(m_cells.front()->Index() < m_cells.back()->Index());
    }

    return retVal;
  }

public:
  //iterators for incident cells;
  typedef typename tLIST<cCELL*>::iterator cell_iterator;

  cell_iterator CellsBegin() {
    return m_cells.begin();
  }

  cell_iterator CellsEnd() {
    return m_cells.end();
  }

protected:
  tLIST<cCELL*> m_cells;
  BOOL m_isFromInputSurface;
};


template <typename _Mesh_Entities,
	  typename _Storage_Policy>
class tVOLUME_GRID_OBJECT_FACTORY : public tMESH_OBJECT_FACTORY< _Mesh_Entities, _Storage_Policy >
{
  typedef tVOLUME_GRID_OBJECT_FACTORY<_Mesh_Entities, _Storage_Policy> cSELF;
  typedef tMESH_OBJECT_FACTORY<_Mesh_Entities, _Storage_Policy> cBASE;

public:
  _TOBJECT_FACTORY_ENTITIES(_Mesh_Entities);
  typedef tGRID_CELL<cFACET> cCELL;

  typedef typename tTABLE<cCELL>::iterator cell_iterator;
public:
  tVOLUME_GRID_OBJECT_FACTORY() : cBASE(), m_cells(_Storage_Policy::numObjectsInPage,
						   _Storage_Policy::logOfObjectsInPage,
						   _Storage_Policy::numInitialPages, 0)
  {}

  ~tVOLUME_GRID_OBJECT_FACTORY()
  {}

  cCELL* NewCell()
  {
    cCELL* cell = m_cells.new_object();
    cell->Index(m_cells.size() - 1);
    return cell;
  }

  cCELL* Cell(iCELL cellIndex) {
    ASSERT(cellIndex >= 0 && cellIndex < m_cells.size());
    return m_cells.object_at(cellIndex);
  }

  INT NumCells() const {
    return m_cells.size();
  }

  cFACET* Facet(iFACE faceIndex) {
    return cBASE::Facet(faceIndex);
  }

public:
  cell_iterator CellsBegin() {
    return m_cells.begin();
  }

  cell_iterator CellsEnd() {
    return m_cells.end();
  }

private:
  tTABLE<cCELL> m_cells;
};

template <typename _Mesh_Entities,
	  typename _Storage_Policy,
	  template <typename _ME, typename _SP> class _Object_Factory >
class tVOLUMETRIC_GRID : public tMESH < _Mesh_Entities,	_Storage_Policy, _Object_Factory >
	  {
	  public:
	    typedef tMESH < _Mesh_Entities, _Storage_Policy, _Object_Factory > cBASE;
	    typedef tVOLUMETRIC_GRID< _Mesh_Entities, _Storage_Policy, _Object_Factory > cSELF;

	    _TMESH_ENTITIES(cBASE);
	    typedef tGRID_CELL<cFACET> cCELL;
	    typedef typename _Object_Factory<_Mesh_Entities, _Storage_Policy>::cell_iterator cell_iterator;
	  public:
	    tVOLUMETRIC_GRID(const cBOX3 &simVol = cBOX3()) : cBASE(),
							      m_simVol(simVol),
							      m_cell(NULL)
	    {}


	    ~tVOLUMETRIC_GRID()
	    {}

	    //Methods to add new cells to the grid.
	    VOID NewCellStart(iCELL cellIndex = INVALID_ICELL) {
	      if(cellIndex == INVALID_ICELL)
		m_cell = cBASE::Factory().NewCell();
	      else
		m_cell = cBASE::Factory().Cell(cellIndex);
	    }

	    VOID NewCellAddFace(iFACE faceIndex) {
	      cFACET* face = cBASE::Factory().Facet(faceIndex);
	      m_cell->AppendFace(face);
	      face->AppendCell(m_cell);
	    }

	    iCELL NewCellClose() {
	      iCELL cellIndex = m_cell->Index();
	      //ProcessNmEdgesInBase();
	      m_cell = NULL;
	      return cellIndex;
	    }

	    cBOX3 SimulationVolume() {
	      return m_simVol;
	    }

	    /*
	     * Access methods.
	     */
	    cCELL* Cell(iCELL cellIndex) {
	      return cBASE::Factory().Cell(cellIndex);
	    }

	    cFACET* Face(iFACE faceIndex) {
	      return cBASE::Factory().Facet(faceIndex);
	    }

	    BOOL VerifyCells() {
	      cell_iterator currCell = CellsBegin();
	      cell_iterator lastCell = CellsEnd();

	      INT numFailedCells = 0;
	      for( ; currCell != lastCell ; currCell++ ){
		if(!currCell->Verify()){
		  if(IsDebugOn()) {
		    printf("cell %d is bad\n", currCell->Index());
		    currCell->GridIndex().Print();
		  }
		  
		  sGENERIC_FACET_FILTER filter;

		  typename cCELL::face_iterator currFace = currCell->FacesBegin();
		  typename cCELL::face_iterator lastFace = currCell->FacesEnd();

		  for( ; currFace != lastFace ; currFace++ ) {
		    filter.Add((*currFace)->Index());
		  }
		  
		  char filename[128];
		  sprintf(filename, "failed_cut_cell_%d.off\0", numFailedCells++);

      		  ExportToOff<cSELF, sGENERIC_FACET_FILTER>(filename, *this, filter);
		  
		}
	      }

	      return true;
	    }

	    BOOL HasInputSurface() {
	      typename cBASE::facet_iterator currFace = cBASE::FacetsBegin();
	      typename cBASE::facet_iterator lastFace = cBASE::FacetsEnd();
    
	      BOOL hasInputSurface = false;
    
	      for( ; currFace != lastFace ; currFace++ ) {
		if(currFace->IsFromInputSurface())
		  hasInputSurface = true;
	      }

	      return hasInputSurface;
	    }
  
	    BOOL Verify() {
	      return cBASE::Verify() && VerifyCells();
	    }

	  public:
	    /*
	     * Various book keeping info.
	     */
	    INT NumVertices() {
	      return cBASE::NumVertices();
	    }

	    INT NumFaces() {
	      return cBASE::NumFacets();
	    }

	    INT NumCells() {
	      return cBASE::Factory().NumCells();
	    }

	    /*
	     * Computes num interior faces.
	     */

	    INT NumInteriorFaces() {
	      typename cBASE::facet_iterator currFace = cBASE::FacetsBegin();
	      typename cBASE::facet_iterator lastFace = cBASE::FacetsEnd();

	      INT numFaces = 0;
	      for( ; currFace != lastFace ; currFace++ )
		if(currFace->IsInterior())
		  numFaces++;

	      return numFaces;
	    }

	    iFACET InsertDiagonal(iFACET facetIndex, iVERTEX diagEndPoint1,
				  iVERTEX diagEndPoint2, iFACET newFacetIndex = INVALID_IFACET) {
	      iFACET newIndex = cBASE::InsertDiagonal(facetIndex, diagEndPoint1, diagEndPoint2, newFacetIndex);
	      if(newIndex == INVALID_IFACET)
		return INVALID_IFACET;


	      cFACET *newFacet = cBASE::Facet(newIndex);
	      typename cFACET::cell_iterator currCell = newFacet->CellsBegin();
	      typename cFACET::cell_iterator lastCell = newFacet->CellsEnd();

	      for( ; currCell != lastCell ; currCell++ ) {
		(*currCell)->AppendFace(newFacet);
	      }

	      return newIndex;
	    }

	  public:
	    cell_iterator CellsBegin() {
	      return cBASE::Factory().CellsBegin();
	    }

	    cell_iterator CellsEnd() {
	      return cBASE::Factory().CellsEnd();
	    }

	  private:
	    //NM edge representation in tMESH uses up a lot of memory for VOLUMETRIC_GRID.
	    // This method tries to keep it in check.
	    VOID ProcessNmEdgesInBase() ;

	  private:
	    cBOX3 m_simVol;
	    cCELL *m_cell;
	  };

struct sVOL_GRID_STORAGE_POLICY {
  static const INT numObjectsInPage = 512;
  static const INT logOfObjectsInPage = 9;
  static const INT numInitialPages = 512;
};

TYPEDEF_SUBMESH(cVOLUMETRIC_GRID, tVOLUMETRIC_GRID, sVOL_GRID_STORAGE_POLICY,
		tVOLUME_GRID_OBJECT_FACTORY, tVERTEX, tHALF_EDGE, tVOLUME_GRID_FACE);


VOID ExportCellToOff(cVOLUMETRIC_GRID *grid, iCELL cellIndex, char *filename);

#define _TVOL_GRID_TMPL_TYPE				\
  template <typename _Mesh_Entities,			\
	    typename _Storage_Policy,			\
	    template <typename _ME, typename _SP>	\
  class _Object_Factory >


#define _TVOL_GRID_TMPL_DECL						\
  tVOLUMETRIC_GRID<_Mesh_Entities, _Storage_Policy, _Object_Factory>


//Defining boundary facet filter to export boundary faces to OFF format.

struct sVOLUMETRIC_GRID_BOUNDARY_FILTER {
  public:
    sVOLUMETRIC_GRID_BOUNDARY_FILTER(cVOLUMETRIC_GRID *grid = NULL) : m_grid(grid)
    { }

    BOOL operator () (iFACE faceIndex) const
    {
      return m_grid->Face(faceIndex)->IsBoundary() &&
	!m_grid->SimulationVolume().HasOnBoundary(m_grid->Face(faceIndex)->MeanPoint());
    }

    sCOLOR Color(iFACE faceIndex) const {
      return sCOLOR();
    }

  private:
    cVOLUMETRIC_GRID *m_grid;
  };

struct sVOLUMETRIC_GRID_BOUNDARY_CELL_FILTER {
public:
  sVOLUMETRIC_GRID_BOUNDARY_CELL_FILTER(cVOLUMETRIC_GRID *grid = NULL) : m_grid(grid)
  { }

  BOOL operator () (iFACE faceIndex) const
  {
    if(m_grid->Face(faceIndex)->IsBoundary() &&
       !m_grid->SimulationVolume().HasOnBoundary(m_grid->Face(faceIndex)->MeanPoint()))
      return true;

    cVOLUMETRIC_GRID::cFACET* facet = m_grid->Face(faceIndex);

    cVOLUMETRIC_GRID::cFACET::cell_iterator currCell = facet->CellsBegin();
    cVOLUMETRIC_GRID::cFACET::cell_iterator lastCell = facet->CellsEnd();

    for( ; currCell != lastCell ; currCell++ ) {
      cVOLUMETRIC_GRID::cCELL* cell = (*currCell);

      cVOLUMETRIC_GRID::cCELL::face_iterator currFace = cell->FacesBegin();
      cVOLUMETRIC_GRID::cCELL::face_iterator lastFace = cell->FacesEnd();

      for( ; currFace != lastFace ; currFace++ ) {
	cVOLUMETRIC_GRID::cFACET* f = (*currFace);
	if(f->IsBoundary() &&
	   !m_grid->SimulationVolume().HasOnBoundary(f->MeanPoint()))
	  return true;

      }
    }

    return false;
  }

  sCOLOR Color(iFACE faceIndex) const {
    if(m_grid->Face(faceIndex)->IsBoundary() &&
       !m_grid->SimulationVolume().HasOnBoundary(m_grid->Face(faceIndex)->MeanPoint())){
      sCOLOR boundaryFacetColor(1.0, 0.0, 0.0, 1.0);
      return boundaryFacetColor;
    }
    else
      return sCOLOR();
  }

private:
  cVOLUMETRIC_GRID *m_grid;
};

struct sCUT_CELLS_FILTER {

public:
  sCUT_CELLS_FILTER(cVOLUMETRIC_GRID *grid = NULL) : m_grid(grid)
  {}

  VOID Add(iCELL_INDEX index) {
    m_indices.insert(index);
  }

  BOOL operator() (iFACE faceIndex) const {
    cVOLUMETRIC_GRID::cFACET* facet = m_grid->Face(faceIndex);

    cVOLUMETRIC_GRID::cFACET::cell_iterator currCell = facet->CellsBegin();
    cVOLUMETRIC_GRID::cFACET::cell_iterator lastCell = facet->CellsEnd();

    BOOL foundCell = false;
    for( ; currCell != lastCell ; currCell++ ) {
      cVOLUMETRIC_GRID::cCELL* cell = (*currCell);
      if(m_indices.find(cell->GridIndex()) != m_indices.end())
	foundCell = true;
    }

    return foundCell;
  }

  sCOLOR Color(iFACE faceIndex) const {
    return sCOLOR();
  }  

private:
  cVOLUMETRIC_GRID *m_grid;
  std::set<iCELL_INDEX> m_indices;
};

struct sCUT_CELLS_FILTER2 {
public:
  sCUT_CELLS_FILTER2(cVOLUMETRIC_GRID *grid = NULL) : m_grid(grid)
  {}

  VOID Add(iCELL cellIndex) {
    m_indices.insert(cellIndex);
  }
  BOOL operator() (iFACE faceIndex) const {
    cVOLUMETRIC_GRID::cFACET* facet = m_grid->Face(faceIndex);

    cVOLUMETRIC_GRID::cFACET::cell_iterator currCell = facet->CellsBegin();
    cVOLUMETRIC_GRID::cFACET::cell_iterator lastCell = facet->CellsEnd();

    BOOL foundCell = false;
    for( ; currCell != lastCell ; currCell++ ) {
      cVOLUMETRIC_GRID::cCELL* cell = (*currCell);
      if(m_indices.find(cell->Index()) != m_indices.end())
	foundCell = true;
    }

    return foundCell;
  }

  sCOLOR Color(iFACE faceIndex) const {
    return sCOLOR();
  }  

private:
  std::set<iCELL> m_indices;
  cVOLUMETRIC_GRID *m_grid;
};
#endif /* VOLUME_GRID_HPP_ */
