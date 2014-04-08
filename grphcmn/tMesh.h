#ifndef TMESH_H
#define TMESH_H

#include "tGeometry.h"
#include "tVector.h"
#include "tObject.h"
#include "tQuat.h"
#include "tMatrix.h"
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

class tBSPFile; // forward

struct tVert
{
	float m_data[4];
};

struct tTriangle
{
	unsigned int m_indices[3];
};

// generic mesh class
class tMesh : public tGeometry
{
friend class tMeshReader;
protected:
	tVert* m_vertices;
	int m_numVertices;
	virtual void Init();
	static bool m_dataDeleted;
public:
	tMesh(bool, int&, tMeshReader&);
	virtual ~tMesh();
	int m_numTriangles;
	tTriangle* m_triangles;
	virtual int GetNumIndices() { return m_numTriangles * 3; };
};

class tCubeMesh : public tMesh, public tObject
{
	friend class tSpinningBoxDemo;
protected:
	tVecf m_colors[6];
	tVecf m_distance;
	tVecf m_axis;
	glm::fquat m_offset, m_orientation;
	double m_rotOffset, m_totalRot;
	glm::mat4 m_worldMat;
	glm::mat4 m_translationMat;
	static bool m_dataLoaded;
	float m_angle;
public:
	tCubeMesh(tVecf, double, tVecf, bool, int&, tMeshReader&);
	void RandomFaceColors();
	static tVecf RandomVector(float, float, unsigned int = 0);
	void SetDistance(tVecf distance);
	virtual void Update(); // just do rotations
	virtual void SetUniforms(int);
	virtual void Draw(int);
};

#endif