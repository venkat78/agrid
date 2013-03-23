#include "volumetric_grid.hpp"
#include "vol_grid_queries.hpp"
#include "tmesh.cpp"


_TVOL_GRID_TMPL_TYPE
VOID _TVOL_GRID_TMPL_DECL::ProcessNmEdgesInBase()
{
  tLIST<cHALF_EDGE*> localNmEdges;

  typename cCELL::face_iterator currFace = m_cell->FacesBegin();
  typename cCELL::face_iterator lastFace = m_cell->FacesEnd();

  for( ; currFace != lastFace ; currFace++) {
    typename cFACET::half_edge_circulator currHe = (*currFace)->HalfEdgesBegin();
    typename cFACET::half_edge_circulator lastHe = (*currFace)->HalfEdgesEnd();

    for( ; currHe != lastHe ; currHe++)
      if(currHe->IsNonManifold())
	localNmEdges.push_back(currHe.operator->());
  }

  typename tLIST<cHALF_EDGE*>::iterator currNmEdge = localNmEdges.begin();
  typename tLIST<cHALF_EDGE*>::iterator lastNmEdge = localNmEdges.end();

  for( ; currNmEdge != lastNmEdge ; currNmEdge++) {
    tEDGE_RING<cSELF> edgeRing(*currNmEdge, this);
    edgeRing.MateHalfEdges();
  }

  cBASE::NmEdges().clear();
}

VOID ExportCellToOff(cVOLUMETRIC_GRID *grid, iCELL cellIndex, char *filename)
{
  sGENERIC_FACET_FILTER filter;

  cVOLUMETRIC_GRID::cCELL *cell = grid->Cell(cellIndex);

  cVOLUMETRIC_GRID::cCELL::face_iterator currFace = cell->FacesBegin();
  cVOLUMETRIC_GRID::cCELL::face_iterator lastFace = cell->FacesEnd();

  for( ; currFace != lastFace ; currFace++) {
    filter.Add((*currFace)->Index());
  }

  tmesh::ExportToOff<cVOLUMETRIC_GRID, sGENERIC_FACET_FILTER>(filename, *grid, filter);
}

GEN_SUB_MESH(tVOLUMETRIC_GRID, sVOL_GRID_STORAGE_POLICY, tVOLUME_GRID_OBJECT_FACTORY,
	     tVERTEX, tHALF_EDGE, tVOLUME_GRID_FACE);

GEN_MESH_QUERIES(cVOLUMETRIC_GRID);

GEN_MESH_CLIPPER(cVOLUMETRIC_GRID);

GEN_MESH_FACETS_OPS(cVOLUMETRIC_GRID);
