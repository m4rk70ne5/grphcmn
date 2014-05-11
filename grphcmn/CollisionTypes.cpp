#include "CollisionTypes.h"

float DistanceToPlane(tPlane plane, glm::vec3 point)
{
	return glm::dot(plane.m_normal, point) - plane.m_distance;
}

float IntersectRayPlane(tPlane plane, glm::vec3 start, glm::vec3 end)
{
	glm::vec3 vel = end - start;
	float denom = glm::dot(plane.m_normal, vel);
	if (denom > 1e-6)
	{
		float numerator = plane.m_distance - glm::dot(start, plane.m_normal);
		return (numerator / denom);
	}
	else
	{
		// doesn't intersect
		return 1.0f;
	}
}

glm::vec3 ClosestPointOnLine(const glm::vec3& start, const glm::vec3& end, const glm::vec3& point)
{
	float velMag = glm::length(end - start);
	glm::vec3 d = (end - start) / velMag; // normalize
	float t = glm::dot(d, (point - start));
	// clamp it so that it falls within the line
	// if t is +, make it the end point that is the closest
	// else, make it the start point
	t = glm::clamp(t, 0.0f, velMag);
	return (start + (t * d));
}