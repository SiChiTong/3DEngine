#pragma once

#include "Graphics/SceneGraph/GNode.h"

class ContainerGNode : public GNode
{
public:
	ContainerGNode(uint32_t id, BoundingSphere boundingSphere);
	ContainerGNode(uint32_t id);
	~ContainerGNode();
	GNODE_TYPE GetType();
	void SetBoundingSphere(BoundingSphere boundingSphere);
	BoundingSphere GetBoundingSphere();
	void MergeExistngChildSpheres();
	void CalculateAndMergeChildSpheres();

private:
	BoundingSphere boundingSphere;
};
