/*
 * cylinder.hpp
 *
 *  Created on: Feb 28, 2013
 */

#ifndef _SURFACE_MESH_CYLINDER_HPP_
#define _SURFACE_MESH_CYLINDER_HPP_

#include "surface_mesh_class.hpp"

  class cCYLINDER{
  public:
    cCYLINDER();
    cCYLINDER(cPOINT3 origin, cVECTOR3 axis, REAL radius);
    cPOINT3 Origin() {return m_origin;}
    cVECTOR3 Axis() {return m_axis;}
    REAL Radius() {return m_radius;}
    VOID Origin(cPOINT3 &origin) {m_origin = origin;}
    VOID Axis(cVECTOR3 &axis) {m_axis = axis;}
    VOID Radius(REAL radius) {m_radius = radius;}

  private: //member fields
    cPOINT3 m_origin;
    cVECTOR3 m_axis;
    REAL m_radius;
  };

  class cCYLINDER_BUILDER{
  public:
    cCYLINDER_BUILDER(cSURFACE_MESH *mesh);
    INT RecomputeAll();
    BOOL RecomputeManifold(iMANIFOLD manifoldIndex);
    BOOL IsManyfoldCylinder(iMANIFOLD manifoldIndex, cCYLINDER *p_cylinder);
    VOID TesselateCylinder(cCYLINDER &cylinder, INT numVertices = 100);

  private:
    REAL BuildCircle(iFACET facetIndex, INT numVertices, cPOINT3 *p_center);

  private: //member fields
    cSURFACE_MESH *m_mesh;
  };

#endif // _SURFACE_MESH_CYLINDER_HPP_
