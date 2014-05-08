#ifndef TGEOMETRY_H
#define TGEOMETRY_H

#include <gl/glew.h>
#include <gl/wglew.h>
#include "CollisionTypes.h"

struct tVert;

class tGeometry
{
protected:
	tGeometry(bool, int&);
	virtual void Init() {};
	GLuint m_vertexBuffer, m_indexBuffer, m_textureBuffer;
public:
	bool m_aaBB; // indicates whether or not this geometry has axis-aligned bounding box
	virtual ~tGeometry() {};
	virtual int GetNumIndices() {return 0;};
	virtual void SetUniforms(int) {};
	virtual void Draw(int);
	virtual void RecordCollisionPoint(glm::vec3, glm::vec3, tCollisionData&) {};
};

#endif