#pragma once

#include <map>
#include "Common/DirectX.h"
#include "Geometry/Models.h"
#include "Common/GeneralUtils.h"

class GameAssets
{
public:
	GameAssets(std::wstring modelPath, std::wstring texturePath, std::wstring animationPath);
	GameAssets();
	~GameAssets();
	std::map<std::wstring,RigidModel> rigidModels;
	std::map<std::wstring,SkeletalModel> skeletalModels;
	std::map<std::wstring, ModelAnimation*> animations;
	std::map<std::wstring,ID3D11ShaderResourceView*> textures;
	std::wstring modelPath;
	std::wstring texturePath;
	std::wstring animationPath;
};
