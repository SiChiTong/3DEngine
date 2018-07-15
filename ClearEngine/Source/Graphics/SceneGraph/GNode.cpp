#include "Graphics/SceneGraph/GNode.h"

GNode::GNode(uint32_t id)
	: CNode(id) {}

GNode::~GNode()
{
	//Ensure all child nodes are also deleted.
	DeleteAllChildren();
}

