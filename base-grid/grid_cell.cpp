#include "defs.hpp"
#include "grid_cell.hpp"

namespace base_grid {

template<typename _MESH_TYPE>
typename tGRID_CELL<_MESH_TYPE>::cENTRY*
tGRID_CELL<_MESH_TYPE>::Entry(cMANIFOLD_RECORD *record) {
  INT numEntries = m_entries.size();

  for (INT i = 0; i < numEntries; i++) {
    if (m_entries[i]->Record() == record)
      return m_entries[i];
  }

  cENTRY *newEntry = ::new cENTRY(record);
  m_entries.push_back(newEntry);
  return newEntry;
}

template<typename _MESH_TYPE>
VOID tGRID_CELL<_MESH_TYPE>::Register(cMANIFOLD_RECORD *record,
                                      iFACET facetIndex) {
  cENTRY *entry = Entry(record);
  entry->Register(facetIndex);
  m_color = GRAY;
}

}
