#pragma once

#include "Common/DirectX.h"

class Light
{
public:
	~Light();
	void SetDiffuse(DirectX::XMFLOAT4 diffuse);
	DirectX::XMFLOAT4 GetDiffuse();
	void SetSpecular(DirectX::XMFLOAT4 specular);
	DirectX::XMFLOAT4 GetSpecular();

protected:
	Light(DirectX::XMFLOAT4 diffuse, DirectX::XMFLOAT4 specular);
	DirectX::XMFLOAT4 diffuse;
	DirectX::XMFLOAT4 specular;
};
