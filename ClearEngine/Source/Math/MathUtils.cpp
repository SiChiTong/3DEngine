#include "Math/MathUtils.h"
#include "Math/SIMDConversionUtils.h"
#include <vector>

using namespace DirectX;
using namespace physx;

XMFLOAT4 MathUtils::GetQuaternionProduct(XMFLOAT4& q1, XMFLOAT4& q2)
{
	XMFLOAT4 result;
	result.x = (q2.w * q1.x) + (q2.x * q1.w) + (q2.y * q1.z) - (q2.z * q1.y);
	result.y = (q2.w * q1.y) - (q2.x * q1.z) + (q2.y * q1.w) + (q2.z * q1.x);
	result.z = (q2.w * q1.z) + (q2.x * q1.y) - (q2.y * q1.x) + (q2.z * q1.w);
	result.w = (q2.w * q1.w) - (q2.x * q1.x) - (q2.y * q1.y) - (q2.z * q1.z);
	return result;
};

bool MathUtils::Float3AreEqual(XMFLOAT3& p1, XMFLOAT3& p2)
{
	if (p1.x == p2.x && p1.y == p2.y && p1.z == p2.z)
	{
		return true;
	}

	return false;
};

bool MathUtils::Float4AreEqual(XMFLOAT4& v1, XMFLOAT4& v2)
{
	if (v1.x == v2.x && v1.y == v2.y && v1.z == v2.z && v1.w == v2.w)
	{
		return true;
	}

	return false;
};

XMFLOAT3 MathUtils::NormalizeFloat3(XMFLOAT3 v)
{
	float length = sqrt(pow(v.x, 2) + pow(v.y, 2)
		+ pow(v.z, 2));
	length = 1.0f / length;
	v.x *= length;
	v.y *= length;
	v.z *= length;
	return v;
};

XMFLOAT3 MathUtils::AddFloat3(XMFLOAT3& v1, XMFLOAT3& v2)
{
	return XMFLOAT3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
};

XMFLOAT3 MathUtils::SubtractFloat3(XMFLOAT3& v1, XMFLOAT3& v2)
{
	return XMFLOAT3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
};

XMFLOAT3 MathUtils::MultiplyFloat3(XMFLOAT3& v1, XMFLOAT3& v2)
{
	return XMFLOAT3(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
};

XMFLOAT3 MathUtils::ScaleFloat3(XMFLOAT3& v, float scale)
{
	return XMFLOAT3(v.x * scale, v.y * scale, v.z * scale);
};

XMFLOAT3 MathUtils::GetCrossProduct(XMFLOAT3& v1, XMFLOAT3& v2)
{
	XMFLOAT3 result;
	result.x = (v1.y * v2.z) - (v1.z * v2.y);
	result.y = -1 * ((v1.x * v2.z) - (v1.z * v2.x));
	result.z = (v1.x * v2.y) - (v1.y * v2.x);
	return result;
}

void MathUtils::CreatePxVec3Vector(std::vector<DVertex>& inVector, std::vector<PxVec3>& outVector)
{
	if (!outVector.empty())
	{
		return;
	}
	for (int i = 0; i < inVector.size(); i++)
	{
		XMFLOAT3 float3 = inVector[i].pos;
		outVector.push_back(PxVec3(float3.x, float3.y, float3.z));
	}
}

void MathUtils::CreatePxU32Vector(std::vector<DWORD>& inVector, std::vector<physx::PxU32>& outVector)
{
	if (!outVector.empty())
	{
		return;
	}
	for (int i = 0; i < inVector.size(); i++)
	{
		outVector.push_back(inVector[i]);
	}
}
