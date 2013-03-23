#include "defs.hpp"
#include "cell.hpp"

namespace agrid_boolean {
//   VOID cGRAY_CELL_ENTRY::ExportToOff(const cBOX3 &box, const iCELL_INDEX cellIndex)
//   {
//     std::map<iFACET, sCOLOR> facetColors;
//     cSURFACE_MESH grayCellMesh;
    
//    BoxToMesh(box, grayCellMesh);

//     sCOLOR boxColor(0.0, 1.0, 1.0, 0.4);

//     INT i = 0;
//     for(; i < 6 ; i++)
//       facetColors[i] = boxColor;

//     iFACET oldLargestFacetLabel = grayCellMesh.LargestFacetIndex();
//     MeshFromFacets(m_facets, *m_mesh, grayCellMesh);
//     iFACET newLargestFacetLabel = grayCellMesh.LargestFacetIndex();

//     sCOLOR facetColor(1.0, 0.0, 0.0, 1.0);

//     for( i = oldLargestFacetLabel + 1; i <= newLargestFacetLabel ; i++ )
//       facetColors[i] = facetColor;

//     char grayFileName[80];
//     sprintf(grayFileName,"gray_cell_%d.off", cellIndex);
    
//     FILE *file = fopen(grayFileName, "wb+");
//     io::ExportToOff(file, grayCellMesh, &facetColors);
//     fclose(file);

//     std::list<cCUT_CELL_MESH*>::iterator currCutCellMesh = m_cutCellMeshes.begin();
//     std::list<cCUT_CELL_MESH*>::iterator lastCutCellMesh = m_cutCellMeshes.end();
    
//     i = 0;
//     for( ; currCutCellMesh != lastCutCellMesh ; currCutCellMesh++) {
//       char cutCellFileName[128];
//       cCUT_CELL_MESH *mesh = *currCutCellMesh;
//       sprintf(cutCellFileName,"cut_cell_%d_%d.off", cellIndex, i);
//       i++;
      
//       file = fopen(cutCellFileName, "wb+");
//       io::ExportToOff(file, *mesh, NULL);
//       fclose(file);
//     }
//   }

//   cCUT_CELL_MESH* cWHITE_CELL::CreateCutCellMesh()
//   {
//     m_mesh = ::new cCUT_CELL_MESH();
//     BoxToMesh(Box(), *m_mesh);
//     return m_mesh;
//   }

  VOID cGRAY_CELL_ENTRY::Close(iCELL_INDEX index)
  {
    cut_cell_iterator currCutCell = CutCellsBegin();
    cut_cell_iterator lastCutCell = CutCellsEnd(); //cutCells.end();
    
    for( ; currCutCell != lastCutCell ; currCutCell++ ) {
      if(!(*currCutCell)->Close()) {
	printf("Failed to close cut-cell\n");
	index.Print();
      }
      
      if(!(*currCutCell)->IsClosed()) {
	printf("Failed to close cut-cell\n");
	index.Print();
      }
    }
  }

  VOID cGRAY_CELL_ENTRY::Export(const cBOX3 &box, iCELL_INDEX index)
  {
    /*
     * Exporting gray-cell entry to off.
     */

    std::map<iFACET, sCOLOR> facetColors;
    cSURFACE_MESH grayCellMesh;
    
    BoxToMesh(box, grayCellMesh);

    sCOLOR boxColor(0.0, 1.0, 1.0, 0.4);
    
    INT i = 0;
    for(; i < 6 ; i++)
      facetColors[i] = boxColor;
    
    iFACET oldLargestFacetLabel = grayCellMesh.LargestFacetIndex();
    tmesh::MeshFromFacets(m_facets, *(m_record->Mesh()), grayCellMesh);
    iFACET newLargestFacetLabel = grayCellMesh.LargestFacetIndex();

    sCOLOR facetColor(1.0, 0.0, 0.0, 1.0);

    for( i = oldLargestFacetLabel + 1; i <= newLargestFacetLabel ; i++ )
      facetColors[i] = facetColor;

    char grayFileName[80];
    sprintf(grayFileName,"gray_cell_entry_%d_%d_%d_%d.off", (INT)index.x, (INT)index.y, (INT)index.z,
	    m_record->Manifold()->Index());

    FILE *file = fopen(grayFileName, "wb+");
    io::ExportToOff(file, grayCellMesh, &facetColors);
    fclose(file);

    cut_cell_iterator currCutCell = CutCellsBegin();
    cut_cell_iterator lastCutCell = CutCellsEnd();

    INT j = 0;
    for( ; currCutCell != lastCutCell ; currCutCell++, j++ ) {
      char filename[128];
      sprintf(filename,"cut_cell_%d_%d_%d_%d_%d.off\0", (INT)index.x, (INT)index.y, (INT)index.z,
	      m_record->Manifold()->Index(), j);
      
      ExportToOff<cCUT_CELL>(filename, *(currCutCell.operator*()));
    }
  }

  cGRAY_CELL_ENTRY* cGRAY_CELL::Entry(cMANIFOLD_RECORD *record)
  {
    INT numEntries = m_entries.size();

    for(INT i = 0 ; i < numEntries ; i++) {
      if(m_entries[i]->Record() == record)
	return m_entries[i];
    }

    cGRAY_CELL_ENTRY *newEntry = ::new cGRAY_CELL_ENTRY(record);
    m_entries.push_back(newEntry);
    return newEntry;
  }

  VOID cGRAY_CELL::Register(cMANIFOLD_RECORD *record, iFACET facetIndex)
  {
    //    ASSERT(m_entries.size() <= 1);
    cGRAY_CELL_ENTRY *entry = Entry(record);
    entry->Register(facetIndex);
  }

  /* 
   * Dumps an OFF file representing the contents of the cell.
   */
  VOID cGRAY_CELL::Export()
  {
    for(INT i = 0 ; i < m_entries.size() ; i++ ) {
      m_entries[i]->Export(Box(), Index());
    }

    if(m_cutCells.size() > 0) {
      cut_cell_iterator currCutCell = CutCellsBegin();
      cut_cell_iterator lastCutCell = CutCellsEnd();
      
      iCELL_INDEX index = Index();

      INT j = 0;
      for( ; currCutCell != lastCutCell ; currCutCell++, j++ ) {
	char filename[128];
	sprintf(filename,"boolean_cut_cell_%d_%d_%d_%d.off\0", (INT)index.x, (INT)index.y, (INT)index.z, j);
	ExportToOff<cCUT_CELL>(filename, *(currCutCell.operator*()));
      }
    }
  }

  VOID cGRAY_CELL::Close()
  {
    entry_iterator currEntry = begin(); //entries.begin();
    entry_iterator lastEntry = end(); //entries.end();
    
    if(NumEntries() > 1) {
      cut_cell_iterator currCutCell = CutCellsBegin();
      cut_cell_iterator lastCutCell = CutCellsEnd(); //cutCells.end();
      for( ; currCutCell != lastCutCell ; currCutCell++ ) {
	if(!(*currCutCell)->Close()){
	  printf("Failed to close cut-cell\n");
	  Index().Print();
	}

	if(!(*currCutCell)->IsClosed()) {
	  printf("Failed to close cut-cell\n");
	  Index().Print();
	}
      }
    }
    else {
      entry_iterator currEntry = begin();
      entry_iterator lastEntry = end();

      for( ; currEntry != lastEntry ; currEntry++ ) {
	(*currEntry)->Close(Index());
      }
    }
  }

  VOID cGRAY_CELL::ExportManifolds()
  {
    /*
     * Exporting gray-cell entry to off.
     */
    
    std::map<iFACET, sCOLOR> facetColors;
    cSURFACE_MESH grayCellMesh;
    
    BoxToMesh(Box(), grayCellMesh);
    
    sCOLOR boxColor(0.0, 1.0, 1.0, 0.4);
    
    INT i = 0;
    for(; i < 6 ; i++)
      facetColors[i] = boxColor;
  
    for(i = 0 ; i < m_entries.size() ; i++ ) {
      iFACET oldLargestFacetLabel = grayCellMesh.LargestFacetIndex();
      tmesh::MeshFromFacets(m_entries[i]->Facets(), *(m_entries[i]->Record()->Mesh()), grayCellMesh);
      iFACET newLargestFacetLabel = grayCellMesh.LargestFacetIndex();
    
      sCOLOR facetColor = GetColor(i); 
    
      for( INT j = oldLargestFacetLabel + 1; j <= newLargestFacetLabel ; j++ )
	facetColors[j] = facetColor;
    }


    char grayFileName[80];
    sprintf(grayFileName,"gray_cell_%d_%d_%d.off", (INT)Index().x, (INT)Index().y, (INT)Index().z);
    
    FILE *file = fopen(grayFileName, "wb+");
    io::ExportToOff(file, grayCellMesh, &facetColors);
    fclose(file);
  }

}
