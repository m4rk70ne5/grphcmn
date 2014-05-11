#ifndef TOCTREE_H
#define TOCTREE_H

#include "tMesh.h"
#include "CollisionTypes.h"
#include <vector>

struct tOctreeNode
{
	tOctreeNode* m_children[8];
	tPlane m_planes[3];
	std::vector<tMesh*> m_data; // for leaf nodes, maximum of 4 meshes
	glm::vec3 m_min, m_max; // min and max extents
	// automatically calculate below
	// splitter planes
	// center
	glm::vec3 m_center;
	tOctreeNode();
	virtual ~tOctreeNode();
	bool HasChildren()
	{
		// does parent node have children?
		int count = 0;
		while (count < 8 && m_children[count++] == NULL);
		return (count < 8);
	}
};

class tOctree : public tGeometry
{
protected:
	tOctreeNode m_root;
	void Insert(tMesh*, tOctreeNode*); // insert each mesh into the tree
	void SetExtents(tOctreeNode*, glm::vec3, glm::vec3);
	static std::vector<int> CalculateSubDiv(tOctreeNode*, tMesh*);
	static int GetQuadrant(glm::vec3 test, glm::vec3 center);
	static void CalculateSideHit(glm::vec3, float, float&, float, float, std::pair<glm::vec3, float>&);
	static float GetOutOfBox(float, float, float, float);
	std::vector<glm::vec3> m_vertexData; // for drawing
	void AddPointsToBuffer(tOctreeNode*);
	void RecordCollisionPointW(glm::vec3, glm::vec3, tCollisionData&, tOctreeNode* = NULL);
public:
	tOctree(std::vector<tMesh*>&, int&); // populate octree based off of extents
	virtual ~tOctree(); // destruct all the nodes
	virtual void Draw(int);
	virtual void SetUniforms(int);
	virtual void RecordCollisionPoint(glm::vec3, glm::vec3, tCollisionData&);
	void CreateVertexBuffers(int); // vao
	glm::vec3 GetCenter() { return m_root.m_center; };
};

#endif