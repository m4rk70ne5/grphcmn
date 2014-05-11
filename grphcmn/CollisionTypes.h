#ifndef COLLISIONTYPES_H
#define COLLISIONTYPES_H

#include <glm/vec3.hpp>
#include <glm/geometric.hpp>

struct tPlane
{
	// this plane equation is dot(point, normal) = d
	// or ax + by + cz = d
	float m_distance; // distance from origin
	glm::vec3 m_point;
	glm::vec3 m_normal;
	tPlane(glm::vec3 n, glm::vec3 p) : m_normal(n), m_point(p)
	{
		m_distance = glm::dot(m_point, m_normal);
	}
	tPlane()
	{
		m_distance = 0.0;
		m_point = glm::vec3();
		m_normal = glm::vec3();
	}
};

struct tCollisionData
{
	float m_closestHit, m_offset;
	tPlane m_plane;
	glm::vec3 m_rayOrig; // the absolute start of the ray being tested
	glm::vec3 m_velocity; // velocity of the whole original ray (used to determine actual ratios)
	tCollisionData(glm::vec3 rayStart, glm::vec3 rayEnd, float closestHit = 1.0f, float offset = 1.0f) : m_rayOrig(rayStart), 
		m_closestHit(closestHit), m_offset(offset)
	{
		m_velocity = rayEnd - rayStart;
	}
};

float DistanceToPlane(tPlane, glm::vec3);
float IntersectRayPlane(tPlane, glm::vec3, glm::vec3);
glm::vec3 ClosestPointOnLine(const glm::vec3&, const glm::vec3&, const glm::vec3&);

#endif