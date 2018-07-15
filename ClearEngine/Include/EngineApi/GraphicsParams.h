#pragma once

struct GraphicsParams
{
	std::wstring appName;
	bool vSyncEnabled;
	float screenWidth;
	float screenHeight;
	bool windowed;
	std::wstring modelPath;
	std::wstring texturePath;
	std::wstring animationPath;
	bool multiSamplingenabled;
	UINT multiSamplingCount;
	UINT refreshRate;
	GraphicsParams()
	{
		appName = L"Clear Engine";
		vSyncEnabled = false;
		screenWidth = 1920;
		screenHeight = 1080;
		windowed = true;
		modelPath = L"";
		texturePath = L"";
		animationPath = L"";
		multiSamplingenabled = true;
		multiSamplingCount = 4;
		refreshRate = 60;
	}
};
