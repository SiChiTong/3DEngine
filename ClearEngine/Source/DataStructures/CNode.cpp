#include "DataStructures/CNode.h"

CNode::CNode(uint32_t id)
	: id(id) {}

CNode::~CNode() {}

unsigned int CNode::GetId()
{
	return id;
}

void CNode::SetParent(CNode* node)
{
	this->parent = node;
}

CNode* CNode::GetParent()
{
	return parent;
}

const std::vector<CNode*>* CNode::GetChildren()
{
	return &children;
}

void CNode::AddChild(CNode* node)
{
	children.push_back(node);
}

void CNode::RemoveChild(CNode* node)
{
	std::vector<CNode*>::iterator iter;
	for (iter = children.begin(); iter != children.end(); ++iter)
	{
		if ((*iter)->id == node->id)
		{
			children.erase(iter);
			break;
		}
	}
}

void CNode::RemoveAllChildren()
{
	std::vector<CNode*>::iterator iter;
	for (iter = children.begin(); iter != children.end(); ++iter)
	{
		children.erase(iter);
	}
}

void CNode::DeleteAllChildren()
{
	children.clear();
}
