#pragma once

#include "Common/DirectX.h"
#include "Models.h"
#include "GeometryTypes.h"

typedef enum IntersectionTestResult
{
	INSIDE,
	INTERSECTING,
	OUTSIDE
};

class GeometryUtils
{
public:
	static void ComputeRigidModelData(RigidModel& model);
	static void CreateRigidModelAABB(RigidModel& model);
	static void CreateModelSubsetAABB(RigidModel& model, int subsetIndex);
	static void ComputeSkeletalModelData(SkeletalModel& model);
	static void CreateRigidModelObjects(ID3D11Device1* device, RigidModel& model);
	static void CreateSkeletalModelObjects(ID3D11Device1* device, SkeletalModel& model);
	static void CreateMeshPolygon(const std::vector<DVertex>& vertices, int index1, int index2, int index3, MeshPolygon& meshPolygon);
	static void CreateBoundingSphere(std::vector<DirectX::XMFLOAT3> const& points, BoundingSphere& boundingSphere);
	static void MergeBoundingSpheres(BoundingSphere const& sphere1, BoundingSphere const& sphere2, BoundingSphere& merge);
	static void MergeBoundingSpheres(std::vector<BoundingSphere> const& spheres, BoundingSphere& merge);
	static Ray ComputeIntersectionLine(DirectX::FXMVECTOR p1, DirectX::FXMVECTOR p2);
	static DirectX::XMVECTOR ComputeIntersection(DirectX::FXMVECTOR& plane, const Ray& ray);
};

