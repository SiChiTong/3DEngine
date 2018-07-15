#include "Loading/GameAssets.h"

GameAssets::GameAssets(std::wstring modelPath, std::wstring texturePath, std::wstring animationPath)
	: modelPath(modelPath), texturePath(texturePath), animationPath(animationPath){}

GameAssets::GameAssets(){}

GameAssets::~GameAssets()
{
	GeneralUtils::ClearMapWithSecondEntryPointers<std::wstring, ModelAnimation>(animations);
	GeneralUtils::ClearMapWithSecondEntryPointers<std::wstring, ID3D11ShaderResourceView>(textures, true);
}

