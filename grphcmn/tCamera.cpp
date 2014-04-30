#include "tCamera.h"
#include <SDL2/SDL.h>
#include <gl/glew.h>
#include <gl/wglew.h>
#include <glm/gtc/type_ptr.hpp>

tCamera::tCamera(tVecf pos, tVecf up, tVecf view, float speed, bool block) : m_speed(speed), m_Block(block)
{
	m_dx = 0;
	m_dy = 0;
	m_dtLeft = m_dtRight = m_dtForward = m_dtBack = false;
	m_position = glm::vec3(pos.x, pos.y, pos.z);
	m_upVector = m_originalUp = glm::vec3(up.x, up.y, up.z);
	m_view = m_originalView = glm::vec3(view.x, view.y, view.z);
}

void tCamera::Update()
{
	const Uint8 *keyState = SDL_GetKeyboardState(NULL);
	m_dtForward = (keyState[SDL_SCANCODE_UP]);
	m_dtBack = (keyState[SDL_SCANCODE_DOWN]);
	m_dtLeft = (keyState[SDL_SCANCODE_LEFT]);
	m_dtRight = (keyState[SDL_SCANCODE_RIGHT]);

	// if no movement at all, return
	if (!(m_dtForward + m_dtBack + m_dtLeft + m_dtRight) && m_dx == 0 && m_dy == 0)
	{
		return;
	}
	
	// tVecf viewVec = m_lookAt - m_position; // calculate view
	glm::vec3 strafeVec;
	// tVecf strafeVec = tVecf(1.0f, 0.0f, 0.0f); // calculate strafe
	glm::mat4 rotMat;

	if (m_dx != 0 || m_dy != 0)
	{
		// do what you will with mouse movements
		double rotAroundY = m_dx / 500.0; // angle around world's Y axis, scale down angle
		double rotAroundStrafe = m_dy / 500.0; // up and down rotations about model's X axis

		if (m_dy != 0)
		{
			// rotate around local x axis
			glm::dvec3 axis(1.0f, 0.0f, 0.0f);
			glm::fquat xQuat = glm::fquat(glm::angleAxis(glm::degrees(-rotAroundStrafe), axis));
			m_orientation = glm::normalize(m_orientation * xQuat);
		}
		if (m_dx != 0)
		{
			// rotate around world y axis
			glm::dvec3 axis(0.0f, 1.0f, 0.0f);
			glm::fquat yQuat = glm::fquat(glm::angleAxis(glm::degrees(-rotAroundY), axis));
			m_orientation = glm::normalize(yQuat * m_orientation);
		}
		// recalculate view vector
		glm::fquat rotateView = m_orientation * (glm::fquat(0.0f, m_originalView) * glm::conjugate(m_orientation));
		m_view = glm::normalize(glm::axis(rotateView));
		// recalculate up vector
		glm::fquat rotateUp = m_orientation * (glm::fquat(0.0f, m_originalUp) * glm::conjugate(m_orientation));
		m_upVector = glm::normalize(glm::axis(rotateUp));
	}

	// left/right movement
	if (m_dtLeft || m_dtRight)
	{
		strafeVec = glm::cross(m_view, m_upVector); // calculate strafe
		// left
		if (m_dtLeft)
		{
			m_position -= strafeVec * m_speed;
		}
		// right
		else if (m_dtRight)
		{
			m_position += strafeVec * m_speed;
		}
	}
	// forward/backward
	if (m_dtForward || m_dtBack)
	{
		// move forward
		if (m_dtForward)
		{
			m_position += m_view * m_speed;
		}

		// move backward
		else if (m_dtBack)
		{	
			m_position -= m_view * m_speed;
		}
	}

	// set translation components of camTranslationMat
	// inverted camera positions
	glm::mat4 transMat;
	transMat[3] = glm::vec4(-m_position.x, -m_position.y, -m_position.z, 1.0f);
	// and set the camera translation matrix
	// the inverse of the m_orientation quaternion
	rotMat = glm::mat4_cast(glm::inverse(m_orientation));
	m_transformationMat = rotMat * transMat; // inverse order
}

void tCamera::Handler(int key, bool on)
{
	m_dtForward = (key == SDLK_UP && on);
	m_dtBack = (key == SDLK_DOWN && on);
	m_dtLeft = (key == SDLK_LEFT && on);
	m_dtRight = (key == SDLK_RIGHT && on);
}

void tCamera::MouseHandler(int x, int y, const int& maxX, const int& maxY)
{
	int centerX = maxX / 2;
	int centerY = maxY / 2;

	m_dx = x - centerX;
	m_dy = y - centerY;

	SDL_WarpMouseInWindow(NULL, centerX, centerY);
}

void tCamera::SetCamUniform(int prog, std::string name)
{
	// set the camera matrix
	if (!m_Block)
	{
		GLint camLoc = glGetUniformLocation(prog, name.c_str());
		glProgramUniformMatrix4fv(prog, camLoc, 1, GL_FALSE, glm::value_ptr(m_transformationMat));
	}
	else
	{
		glBindBuffer(GL_UNIFORM_BUFFER, m_blockUBO);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(m_transformationMat));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
}
