#pragma once

#include <DirectXMath.h>
#include <windows.h>

struct AppParams
{
	HINSTANCE hInstance;
	HINSTANCE hPrevInstance;
	LPSTR     lpCmdLine;
	int nCmdShow;
	std::wstring appName = L"Sample";
	std::wstring modelPath = L"";
	std::wstring texturePath = L"";
	std::wstring animationPath = L"";
};

struct GraphicsParams
{
	bool vSyncEnabled;
	float screenWidth;
	float screenHeight;
	bool windowed;
	bool multiSamplingenabled;
	uint32_t multiSamplingCount;
	uint32_t refreshRate;
	GraphicsParams()
	{
		vSyncEnabled = false;
		screenWidth = 1920;
		screenHeight = 1080;
		windowed = true;
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
	AppParams appParams;
	GraphicsParams graphicsParams;
	PhysicsParams physicsParams;
};
