#include "tMesh.h"
#include "tMeshReader.h"
#include "Utility.h"
#include "tModeToggler.h"
#include "tProgramManager.h"
#include <random>
#include <array>
#include <time.h>
#include <math.h>
#include <gl/glew.h>
#include <gl/wglew.h>

bool tCubeMesh::m_dataLoaded = false;
bool tMesh::m_dataDeleted = false;
GLuint tBoundingBox::m_vaoId = 0;

tMesh::tMesh(bool batch, int& vaoId, tMeshReader& mr) : tGeometry(batch, vaoId), m_boundingBox(vaoId)
{
	// load data
	mr.LoadMesh(*this);
	
	// create buffers
	glBindVertexArray(vaoId);
	
	Init();
	
	glBindVertexArray(0); // all done with init, unbind

	// defaults
	m_aaBB = false;
}

tMesh::~tMesh()
{
	// de-allocate vertices and indices
	if (!m_dataDeleted)
	{
		delete m_vertices;
		delete m_triangles;
		m_dataDeleted = true;
	}
}

void tMesh::Init()
{
	// vertex buffer
	glGenBuffers(1, &m_vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, m_numVertices * sizeof(tVert), m_vertices, GL_STATIC_DRAW);

	// index buffer
	glGenBuffers(1, &m_indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_numTriangles * sizeof(tTriangle), m_triangles, GL_STATIC_DRAW);

	// vertex attribute array
	glEnableVertexAttribArray(0); // 0 will always be position
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(tVert), 0);
}

void tMesh::CreateBoundingBox(int& vaoId, bool createNew, std::string prog)
{
	m_boundingBox.SetProgram(prog);
	m_boundingBox.SetExtents(m_vertices, m_numVertices);
	m_boundingBox.m_faces[0] = tBoundingBox::CreateFaceFromExtents(m_boundingBox.m_max, m_boundingBox.m_min, glm::vec3(0, 0, 1));
	m_boundingBox.m_faces[1] = tBoundingBox::CreateFaceFromExtents(m_boundingBox.m_max, m_boundingBox.m_min, glm::vec3(0, 0, -1));
	m_boundingBox.m_faces[2] = tBoundingBox::CreateFaceFromExtents(m_boundingBox.m_max, m_boundingBox.m_min, glm::vec3(0, 1, 0));
	m_boundingBox.m_faces[3] = tBoundingBox::CreateFaceFromExtents(m_boundingBox.m_max, m_boundingBox.m_min, glm::vec3(0, -1, 0));
	m_aaBB = true;

	// geometry
	// vao
	if (createNew)
	{
		glGenVertexArrays(1, (GLuint*)&vaoId);
		m_boundingBox.m_vaoId = vaoId;
	}
}

tCubeMesh::tCubeMesh(tVecf distance, double rot, tVecf axis, bool batch, int& vaoId, tMeshReader& mr) : m_distance(distance), 
	tMesh(batch, vaoId, mr), m_rotOffset(rot), m_axis(axis)
{
	// calculate offset based off of rotation angle and axis
	m_totalRot = 0.0f;
	m_angle = 0.0f;

	// translation matrix
	SetDistance(distance);

	axis.normalize();
	m_offset = glm::fquat(glm::angleAxis(glm::degrees(rot), glm::dvec3(axis.x, axis.y, axis.z)));
	// now set colors
	RandomFaceColors((int)(distance.x + distance.y));
	m_dataLoaded = true;
}

void tCubeMesh::RandomFaceColors(int offset)
{
	using namespace std;
	string seed_string = "Spinning Cubes" + static_cast<char>(offset);
	seed_seq seed(seed_string.begin(), seed_string.end());
	array<unsigned int, 6> sequence;
	seed.generate(sequence.begin(), sequence.end()); // crete a sequence of 6 unique seed values
	for (int i = 0; i < 6; i++)
	{
		m_colors[i] = RandomVector(0.5f, 1.0f, sequence[i]);
	}
}

void tCubeMesh::SetUniforms(int prog)
{
	// at the time of this call, the correct program should be "in use"
	// world matrix
	GLint worldLoc = glGetUniformLocation(prog, "world");
	glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(m_worldMat));
}

void tCubeMesh::Update()
{
	// increase rotation quaternion by amount
	m_orientation = glm::normalize(m_orientation * m_offset);
	
	// convert to world matrix
	m_worldMat =  m_translationMat * glm::mat4_cast(m_orientation); // translate, and then rotate
}

tVecf tCubeMesh::RandomVector(float min, float max, unsigned int seed_val)
{
	tVecf vec;

	// finds a position for the cube, between min and max
	using namespace std;
	
	// set up random number generator
	default_random_engine randGen;
	uniform_real_distribution<float> distribution(min, max);
	
	unsigned int seed_offset = seed_val;
	seed_val = time(NULL);
	seed_val += seed_offset;
	randGen.seed(seed_val);
	// generate numbers
	vec.x = distribution(randGen);
	vec.y = distribution(randGen);
	vec.z = distribution(randGen);
	vec.w = 1.0f;

	return vec;
}

void tCubeMesh::Draw(int prog)
{
	// cycle through faces
	float colors[4];
	GLint colorLoc = glGetUniformLocation(prog, "cubeColor");
	for (int i = 0; i < 6; i++)
	{
		// set the color uniform according to the face index
		m_colors[i].GetData(colors);
		glUniform4fv(colorLoc, 1, colors);
		// draw six indices at a time
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (GLvoid*)(sizeof(tTriangle) * 2 * i));
	}
}

void tCubeMesh::SetDistance(tVecf distance) 
{ 
	// turns the distance into a translation matrix
	glm::vec3 a(distance.x, distance.y, distance.z);
	m_translationMat[3] = glm::vec4(a, 1.0f);
	if (m_aaBB)
	{
		m_boundingBox.SetDistance(glm::vec3(distance.x, distance.y, distance.z));
	}
}

tBoundingBox::tBoundingBox(int& vaoId) : tGeometry(false, vaoId)
{
}

void tBoundingBox::SetExtents(tVert* vertices, int numVertices)
{
	// cycle through all the coordinates
	// compute max and min
	for (int i = 0; i < numVertices; i++)
	{
		// max
		m_max.x = max(m_max.x, vertices[i].m_data[0]);
		m_max.y = max(m_max.y, vertices[i].m_data[1]);
		m_max.z = max(m_max.z, vertices[i].m_data[2]);
		// min
		m_min.x = min(m_min.x, vertices[i].m_data[0]);
		m_min.y = min(m_min.y, vertices[i].m_data[1]);
		m_min.z = min(m_min.z, vertices[i].m_data[2]);
	}
}

void tBoundingBox::Draw(int prog)
{
	// if bounding box drawing mode is enabled...
	tModeToggler* pMT = tModeToggler::GetModeToggler();
	if (pMT->m_bbDraw && m_aaBB)
	{
		// ignore prog paramater
		// use your own
		int numRects = 4;
		int numPoints = 4;
		// cycle through the points
		for (int i = 0; i < 4; i++)
		{
			glDrawArrays(GL_LINE_LOOP, (m_offset * numRects * numPoints) + (i * 4), 4); // send four points
		}
	}
}

tRect tBoundingBox::CreateFaceFromExtents(glm::vec3 max, glm::vec3 min, glm::vec3 face)
{
	glm::vec3 filter = -glm::abs(face) + glm::vec3(1,1,1);
	face = ((face.x + face.y + face.z) > 0) ? glm::vec3(face.x * max.x, face.y * max.y, face.z * max.z) :
												glm::vec3(-face.x * min.x, -face.y * min.y, -face.z * min.z);
	glm::vec3 point1, point2, point3, point4;
	point2 = point1 = glm::vec3(max.x * filter.x, max.y * filter.y, max.z * filter.z) + face;
	point3 = point4 = glm::vec3(min.x * filter.x, min.y * filter.y, min.z * filter.z) + face;

	if (filter.x) 
	{ 
		float temp = point2.x;
		point2.x = point4.x;
		point4.x = temp;
	}
	else if (filter.y) 
	{ 
		float temp = point2.y;
		point2.y = point4.y;
		point4.y = temp;
	}
	else if (filter.z) 
	{ 
		float temp = point2.z;
		point2.z = point4.z;
		point4.z = temp;
	}
	tRect r(point1, point2, point3, point4);
	return r;
}

void tBoundingBox::SetProgram(std::string program)
{
	tProgramManager* pPM = tProgramManager::GetProgramManager();
	m_programID = pPM->GetProgram(program);
}

void tBoundingBox::SetDistance(glm::vec3 dist)
{
	// turns the distance into a translation matrix
	glm::vec3 a(dist.x, dist.y, dist.z);
	m_worldMat[3] = glm::vec4(a, 1.0f);
	// set uniform
	GLint worldLoc = glGetUniformLocation(m_programID, "world");
	glProgramUniformMatrix4fv(m_programID, worldLoc, 1, GL_FALSE, glm::value_ptr(m_worldMat));
}

void tBoundingBox::SetUniforms(int prog)
{
	// set uniform
	GLint worldLoc = glGetUniformLocation(m_programID, "world");
	glProgramUniformMatrix4fv(m_programID, worldLoc, 1, GL_FALSE, glm::value_ptr(m_worldMat));
	
	// set color
	GLint colorLoc = glGetUniformLocation(prog, "inColor");
	glUniform3fv(colorLoc, 1, glm::value_ptr(glm::vec3(0.7, 0.7, 1.0)));
}

glm::vec3 tBoundingBox::ComputeMax(glm::vec3& inMax)
{
	glm::vec4 vecmax = m_worldMat * glm::vec4(m_max, 1.0f);
	float xMax = max(inMax.x, vecmax.x);
	float yMax = max(inMax.y, vecmax.y);
	float zMax = max(inMax.z, vecmax.z);
	return glm::vec3(xMax, yMax, zMax);
}

glm::vec3 tBoundingBox::ComputeMin(glm::vec3& inMin)
{
	glm::vec4 vecmin = m_worldMat * glm::vec4(m_min, 1.0f);
	float xMin = min(inMin.x, vecmin.x);
	float yMin = min(inMin.y, vecmin.y);
	float zMin = min(inMin.z, vecmin.z);
	return glm::vec3(xMin, yMin, zMin);
}