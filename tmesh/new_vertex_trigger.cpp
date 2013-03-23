/*
 * new_vertex_trigger.cpp
 *
 *  Created on: Aug 4, 2011
 */

#include "new_vertex_trigger.hpp"
#include <string.h>
namespace tmesh {

  INT cNEW_VERTEX_TRIGGER::ReadVertexIndices(const char *filename)
  {
//	FILE *file = fopen(m_VertexIndicesFilePath.c_str(), "rb+");
	FILE *file = fopen(filename, "rb+");
	if (file == NULL)
	  return -1;
	char buffer[1024];
	INT lineNumber = 0; //for debugging
	INT readingLevel = 0;

	while (fgets(buffer, 1024, file) != NULL){
	  lineNumber++;
	  if (readingLevel == 0 && buffer[0] == '(')
		readingLevel = 1;
	  else if (readingLevel == 1){
		if (buffer[0] == ')')
		  readingLevel = 0;
		else {
		  iVERTEX vertexIndex;
		  sscanf(buffer, "%d", &vertexIndex);
		  m_vertexIndices.push_back(vertexIndex);
		}
	  }
	}

	fclose(file);
	file = NULL;

	//sort the array
	BOOL swapped = false;

	do {
	  swapped = false;
	  for (INT i=1; i < (INT)m_vertexIndices.size(); i++){
		if (m_vertexIndices[i-1] > m_vertexIndices[i]){
		  iVERTEX v = m_vertexIndices[i-1];
		  m_vertexIndices[i-1] = m_vertexIndices[i];
		  m_vertexIndices[i] = v;
		  swapped = true;
		}
	  }
	} while (swapped);

	return (INT)m_vertexIndices.size();
  }

  INT cNEW_VERTEX_TRIGGER::ReadVertexInfos(const char *filename)
  {
  	FILE *file = fopen(filename, "rb+");
  	if (file == NULL)
  	  return -1;
  	char buffer[1024];
  	INT lineNumber = 0; //for debugging
  	INT readingLevel = 0;
  	INT vertexIndexPos = 0;
	INT numVertices = -1;

	while (fgets(buffer, 1024, file) != NULL){
	  lineNumber++;
	  if (readingLevel == 0 && buffer[0] == '/' && buffer[1] == '/')
		readingLevel = 1;
	  else if (readingLevel == 1){
		sscanf(buffer, "%d", &numVertices);
		readingLevel = 2;
	  }
	  else if (readingLevel == 2){
		readingLevel = 3;
	  }
	  else if (readingLevel == 3){
		if (buffer[0] == ')')
		  break;
		char *buf = buffer+1;
		INT outputIndex = -1;
		sscanf(buf, "%d", &outputIndex);
		if (outputIndex == m_vertexIndices[vertexIndexPos]){
		  sVERTEX_INFO vertexInfo;
		  vertexInfo.outputIndex = outputIndex;
		  REAL vertexCoords[3];
		  buf = strchr(buf, 'G');
		  buf++;
		  sscanf(buf, "%d", &vertexInfo.meshVertexIndex);
		  buf = strchr(buf, '(');
		  buf++;
		  sscanf(buf, "%lg %lg %lg",
				vertexCoords, vertexCoords+1, vertexCoords+2);
		  cPOINT3 vertexPoint(vertexCoords[0], vertexCoords[1], vertexCoords[2]);
		  vertexInfo.vertexPoint = vertexPoint;
		  m_vertexInfoTable.push_back(vertexInfo);
		  vertexIndexPos++;
		}
	  }
	}

	fclose(file);
	file = NULL;

	//sort the array
  	BOOL swapped = false;

  	do {
  	  swapped = false;
  	  for (INT i=1; i < (INT)m_vertexInfoTable.size(); i++){
  		if (m_vertexInfoTable[i-1].meshVertexIndex > m_vertexInfoTable[i].meshVertexIndex){
  		  sVERTEX_INFO v = m_vertexInfoTable[i-1];
  		  m_vertexInfoTable[i-1] = m_vertexInfoTable[i];
  		  m_vertexInfoTable[i] = v;
  		  swapped = true;
  		}
  	  }
  	} while (swapped);
/*
  	for (INT i=0; i < (INT)m_vertexInfoTable.size(); i++)
  	  printf("%d:\tg %d\tv %d\t%lf %lf %lf\n",
  			  i, m_vertexInfoTable[i].meshVertexIndex,
  			  m_vertexInfoTable[i].outputIndex,
  			  m_vertexInfoTable[i].vertexPoint[0],
  			  m_vertexInfoTable[i].vertexPoint[1],
  			  m_vertexInfoTable[i].vertexPoint[2]);
  			  */
  	return (INT)m_vertexInfoTable.size();
  }

  BOOL cNEW_VERTEX_TRIGGER::DoTrigger(iVERTEX newVertexIndex)
  {
	ASSERT(newVertexIndex <= m_vertexInfoTable[m_currVertexInfoIndex].meshVertexIndex);
	if (newVertexIndex == m_vertexInfoTable[m_currVertexInfoIndex].meshVertexIndex){
		m_currVertexInfoIndex++;
		return true;
	}
	else
	  return false;
  }

}//namespace
