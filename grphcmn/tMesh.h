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
#include <string>

class tBSPFile; // forward

struct tVert
{
	float m_data[4];
};

struct tTriangle
{
	unsigned int m_indices[3];
};

struct tRect
{
	glm::vec3 m_points[4];
	tRect() { };
	tRect(glm::vec3 pt1, glm::vec3 pt2, glm::vec3 pt3, glm::vec3 pt4)
	{
		m_points[0] = pt1;
		m_points[1] = pt2;
		m_points[2] = pt3;
		m_points[3] = pt4;
	}
};

class tBoundingBox : public tGeometry
{
	friend class tMesh;
	friend class tOctree;
protected:
	glm::vec3 m_max, m_min;
	tRect m_faces[4];
	int m_programID;
	static tRect CreateFaceFromExtents(glm::vec3, glm::vec3, glm::vec3);
	int m_offset;
	glm::mat4 m_worldMat;
public:
	static GLuint m_vaoId;
	tBoundingBox(int&);
	void SetExtents(tVert*, int);
	virtual void Draw(int);
	virtual void SetUniforms(int);
	tRect* GetFaces() { return m_faces; }
	void SetOffset(int offset) { m_offset = offset; };
	void SetProgram(std::string);
	void SetDistance(glm::vec3);
	glm::vec3 ComputeMax(glm::vec3&);
	glm::vec3 ComputeMin(glm::vec3&);
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
	tBoundingBox m_boundingBox;
	virtual int GetNumIndices() { return m_numTriangles * 3; };
	void CreateBoundingBox(int&, bool, std::string);
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
	void RandomFaceColors(int = 0);
	static tVecf RandomVector(float, float, unsigned int = 0);
	void SetDistance(tVecf distance);
	virtual void Update(); // just do rotations
	virtual void SetUniforms(int);
	virtual void Draw(int);
};

#endif