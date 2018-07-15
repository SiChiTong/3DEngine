#pragma once

#include "CActor.h"
#include "Graphics/Actors/RigidGraphicsActor.h"
#include "Geometry/Models.h"
#include "Physics/CPhysics.h"

class RigidActor : public CActor
{
	public:
		RigidActor(std::wstring name, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT4 rotation, DirectX::XMFLOAT3 position, RigidModel* model, physx::PxRigidActor* physicsActor = nullptr);
		~RigidActor();
		void Update(double time);
		RigidModel* GetModel();
		ModelSubset* GetModelSubset(int subsetIndex);  
		CActorType GetType();
		physx::PxRigidActor* GetPhysicsActor();
		void SetScale(DirectX::XMFLOAT3 scale);
		void SetRotation(DirectX::XMFLOAT4 rotation);
		void SetPosition(DirectX::XMFLOAT3 position);

	private:
		RigidModel* model;
		physx::PxRigidActor* physicsActor;
		RigidGraphicsActor* graphicsActor;

};

