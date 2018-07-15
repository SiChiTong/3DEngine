#pragma once

#include <vector>

class CNode
{
public:
	CNode(uint32_t id = 0);
	virtual ~CNode();
	uint32_t GetId();
	void SetParent(CNode* node);
	CNode* GetParent();
	const std::vector<CNode*>* GetChildren();
	void AddChild(CNode* node);
	void RemoveChild(CNode* node);
	void RemoveAllChildren();
	void DeleteAllChildren();

protected:
	unsigned int id;
	CNode* parent;
	std::vector<CNode*> children;
};
