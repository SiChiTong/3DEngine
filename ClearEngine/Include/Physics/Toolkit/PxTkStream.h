// This code contains NVIDIA Confidential Information and is disclosed to you
// under a form of NVIDIA software license agreement provided separately to you.
//
// Notice
// NVIDIA Corporation and its licensors retain all intellectual property and
// proprietary rights in and to this software and related documentation and
// any modifications thereto. Any use, reproduction, disclosure, or
// distribution of this software and related documentation without an express
// license agreement from NVIDIA Corporation is strictly prohibited.
//
// ALL NVIDIA DESIGN SPECIFICATIONS, CODE ARE PROVIDED "AS IS.". NVIDIA MAKES
// NO WARRANTIES, EXPRESSED, IMPLIED, STATUTORY, OR OTHERWISE WITH RESPECT TO
// THE MATERIALS, AND EXPRESSLY DISCLAIMS ALL IMPLIED WARRANTIES OF NONINFRINGEMENT,
// MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE.
//
// Information and code furnished is believed to be accurate and reliable.
// However, NVIDIA Corporation assumes no responsibility for the consequences of use of such
// information or for any infringement of patents or other rights of third parties that may
// result from its use. No license is granted by implication or otherwise under any patent
// or patent rights of NVIDIA Corporation. Details are subject to change without notice.
// This code supersedes and replaces all information previously supplied.
// NVIDIA Corporation products are not authorized for use as critical
// components in life support devices or systems without express written approval of
// NVIDIA Corporation.
//
// Copyright (c) 2008-2013 NVIDIA Corporation. All rights reserved.
// Copyright (c) 2004-2008 AGEIA Technologies, Inc. All rights reserved.
// Copyright (c) 2001-2004 NovodeX AG. All rights reserved.  

#ifndef PX_TOOLKIT_STREAM_H
#define PX_TOOLKIT_STREAM_H

#include "common/PxIO.h"
#include "extensions/PxDefaultStreams.h"

#include "cooking/PxCooking.h"
namespace physx
{
	class PxTriangleMesh;
	class PxConvexMesh;
	class PxPhysics;
	class PxCooking;
	class PxTriangleMeshDesc;
}

namespace PxToolkit
{
	using namespace physx;

	// temporarily located here
	physx::PxTriangleMesh* createTriangleMesh32(physx::PxPhysics& physics, physx::PxCooking& cooking, const PxVec3* verts, PxU32 vertCount, const PxU32* indices32, PxU32 triCount, bool flipNormals);
	physx::PxConvexMesh* createConvexMesh(physx::PxPhysics& physics, physx::PxCooking& cooking, const PxVec3* verts, PxU32 vertCount, PxConvexFlags flags);
	physx::PxConvexMesh* createConvexMeshSafe(physx::PxPhysics& physics, physx::PxCooking& cooking, const PxVec3* verts, PxU32 vertCount, PxConvexFlags flags, PxU32 vLimit = 256);
	
	//this function is for MultiMaterial and MultiMaterialTerrain
	physx::PxTriangleMesh* createTriangleMesh32(physx::PxPhysics& physics, physx::PxCooking& cooking, physx::PxTriangleMeshDesc* meshDesc);
}



#endif
