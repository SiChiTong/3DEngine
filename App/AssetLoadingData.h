#ifndef AssetLoadingData_H
#define AssetLoadingData_H

#include <iostream>
#include <algorithm>
#include <vector>

//Serialize
struct RigidModelLoadingInfo
{
	bool loaded;
	std::wstring filePath;
};

//Serialize
struct AssetLoadingInfo
{
	std::wstring modelPath;
	std::wstring texturePath;
	std::wstring animationPath;
	std::vector<RigidModelLoadingInfo> rigidModelInfo;
};

#endif