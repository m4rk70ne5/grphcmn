#include "tMesh.h"
#include "tMeshReader.h"
#include "Utility.h"
#include <random>
#include <array>
#include <time.h>
#include <math.h>
#include <gl/glew.h>
#include <gl/wglew.h>

bool tCubeMesh::m_dataLoaded = false;
bool tMesh::m_dataDeleted = false;

tMesh::tMesh(bool batch, int& vaoId, tMeshReader& mr) : tGeometry(batch, vaoId)
{
	// load data
	mr.LoadMesh(*this);

	// create buffers
	glBindVertexArray(vaoId);
	
	Init();
	
	glBindVertexArray(0); // all done with init, unbind
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
	RandomFaceColors();
	m_dataLoaded = true;
}

void tCubeMesh::RandomFaceColors()
{
	using namespace std;
	string seed_string = "Spinning Cubes";
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
	
	if (seed_val == 0)
	{
		seed_val = time(NULL);
	}
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
}