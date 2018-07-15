#pragma once

#include "Math/CMath.h"

class IRigidActor 
{
public:
	virtual void SetScale(DirectX::XMFLOAT3 scale) = 0;
	virtual void SetRotation(DirectX::XMFLOAT4 rotation) = 0;
	virtual void SetPosition(DirectX::XMFLOAT3 position) = 0;
};

