#pragma once

#include <map>
#include <string>
#include "Geometry/GeometryCreator.h"
#include "Loading/AssetLoader.h"
#include "Graphics/Rendering/CRenderer.h"
#include "Time/Timer.h"
#include "Input/CInput.h"
#include "Sound/Mp3.h"
#include "Sound/SoundClass.h"
#include "Physics/PhysicsManager.h"
#include "PhysicsParams.h"
#include "Actors/RigidActor.h"
#include "Actors/SkeletalActor.h"

class CEngine 
{

public:
	CEngine(GraphicsParams graphicsParams, PhysicsParams physicsParams);
	~CEngine();
	int Run();
	
private:
	void InitializeWindow(HINSTANCE hInstance, int showWnd, LPWSTR wndClassName, GraphicsParams graphicsParams);
	static LRESULT CALLBACK wndProc(HWND windowHandle, UINT msg, WPARAM wParam, LPARAM lParam);
	physx::PxRigidStatic* CreateStaticGroundPlane(physx::PxReal distance);
	RigidActor* CreatePlaneActor(std::wstring name, RigidModel* model, DirectX::XMFLOAT3 scale, physx::PxReal distance);
	RigidActor* CreateTriangleMeshActor(std::wstring name, RigidModel* model, bool dynamic, physx::PxReal scale, physx::PxQuat rotation, physx::PxVec3 position, bool flipNormals = false);
	RigidActor* CreateBoxActor(std::wstring name, RigidModel* model, physx::PxVec3 scale, physx::PxQuat rotation, physx::PxVec3 position);
	RigidActor* CreateSphereActor(std::wstring name, RigidModel* model, physx::PxReal radius, physx::PxVec3 position);
	WNDCLASSEX window;
	HWND windowHandle;
	UINT numElements;
	CRenderer* renderer;
	PhysicsManager* physicsManager;
	Timer* timer;
	CInput* input;
	SoundClass soundPlayer;
	Mp3 player;
	GameAssets* gameAssets;
	GeometryCreator* geometryCreator;
	AssetLoader* assetLoader;
};
