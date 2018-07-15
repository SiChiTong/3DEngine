#include "Lights/SpotLight.h"

using namespace DirectX;

SpotLight::SpotLight(DirectX::XMFLOAT4 diffuse, DirectX::XMFLOAT4 specular, DirectX::XMFLOAT3 position, float range, DirectX::XMFLOAT3 direction, float cone, DirectX::XMFLOAT3 attenuation)
	: Light(diffuse, specular), position(position), range(range), direction(direction), cone(cone), attenuation(attenuation) { }

SpotLight::~SpotLight() {}

void SpotLight::SetPosition(XMFLOAT3 position)
{
	this->position = position;
}

XMFLOAT3 SpotLight::GetPosition()
{
	return position;
}

void SpotLight::SetRange(float range)
{
	this->range = range;
}

float SpotLight::GetRange()
{
	return range;
}

void SpotLight::SetDirection(XMFLOAT3 direction)
{
	this->direction = direction;
}

XMFLOAT3 SpotLight::GetDirection()
{
	return direction;
}

void SpotLight::SetCone(float cone)
{
	this->cone = cone;
}

float SpotLight::GetCone()
{
	return cone;
}

void SpotLight::SetAttenuation(XMFLOAT3 attenuation)
{
	this->attenuation = attenuation;
}

XMFLOAT3 SpotLight::GetAttenuation()
{
	return attenuation;
}