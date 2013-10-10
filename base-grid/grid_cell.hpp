#ifndef _BASE_GRID_GRID_CELL_HPP_
#define _BASE_GRID_GRID_CELL_HPP_

#include "manifold_record.hpp"

namespace base_grid {

  /*
   * Class to represent each entry in a grid cell.
   * Represents each manifold crossing a grid cell.
   */
  template<typename _MESH_TYPE>
  class tGRID_CELL_ENTRY {
     public:
      typedef tMANIFOLD_RECORD<_MESH_TYPE> cMANIFOLD_RECORD;

     public:
      tGRID_CELL_ENTRY(cMANIFOLD_RECORD *record = NULL)
          : m_record(record) {
      }

      VOID Register(cMANIFOLD_RECORD *record) {
        m_record = record;
      }

      VOID Register(iFACET facetIndex) {
        m_facets.push_back(facetIndex);
      }

      std::vector<iFACET> & Facets() {
        return m_facets;
      }

      cMANIFOLD_RECORD *Record() {
        return m_record;
      }

      ~tGRID_CELL_ENTRY() {
        std::vector<cCUT_CELL*>::iterator currCutCell = m_cutCells.begin();
        std::vector<cCUT_CELL*>::iterator lastCutCell = m_cutCells.end();

        for (; currCutCell != lastCutCell; currCutCell++)
          delete *currCutCell;
      }

      VOID PrintFacets(FILE *stream = stdout) {
        std::vector<iFACET>::iterator currFacet = m_facets.begin();
        std::vector<iFACET>::iterator lastFacet = m_facets.end();

        fprintf(stream, "Facets : ");

        for (; currFacet != lastFacet; currFacet++) {
          fprintf(stream, "%d ", *currFacet);
        }

        fprintf(stream, "\n");
        fflush(stream);
      }

      cCUT_CELL* NewCutCell() {
        cCUT_CELL *cell = ::new cCUT_CELL();
        m_cutCells.push_back(cell);
        return cell;
      }

      VOID RemoveCutCells() {
        std::vector<cCUT_CELL*>::iterator currCutCell = m_cutCells.begin();
        std::vector<cCUT_CELL*>::iterator lastCutCell = m_cutCells.end();

        for (; currCutCell != lastCutCell; currCutCell++)
          delete *currCutCell;

        m_cutCells.clear();
      }

     public:
      typedef std::vector<cCUT_CELL*>::iterator cut_cell_iterator;

      cut_cell_iterator CutCellsBegin() {
        return m_cutCells.begin();
      }
      cut_cell_iterator CutCellsEnd() {
        return m_cutCells.end();
      }

      std::vector<cCUT_CELL*>& CutCells() {
        return m_cutCells;
      }

      VOID Clip(eCOORD coord, REAL val);
     private:
      cMANIFOLD_RECORD *m_record;
      std::vector<cCUT_CELL*> m_cutCells;
      std::vector<iFACET> m_facets;  //mesh facets.
  };

  /*
   * Abstraction for each cell in a grid.
   */
  template<typename _MESH_TYPE>
  class tGRID_CELL {
     public:
      typedef tGRID_CELL_ENTRY<_MESH_TYPE> cENTRY;
      typedef typename cENTRY::cMANIFOLD_RECORD cMANIFOLD_RECORD;
      typedef std::vector<cENTRY*> cENTRIES;
     public:
      //
      tGRID_CELL()
          : m_color(UNKNOWN),
            m_index(-1) {
        m_vertexColors[0][0][0] = m_vertexColors[0][0][1] = m_vertexColors[0][1][0] = m_vertexColors[0][1][1] = m_vertexColors[1][0][0] =
            m_vertexColors[1][0][1] = m_vertexColors[1][1][0] = m_vertexColors[1][1][1] = UNKNOWN;

      }

      ~tGRID_CELL() {
        typename cENTRIES::iterator currEntry = m_entries.begin();
        typename cENTRIES::iterator lastEntry = m_entries.end();

        for (; currEntry != lastEntry; currEntry++)
          delete currEntry.operator*();
      }

     public:
      typedef typename std::vector<cENTRY*>::iterator entry_iterator;

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
      VOID Index(INT index) {
        m_index = index;
      }
      INT Index() {
        return m_index;
      }
      const cBOX3& Box() const {
        return m_box;
      }
      VOID Box(const cBOX3 &box) {
        m_box = box;
      }
      eCELL_COLOR Color() const {
        return m_color;
      }
      VOID VertexColor(eCELL_COLOR color, INT i, INT j, INT k) {
        m_vertexColors[i][j][k] = color;
      }

      eCELL_COLOR VertexColor(INT i, INT j, INT k) {
        return m_vertexColors[i][j][k];
      }

      VOID Clip(eCOORD coord, REAL val);
      VOID Color(eCELL_COLOR color) {
        m_color = color;
        if (color != GRAY) {
          m_vertexColors[0][0][0] = m_vertexColors[0][0][1] = m_vertexColors[0][1][0] = m_vertexColors[0][1][1] = m_vertexColors[1][0][0] =
              m_vertexColors[1][0][1] = m_vertexColors[1][1][0] = m_vertexColors[1][1][1] = color;
        }
      }
     public:
      typedef std::vector<cCUT_CELL*>::iterator cut_cell_iterator;

      cut_cell_iterator CutCellsBegin() {
        return m_cutCells.begin();
      }
      cut_cell_iterator CutCellsEnd() {
        return m_cutCells.end();
      }

      std::vector<cCUT_CELL*>& CutCells() {
        return m_cutCells;
      }

     private:
      cENTRY *Entry(cMANIFOLD_RECORD *record);

     private:
      INT m_index;
      std::vector<cENTRY*> m_entries;
      std::vector<cCUT_CELL*> m_cutCells;
      cBOX3 m_box;
      eCELL_COLOR m_color;
      eCELL_COLOR m_vertexColors[2][2][2];
  };

}
#endif //_BASE_GRID_GRID_CELL_HPP_
