#pragma once

#include "GNode.h"
#include "Graphics/Actors/GraphicsActor.h"

class DataGNode : public GNode
{
public:
	DataGNode(uint32_t id, DrawableEntity* drawableEntity);
	~DataGNode();
	GNODE_TYPE GetType();
	BoundingSphere GetBoundingSphere();
	DrawableEntity* GetDrawableEntity();

private:
	DrawableEntity* drawableEntity;
};
