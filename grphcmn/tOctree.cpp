#include "tOctree.h"
#include "tModeToggler.h"
#include <math.h>

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
	int count = 0;
	while (count < 8 && parent->m_children[count++] == NULL);
	if (count < 8)
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
		   for (size_t s = 0; s < parent->m_data.size(); s++)
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
			glm::vec3 offsetPt = glm::vec3(testPoint) - pNode->m_center;
			bool px = offsetPt.x > 0;
			bool py = offsetPt.y > 0;
			bool pz = offsetPt.z > 0;
			int quadrant = (4 * pz) + (2 * py) + px;
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