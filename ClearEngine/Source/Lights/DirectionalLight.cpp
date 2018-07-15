#include "Lights/DirectionalLight.h"

using namespace DirectX;

DirectionalLight::DirectionalLight(DirectX::XMFLOAT4 diffuse, DirectX::XMFLOAT4 specular, DirectX::XMFLOAT3 direction)
	: Light(diffuse, specular), direction(direction) { }

DirectionalLight::~DirectionalLight()
{}

void DirectionalLight::SetDirection(XMFLOAT3 direction)
{
	this->direction = direction;
}

XMFLOAT3 DirectionalLight::GetDirection()
{
	return direction;
}