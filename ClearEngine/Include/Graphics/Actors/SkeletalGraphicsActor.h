#pragma once

#include "GraphicsActor.h"
#include "Geometry/Models.h"

class SkeletalGraphicsActor : public GraphicsActor
{
public:
	SkeletalGraphicsActor(SkeletalModel* model, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT4 rotation, DirectX::XMFLOAT3 position, bool stationary);
	~SkeletalGraphicsActor();
	void Update(double time);
	void Render(ID3D11DeviceContext1* devCon);
	void Render(ID3D11DeviceContext1* devCon, ID3D11Buffer* instanceBuffer, UINT instanceStride);
	SkeletalModel* GetModel();
	ModelSubset* GetModelSubset(int subsetIndex);
	GraphicsActorType GetType();

private:
	SkeletalModel* model;
	std::vector<DrawableEntity*> drawableEntities;
};
