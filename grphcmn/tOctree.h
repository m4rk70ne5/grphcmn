#ifndef TOCTREE_H
#define TOCTREE_H

#include "tMesh.h"
#include <vector>

struct tOctreeNode
{
	tOctreeNode* m_children[8];
	std::vector<tMesh*> m_data; // for leaf nodes, maximum of 4 meshes
	glm::vec3 m_min, m_max; // min and max extents
	// automatically calculate below
	// splitter planes
	// center
	glm::vec3 m_center;
	tOctreeNode();
	virtual ~tOctreeNode();
};

class tOctree : public tGeometry
{
protected:
	tOctreeNode m_root;
	void Insert(tMesh*, tOctreeNode*); // insert each mesh into the tree
	void SetExtents(tOctreeNode*, glm::vec3, glm::vec3);
	static std::vector<int> CalculateSubDiv(tOctreeNode*, tMesh*);
	std::vector<glm::vec3> m_vertexData; // for drawing
	void AddPointsToBuffer(tOctreeNode*);
public:
	tOctree(std::vector<tMesh*>&, int&); // populate octree based off of extents
	virtual ~tOctree(); // destruct all the nodes
	virtual void Draw(int);
	virtual void SetUniforms(int);
	void CreateVertexBuffers(int); // vao
};

#endif