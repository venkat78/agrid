#ifndef _AGRID_BOOLEAN_CELL_HPP_
#define _AGRID_BOOLEAN_CELL_HPP_

#include "manifold_record.hpp"

namespace agrid_boolean {
  /*
   * Definition of various cell types in a rectilinear grid.
   */
  class cCELL
  {
  public:
    cCELL() {}
    cCELL(iCELL_INDEX gridIndex, const cBOX3 &box) : m_index(gridIndex), m_box(box)
    {}

  public:
          iCELL_INDEX Index() const { return m_index; }
    const cBOX3& Box() const { return m_box; }

  private:
    /*
     * Index in rectilinear grid.
     */
    iCELL_INDEX m_index; 

    /*
     * Bouding box in cell.
     */
    cBOX3 m_box;

  };

  
  /*
   * Description of a gray cell.
   */

  /*
   *  Entries for a gray cell. currently hold only mesh, eventually have to hold
   *  a shell.
   */
  class cGRAY_CELL_ENTRY
  {
  public:
    cGRAY_CELL_ENTRY(cMANIFOLD_RECORD *record = NULL) : m_record(record) { }

    VOID Register(cMANIFOLD_RECORD *record) {
      m_record = record;
    }

    VOID Register(iFACET facetIndex) {
      m_facets.push_back(facetIndex);
    }

    VOID Close(iCELL_INDEX index);

    std::vector<iFACET> & Facets()
    {
      return m_facets;
    }
    
    cMANIFOLD_RECORD *Record() { return m_record; }
    
    cCUT_CELL* NewCutCell() {
      cCUT_CELL *cell = ::new cCUT_CELL();
      m_cutCells.push_back(cell);
      return cell;
    }

    VOID RemoveCutCells() {
      std::vector<cCUT_CELL*>::iterator currCutCell = m_cutCells.begin();
      std::vector<cCUT_CELL*>::iterator lastCutCell = m_cutCells.end();
      
      for( ; currCutCell != lastCutCell ; currCutCell++)
	delete *currCutCell;

      m_cutCells.clear();
    }

    ~cGRAY_CELL_ENTRY() { 
      std::vector<cCUT_CELL*>::iterator currCutCell = m_cutCells.begin();
      std::vector<cCUT_CELL*>::iterator lastCutCell = m_cutCells.end();
      
      for( ; currCutCell != lastCutCell ; currCutCell++)
	delete *currCutCell;
    }

    VOID PrintFacets(FILE *stream = stdout) {
      std::vector<iFACET>::iterator currFacet = m_facets.begin();
      std::vector<iFACET>::iterator lastFacet = m_facets.end();
      
      fprintf(stream, "Facets : ");

      for( ; currFacet != lastFacet ; currFacet++ ) {
	fprintf(stream, "%d ", *currFacet); 
      }
      
      fprintf(stream, "\n");
      fflush(stream);
    }
    
    VOID Export(const cBOX3 &box, iCELL_INDEX index);
  public:
    typedef std::vector<cCUT_CELL*>::iterator cut_cell_iterator;

    cut_cell_iterator CutCellsBegin() { return m_cutCells.begin(); }
    cut_cell_iterator CutCellsEnd() { return m_cutCells.end(); }

    std::vector<cCUT_CELL*>& CutCells() {
      return m_cutCells;
    }

  private:
    cMANIFOLD_RECORD *m_record;
    std::vector<cCUT_CELL*> m_cutCells;
    std::vector<iFACET> m_facets; //mesh facets.
  };
  
  
  class cGRAY_CELL : public cCELL
  {
  public:
    //    typedef std::list<cGRAY_CELL_ENTRY*> cENTRIES;
    cGRAY_CELL() : cCELL()
    {}

    cGRAY_CELL(iCELL_INDEX index, const cBOX3& box) : cCELL(index,box)
    {}

    ~cGRAY_CELL() {
      std::vector<cGRAY_CELL_ENTRY*>::iterator currEntry = m_entries.begin();
      std::vector<cGRAY_CELL_ENTRY*>::iterator lastEntry = m_entries.end();
      
      for( ; currEntry != lastEntry ; currEntry++)
 	delete currEntry.operator*();


      std::vector<cCUT_CELL*>::iterator currCutCell = m_cutCells.begin();
      std::vector<cCUT_CELL*>::iterator lastCutCell = m_cutCells.end();
      
      for( ; currCutCell != lastCutCell ; currCutCell++)
	delete *currCutCell;
    }

    VOID Close();
  public:
    typedef std::vector<cGRAY_CELL_ENTRY*>::iterator entry_iterator;
    
    entry_iterator begin() {
      return m_entries.begin();
    }

    entry_iterator end() {
      return m_entries.end();
    }

  public:
    cCUT_CELL* NewCutCell() {
      cCUT_CELL *cell = ::new cCUT_CELL();
      m_cutCells.push_back(cell);
      return cell;
    }

    VOID Register(cMANIFOLD_RECORD *record, iFACET facet);
    VOID Export();
    VOID ExportManifolds();
    INT NumEntries() {
      return m_entries.size();
    }

    VOID DeleteIndividualManifoldCutCells() {
      std::vector<cGRAY_CELL_ENTRY*>::iterator currEntry = m_entries.begin();
      std::vector<cGRAY_CELL_ENTRY*>::iterator lastEntry = m_entries.end();
      
      for( ; currEntry != lastEntry ; currEntry++)
	(*currEntry)->RemoveCutCells();
    }

    VOID DeleteCutCells() {
      DeleteIndividualManifoldCutCells();

      std::vector<cCUT_CELL*>::iterator currCutCell = m_cutCells.begin();
      std::vector<cCUT_CELL*>::iterator lastCutCell = m_cutCells.end();
      
      for( ; currCutCell != lastCutCell ; currCutCell++)
	delete *currCutCell;

      m_cutCells.clear();
    }

  public:
    typedef std::vector<cCUT_CELL*>::iterator cut_cell_iterator;

    cut_cell_iterator CutCellsBegin() { return m_cutCells.begin(); }
    cut_cell_iterator CutCellsEnd() { return m_cutCells.end(); }
    
    std::vector<cCUT_CELL*>& CutCells() {
      return m_cutCells;
    }

  private:
    cGRAY_CELL_ENTRY *Entry(cMANIFOLD_RECORD *record);

  private:
    //    cENTRY_MAP m_map;
    std::vector<cGRAY_CELL_ENTRY*> m_entries;
    std::vector<cCUT_CELL*> m_cutCells;
  };

}
#endif //_AGRID_BOOLEAN_CELL_HPP_
