/*
 * off.hpp
 *
 *  Created on: Aug 2, 2009
 */

#ifndef OFF_HPP_
#define OFF_HPP_

#include <map>
#include <cstdio>

#include "defs.hpp"

namespace io {
  template <typename _Mesh_Type>
  VOID ImportOff(FILE *file, _Mesh_Type &mesh)
  {
    //Assumes there are no comment lines.
    char buffer[1024];
    INT numFacets = 0, numVertices = 0;
    //Reading the header line.
    fgets(buffer, 1024, file);

    //Reading num facets and num vertices
    fgets(buffer, 1024, file);
    sscanf(buffer, "%d %d", &numVertices, &numFacets);

    for(INT nv  = 0; nv < numVertices ; nv++) {
      fgets(buffer, 1024, file);
      REAL x, y, z;
      sscanf(buffer, "%lf %lf %lf", &x, &y, &z);
      mesh.NewVertex(x, y, z);
    }

    for(INT nf = 0; nf < numFacets ; nf++) {
      fgets(buffer, 1024, file);
      INT nfv = 0; 
      char *currBuffer = buffer;
      sscanf(currBuffer, "%d", &nfv);

      mesh.NewFacetStart();
     
      for(INT i = 0; i < nfv; i++) {
	currBuffer = strchr(currBuffer, ' ');
	currBuffer++;

	iVERTEX v;
	sscanf(currBuffer, "%d", &v);
	mesh.NewFacetAddVertex(v);
      }
    
      mesh.NewFacetClose();
    }
  }

  template <typename _Mesh_Type>
  VOID ImportOff(char *filename, _Mesh_Type &mesh)
  {
    FILE *fp = fopen(filename, "rb+");
    
    ImportOff<_Mesh_Type>(fp, mesh);
    
    fclose(fp);
  }

  template <typename _Mesh_Type>
  bool ExportToOff(FILE *file , _Mesh_Type &mesh, std::map<iFACET, sCOLOR> *facetColors = NULL)
  {
    INT *vertexIndexMap = ::new INT[mesh.LargestVertexIndex() + 1];
    memset(vertexIndexMap, -1, sizeof(INT)*(mesh.LargestVertexIndex() + 1));

    INT numVertices = 0;

    typename _Mesh_Type::vertex_iterator currVtx = mesh.VerticesBegin();
    typename _Mesh_Type::vertex_iterator lastVtx = mesh.VerticesEnd();

    for( ; currVtx != lastVtx ; currVtx++)
      vertexIndexMap[currVtx->Index()] = numVertices++;

    INT numFacets = mesh.NumFacets();

    fprintf(file, "OFF\n");
    fprintf(file, "%d %d 0\n", numVertices, numFacets);

    typename _Mesh_Type::vertex_iterator currVtx1 = mesh.VerticesBegin();

    for( ; currVtx1 != lastVtx ; currVtx1++) {
      cPOINT3 vtxPoint = mesh.Vertex(currVtx1->Index())->Point();
      fprintf(file, "%lf %lf %lf \n", vtxPoint[0], vtxPoint[1], vtxPoint[2]);
    }

    typename _Mesh_Type::facet_iterator currFacet = mesh.FacetsBegin();
    typename _Mesh_Type::facet_iterator lastFacet = mesh.FacetsEnd();

    for( ; currFacet != lastFacet ; currFacet++) {
      INT numVertices = currFacet->NumVertices();
      fprintf(file, "%d ", numVertices);

      typename _Mesh_Type::cFACET::vertex_circulator facetVtx = currFacet->VerticesBegin();
      typename _Mesh_Type::cFACET::vertex_circulator lastVtx = currFacet->VerticesEnd();

      for( ; facetVtx != lastVtx ; facetVtx++) {
    	fprintf(file, "%d ", vertexIndexMap[facetVtx->Index()]);
      }

      if(facetColors) {
	sCOLOR color = facetColors->find(currFacet->Index())->second;
	fprintf(file, "%lf %lf %lf %lf ", color.r, color.g, color.b, color.alpha);
      }

      fprintf(file, "\n");
    }

    delete [] vertexIndexMap;
    return true;
  }

  template <typename _Mesh_Type>
  bool ExportToOff(char *filename , _Mesh_Type &mesh, std::map<iFACET, sCOLOR> *facetColors = NULL)
  {
    FILE *fp = fopen(filename, "wb+");

    if(fp == NULL)
      return false;

    BOOL retVal = ExportToOff<_Mesh_Type>(fp, mesh, facetColors);
    fflush(fp);
    fclose(fp);

    return retVal;
  }

}

#endif /* OFF_HPP_ */
