#pragma once

#include "Light.h"

class DirectionalLight : public Light
{
public:
	DirectionalLight(DirectX::XMFLOAT4 diffuse, DirectX::XMFLOAT4 specular, DirectX::XMFLOAT3 direction);
	~DirectionalLight();
	void SetDirection(DirectX::XMFLOAT3 direction);
	DirectX::XMFLOAT3 GetDirection();

private:
	DirectX::XMFLOAT3 direction;
};
