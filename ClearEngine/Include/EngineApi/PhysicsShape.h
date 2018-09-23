#pragma once

typedef enum PhysicsShapeType
{
	BOX,
	SPHERE
};

class PhysicsShape
{
public:
	virtual ~PhysicsShape() {}
	virtual PhysicsShapeType GetType() = 0;

protected:
	PhysicsShape() {}
};
