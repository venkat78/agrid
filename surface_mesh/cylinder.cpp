/*
 * cylinder.cpp
 *
 *  Created on: Feb 28, 2013
 */

#include "cylinder.hpp"

cCYLINDER::cCYLINDER()
{

}

cCYLINDER::cCYLINDER(cPOINT3 origin, cVECTOR3 axis, REAL radius)
{
  m_origin = origin;
  m_axis = axis;
  m_radius = radius;
}

cCYLINDER_BUILDER::cCYLINDER_BUILDER(cSURFACE_MESH *mesh)
{
  m_mesh = mesh;
}

INT cCYLINDER_BUILDER::RecomputeAll()
{
  INT numRecomputed = 0;
  for (iMANIFOLD m = 0; m < m_mesh->NumManifolds(); m++){
    if (RecomputeManifold(m))
      numRecomputed++;
  }
  m_mesh->Verify();
  return numRecomputed;
}

BOOL cCYLINDER_BUILDER::RecomputeManifold(iMANIFOLD manifoldIndex)
{
  cCYLINDER cylinder;
  printf("manifold %d has %d facets\n",
      manifoldIndex,
      m_mesh->Manifold(manifoldIndex)->NumFacets());
  if (!IsManyfoldCylinder(manifoldIndex, &cylinder))
    return false;
  //move the cylinder origin inwards
  cPOINT3 newOrigin = cylinder.Origin() - cylinder.Axis()*0.2;
  cylinder.Origin(newOrigin);
  m_mesh->DeleteManifold(manifoldIndex);
  if (!m_mesh->Verify())
    printf("something is wrong\n");
  TesselateCylinder(cylinder);
  return true;
}

BOOL cCYLINDER_BUILDER::IsManyfoldCylinder(iMANIFOLD manifoldIndex, cCYLINDER *p_cylinder)
{
  //the manifold is supposed to have maximum flat faces by calling MergeCoplanarFacets

  //find two large polygonsm, and if they are circular with the same radius and parallel, buld the cylinder
  INT maxFacetIndex[2] = {INVALID_IFACET, INVALID_IFACET};
  INT maxNumVertices[2] = {0, 0};

  cSURFACE_MESH::cMANIFOLD::facet_iterator currFacet =
    m_mesh->Manifold(manifoldIndex)->FacetsBegin();
  cSURFACE_MESH::cMANIFOLD::facet_iterator lastFacet = m_mesh->Manifold(manifoldIndex)->FacetsEnd();

  for ( ; currFacet != lastFacet ; currFacet++ ) {
    INT numVert = currFacet->NumVertices();
    iFACET facetIndex = currFacet->Index();
    if (numVert > maxNumVertices[0]){
      maxNumVertices[0] = numVert;
      maxFacetIndex[0] = facetIndex;
    }
    else if (numVert > maxNumVertices[1]){
      maxNumVertices[1] = numVert;
      maxFacetIndex[1] = facetIndex;
    }
  }
  if (maxFacetIndex[0] < 8 || maxFacetIndex[1] < 8)
    return false;

  cVECTOR3 normalBottom = m_mesh->Facet(maxFacetIndex[0])->UnitNormal();
  cVECTOR3 normalTop = m_mesh->Facet(maxFacetIndex[1])->UnitNormal();
  cVECTOR3 shouldBeZero = normalBottom + normalTop;
  if (fabs(shouldBeZero[0]) >= 1e-4 ||
      fabs(shouldBeZero[1]) >= 1e-4 ||
      fabs(shouldBeZero[2]) >= 1e-4)
      return false;

  cPOINT3 centerBottom(0, 0, 0), centerTop(0,0,0);
  //find centers and radii
  REAL rSqBottom = BuildCircle(maxFacetIndex[0], maxNumVertices[0], &centerBottom);
  if (rSqBottom <= 0.0)
    return false;
  REAL rSqTop = BuildCircle(maxFacetIndex[1], maxNumVertices[1], &centerTop);
  if (rSqTop<= 0.0)
    return false;

  if (fabs(rSqBottom-rSqTop) > 1e-4*(rSqBottom-rSqTop))
    return false;
  cVECTOR3 axis = centerTop-centerBottom;
  p_cylinder->Axis(axis);
  p_cylinder->Origin(centerBottom);
  p_cylinder->Radius(sqrt(0.5*(rSqBottom+rSqTop)));
  //perhaps more checks may be needed, like planarity of the facets
  return true;
}

//finds a circle approximating the facet. Retruns a negative number if failed
REAL cCYLINDER_BUILDER::BuildCircle(iFACET facetIndex, INT numVertices,
    cPOINT3 *p_center)
{
  //find centers
  //for now we assume that the points are really located on the circles and take just the mean point of those.
  //the real least squares approximation is non-linear, and we skip it for now

  REAL rSq = 0.0;
  cPOINT3 center(0, 0, 0);
  cSURFACE_MESH::cFACET::vertex_circulator currVtx =
    m_mesh->Facet(facetIndex)->VerticesBegin();
  cSURFACE_MESH::cFACET::vertex_circulator lastVtx =
    m_mesh->Facet(facetIndex)->VerticesEnd();

  for( ; currVtx != lastVtx ; currVtx++ ){
    center += currVtx->Point();
  }

  center *= 1.0/((REAL)numVertices);
  REAL rMinSq = cLIMITS::Max(), rMaxSq = 0.0;

  currVtx = m_mesh->Facet(facetIndex)->VerticesBegin();
  lastVtx =  m_mesh->Facet(facetIndex)->VerticesEnd();

  for( ; currVtx != lastVtx ; currVtx++ ){
    REAL distSq = SquaredDistance(center, currVtx->Point());
    if (distSq > rMaxSq)
      rMaxSq = distSq;
    if (distSq < rMinSq)
      rMinSq = distSq;
  }
  if (rMaxSq - rMinSq > 1e-4*(rMinSq+rMaxSq))
    return -1;
  *p_center = center;
  return 0.5*(rMinSq+rMaxSq);
}

//creates facets forming cylinder
VOID cCYLINDER_BUILDER::TesselateCylinder(cCYLINDER &cylinder,
    INT numVertices)
{
  std::vector<iVERTEX> cylinderLowerVertices(numVertices);
  std::vector<iVERTEX> cylinderUpperVertices(numVertices);
  //new vertex indices should be contiguous, but I'd rather save them temporarily

  cVECTOR3 basisVector1 = cylinder.Axis().PerpVector();
  cVECTOR3 basisVector2 = Cross(cylinder.Axis(), basisVector1);
  basisVector1 = basisVector1.Normalize();
  basisVector2 = basisVector2.Normalize();
  basisVector1 *= cylinder.Radius();
  basisVector2 *= cylinder.Radius();

  //build the vertices

  REAL angleStep = 2.0*M_PI/((REAL)numVertices);
  REAL angle = 0.0;

  for (INT i=0; i < numVertices; i++, angle += angleStep){
    cVECTOR3 vertexVector = basisVector1*cos(angle)+ basisVector2*sin(angle);
    cPOINT3 vertexPoint = cylinder.Origin()+ vertexVector;
    iVERTEX newVertexIndex = m_mesh->NewVertex(vertexPoint);
    cylinderLowerVertices[i] = newVertexIndex;
    vertexPoint += cylinder.Axis();
    newVertexIndex = m_mesh->NewVertex(vertexPoint);
    cylinderUpperVertices[i] = newVertexIndex;
  }

  //bottom facet
  m_mesh->NewFacetStart();
  for (INT i=0; i < numVertices; i++){
    iVERTEX vertexIndex = cylinderLowerVertices[numVertices-i-1];
    m_mesh->NewFacetAddVertex(vertexIndex);
  }
  iFACET bottomFacetIndex = m_mesh->NewFacetClose();

  //top facet
  m_mesh->NewFacetStart();
  for (INT i=0; i < numVertices; i++){
    iVERTEX vertexIndex = cylinderUpperVertices[i];
    m_mesh->NewFacetAddVertex(vertexIndex);
  }
  iFACET topFacetIndex = m_mesh->NewFacetClose();

  //build the quads
  for (INT i=1; i < numVertices; i++){
    m_mesh->NewQuad(cylinderUpperVertices[i-1], cylinderLowerVertices[i-1],
        cylinderLowerVertices[i], cylinderUpperVertices[i]);
  }
  m_mesh->NewQuad(cylinderUpperVertices[numVertices-1], cylinderLowerVertices[numVertices-1],
      cylinderLowerVertices[0], cylinderUpperVertices[0]);

  FILE *mfile = fopen("mesh_cyl.txt", "wb+");
  m_mesh->Print(mfile);
  fclose(mfile);

  if (!m_mesh->Verify())
    printf("bad mesh cylinder construction\n");
  //normal of the bottom facet is opposite of and normal of the top facet coincides
  //with the ctylinder axis
  cVECTOR3 bottomNormal = m_mesh->Facet(bottomFacetIndex)->UnitNormal();
  cVECTOR3 topNormal = m_mesh->Facet(topFacetIndex)->UnitNormal();
  if (bottomNormal*cylinder.Axis() >= 0.0 || topNormal*cylinder.Axis() <= 0.0)
    printf("wrong cylinder orientation\n");
}


