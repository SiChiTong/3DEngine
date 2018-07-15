#include "Graphics/Camera/Frustum.h"
#include "Math/SIMDConversionUtils.h"

using namespace DirectX;

Frustum::Frustum(CXMMATRIX matrix)
	: mMatrix(), mCorners(), mPlanes()
{
	SetMatrix(matrix);
}

const XMFLOAT4& Frustum::Near() const
{
	return mPlanes[NearPlane];
}

const XMFLOAT4& Frustum::Far() const
{
	return mPlanes[FarPlane];
}

const XMFLOAT4& Frustum::Left() const
{
	return mPlanes[LeftPlane];
}

const XMFLOAT4& Frustum::Right() const
{
	return mPlanes[RightPlane];
}

const XMFLOAT4& Frustum::Top() const
{
	return mPlanes[TopPlane];
}

const XMFLOAT4& Frustum::Bottom() const
{
	return mPlanes[BottomPlane];
}

XMVECTOR Frustum::NearVector() const
{
	return FV(mPlanes[NearPlane]);
}

XMVECTOR Frustum::FarVector() const
{
	return FV(mPlanes[FarPlane]);
}

XMVECTOR Frustum::LeftVector() const
{
	return FV(mPlanes[LeftPlane]);
}

XMVECTOR Frustum::RightVector() const
{
	return FV(mPlanes[RightPlane]);
}

XMVECTOR Frustum::TopVector() const
{
	return FV(mPlanes[TopPlane]);
}

XMVECTOR Frustum::BottomVector() const
{
	return FV(mPlanes[BottomPlane]);
}

const XMFLOAT3* Frustum::Corners() const
{
	return mCorners;
}

const XMFLOAT4* Frustum::Planes() const
{
	return mPlanes;
}

XMMATRIX Frustum::Matrix() const
{
	return FM(mMatrix);
}

void Frustum::SetMatrix(CXMMATRIX matrix)
{
	XMFLOAT4X4 m = MF(matrix);
	SetMatrix(m);
}

void Frustum::SetMatrix(const XMFLOAT4X4& matrix)
{
	mMatrix = matrix;

	//Front Plane
	mPlanes[0].x = -matrix._13;
	mPlanes[0].y = -matrix._23;
	mPlanes[0].z = -matrix._33;
	mPlanes[0].w = -matrix._43;

	//Back Plane
	mPlanes[1].x = -matrix._14 + matrix._13;
	mPlanes[1].y = -matrix._24 + matrix._23;
	mPlanes[1].z = -matrix._34 + matrix._33;
	mPlanes[1].w = -matrix._44 + matrix._43;

	//Left Plane
	mPlanes[2].x = -matrix._14 - matrix._11;
	mPlanes[2].y = -matrix._24 - matrix._21;
	mPlanes[2].z = -matrix._34 - matrix._31;
	mPlanes[2].w = -matrix._44 - matrix._41;

	//Right Plane
	mPlanes[3].x = -matrix._14 + matrix._11;
	mPlanes[3].y = -matrix._24 + matrix._21;
	mPlanes[3].z = -matrix._34 + matrix._31;
	mPlanes[3].w = -matrix._44 + matrix._41;

	//Top Plane
	mPlanes[4].x = -matrix._14 + matrix._12;
	mPlanes[4].y = -matrix._24 + matrix._22;
	mPlanes[4].z = -matrix._34 + matrix._32;
	mPlanes[4].w = -matrix._44 + matrix._42;

	//Bottom Plane
	mPlanes[5].x = -matrix._14 - matrix._12;
	mPlanes[5].y = -matrix._24 - matrix._22;
	mPlanes[5].z = -matrix._34 - matrix._32;
	mPlanes[5].w = -matrix._44 - matrix._42;

	for (int i = 0; i < 6; i++)
	{
		XMVECTOR vector = XMVectorSet(mPlanes[i].x, mPlanes[i].y, mPlanes[i].z, mPlanes[i].w);
		XMVECTOR length = XMVector3Length(vector);

		mPlanes[i] = VF4(XMVectorDivide(vector, length));
	}

	//Front Left Side
	Ray ray = GeometryUtils::ComputeIntersectionLine(FV(mPlanes[0]), FV(mPlanes[2]));
	//Front Top Left Corner
	mCorners[0] = VF3(GeometryUtils::ComputeIntersection(FV(mPlanes[4]), ray));
	//Front Bottom Left Corner
	mCorners[3] = VF3(GeometryUtils::ComputeIntersection(FV(mPlanes[5]), ray));

	//Front Right Side
	ray = GeometryUtils::ComputeIntersectionLine(FV(mPlanes[3]), FV(mPlanes[0]));
	//Front Top Right Corner
	mCorners[1] = VF3(GeometryUtils::ComputeIntersection(FV(mPlanes[4]), ray));
	//Front Bottom Right Corner
	mCorners[2] = VF3(GeometryUtils::ComputeIntersection(FV(mPlanes[5]), ray));

	//Back Left Side
	ray = GeometryUtils::ComputeIntersectionLine(FV(mPlanes[2]), FV(mPlanes[1]));
	//Back Top Left Corner
	mCorners[4] = VF3(GeometryUtils::ComputeIntersection(FV(mPlanes[4]), ray));
	//Back Bottom Left Corner
	mCorners[7] = VF3(GeometryUtils::ComputeIntersection(FV(mPlanes[5]), ray));

	//Back Right Side
	ray = GeometryUtils::ComputeIntersectionLine(FV(mPlanes[1]), FV(mPlanes[3]));
	//Back Top Right Corner
	mCorners[5] = VF3(GeometryUtils::ComputeIntersection(FV(mPlanes[4]), ray));
	//Back Bottom Right Corner
	mCorners[6] = VF3(GeometryUtils::ComputeIntersection(FV(mPlanes[5]), ray));
}

IntersectionTestResult Frustum::Contains(const BoundingSphere& sphere) const
{
	float distance;

	// calculate our distances to each of the planes
	for (int i = 0; i < 6; ++i) {
		FXMVECTOR normal = FV(mPlanes[i]);

		// find the distance to this plane
		XMVECTOR dot = XMVector3Dot(normal, FV(sphere.centre));
		distance = XMVectorGetX(dot) + mPlanes[i].w;

		// if this distance is < -sphere.radius, we are outside. (distance will be minus value if on outside side)
		if (distance < -sphere.radius)
			return(OUTSIDE);

		// else if the distance is between +- radius, then we intersect
		if ((float)fabs(distance) < sphere.radius)
			return(INTERSECTING);
	}

	// otherwise sohere is fully in view
	return(INSIDE);
}



