#include "Lights/Light.h"

using namespace DirectX;

Light::Light(DirectX::XMFLOAT4 diffuse, DirectX::XMFLOAT4 specular)
	: diffuse(diffuse), specular(specular) { }

Light::~Light()
{}

void Light::SetDiffuse(XMFLOAT4 diffuse)
{
	this->diffuse = diffuse;
}

XMFLOAT4 Light::GetDiffuse()
{
	return diffuse;
}

void Light::SetSpecular(XMFLOAT4 specular)
{
	this->specular = specular;
}

XMFLOAT4 Light::GetSpecular()
{
	return specular;
}