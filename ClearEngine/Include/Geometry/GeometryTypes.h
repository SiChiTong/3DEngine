#pragma once


#include <vector>
#include <list>
#include "Math/CMath.h"

struct Ray
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 direction;

	Ray() {}
	Ray(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 direction)
		: position(position), direction(direction) {}
};

struct AABB
{
	DirectX::XMFLOAT3 min;
	DirectX::XMFLOAT3 max;
};

struct SpatialAABB
{
	DirectX::XMFLOAT3 centre;
	DirectX::XMFLOAT3 halfExtents;
};

struct BoundingSphere
{
	//serialize

	DirectX::XMFLOAT3 centre = DirectX::XMFLOAT3(0, 0, 0);
	float radius = 0;

	BoundingSphere() {}
	BoundingSphere(DirectX::XMFLOAT3 centre, float radius)
		: centre(centre), radius(radius) {}
};

struct HeightMapInfo
{
	int terrainWidth;
	int terrainHeight;
	float minHeight;
	DirectX::XMFLOAT3* heightMap;	// Array to store terrain's vertex positions
};

struct FrameData
{
	//serialize

	int frameID;
	std::vector<float> frameData;
};

struct Joint
{
	//serialize

	std::wstring name;
	int parentID;
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT4 orientation;
};

struct ModelAnimation
{
	//serialize

	int numFrames;
	int numJoints;
	float frameRate;
	int numAnimatedComponents;

	float frameTime;
	float totalAnimTime;
	float currAnimTime;

	std::vector<std::vector<Joint>> frameSkeleton;
};

