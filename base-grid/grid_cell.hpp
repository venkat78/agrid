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

 public:

 private:
  cMANIFOLD_RECORD *m_record;
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
  cBOX3 Box()
  {
    return cBOX3();
  }
 public:

 private:
  cENTRY *Entry(cMANIFOLD_RECORD *record);

 private:
  INT m_index;
  std::vector<cENTRY*> m_entries;
  eCELL_COLOR m_color;
};

}
#endif //_BASE_GRID_GRID_CELL_HPP_
