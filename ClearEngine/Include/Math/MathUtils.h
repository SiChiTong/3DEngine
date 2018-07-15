#pragma once

#include <windows.h>
#include "CMath.h"
#include "Physics/CPhysics.h"
#include "geometry/GeometryTypes.h"

class MathUtils
{

	public:
		static DirectX::XMFLOAT4 GetQuaternionProduct(DirectX::XMFLOAT4& q1, DirectX::XMFLOAT4& q2);	
		static bool Float3AreEqual(DirectX::XMFLOAT3& p1, DirectX::XMFLOAT3& p2);
		static bool Float4AreEqual(DirectX::XMFLOAT4& v1, DirectX::XMFLOAT4& v2);
		static DirectX::XMFLOAT3 NormalizeFloat3(DirectX::XMFLOAT3 v);
		static DirectX::XMFLOAT3 AddFloat3(DirectX::XMFLOAT3& v1, DirectX::XMFLOAT3& v2);
		static DirectX::XMFLOAT3 SubtractFloat3(DirectX::XMFLOAT3& v1, DirectX::XMFLOAT3& v2);
		static DirectX::XMFLOAT3 MultiplyFloat3(DirectX::XMFLOAT3& v1, DirectX::XMFLOAT3& v2);
		static DirectX::XMFLOAT3 ScaleFloat3(DirectX::XMFLOAT3& v, float scale);
		static DirectX::XMFLOAT3 GetCrossProduct(DirectX::XMFLOAT3& v1, DirectX::XMFLOAT3& v2);
		static Ray ComputeIntersectionLine(DirectX::FXMVECTOR p1, DirectX::FXMVECTOR p2);
		static void CreatePxVec3Vector(std::vector<DVertex>& inVector, std::vector<physx::PxVec3>& outVector);
		static void CreatePxU32Vector(std::vector<DWORD>& inVector, std::vector<physx::PxU32>& outVector);
		DirectX::XMFLOAT3 ConvPxVec3(physx::PxVec3 vec3);
		DirectX::XMFLOAT4 ConvPxQuat(physx::PxQuat quat);
		physx::PxVec3 CreatePxVec3(DirectX::XMFLOAT3 float3);
		physx::PxQuat CreatePxQuat(DirectX::XMFLOAT4 float4);
		physx::PxTransform CreatePxTransform(physx::PxQuat orientation, physx::PxVec3 position);
};

inline DirectX::XMFLOAT3 ConvPxVec3(physx::PxVec3 vec3)
{
	return DirectX::XMFLOAT3(vec3.x, vec3.y, vec3.z);
}

inline DirectX::XMFLOAT4 ConvPxQuat(physx::PxQuat quat)
{
	return DirectX::XMFLOAT4(quat.x, quat.y, quat.z, quat.w);
}

inline physx::PxVec3 CreatePxVec3(DirectX::XMFLOAT3 float3)
{
	return physx::PxVec3(float3.x, float3.y, float3.z);
}

inline physx::PxQuat CreatePxQuat(DirectX::XMFLOAT4 float4)
{
	return physx::PxQuat(float4.x, float4.y, float4.z, float4.w);
}

inline physx::PxTransform CreatePxTransform(physx::PxQuat orientation, physx::PxVec3 position)
{
	physx::PxTransform transform = physx::PxTransform();
	transform.q = orientation;
	transform.p = position;
	return transform;
}


//Leaving here for future use.

//float dot = XMVectorGetX(XMVector3Dot(currentForward, newForward));

//if (std::abs(dot - (-1.0f)) < 0.00000001f)
//{
//XMVECTOR axis = XMVector3Normalize(XMVector3Cross(forward.GetXMVector(), newForward));
//if (!XMVector3Equal(axis, XMVectorZero()))
//{
//float angle = XMScalarACos(dot);

//XMVECTOR rot = XMQuaternionRotationAxis(axis, angle);

//up.SetXMVector(XMVector3Rotate(up.GetXMVector(), rot));
//}
//}