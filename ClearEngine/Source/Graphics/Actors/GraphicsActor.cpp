#include "Graphics/Actors/GraphicsActor.h"
#include "Math/MathUtils.h"
#include "Math/SIMDConversionUtils.h"

using namespace DirectX;

GraphicsActor::GraphicsActor(DirectX::XMFLOAT3 scale, DirectX::XMFLOAT4 rotation, DirectX::XMFLOAT3 position, bool stationary)
: scale(scale), rotation(rotation), position(position), stationary(stationary)
{
	id = 0;
}

GraphicsActor::~GraphicsActor() {}

void GraphicsActor::Update(double time)
{
	worldMatrix = MF(XMMatrixScaling(scale.x, scale.y, scale.z) * XMMatrixRotationQuaternion(FV(rotation)) * XMMatrixTranslation(position.x, position.y, position.z));
}

void GraphicsActor::SetScale(DirectX::XMFLOAT3 scale)
{
	this->scale = scale;
}

DirectX::XMFLOAT3 GraphicsActor::GetScale()
{
	return scale;
}

void GraphicsActor::SetRotation(DirectX::XMFLOAT4 rotation)
{
	this->rotation = rotation;
}

XMFLOAT4 GraphicsActor::GetRotation()
{
	return rotation;
}

void GraphicsActor::SetPosition(DirectX::XMFLOAT3 position)
{
	this->position = position;
}

XMFLOAT3 GraphicsActor::GetPosition()
{
	return position;
}

XMFLOAT4X4 GraphicsActor::GetWorldMatrix()
{
	return worldMatrix;
}

void GraphicsActor::SetWorldMatrix(XMFLOAT4X4 worldMatrix)
{
	this->worldMatrix = worldMatrix;
}

XMMATRIX GraphicsActor::GetXMWorldMatrix()
{
	return FM(worldMatrix);
}

XMMATRIX GraphicsActor::GetTransposedXMWorldMatrix()
{
	return XMMatrixTranspose(XMLoadFloat4x4(&worldMatrix));
}

bool GraphicsActor::IsStationary()
{
	return this->stationary;
}



