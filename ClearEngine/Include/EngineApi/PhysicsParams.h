#pragma once

#include <DirectXMath.h>

struct PhysicsParams
{
	DirectX::XMFLOAT3 gravity;
	bool enableCCD;

	PhysicsParams()
	{
		gravity = DirectX::XMFLOAT3(0.0f, -9.81f, 0.0f);
		enableCCD = true;
	}
};