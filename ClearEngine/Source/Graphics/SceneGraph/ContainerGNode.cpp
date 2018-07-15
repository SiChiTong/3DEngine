#include "Graphics/SceneGraph/ContainerGNode.h"
#include "GeometryUtils.h"

ContainerGNode::ContainerGNode(uint32_t id, BoundingSphere boundingSphere)
	: GNode(id), boundingSphere(boundingSphere) {}

ContainerGNode::ContainerGNode(uint32_t id)
	: GNode(id) {}

ContainerGNode::~ContainerGNode() {}

GNode::GNODE_TYPE ContainerGNode::GetType()
{
	return GNode::CONTAINER;
}

void ContainerGNode::SetBoundingSphere(BoundingSphere boundingSphere)
{
	this->boundingSphere = boundingSphere;
}

BoundingSphere ContainerGNode::GetBoundingSphere()
{
	return boundingSphere;
}

//Used when child container node spheres already calculated.
void ContainerGNode::MergeExistngChildSpheres()
{
	std::vector<BoundingSphere> spheres;
	for (auto* child : children)
	{
		spheres.push_back(((GNode*)child)->GetBoundingSphere());
	}
	GeometryUtils::MergeBoundingSpheres(spheres, boundingSphere);
}


//Used when child container node spheres have not yet been calculated.
void ContainerGNode::CalculateAndMergeChildSpheres()
{
	std::vector<BoundingSphere> spheres;
	for (auto* child : children)
	{
		auto* gNode = (GNode*)child;
		if (gNode->GetType() == GNODE_TYPE::CONTAINER)
		{
			((ContainerGNode*)gNode)->CalculateAndMergeChildSpheres();
		}
		spheres.push_back(gNode->GetBoundingSphere())
	}
	GeometryUtils::MergeBoundingSpheres(spheres, boundingSphere);
}