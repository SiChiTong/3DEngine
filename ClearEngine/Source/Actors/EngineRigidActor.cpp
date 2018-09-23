#include "Actors/EngineRigidActor.h"
#include "Math/MathUtils.h"
#include "Math/SIMDConversionUtils.h"
using namespace DirectX;
using namespace physx;

EngineRigidActor::EngineRigidActor(RigidActor* rigidActor, RigidGraphicsActor* graphicsActor, physx::PxRigidActor* physicsActor)
{
	if (physicsActor)
	{
		physicsActor->setGlobalPose(CreatePxTransform(CreatePxQuat(graphicsActor->GetRotation()), CreatePxVec3(graphicsActor->GetPosition())));
	}
}

EngineRigidActor::~EngineRigidActor()
{
	physicsActor->release();
	delete graphicsActor;
}

void EngineRigidActor::UpdatePhysicsActor(double time)
{
	if (physicsActor)
	{
		//Update the physics engine actor position and rotation.
		physicsActor->setGlobalPose(CreatePxTransform(CreatePxQuat(actor->GetRotation()), physicsActor->getGlobalPose().p));
		physicsActor->setGlobalPose(CreatePxTransform(physicsActor->getGlobalPose().q, CreatePxVec3(actor->GetPosition())));
	}
}

void EngineRigidActor::Update(double time)
{
	if (physicsActor)
	{
		//Update the engine actor's rotation and position to equal the physics engine actor.
		actor->SetRotation(ConvPxQuat(physicsActor->getGlobalPose().q));
		actor->SetPosition(ConvPxVec3(physicsActor->getGlobalPose().p));
	}
	XMFLOAT3 scale = actor->GetScale();
	XMFLOAT3 position = actor->GetPosition();
	XMMATRIX xmWorldMatrix = XMMatrixScaling(scale.x, scale.y, scale.z) * XMMatrixRotationQuaternion(FV(actor->GetRotation())) * XMMatrixTranslation(position.x, position.y, position.z);
	graphicsActor->SetWorldMatrix(MF(xmWorldMatrix));
}

RigidActor* EngineRigidActor::GetActor()
{

}

PxRigidActor* EngineRigidActor::GetPhysicsActor()
{
	return physicsActor;
}

RigidGraphicsActor* EngineRigidActor::GetGraphicsActor()
{
	return graphicsActor;
}

