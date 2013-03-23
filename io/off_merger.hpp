/*
 * off_merger.hpp
 *
 *  Created on: Feb 1, 2011
 */

#ifndef OFF_MERGER_HPP_
#define OFF_MERGER_HPP_


#include "defs.hpp"
#include "off_reader.hpp"

namespace io {

class cOFF_MERGER{

public:
	cOFF_MERGER();
	cOFF_MERGER(FILE *offBase, FILE *mapBase);
	VOID Append(FILE *offAdd, FILE *mapAdd);
	VOID Export();
	VOID Import();
	VOID ExportFiles(FILE *offFile, FILE *mapFile);

private:
	VOID ImportBase() { m_baseReader.Import();}
	VOID ImportAdd() { m_addReader.Import();}

private:
	FILE *m_offBase;
	FILE *m_offAdd;
	FILE *m_mapBase;
	FILE *m_mapAdd;
	FILE *m_offExport;
	FILE *m_mapExport;
	cOFF_READER m_baseReader;
	cOFF_READER m_addReader;
};

}//namespace
#endif /* OFF_MERGER_HPP_ */
