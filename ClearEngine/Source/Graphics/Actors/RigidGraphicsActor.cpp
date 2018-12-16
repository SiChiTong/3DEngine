#include "Graphics/Actors/RigidGraphicsActor.h"
#include "Common/GeneralUtils.h"

RigidGraphicsActor::RigidGraphicsActor(RigidModel* model, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT4 rotation, DirectX::XMFLOAT3 position, bool stationary)
: GraphicsActor(scale, rotation, position, stationary), model(model)
{
	for (int i = 0; i < model->subsets.size(); ++i)
	{
		drawableEntities.push_back(new DrawableEntity());
	}
}

RigidGraphicsActor::~RigidGraphicsActor()
{
	GeneralUtils::DeleteAllVectorPointers<DrawableEntity>(drawableEntities);
}

void RigidGraphicsActor::Update(double time)
{
	GraphicsActor::Update(time);
}

void RigidGraphicsActor::Render(ID3D11DeviceContext1* devCon)
{
	model->vertexBuffer->Bind(devCon, 0);
	model->indexBuffer->Bind(devCon, 0);
}

void RigidGraphicsActor::Render(ID3D11DeviceContext1* devCon, ID3D11Buffer* insanceBuffer, UINT instanceStride)
{
	ID3D11Buffer* vertexBuffers[2] = { model->vertexBuffer->GetBuffer(), insanceBuffer };

	UINT strides[2] = { model->vertexBuffer->GetElementSize(), instanceStride };
	UINT offsets[2] = { 0, 0 };

	devCon->IASetVertexBuffers(0, 2, vertexBuffers, strides, offsets);
	model->indexBuffer->Bind(devCon);
}

RigidModel* RigidGraphicsActor::GetModel()
{
	return this->model;
}

ModelSubset* RigidGraphicsActor::GetModelSubset(int index)
{
	return &model->subsets[index];
}

GraphicsActorType RigidGraphicsActor::GetType()
{
	return GraphicsActorType::RIGID;
}