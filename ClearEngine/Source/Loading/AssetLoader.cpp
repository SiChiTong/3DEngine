#include "Loading/AssetLoader.h"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <istream>
#include <map>
#include <fstream>
#include "Loading/Textures/DDSTextureLoader.h"
#include "Loading/Textures/WICTextureLoader.h"
#include "Loading/AssetLoadingUtil.h"
#include "Common/GeneralUtils.h"
#include "Math/MathUtils.h"
#include "Graphics/Resources/VertexBuffer.h"
#include "Graphics/Resources/IndexBuffer.h"
#include "Graphics/Resources/StructuredBuffer.h"
#include "Graphics/Rendering/GraphicsHelper.h"
#include "Utils/GeometryUtils.h"

using namespace DirectX;

AssetLoader::AssetLoader(ID3D11Device1* device, ID3D11DeviceContext1* devCon, IDXGISwapChain1* swapChain, GameAssets* gameAssets)
: device(device), devCon(devCon), swapChain(swapChain), gameAssets(gameAssets)
{
	AssetLoadingUtil::textureTypeMappings[TextureTypeDiffuse] = aiTextureType_DIFFUSE;
	AssetLoadingUtil::textureTypeMappings[TextureTypeSpecularMap] = aiTextureType_SPECULAR;
	AssetLoadingUtil::textureTypeMappings[TextureTypeAmbient] = aiTextureType_AMBIENT;
	AssetLoadingUtil::textureTypeMappings[TextureTypeHeightMap] = aiTextureType_HEIGHT;
	AssetLoadingUtil::textureTypeMappings[TextureTypeNormalMap] = aiTextureType_NORMALS;
	AssetLoadingUtil::textureTypeMappings[TextureTypeSpecularPowerMap] = aiTextureType_SHININESS;
	AssetLoadingUtil::textureTypeMappings[TextureTypeDisplacementMap] = aiTextureType_DISPLACEMENT;
}

AssetLoader::~AssetLoader() {}

bool AssetLoader::LoadRigidModel(std::wstring filePath, std::wstring modelName, bool flipUVs, bool flipWindingOrder)
{
	Assimp::Importer importer;
	UINT flags = aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType;
	if (flipWindingOrder)
	{
		//These need to be on together or it will not work properly.
		flags |= aiProcess_FlipWindingOrder;
		flags |= aiProcess_MakeLeftHanded;
	}
	if (flipUVs)
	{
		flags |= aiProcess_FlipUVs;
	}
	std::string dest;
	GeneralUtils::WStringToString(gameAssets->modelPath + filePath, dest);
	const aiScene* scene = importer.ReadFile(dest, flags);
	if (!scene)
	{
		return false;
	}
	RigidModel& model = gameAssets->rigidModels[modelName];
	AssetLoadingUtil::ExtractMaterialsFromScene(scene, model.materials);
	if (scene->HasMeshes())
	{
		AssetLoadingUtil::ExtractRigidMeshesFromScene(scene, model);
	}
	CreateGraphicsObjectsForRigidModel(model);
	return true;
}

bool AssetLoader::LoadSkeletalModel(std::wstring filePath, std::wstring modelName, bool flipUVs, bool flipWindingOrder, XMFLOAT4 rootBoneRotation)
{
	Assimp::Importer importer;
	UINT flags = aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType;
	if (flipWindingOrder)
	{
		//These need to be on together or it will not work properly.
		flags |= aiProcess_FlipWindingOrder;
		flags |= aiProcess_MakeLeftHanded;
	}
	if (flipUVs)
	{
		flags |= aiProcess_FlipUVs;
	}
	std::string dest;
	GeneralUtils::WStringToString(gameAssets->modelPath + filePath, dest);
	const aiScene* scene = importer.ReadFile(dest, flags);
	if (!scene)
	{
		return false;
	}
	SkeletalModel& model = gameAssets->skeletalModels[modelName];
	AssetLoadingUtil::ExtractMaterialsFromScene(scene, model.materials);
	if (scene->HasMeshes())
	{
		AssetLoadingUtil::ExtractSkeletalMeshesFromScene(scene, model);
	}
	if (scene->HasAnimations())
	{
		AssetLoadingUtil::ExtractAnimationsFromScene(scene, gameAssets->animations, model, rootBoneRotation);
	}
	CreateGraphicsObjectsForSkeletalModel(model);
	return true;
}

void AssetLoader::CreateGraphicsObjectsForRigidModel(RigidModel& model)
{
	GeometryUtils::CreateRigidModelObjects(device, model);

	for (int i = 0; i < model.subsets.size(); i++)
	{
		SurfaceMaterial* material = &model.materials[model.subsets[i].matIndex];
		for (MaterialTexture& texture : material->textures)
		{
			bool result = LoadTexture(texture.name);
			texture.data = result ? gameAssets->textures[texture.name] : nullptr;
		}
	}
}


void AssetLoader::CreateGraphicsObjectsForSkeletalModel(SkeletalModel& model)
{
	GeometryUtils::CreateSkeletalModelObjects(device, model);

	for (int i = 0; i < model.subsets.size(); i++)
	{
		SurfaceMaterial* material = &model.materials[model.subsets[i].matIndex];
		for (MaterialTexture& texture : material->textures)
		{
			bool result = LoadTexture(texture.name);
			texture.data = result ? gameAssets->textures[texture.name] : nullptr;
		}
	}
}

bool AssetLoader::LoadTexture(std::wstring fileName)
{
	std::map<std::wstring, ID3D11ShaderResourceView*>::iterator iter = gameAssets->textures.find(fileName);
	if (iter != gameAssets->textures.end())
	{
		return true;
	}
	std::wstring filePath = gameAssets->texturePath + fileName;
	ID3D11ShaderResourceView* texture;
	HRESULT hr = DirectX::CreateWICTextureFromFile(device, devCon, &filePath[0], nullptr, &texture);
	if (texture)
	{
		gameAssets->textures[fileName] = texture;
	}
	return hr == S_OK;
}

bool AssetLoader::LoadCubeMap(std::wstring fileName)
{
	std::map<std::wstring, ID3D11ShaderResourceView*>::iterator iter = gameAssets->textures.find(fileName);
	if (iter != gameAssets->textures.end())
	{
		return true;
	}
	std::wstring filePath = gameAssets->texturePath + fileName;
	HRESULT hr;
	ID3D11Texture2D* smTexture = 0;
	hr = DirectX::CreateDDSTextureFromFile(device, &filePath[0], (ID3D11Resource**)&smTexture, 0);
	D3D11_TEXTURE2D_DESC smTextureDesc;
	smTexture->GetDesc(&smTextureDesc);
	D3D11_SHADER_RESOURCE_VIEW_DESC smViewDesc;
	smViewDesc.Format = smTextureDesc.Format;
	smViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	smViewDesc.TextureCube.MipLevels = smTextureDesc.MipLevels;
	smViewDesc.TextureCube.MostDetailedMip = 0;
	ID3D11ShaderResourceView* texture;
	hr = device->CreateShaderResourceView(smTexture, &smViewDesc, &texture);
	if (texture)
	{
		gameAssets->textures[fileName] = texture;
	}
	return hr == S_OK;
}


