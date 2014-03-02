#ifndef TGEOMETRY_H
#define TGEOMETRY_H

#include <gl/glew.h>
#include <gl/wglew.h>

struct tVert;

class tGeometry
{
protected:
	tGeometry(bool, int&);
	virtual void Init() {};
	GLuint m_vertexBuffer, m_indexBuffer;
public:
	virtual ~tGeometry() {};
	virtual int GetNumIndices() {return 0;};
	virtual void SetUniforms(int) {};
	virtual void Draw(int);
};

#endif