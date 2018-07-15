#pragma once

#include "Light.h"

class SpotLight : public Light
{
public:
	SpotLight(DirectX::XMFLOAT4 diffuse, DirectX::XMFLOAT4 specular, DirectX::XMFLOAT3 position, float range, DirectX::XMFLOAT3 direction, float cone, DirectX::XMFLOAT3 attenuation);
	~SpotLight();
	void SetPosition(DirectX::XMFLOAT3 position);
	DirectX::XMFLOAT3 GetPosition();
	void SetRange(float range);
	float GetRange();
	void SetDirection(DirectX::XMFLOAT3 direction);
	DirectX::XMFLOAT3 GetDirection();
	void SetCone(float cone);
	float GetCone();
	void SetAttenuation(DirectX::XMFLOAT3 attenuation);
	DirectX::XMFLOAT3 GetAttenuation();

private:
	DirectX::XMFLOAT3 position;
	float range;
	DirectX::XMFLOAT3 direction;
	float cone;
	DirectX::XMFLOAT3 attenuation;
};
