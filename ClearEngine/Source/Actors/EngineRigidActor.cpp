#include "Actors/EngineRigidActor.h"
#include "Math/MathUtils.h"
#include "Math/SIMDConversionUtils.h"
using namespace DirectX;
using namespace physx;

EngineRigidActor::EngineRigidActor(RigidActor* rigidActor, RigidModel* model, physx::PxRigidActor* physicsActor)
	: CActor(name, scale, rotation, position), model(model), physicsActor(physicsActor)
{
	if (physicsActor)
	{
		physicsActor->setGlobalPose(CreatePxTransform(CreatePxQuat(rotation), CreatePxVec3(position)));
	}
}

EngineRigidActor::~EngineRigidActor()
{
	physicsActor->release();
}

void EngineRigidActor::Update(double time)
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

PxEngineRigidActor* EngineRigidActor::GetPhysicsActor()
{
	return physicsActor;
}

//Changing scale of a rigid actor is disallowed to prevent any mismatch between graphics and physics.
void EngineRigidActor::SetScale(DirectX::XMFLOAT3 scale) {}

void EngineRigidActor::SetRotation(DirectX::XMFLOAT4 rotation)
{
	this->CActor::SetRotation(rotation);
	if (physicsActor)
	{
		physicsActor->setGlobalPose(CreatePxTransform(CreatePxQuat(rotation), physicsActor->getGlobalPose().p));
	}
}

void EngineRigidActor::SetPosition(DirectX::XMFLOAT3 position)
{
	this->CActor::SetPosition(position);
	if (physicsActor)
	{
		physicsActor->setGlobalPose(CreatePxTransform(physicsActor->getGlobalPose().q, CreatePxVec3(position)));
	}
}

