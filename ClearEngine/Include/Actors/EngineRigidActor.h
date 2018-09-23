#pragma once

#include "RigidActor.h"
#include "Graphics/Actors/RigidGraphicsActor.h"
#include "Geometry/Models.h"
#include "Physics/CPhysics.h"

class EngineRigidActor 
{
public:
	EngineRigidActor(RigidActor* actor, RigidGraphicsActor* graphicsActor, physx::PxRigidActor* physicsActor = nullptr);
	~EngineRigidActor();
	void UpdatePhysicsActor(double time);
	void Update(double time);
	RigidActor* GetActor();
	RigidGraphicsActor* GetGraphicsActor();
	physx::PxRigidActor* GetPhysicsActor();

private:
	RigidActor* actor;
	physx::PxRigidActor* physicsActor;
	RigidGraphicsActor* graphicsActor;

};

