#include "Graphics/SceneGraph/DataGNode.h"

DataGNode::DataGNode(uint32_t id, DrawableEntity* drawableEntity)
	: GNode(id), drawableEntity(drawableEntity) {}

DataGNode::~DataGNode() {}

GNode::GNODE_TYPE DataGNode::GetType()
{
	return GNode::CONTAINER;
}

BoundingSphere DataGNode::GetBoundingSphere()
{
	return drawableEntity->boundingVolume;
}

DrawableEntity* DataGNode::GetDrawableEntity()
{
	return drawableEntity;
}