#ifndef TMESHREADER_H
#define TMESHREADER_H

#include <string>
#include <fstream>
#include "tMesh.h"

class tMeshReader
{
protected:
	std::string m_file;
	std::string ReadNextLine(std::ifstream*);
public:
	tMeshReader(std::string);
	int ReadVertices(tVert*&);
	int ReadTriangles(tTriangle*&);
	void LoadMesh(tMesh&);
};

#endif