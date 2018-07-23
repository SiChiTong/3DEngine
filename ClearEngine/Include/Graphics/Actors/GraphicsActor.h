#pragma once

#include "Common/DirectX.h"
#include "Geometry/GeometryTypes.h"

typedef enum GraphicsActorType
{
	RIGID,
	SKELETAL,
	PARTICLE_SYSTEM
};

struct DrawableEntity
{
	GraphicsActor* actor;
	UINT subsetIndex;
	BoundingSphere boundingVolume;
};

class GraphicsActor
{
	friend class CRenderer;
	public:
		~GraphicsActor();
		virtual GraphicsActorType GetType() = 0;
		virtual void Update(double time);
		void SetScale(DirectX::XMFLOAT3 scale);
		DirectX::XMFLOAT3 GetScale();
		void SetRotation(DirectX::XMFLOAT4 rotation);
		DirectX::XMFLOAT4 GetRotation();
		void SetPosition(DirectX::XMFLOAT3 position);
		DirectX::XMFLOAT3 GetPosition();
		DirectX::XMFLOAT4X4 GetWorldMatrix();
		void SetWorldMatrix(DirectX::XMFLOAT4X4 worldMatrix);
		DirectX::XMMATRIX GetXMWorldMatrix();
		DirectX::XMMATRIX GetTransposedXMWorldMatrix();
		bool IsStationary();

	protected:
		GraphicsActor(DirectX::XMFLOAT3 scale, DirectX::XMFLOAT4 rotation, DirectX::XMFLOAT3 position, bool stationary = true);
		UINT id;
		DirectX::XMFLOAT3 scale;
		DirectX::XMFLOAT4 rotation;
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4X4 worldMatrix;
		bool stationary;
};