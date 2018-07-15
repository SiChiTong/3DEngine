
#pragma once


#include <vector>
#include <list>
#include "Common/DirectX.h"
#include "Graphics/Rendering/GraphicsTypes.h"
#include "Graphics/Resources/VertexBuffer.h"
#include "Graphics/Resources/IndexBuffer.h"
#include "Graphics/Resources/StructuredBuffer.h"

struct ModelSubset
{
	//serialize

	int matIndex = 0;
	int vertStartIndex = 0;
	UINT indexCount = 0;
	UINT vertexCount = 0;
	AABB aabb;
	BoundingSphere boundingSphere;
	//rigid model meshes only
	std::vector<DirectX::XMFLOAT3> positions;
	//skeletal model meshes only
	std::vector<UINT> jointIndices;
};

struct RigidModel
{
	//serialize

	std::vector<DVertex> vertices;
	std::vector<DWORD> indices;
	std::vector<SurfaceMaterial> materials;
	std::vector<ModelSubset> subsets;
	VertexBuffer<DVertex>* vertexBuffer;
	IndexBuffer* indexBuffer;
	BoundingSphere boundingSphere;
};

struct SkeletalModel
{
	//serialize

	std::vector<SurfaceMaterial> materials;
	std::vector<AnimVertex> vertices;
	std::vector<DWORD> indices;
	VertexBuffer<AnimVertex>* vertexBuffer;
	IndexBuffer* indexBuffer;
	std::vector<Joint> joints;
	std::vector<Weight> weights;
	std::vector<ModelSubset> subsets;
	//Structured buffers used for gpu skinning.
	StructuredBuffer<ShaderJoint>* jointSB;
	StructuredBuffer<Weight>* weightSB;
	//Bounding spheres for each bone in bone local space.
	//std::vector<BoundingSphere> boneSpheres;
	BoundingSphere boundingSphere;
};
