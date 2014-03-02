#ifndef TCAMERA_H
#define TCAMERA_H

#include "tObject.h"
#include "tQuat.h"
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>

class tCamera : public tObject
{
protected:
	glm::fquat m_orientation;
	glm::vec3 m_position, m_upVector, m_view;
	glm::vec3 m_originalView, m_originalUp;
	float m_speed;
public:
	bool m_dtForward, m_dtBack; // back and forward
	bool m_dtLeft, m_dtRight; // left/right movement
	int m_dx, m_dy; // mouse movements change
	glm::mat4 m_transformationMat;
	tCamera(tVecf pos, tVecf up, tVecf view, float speed);
	virtual void Update(); // translate deltas to changes in position and orientation
	virtual void Handler(int, bool); // deal with key presses
	virtual void MouseHandler(int, int, const int&, const int&);
};

#endif