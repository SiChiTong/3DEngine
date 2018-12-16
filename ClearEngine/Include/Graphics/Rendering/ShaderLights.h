#pragma once

#include "Common/DirectX.h"

//Describes a directional light in the scene.
struct ShaderDirectionalLight
{
	ShaderDirectionalLight()
	{
		ZeroMemory(this, sizeof(ShaderDirectionalLight));
	}
	DirectX::XMFLOAT3 dir;
	BOOL used;
	DirectX::XMFLOAT4 diffuse;
	DirectX::XMFLOAT4 specular;
};

//Describes a point light in the scene.
struct ShaderPointLight
{
	ShaderPointLight()
	{
		ZeroMemory(this, sizeof(ShaderPointLight));
	}
	DirectX::XMFLOAT3 pos;
	float range;
	DirectX::XMFLOAT3 att;
	BOOL used;
	DirectX::XMFLOAT4 diffuse;
	DirectX::XMFLOAT4 specular;
};

//Describes a spot light in the scene.
struct ShaderSpotLight
{
	ShaderSpotLight()
	{
		ZeroMemory(this, sizeof(ShaderSpotLight));
	}
	DirectX::XMFLOAT3 pos;
	float range;
	DirectX::XMFLOAT3 dir;
	float cone;
	DirectX::XMFLOAT3 att;
	BOOL used;
	DirectX::XMFLOAT4 diffuse;
	DirectX::XMFLOAT4 specular;
};