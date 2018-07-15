#pragma once

#include "Common/DirectX.h"
#include "Utils/GeometryUtils.h"
#include "Geometry/GeometryTypes.h"

enum FrustumPlane
{
	NearPlane = 0,
	FarPlane,
	LeftPlane,
	RightPlane,
	TopPlane,
	BottomPlane
};

class Frustum
{
public:
	Frustum(CXMMATRIX matrix);

	const DirectX::XMFLOAT4& Near() const;
	const DirectX::XMFLOAT4& Far() const;
	const DirectX::XMFLOAT4& Left() const;
	const DirectX::XMFLOAT4& Right() const;
	const DirectX::XMFLOAT4& Top() const;
	const DirectX::XMFLOAT4& Bottom() const;

	DirectX::XMVECTOR NearVector() const;
	DirectX::XMVECTOR FarVector() const;
	DirectX::XMVECTOR LeftVector() const;
	DirectX::XMVECTOR RightVector() const;
	DirectX::XMVECTOR TopVector() const;
	DirectX::XMVECTOR BottomVector() const;

	const DirectX::XMFLOAT3* Corners() const;
	const DirectX::XMFLOAT4* Planes() const;

	DirectX::XMMATRIX Matrix() const;
	void SetMatrix(DirectX::CXMMATRIX matrix);
	void SetMatrix(const DirectX::XMFLOAT4X4& matrix);

	IntersectionTestResult Contains(const BoundingSphere& sphere) const;

private:
	Frustum();
	DirectX::XMFLOAT4X4 mMatrix;
	DirectX::XMFLOAT3 mCorners[8];
	DirectX::XMFLOAT4 mPlanes[6];
};


