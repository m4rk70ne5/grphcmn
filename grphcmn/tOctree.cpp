#include "tOctree.h"
#include "tModeToggler.h"
#include <math.h>
#include <map>
#include <algorithm>
#include <unordered_map>

tOctreeNode::tOctreeNode()
{
	for (int i = 0; i < 8; i++)
	{
		m_children[i] = NULL;
	}
	m_data.reserve(4);
}

tOctreeNode::~tOctreeNode()
{
	// traverse the tree and delete all nodes
	for (int i = 0; i < 8; i++)
	{
		if (m_children[i] != NULL)
		{
			delete m_children[i];
		}
	}
}

tOctree::tOctree(std::vector<tMesh*>& meshes, int& vao) : tGeometry(true, vao)
{
	glm::vec3 min, max;
	// find min and max of root box by cycling through all its bounding boxes
	for (auto i = meshes.begin(); i != meshes.end(); i++)
	{
		if ((*i)->m_aaBB)
		{
			// bounding box has been created
			tBoundingBox& bb = (*i)->m_boundingBox;
			// get max
			max = bb.ComputeMax(max);
			min = bb.ComputeMin(min);
		}
	}
	// offset the min and max so you don't accidentally overstep bounds
	min -= glm::vec3(5.0f, 5.0f, 5.0f);
	max += glm::vec3(5.0f, 5.0f, 5.0f);
	// set the root box's min and max
	SetExtents(&m_root, min, max);
	
	// now insert meshes
	for (auto i = meshes.begin(); i != meshes.end(); i++)
	{
		Insert(*i, &m_root);
	}
}

tOctree::~tOctree()
{
	// traverse the tree and delete all nodes
	for (int i = 0; i < 8; i++)
	{
		if (m_root.m_children[i] != NULL)
		{
			delete m_root.m_children[i];
		}
	}
}

void tOctree::Insert(tMesh* pMesh, tOctreeNode* parent)
{
	tBoundingBox& bb = pMesh->m_boundingBox;
	// does parent node have children?
	if (parent->HasChildren())
	{
		// if so, recurse in the appropriate child (the quadrant where the mesh lies in completely; if it spans n quadrants, 
		// store it in all quadrants by making n recursive calls
		std::vector<int> subDiv = CalculateSubDiv(parent, pMesh);
		for (auto sd = subDiv.begin(); sd != subDiv.end(); sd++)
		{
			Insert(pMesh, parent->m_children[*sd]);
		}
	}
	else
	{
	   // if not, store data in this parent node
	   // maximum of 5
	   if (parent->m_data.size() <= 4)
	   {
		   parent->m_data.push_back(pMesh);
	   }
	   else
	   {
			// if data already exists, subdivide the node and move old meshes and new child to subnodes
		   // subdivide
		   glm::vec3 extent, min_vec, max_vec;
		   bool px, py, pz; // 1 is for max, 0 is for min
		   for (int i = 0; i < 8; i++)
		   {
			   px = i & 0x0001;
			   py = i & 0x0002;
			   pz = i & 0x0004;
			   extent.x = px ? parent->m_max.x : parent->m_min.x;
			   extent.y = py ? parent->m_max.y : parent->m_min.y;
			   extent.z = pz ? parent->m_max.z : parent->m_min.z;
			   // compute min and max extent
			   max_vec.x = std::max(extent.x, parent->m_center.x);
			   max_vec.y = std::max(extent.y, parent->m_center.y);
			   max_vec.z = std::max(extent.z, parent->m_center.z);

			   min_vec.x = std::min(extent.x, parent->m_center.x);
			   min_vec.y = std::min(extent.y, parent->m_center.y);
			   min_vec.z = std::min(extent.z, parent->m_center.z);

			   // now create the node
			   parent->m_children[i] = new tOctreeNode();
			   SetExtents(parent->m_children[i], min_vec, max_vec);
		   }
		   // move
		   // now figure out where old and new meshes should go
		   while (!parent->m_data.empty())
		   {
				tMesh* pOld = parent->m_data.back();
				// figure out which sub-quadrant(s) this should go into
				std::vector<int> subDiv = CalculateSubDiv(parent, pOld);
				// recurse
				for (auto sd = subDiv.begin(); sd != subDiv.end(); sd++)
				{
					Insert(pOld, parent->m_children[*sd]);
				}
				parent->m_data.pop_back(); // <-
		   }
		   // figure out which sub-quadrant(s) this should go into
		   std::vector<int> subDiv = CalculateSubDiv(parent, pMesh);
		   // recurse
		   for (auto sd = subDiv.begin(); sd != subDiv.end(); sd++)
		   {
				Insert(pMesh, parent->m_children[*sd]);
		   }
	   }
	}
	return;
}

void tOctree::SetExtents(tOctreeNode* pNode, glm::vec3 min, glm::vec3 max)
{
	pNode->m_max = max;
	pNode->m_min = min;
	// calculate center
	// midpoint formula
	pNode->m_center = (max + min) * 0.5f;
	// calculate plane data (for collision detection)
	pNode->m_planes[0] = tPlane(glm::vec3(1.0f, 0.0f, 0.0f), pNode->m_center);
    pNode->m_planes[1] = tPlane(glm::vec3(0.0f, 1.0f, 0.0f), pNode->m_center);
    pNode->m_planes[2] = tPlane(glm::vec3(0.0f, 0.0f, 1.0f), pNode->m_center);
}

void tOctree::Draw(int prog)
{
	// if octree drawing mode is enabled...
	tModeToggler* pMT = tModeToggler::GetModeToggler();
	if (pMT->m_OctDraw)
	{
		// ignore prog parameter
		// use your own
		int numRects = 4;
		int numPoints = 4;
		// cycle through the points
		for (int i = 0; i < m_vertexData.size(); i += 4)
		{
			glDrawArrays(GL_LINE_LOOP, i, 4); // send four points
		}
	}
}

std::vector<int> tOctree::CalculateSubDiv(tOctreeNode* pNode, tMesh* pMesh)
{
	// go through three rectangles points
	tBoundingBox& pBB = pMesh->m_boundingBox;
	tRect* pRect = pBB.GetFaces();
	std::vector<int> childList;
	bool childBits[8];
	for (int c = 0; c < 8; c++)
	{
		childBits[c] = false;
	}
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			glm::vec4 testPoint = pBB.m_worldMat * glm::vec4(pRect[i].m_points[j], 1.0f);
			int quadrant = GetQuadrant(glm::vec3(testPoint), pNode->m_center);
			childBits[quadrant] = true;
		}
	}
	for (int d = 0; d < 8; d++)
	{
		if (childBits[d])
		{
			childList.push_back(d);
		}
	}
	return childList;
}

void tOctree::CreateVertexBuffers(int vao)
{
	// each octree node has an extent and a center...
	// use this info to calculate the eight points and the four rects
	AddPointsToBuffer(&m_root);
	// create the buffer
	glBindVertexArray(vao);
	// buffer operations
	// vertex buffer
	GLuint vertexBuffer;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, m_vertexData.size() * sizeof(glm::vec3), static_cast<GLvoid*>(&m_vertexData[0]), GL_STATIC_DRAW);
	// make last arg GL_STREAM_DRAW in the future
	// vertex attribute array
	glEnableVertexAttribArray(0); // 0 will always be position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glBindVertexArray(0);
}

void tOctree::AddPointsToBuffer(tOctreeNode* pNode)
{
	int uselessVao = 0;
	tBoundingBox temp(uselessVao);
	temp.m_faces[0] = tBoundingBox::CreateFaceFromExtents(pNode->m_max, pNode->m_min, glm::vec3(0, 0, 1));
	temp.m_faces[1] = tBoundingBox::CreateFaceFromExtents(pNode->m_max, pNode->m_min, glm::vec3(0, 0, -1));
	temp.m_faces[2] = tBoundingBox::CreateFaceFromExtents(pNode->m_max, pNode->m_min, glm::vec3(0, 1, 0));
	temp.m_faces[3] = tBoundingBox::CreateFaceFromExtents(pNode->m_max, pNode->m_min, glm::vec3(0, -1, 0));
	for (int j = 0; j < 4; j++)
	{
		for (int k = 0; k < 4; k++)
		{
			m_vertexData.push_back(temp.m_faces[j].m_points[k]);
		}
	}
	
	// traverse the tree
	for (int i = 0; i < 8; i++)
	{
		if (pNode->m_children[i] != NULL)
		{
			AddPointsToBuffer(pNode->m_children[i]);
		}
	}
	return;
}

void tOctree::SetUniforms(int prog)
{
	// set uniform
	GLint worldLoc = glGetUniformLocation(prog, "world");
	glProgramUniformMatrix4fv(prog, worldLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4())); // reset it back to identity

	// set color
	GLint colorLoc = glGetUniformLocation(prog, "inColor");
	glUniform3fv(colorLoc, 1, glm::value_ptr(glm::vec3(1.0, 0.7, 0.7)));
}

void tOctree::RecordCollisionPoint(glm::vec3 start, glm::vec3 end, tCollisionData& cd)
{
	RecordCollisionPointW(start, end, cd);
}

void tOctree::RecordCollisionPointW(glm::vec3 start, glm::vec3 end, tCollisionData& cd, tOctreeNode* pNode)
{
	if (pNode == NULL)
	{
		pNode = &m_root;
	}
	// first check the point along the ray that is first in the octree
	// if there is no point, you can just return
	// (or, you can just make sure the camera is inside the octree to begin with so you don't have to calculate
	// this every time
	// first, check if there is children
	// if no children, skip all the subdivision and just check each object in this leaf node
	glm::vec3 vel = end - start;
	float fullDistance = glm::length(vel); // should give me magnitude?
	if (pNode->HasChildren())
	{
		// figure out how to split the rays up
		// map<pair<int, bool>, float> planeCollisions; // float = ratio from start, int = plane index, bool = front?
		std::vector<float> ratios;
		ratios.push_back(0.0f); // the start value
		for (int i = 0; i < 3; i++)
		{
			float startToPlane = DistanceToPlane(pNode->m_planes[i], start);
			float endToPlane = DistanceToPlane(pNode->m_planes[i], end);
			if (startToPlane * endToPlane < 0)
			{
				// different signs, therefore collision with plane
				float ratioToHit = IntersectRayPlane(pNode->m_planes[i], start, end);
				ratios.push_back(ratioToHit);
			}
		}
		ratios.push_back(1.0f); /// the end
		// sort the ratios (smallest to greatest), removing duplicate ratio values
		std::sort(ratios.begin(), ratios.end());
		auto last = std::unique(ratios.begin(), ratios.end());
		ratios.erase(last, ratios.end());
		// store ray parts
		std::vector<std::pair<std::pair<glm::vec3, glm::vec3>, int>> rayParts;
		for (auto ratio = ratios.begin(); ratio != ratios.end() - 1; ratio++)
		{
			glm::vec3 partStart = start + (*ratio * vel);
			glm::vec3 partEnd = start + (*(ratio + 1) * vel);
			// grab the midpoint
			glm::vec3 midPoint = (partStart + partEnd) * 0.5f;
			int quadrant = GetQuadrant(midPoint, pNode->m_center);
			rayParts.push_back(std::pair<std::pair<glm::vec3, glm::vec3>, int>(std::pair<glm::vec3, glm::vec3>(partStart, partEnd), 
				quadrant));
		}
		// cycle through rayParts, calling record collision Point
		bool foundCollision = false;
		for (auto part = rayParts.begin(); part != rayParts.end() && !foundCollision; part++)
		{
			RecordCollisionPointW(part->first.first, part->first.second, cd, pNode->m_children[part->second]);
			if (cd.m_closestHit < 1.0f)
			{
				foundCollision = true;
			}
		}
	}
	// if it doesn't collide with any planes, or there are no planes (and thus no children), cycle through all the objects
	// of this node (bounding boxes) to see if there is a collision anywhere.  if there is, record the collision (only if it is the
	// closest one so far) and return.  if not, just return
	else
	{
		// detecting collision isn't too difficult with AABB's...
		for (auto i = pNode->m_data.begin(); i != pNode->m_data.end(); )
		{
			// convert min and max coordinates by multiplying it by the translation matrix?  check
			glm::vec3 maxPt = glm::vec3((*i)->m_boundingBox.m_worldMat * glm::vec4((*i)->m_boundingBox.m_max, 1.0));
			glm::vec3 minPt = glm::vec3((*i)->m_boundingBox.m_worldMat * glm::vec4((*i)->m_boundingBox.m_min, 1.0));
			glm::vec3 offset = glm::vec3(cd.m_offset, cd.m_offset, cd.m_offset);
			maxPt += offset;
			minPt -= offset;
			glm::vec3 center = (maxPt + minPt) / 2.0f; // optimization:  store this as part of bounding box data

			// does the ray pass through the box?
			glm::vec3 pointOnRay = ClosestPointOnLine(start, end, center);
			// does this point fall within the box?
			if (pointOnRay.x < maxPt.x && pointOnRay.x > minPt.x && pointOnRay.y < maxPt.y && pointOnRay.y > minPt.y && 
				pointOnRay.z < maxPt.z && pointOnRay.z > minPt.z)
			{
				// found out what sides the ray crossed
				float closest = 0.0f;
				std::pair<glm::vec3, float> closestCollision;
				int x = -1 * (start.x <= minPt.x) + (start.x >= maxPt.x);
				int y = -1 * (start.y <= minPt.y) + (start.y >= maxPt.y);
				int z = -1 * (start.z <= minPt.z) + (start.z >= maxPt.z);
				
				if (x == 0 && y == 0 && z == 0)
				{
					// we got inside the box by mistake
					// (hopefully by floating point precision error)
					// shoot the ray backwards, finding the new starting spot
					// the smallest (the negative) distance wins,
					// and the smallest ratio wins
					float xRatio = (GetOutOfBox(minPt.x, maxPt.x, start.x, vel.x) - start.x) / -vel.x;
					float yRatio = (GetOutOfBox(minPt.y, maxPt.y, start.y, vel.y) - start.y) / -vel.y;
					float zRatio = (GetOutOfBox(minPt.z, maxPt.z, start.z, vel.z) - start.z) / -vel.z;
					float finalRatio = glm::min(xRatio, glm::min(yRatio, zRatio));
					start += -vel * finalRatio;
					// now do the whole thing over again
					continue;
				}
				else
				{
					// check which side (x, y, or z) was actually hit
					if (x != 0)
					{
						float dist; // determines the last plane the ray passes through
						if (x == 1)
						{
							// right
							dist = pointOnRay.x - maxPt.x; // dist should be negative
						}
						else if (x == -1)
						{
							// left
							dist = pointOnRay.x - minPt.x; // dist should be positive
						}
						float distToOrig = pointOnRay.x - cd.m_rayOrig.x;
						CalculateSideHit(glm::vec3(x, 0.0, 0.0), dist, closest, distToOrig, cd.m_velocity.x, closestCollision);
					}
					if (y != 0)
					{
						float dist;
						if (y == 1)
						{
							dist = pointOnRay.y - maxPt.y;
						}
						else if (y == -1)
						{
							dist = pointOnRay.y - minPt.y;
						}
						float distToOrig = pointOnRay.y - cd.m_rayOrig.y;
						CalculateSideHit(glm::vec3(0.0, y, 0.0), dist, closest, distToOrig, cd.m_velocity.y, closestCollision);
					}
					if (z !=0)
					{
						float dist;
						if (z == 1)
						{
							dist = pointOnRay.z - maxPt.z;
						}
						else if (z == -1)
						{
							dist = pointOnRay.z - minPt.z;
						}
						float distToOrig = pointOnRay.z - cd.m_rayOrig.z;
						CalculateSideHit(glm::vec3(0.0, 0.0, z), dist, closest, distToOrig, cd.m_velocity.z, closestCollision);
					}
				}

				// calculate ratio
				glm::vec3 ptOnPlane = ((closestCollision.first.x + closestCollision.first.y + closestCollision.first.z) < 0) ? 
					minPt : maxPt;
				cd.m_closestHit = closestCollision.second;
				cd.m_plane = tPlane(closestCollision.first, ptOnPlane);
				// find the one closest to the point in the box
				// shortest distance from pointonray to each collided side
				// so and so forth... repeat for y and z
			}
			i++; // iterate
		}
	}
	return;
}

int tOctree::GetQuadrant(glm::vec3 test, glm::vec3 center)
{
	bool x = test.x > center.x;
	bool y = test.y > center.y;
	bool z = test.z > center.z;
	return ((4 * z) + (2 * y) + x);
}

void tOctree::CalculateSideHit(glm::vec3 plane, float dist, float& closest, float distToRayOrig, float absDist, 
							   std::pair<glm::vec3, float>& closestCollision)
{
	if (closest == 0.0f || abs(dist) < closest)
	{
		closestCollision.first = plane;
		closest = abs(dist);
		closestCollision.second = (distToRayOrig - dist) / absDist; // this will yield the ratio of hit on plane to velocity
	}
}

float tOctree::GetOutOfBox(float min, float max, float start, float vel)
{
	float ratioToMin = (min - start) / vel;
	float ratioToMax = (max - start) / vel;
	return ((glm::min(ratioToMin, ratioToMax) == ratioToMin) ? min : max);
}