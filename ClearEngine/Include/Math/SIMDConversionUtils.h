#pragma once

#include "Common/DirectX.h"

using namespace DirectX;

class SIMDConversionUtils
{

public:
	XMVECTOR FV(XMFLOAT3 float3) const;
	XMFLOAT3 VF3(XMVECTOR vector) const;
	XMVECTOR FV(XMFLOAT4 float4) const;
	XMFLOAT4 VF4(XMVECTOR vector) const;
	XMMATRIX FM(XMFLOAT4X4 float4x4) const;
	XMFLOAT4X4 MF(XMMATRIX matrix) const;
};

inline XMVECTOR FV(XMFLOAT3 float3) 
{
	return XMLoadFloat3(&float3);
}

inline XMFLOAT3 VF3(XMVECTOR vector) 
{
	XMFLOAT3 float3;
	XMStoreFloat3(&float3, vector);
	return float3;
}

inline XMVECTOR FV(XMFLOAT4 float4) 
{
	return XMLoadFloat4(&float4);
}

inline XMFLOAT4 VF4(XMVECTOR vector) 
{
	XMFLOAT4 float4;
	XMStoreFloat4(&float4, vector);
	return float4;
}

inline XMMATRIX FM(XMFLOAT4X4 float4x4)
{
	return XMLoadFloat4x4(&float4x4);
}

inline XMFLOAT4X4 MF(XMMATRIX matrix)
{
	XMFLOAT4X4 float4x4;
	XMStoreFloat4x4(&float4x4, matrix);
	return float4x4;
}

