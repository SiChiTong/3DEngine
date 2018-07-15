
#include "Physics/PhysicsManager.h"
using namespace physx;

PhysicsManager::PhysicsManager(DriverType driverType, void* graphicsDevice, PhysEngineParams physicsParams)
{
	static PxDefaultErrorCallback gDefaultErrorCallback;
	static PxDefaultAllocator gDefaultAllocatorCallback;

	mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);
	if (!mFoundation)
	{
		MessageBox(0, L"PxCreateFoundation failed!", L"Error", MB_OK);
	}

	mProfileZoneManager = &PxProfileZoneManager::createProfileZoneManager(mFoundation);
	if (!mProfileZoneManager)
	{
		MessageBox(0, L"PxProfileZoneManager::createProfileZoneManager failed!", L"Error", MB_OK);
	}

	bool recordMemoryAllocations = true;
	mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation,
		PxTolerancesScale(), recordMemoryAllocations, mProfileZoneManager);
	if (!mPhysics)
	{
		MessageBox(0, L"PxCreatePhysics failed!", L"Error", MB_OK);
	}

	mCooking = PxCreateCooking(PX_PHYSICS_VERSION, *mFoundation, PxCookingParams(PxTolerancesScale()));
	if (!mCooking)
	{
		MessageBox(0, L"PxCreateCooking failed!", L"Error", MB_OK);
	}

	PxSceneDesc sceneDesc(mPhysics->getTolerancesScale());
	sceneDesc.gravity = physicsParams.gravity;
	sceneDesc.flags = physicsParams.sceneFlags;
	
	PxCudaContextManagerDesc cudaContextManagerDesc;
	switch (driverType)
	{
	case DRIVER_DIRECT3D9:
		cudaContextManagerDesc.interopMode = PxCudaInteropMode::D3D9_INTEROP;
		break;
	case DRIVER_DIRECT3D11:
		cudaContextManagerDesc.interopMode = PxCudaInteropMode::D3D11_INTEROP;
		break;
	case DRIVER_OPENGL:
		cudaContextManagerDesc.interopMode = PxCudaInteropMode::OGL_INTEROP;
		break;
	};
	cudaContextManagerDesc.graphicsDevice = graphicsDevice;

	mCudaContextManager = PxCreateCudaContextManager(*mFoundation, cudaContextManagerDesc, mProfileZoneManager);

	if (!sceneDesc.cpuDispatcher)
	{
		mCpuDispatcher = PxDefaultCpuDispatcherCreate(1);
		if (!mCpuDispatcher)
		{
			MessageBox(0, L"PxDefaultCpuDispatcherCreate failed!", L"Error", MB_OK);
		}
		sceneDesc.cpuDispatcher = mCpuDispatcher;
	}
	if (!sceneDesc.filterShader)
	{
		sceneDesc.filterShader = &PxDefaultSimulationFilterShader;
	}

#ifdef PX_WINDOWS
	if (!sceneDesc.gpuDispatcher && mCudaContextManager)
	{
		sceneDesc.gpuDispatcher = mCudaContextManager->getGpuDispatcher();
	}
#endif

	mScene = mPhysics->createScene(sceneDesc);
	if (!mScene)
	{
		MessageBox(0, L"createScene failed!", L"Error", MB_OK);
	}

	mAccumulator = 0.0f;
	mStepSize = 1.0f / 60.0f;
}

PhysicsManager::~PhysicsManager()
{
	//Very important that these pointers are released in this order.
	mScene->release();
	mCpuDispatcher->release();
	mCudaContextManager->release();
	mCooking->release();
	mPhysics->release();
	mProfileZoneManager->release();
	mFoundation->release();
}

bool PhysicsManager::Simulate(double frameTime)
{
	PxReal pxReal = frameTime;
	
	mAccumulator += frameTime;
	if (mAccumulator < mStepSize)
	{
		return false;
	}

	mAccumulator -= mStepSize;

	mScene->simulate(mStepSize);
	mScene->fetchResults();

	return true;
}

void PhysicsManager::setCCDActive(PxShape& shape)
{
	PxFilterData fd = shape.getSimulationFilterData();
	fd.word3 |= CCD_FLAG;
	shape.setSimulationFilterData(fd);
}

PxPhysics* PhysicsManager::GetPhysics() const
{
	return mPhysics;
}

PxCooking* PhysicsManager::GetCooking() const
{
	return mCooking;
}

PxScene* PhysicsManager::GetScene() const
{
	return mScene;
}





