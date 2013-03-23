/*
 * hole_closer.hpp
 *
 *  Created on: Feb 18, 2013
 */

#ifndef _SURFACE_MESH_HOLE_CLOSER_HPP_
#define _SURFACE_MESH_HOLE_CLOSER_HPP_

#include "surface_mesh_class.hpp"

 class cHOLE_CLOSER {


  public:

    cHOLE_CLOSER(cSURFACE_MESH *mesh, iMANIFOLD manifoldIndex = INVALID_IMANIFOLD); //constructor
    BOOL CloseHoles();


  private: //methods
    BOOL FormLoop(cSURFACE_MESH::half_edge_iterator startHalfEdge);
    VOID CloseHole(cSURFACE_MESH::half_edge_iterator startHalfEdge);

  private: //fields

    cSURFACE_MESH *m_mesh;
    iMANIFOLD m_manifoldIndex;
    INT m_maxHoleSize;
    INT m_holeSize;
    iVERTEX m_loopVertexIndex[128];
    INT m_debugCount;
    BOOL m_debugPrint;
  };


#endif //_SURFACE_MESH_HOLE_CLOSER_HPP_
