#pragma once

#pragma comment(lib,"assimp.lib")

#include <map>
#include <vector>
#include <Importer.hpp>
#include <scene.h>
#include <postprocess.h>
#include "Common/DirectX.h"
#include "Geometry/Models.h"
#include "Geometry/GeometryTypes.h"
#include "Graphics/Rendering/GraphicsTypes.h"

class AssetLoadingUtil
{
public:
	static void ExtractMaterialsFromScene(const aiScene* scene, std::vector<SurfaceMaterial>& materials);
	static bool MaterialHasTextureType(SurfaceMaterial& material, MaterialTextureType textureType);
	static void ExtractRigidMeshesFromScene(const aiScene* scene, RigidModel& model);
	static void ExtractSkeletalMeshesFromScene(const aiScene* scene, SkeletalModel& model);
	static void ExtractAnimationsFromScene(const aiScene* scene, std::map<std::wstring, ModelAnimation*>& animations, SkeletalModel& model, XMFLOAT4 rootBoneRotation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	static std::map<MaterialTextureType, UINT> textureTypeMappings;

private:
	struct BoneMeshInfo
	{
		aiBone* bone;
		aiMesh* mesh;
		UINT meshIndex;
		UINT startVertCount;

		BoneMeshInfo(aiBone* bone, aiMesh* mesh, UINT meshIndex, UINT startVertCount)
			: bone(bone), mesh(mesh), meshIndex(meshIndex), startVertCount(startVertCount) {}
	};
	struct BoneInfo 
	{
		UINT index;
		std::vector<BoneMeshInfo> data;
	};
	static void ExtractRigidMeshesFromNode(const aiScene* scene, const aiNode* node, RigidModel& model, std::vector<UINT>& meshIndices);
	static void ExtractBonesFromScene(aiNode* node, aiNode* parentBone, SkeletalModel& model, std::map<std::wstring, BoneInfo*>& boneData, std::vector<std::vector<Weight>>& weightData, UINT level, UINT targetLevel,
		int parentBoneIndex, bool& greaterDepth);
	static std::wstring FixBoneName(std::wstring name);
};
