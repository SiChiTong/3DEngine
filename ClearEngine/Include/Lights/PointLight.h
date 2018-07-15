#pragma once

#include "Light.h"

class PointLight : public Light
{
public:
	PointLight(DirectX::XMFLOAT4 diffuse, DirectX::XMFLOAT4 specular, DirectX::XMFLOAT3 position, float range, DirectX::XMFLOAT3 attenuation);
	~PointLight();
	void SetPosition(DirectX::XMFLOAT3 position);
	DirectX::XMFLOAT3 GetPosition();
	void SetRange(float range);
	float GetRange();
	void SetAttenuation(DirectX::XMFLOAT3 attenuation);
	DirectX::XMFLOAT3 GetAttenuation();

private:
	DirectX::XMFLOAT3 position;
	float range;
	DirectX::XMFLOAT3 attenuation;
};
