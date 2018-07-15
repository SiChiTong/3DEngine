#pragma once


#include <map>
#include <string>
#include <list>
#include <vector>
#include <stdlib.h>
#include <Windows.h>
#include "CPhysics.h"
#include "Toolkit/PxToolkit.h"

class PhysicsManager 
{
public:
	enum DriverType
	{
		DRIVER_OPENGL = 0,	    // Supports Windows, Linux, MacOSX and PS3.
		DRIVER_GLES2,			// Supports Android (> 2.2) and iPhone.
		DRIVER_DIRECT3D9,		// Supports Windows, XBOX360.
		DRIVER_LIBGCM,			// Supports PS3, currently unused (PS3 uses OpenGL)
		DRIVER_LIBGNM,          // Supports PS4
		DRIVER_LIBGXM,			// Supports PSP2
		DRIVER_DIRECT3D11,		// Supports Windows 7/Vista.
		DRIVER_NULL
	};
	struct PhysEngineParams
	{
		physx::PxVec3 gravity;
		physx::PxSceneFlags sceneFlags;

		PhysEngineParams()
		{
			gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
			sceneFlags |= physx::PxSceneFlag::eENABLE_CCD;
		}
	};
	PhysicsManager(DriverType driverType, void* graphicsDevice, PhysEngineParams physicsParams);
	~PhysicsManager();
	bool Simulate(double frameTime);
	void setCCDActive(physx::PxShape& shape);
	physx::PxPhysics* GetPhysics() const;
	physx::PxCooking* GetCooking() const;
	physx::PxScene* GetScene() const;
	
	
private:
	enum
	{
		CCD_FLAG = 1 << 29,
		SNOWBALL_FLAG = 1 << 30,
		DETACHABLE_FLAG = 1 << 31
	};
	physx::PxFoundation* mFoundation;
	physx::PxProfileZoneManager* mProfileZoneManager;
	physx::PxPhysics* mPhysics;
	physx::PxCooking* mCooking;
	physx::PxScene* mScene;
	physx::PxDefaultCpuDispatcher* mCpuDispatcher;
	physx::PxCudaContextManager* mCudaContextManager;
	physx::PxReal mAccumulator;
	physx::PxReal mStepSize;
};
