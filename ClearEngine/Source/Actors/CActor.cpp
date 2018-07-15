#include "Actors/CActor.h"

using namespace DirectX;

CActor::CActor(std::wstring name, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT4 rotation, DirectX::XMFLOAT3 position)
: name(name), scale(scale), rotation(rotation), position(position)
{	

}

CActor::~CActor() {}

void CActor::Update(double time)
{
	
}

std::wstring CActor::GetName()
{
	return name;
}

void CActor::SetScale(DirectX::XMFLOAT3 scale) 
{
	this->scale = scale;
}

DirectX::XMFLOAT3 CActor::GetScale()
{
	return scale;
}

void CActor::SetRotation(DirectX::XMFLOAT4 rotation)
{
	this->rotation = rotation;
}

XMFLOAT4 CActor::GetRotation()
{
	return rotation;
}

void CActor::SetPosition(DirectX::XMFLOAT3 position)
{
	this->position = position;
}

XMFLOAT3 CActor::GetPosition()
{
	return position;
}





