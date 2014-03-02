#include "tMeshReader.h"
#include <vector>
#include <sstream>
#include <cstring>

using namespace std;

tMeshReader::tMeshReader(string name) : m_file(name)
{
}

string tMeshReader::ReadNextLine(ifstream* pfile)
{
	char buf[256];
	do 
	{
		pfile->getline(buf, 256);
	} while (buf[0] == '#' && !pfile->fail());
	return string(buf);
}

int tMeshReader::ReadVertices(tVert*& vertices)
{
	// vector of vertices
	vector<tVert> vv;
	// open the file
	ifstream infile;
	infile.open(m_file.c_str());

	string s;
	istringstream iss;
	do
	{
		s = ReadNextLine(&infile);
		if (isdigit(s[0]) || s[0] == '-')
		{
            istringstream iss(s);
            tVert v;
            for (int i = 0; i < 4; i++)
            {
                iss >> v.m_data[i];
            }
            vv.push_back(v);
		}
	} while (s.compare("----"));
	// close the file
	infile.close();
	// copy it all over
	int numVert = vv.size();
	vertices = new tVert[numVert];
	memcpy(vertices, &vv[0], sizeof(tVert) * numVert);
	return numVert;
}

int tMeshReader::ReadTriangles(tTriangle*& triangles)
{
	vector<tTriangle> vt;
	// open the file
	ifstream infile;
	infile.open(m_file.c_str());

	char buf[255];
	string s;
	do
	{
		infile.getline(buf, 255);
		s = string(buf);
	} while (s.compare("----"));

	istringstream iss;
	do
	{
		s = ReadNextLine(&infile);
		if (isdigit(s[0]))
		{
            istringstream iss(s);
            tTriangle t;
            for (int i = 0; i < 3; i++)
            {
                iss >> t.m_indices[i];
            }
            vt.push_back(t);
		}
	} while (!infile.eof());
	// close the file
	infile.close();
	// copy it all over
	int numTri = vt.size();
	triangles = new tTriangle[numTri];
	memcpy(triangles, &vt[0], sizeof(tTriangle) * numTri);
	return numTri;
}

void tMeshReader::LoadMesh(tMesh& mesh)
{
	mesh.m_numVertices = ReadVertices(mesh.m_vertices);
	mesh.m_numTriangles = ReadTriangles(mesh.m_triangles);
}