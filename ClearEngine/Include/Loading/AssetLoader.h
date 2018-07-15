#pragma once

#include "Common/DirectX.h"
#include "GameAssets.h"
#include "Geometry/Models.h"
#include "Graphics/Rendering/GraphicsTypes.h"


class AssetLoader
{
public:
	AssetLoader(ID3D11Device1* device, ID3D11DeviceContext1* devCon, IDXGISwapChain1* swapChain, GameAssets* gameAssets);
	~AssetLoader();
	bool LoadRigidModel(std::wstring filePath, std::wstring modelName, bool flipUVs, bool flipWindingOrder);
	bool LoadSkeletalModel(std::wstring filePath, std::wstring modelName, bool flipUVs, bool flipWindingOrder, XMFLOAT4 rootBoneRotation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	bool LoadTexture(std::wstring fileName);
	bool LoadCubeMap(std::wstring fileName);
private:
	void CreateGraphicsObjectsForRigidModel(RigidModel& model);
	void CreateGraphicsObjectsForSkeletalModel(SkeletalModel& model);
	ID3D11Device1* device;
	ID3D11DeviceContext1* devCon;
	IDXGISwapChain1* swapChain;
	GameAssets* gameAssets;
};
