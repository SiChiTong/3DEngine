#include "Actors/RigidActor.h"
#include "Math/MathUtils.h"
#include "Math/SIMDConversionUtils.h"
using namespace DirectX;
using namespace physx;

RigidActor::RigidActor(std::wstring name, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT4 rotation, DirectX::XMFLOAT3 position, RigidModel* model, physx::PxRigidActor* physicsActor)
: CActor(name, scale, rotation, position), model(model), physicsActor(physicsActor)
{	
	if (physicsActor)
	{
		physicsActor->setGlobalPose(CreatePxTransform(CreatePxQuat(rotation), CreatePxVec3(position)));
	}
}

RigidActor::~RigidActor()
{
	physicsActor->release();
}

void RigidActor::Update(double time)
{
	if (physicsActor)
	{
		//Update the engine actor's rotation and position to equal the physics engine actor.
		rotation = ConvPxQuat(physicsActor->getGlobalPose().q);
		position = ConvPxVec3(physicsActor->getGlobalPose().p);
	}
	XMMATRIX xmWorldMatrix = XMMatrixScaling(scale.x, scale.y, scale.z) * XMMatrixRotationQuaternion(FV(rotation)) * XMMatrixTranslation(position.x, position.y, position.z);
	graphicsActor->SetWorldMatrix(MF(xmWorldMatrix));
	this->CActor::Update(time);
}

RigidModel* RigidActor::GetModel()
{
	return this->model;
}

ModelSubset* RigidActor::GetModelSubset(int subsetIndex)
{
	return &this->model->subsets.at(subsetIndex);
}

CActorType RigidActor::GetType()
{
	return CActorType::RIGID;
}

PxRigidActor* RigidActor::GetPhysicsActor()
{
	return physicsActor;
}

//Changing scale of a rigid actor is disallowed to prevent any mismatch between graphics and physics.
void RigidActor::SetScale(DirectX::XMFLOAT3 scale) {}

void RigidActor::SetRotation(DirectX::XMFLOAT4 rotation)
{
	this->CActor::SetRotation(rotation);
	if (physicsActor)
	{
		physicsActor->setGlobalPose(CreatePxTransform(CreatePxQuat(rotation), physicsActor->getGlobalPose().p));
	}
}

void RigidActor::SetPosition(DirectX::XMFLOAT3 position)
{
	this->CActor::SetPosition(position);
	if (physicsActor)
	{
		physicsActor->setGlobalPose(CreatePxTransform(physicsActor->getGlobalPose().q, CreatePxVec3(position)));
	}
}

//bool ModelSubsetSortPredicate(ModelSubset* x, ModelSubset* y)
//{
	//if(x == NULL)
	//{
		//return false;
	//}
	//else if (y == NULL)
	//{
		//return true;
	//}
	//else
	//{
		//return x->GetCameraDistance() > y->GetCameraDistance();
	//}
//}

