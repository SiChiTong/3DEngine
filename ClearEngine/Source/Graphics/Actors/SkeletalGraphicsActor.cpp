#include "Graphics/Actors/SkeletalGraphicsActor.h"
#include "Common/GeneralUtils.h"

SkeletalGraphicsActor::SkeletalGraphicsActor(SkeletalModel* model, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT4 rotation, DirectX::XMFLOAT3 position, bool stationary)
: GraphicsActor(scale, rotation, position, stationary), model(model)
{
	for (uint32_t i = 0; i < model->subsets.size(); ++i)
	{
		drawableEntities.push_back(new DrawableEntity(this, i));
	}
}

SkeletalGraphicsActor::~SkeletalGraphicsActor()
{
	GeneralUtils::DeleteAllVectorPointers<DrawableEntity>(drawableEntities);
}

void SkeletalGraphicsActor::Update(double time)
{
	GraphicsActor::Update(time);
}

void SkeletalGraphicsActor::Render(ID3D11DeviceContext1* devCon)
{
	model->vertexBuffer->Bind(devCon, 0);
	model->indexBuffer->Bind(devCon, 0);
}

SkeletalModel* SkeletalGraphicsActor::GetModel()
{
	return this->model;
}

ModelSubset* SkeletalGraphicsActor::GetModelSubset(int index)
{
	return &model->subsets[index];
}

GraphicsActorType SkeletalGraphicsActor::GetType()
{
	return GraphicsActorType::SKELETAL;
}
