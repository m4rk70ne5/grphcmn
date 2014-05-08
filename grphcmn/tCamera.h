#ifndef TCAMERA_H
#define TCAMERA_H

#include "tObject.h"
#include "tQuat.h"
#include "CollisionTypes.h"
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <vector>

class tGeometry;

class tCamera : public tObject
{
	friend class tBSPTree;
protected:
	glm::fquat m_orientation;
	glm::vec3 m_position, m_upVector, m_view;
	glm::vec3 m_originalView, m_originalUp;
	float m_speed, m_radius;
	// uniform info
	bool m_Block;
	int m_blockUBO;
	std::vector<tGeometry*> m_collidableGeo;
	glm::vec3 DetectCollision(glm::vec3, glm::vec3, tCollisionData&);
public:
	bool m_dtForward, m_dtBack; // back and forward
	bool m_dtLeft, m_dtRight; // left/right movement
	int m_dx, m_dy; // mouse movements change
	glm::mat4 m_transformationMat;
	tCamera(tVecf pos, tVecf up, tVecf view, float speed, bool = false, float = 2.0f);
	glm::vec3 GetPosition() { return m_position; };
	void SetCamPosition(glm::vec3); // DEBUG
	void SetCamUniform(int, std::string);
	void SetUBO(int ubo) { m_blockUBO = ubo; };
	void AddCollidable(tGeometry* pGeometry) { m_collidableGeo.push_back(pGeometry); };
	virtual void Update(); // translate deltas to changes in position and orientation
	virtual void Handler(int, bool); // deal with key presses
	virtual void MouseHandler(int, int, const int&, const int&);
};

#endif