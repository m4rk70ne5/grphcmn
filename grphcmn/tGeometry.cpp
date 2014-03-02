#include "tGeometry.h"

tGeometry::tGeometry(bool newbatch, int& vao_id)
{
	if (newbatch)
	{
		glGenVertexArrays(1, (GLuint*)&vao_id);
	}
	glBindVertexArray(vao_id);
	
	Init();
	
	glBindVertexArray(0); // all done with init, unbind
}

void tGeometry::Draw(int prog)
{
	// the default is just to draw all according to the indices
	glDrawElements(GL_TRIANGLES, GetNumIndices(), GL_UNSIGNED_BYTE, (GLvoid*)0);
}