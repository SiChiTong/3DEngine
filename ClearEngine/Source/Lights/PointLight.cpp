#include "Lights/PointLight.h"

using namespace DirectX;

PointLight::PointLight(DirectX::XMFLOAT4 diffuse, DirectX::XMFLOAT4 specular, DirectX::XMFLOAT3 position, float range, DirectX::XMFLOAT3 attenuation)
	: Light(diffuse, specular), position(position), range(range), attenuation(attenuation) { }

PointLight::~PointLight() {}

void PointLight::SetPosition(XMFLOAT3 position)
{
	this->position = position;
}

XMFLOAT3 PointLight::GetPosition()
{
	return position;
}

void PointLight::SetRange(float range)
{
	this->range = range;
}

float PointLight::GetRange()
{
	return range;
}

void PointLight::SetAttenuation(XMFLOAT3 attenuation)
{
	this->attenuation = attenuation;
}

XMFLOAT3 PointLight::GetAttenuation()
{
	return attenuation;
}