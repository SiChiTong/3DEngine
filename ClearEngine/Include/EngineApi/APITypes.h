#pragma once

#include <DirectXMath.h>
#include <windows.h>

struct ProgramParams
{
	HINSTANCE hInstance;
	HINSTANCE hPrevInstance;
	LPSTR     lpCmdLine;
	int nCmdShow;
};

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
	uint32_t multiSamplingCount;
	uint32_t refreshRate;
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

struct PhysicsParams
{
	DirectX::XMFLOAT3 gravity;
	bool enableCCD;

	PhysicsParams()
	{
		gravity = DirectX::XMFLOAT3(0.0f, -9.81f, 0.0f);
		enableCCD = true;
	}
};

struct EngineParams
{
	ProgramParams programParams;
	GraphicsParams graphicsParams;
	PhysicsParams physicsParams;
};
