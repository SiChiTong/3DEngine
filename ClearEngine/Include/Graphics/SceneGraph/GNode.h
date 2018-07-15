#pragma once

#include "DataStructures/CNode.h"
#include "Geometry/GeometryTypes.h"

class GNode : public CNode
{
public:
	enum GNODE_TYPE
	{
		CONTAINER, DATA
	};

	virtual ~GNode();
	virtual GNODE_TYPE GetType() = 0;
	virtual BoundingSphere GetBoundingSphere() = 0;


protected:
	GNode(uint32_t id);
};
