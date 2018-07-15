#pragma once

#include "GraphicsActor.h"
#include "Geometry/Models.h"

class RigidGraphicsActor : public GraphicsActor
{
	public:
		RigidGraphicsActor(RigidModel* model, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT4 rotation, DirectX::XMFLOAT3 position, bool stationary);
		~RigidGraphicsActor();
		void Update(double time);
		void Render(ID3D11DeviceContext1* devCon);
		void Render(ID3D11DeviceContext1* devCon, ID3D11Buffer* instanceBuffer, UINT instanceStride);
		RigidModel* GetModel();
		ModelSubset* GetModelSubset(int subsetIndex);
		GraphicsActorType GetType();

	private:
		RigidModel* model;
		std::vector<DrawableEntity*> drawableEntities;
};
