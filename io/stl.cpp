/*
 * stl.cpp
 *
 *  Created on: Jul 23, 2009
 */

#include <map>
#include "defs.hpp"
#include "stl.hpp"

namespace io {
  static char* RemoveLeadingWhiteSpace(char* buffer) {
    while (buffer[0] == ' ' || buffer[0] == '\t' || buffer[0] == ',')
      buffer++;

    return buffer;
  }

  BOOL ImportStl(FILE *file, cSURFACE_MESH &mesh) {
    if (file == NULL)
      return false;

    char buffer[1024];
    INT lineNumber = 0;  //for debugging
    char *trimmedBuffer = NULL;
    REAL vertexCoords[3];
    char solidName[128];
    INT currSolidIndex = 0, verticesInFacet = 0, numVertices = 0;
    INT meshVertices[3];
    BOOL validFacetGroup = false;

    std::map<cPOINT3, iVERTEX> pointToVertexMap;

    while (fgets(buffer, 1024, file) != NULL) {
      lineNumber++;
      trimmedBuffer = RemoveLeadingWhiteSpace(buffer);

      if (strncasecmp(trimmedBuffer, "solid", 5) == 0) {
        currSolidIndex++;
        memset(solidName, '\0', 128);
        char *solidNamePtr = trimmedBuffer + 6;
        sscanf(solidNamePtr, "%127s", solidName);

        mesh.NewFacetGroup(solidName);
        validFacetGroup = true;
        //register facetGroup.
      } else if (strncasecmp(trimmedBuffer, "endsolid", 8) == 0) {
        validFacetGroup = false;
      } else if (strncasecmp(trimmedBuffer, "endfacet", 8) == 0) {
        if (verticesInFacet == 3) {
          iFACET facetIndex = mesh.NewTriangle(meshVertices[0], meshVertices[1], meshVertices[2]);  //this is inefficient - keep the facetGroup for as long as you don't hit the solid end -Yury
#ifdef DEBUG
                                               REAL area = mesh.Facet(facetIndex)->Area();
                                               if (area <= 1e-20) {
//    		printf("f %d; small area %lg\n", facetIndex, area);
                                               printf("line %d\n", lineNumber);
                                               for (INT kk = 0; kk < 3; kk++) {
                                                 cPOINT3 vtxPoint = mesh.Vertex(meshVertices[kk])->Point();
                                                 printf("vertex %lf\t%lf\t%lf\n", vtxPoint[0], vtxPoint[1], vtxPoint[2]);
                                               }
                                               ASSERT(false);
                                             }
#endif //DEBUG
          if (facetIndex != INVALID_IFACET && validFacetGroup) {
            //Add facet to facetGroup.
            cFACET_GROUP *facetGroup = mesh.GetFacetGroup(solidName);
            facetGroup->m_facets.push_back(facetIndex);
          }
        }

        verticesInFacet = 0;
      } else if (strncasecmp(trimmedBuffer, "vertex", 6) == 0) {
        sscanf(trimmedBuffer + 6, "%lg %lg %lg", &vertexCoords[0], &vertexCoords[1], &vertexCoords[2]);

        cPOINT3 vertexPoint(vertexCoords[0], vertexCoords[1], vertexCoords[2]);
        std::map<cPOINT3, iVERTEX>::iterator pos = pointToVertexMap.find(vertexPoint);

        if (pos == pointToVertexMap.end()) {
          iVERTEX vertexIndex = mesh.NewVertex(vertexPoint);  //vertexCoords[0], vertexCoords[1], vertexCoords[2]);
          pointToVertexMap[vertexPoint] = vertexIndex;
          meshVertices[verticesInFacet++] = vertexIndex;
        } else {
          meshVertices[verticesInFacet++] = pos->second;
        }

        numVertices++;
      }

    }  //End of reading file.

//remove degenerate faces
    cSURFACE_MESH_CLEANUP cleanup(&mesh, 0.0);
    cleanup.Perform();
    if (mesh.NumVertices() == 0 && mesh.NumFacets() == 0)
      return false;

    return true;
  }

  BOOL ImportStl(FILE *file, cGENERIC_MODEL &model) {
    if (file == NULL)
      return false;

    char buffer[1024];
    INT lineNumber = 0;  //for debugging
    char *trimmedBuffer = NULL;
    REAL vertexCoords[3];
    char solidName[128];
    INT currSolidIndex = 0, verticesInFacet = 0, numVertices = 0;
    INT meshVertices[3];
    BOOL validFacetGroup = false;

    std::map<cPOINT3, iVERTEX> pointToVertexMap;

    cGENERIC_MODEL::cMESH *mesh = model.CreateMesh();

    while (fgets(buffer, 1024, file) != NULL) {
      lineNumber++;
      trimmedBuffer = RemoveLeadingWhiteSpace(buffer);

      if (strncasecmp(trimmedBuffer, "solid", 5) == 0) {
        currSolidIndex++;
        memset(solidName, '\0', 128);
        char *solidNamePtr = trimmedBuffer + 6;
        sscanf(solidNamePtr, "%127s", solidName);

//Create a group in model
        cGENERIC_MODEL::cGROUP *group = model.CreateGroup();
        group->SetName(solidName);
        validFacetGroup = true;
        //register facetGroup.
      } else if (strncasecmp(trimmedBuffer, "endsolid", 8) == 0) {
        validFacetGroup = false;
      } else if (strncasecmp(trimmedBuffer, "endfacet", 8) == 0) {
        if (verticesInFacet == 3) {
          iFACET facetIndex = mesh->NewTriangle(meshVertices[0], meshVertices[1], meshVertices[2]);  //this is inefficient - keep the facetGroup for as long as you don't hit the solid end -Yury
        }
        verticesInFacet = 0;
      } else if (strncasecmp(trimmedBuffer, "vertex", 6) == 0) {
        sscanf(trimmedBuffer + 6, "%lg %lg %lg", &vertexCoords[0], &vertexCoords[1], &vertexCoords[2]);

        cPOINT3 vertexPoint(vertexCoords[0], vertexCoords[1], vertexCoords[2]);
        std::map<cPOINT3, iVERTEX>::iterator pos = pointToVertexMap.find(vertexPoint);

        if (pos == pointToVertexMap.end()) {
          iVERTEX vertexIndex = mesh->NewVertex(vertexPoint);  //vertexCoords[0], vertexCoords[1], vertexCoords[2]);
          pointToVertexMap[vertexPoint] = vertexIndex;
          meshVertices[verticesInFacet++] = vertexIndex;
        } else {
          meshVertices[verticesInFacet++] = pos->second;
        }

        numVertices++;
      }

    }  //End of reading file.

    return true;
  }

}

