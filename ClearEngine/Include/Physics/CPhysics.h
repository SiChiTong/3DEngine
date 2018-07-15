#pragma once

#define DEBUG_APP 

#ifdef DEBUG_APP
	#pragma comment(lib,"PhysX3DEBUG_x64.lib")
	#pragma comment(lib,"PhysX3CharacterKinematicDEBUG_x64.lib")
	#pragma comment(lib,"PhysX3CommonDEBUG_x64.lib")
	#pragma comment(lib,"PhysX3CookingDEBUG_x64.lib")
	#pragma comment(lib,"PhysXProfileSDKDEBUG.lib")
	#pragma comment(lib,"PxTaskDEBUG.lib")
	#pragma comment(lib,"PhysX3ExtensionsDEBUG.lib")
#else
	#pragma comment(lib,"PhysX3CHECKED_x64.lib")
	#pragma comment(lib,"PhysX3CharacterKinematicCHECKED_x64.lib")
	#pragma comment(lib,"PhysX3CommonCHECKED_x64.lib")
	#pragma comment(lib,"PhysX3CookingCHECKED_x64.lib")
	#pragma comment(lib,"PhysXProfileSDKCHECKED.lib")
	#pragma comment(lib,"PxTaskCHECKED.lib")
	#pragma comment(lib,"PhysX3ExtensionsCHECKED.lib")
#endif


#include <PxPhysicsAPI.h>
#include <common/PxPhysXCommonConfig.h>
#include <extensions/PxDefaultSimulationFilterShader.h>
#include <PxRigidActor.h>
#include <PxRigidStatic.h>
#include <PxRigidDynamic.h>


