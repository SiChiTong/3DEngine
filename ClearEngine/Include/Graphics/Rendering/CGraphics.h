#pragma once

#include <map>
#include <string>
#include "Graphics/Camera/Camera.h"
#include "GraphicsHelper.h"
#include "EngineApi\APITypes.h"

class CGraphics
{
public:
	IDXGISwapChain1* GetSwapChain();
	ID3D11Device1* GetDevice();
	ID3D11DeviceContext1* GetDeviceContext();
	ID3D11RenderTargetView* GetRenderTargetView();
	ID3D11DepthStencilView* GetDepthStencilView();

protected:
	CGraphics(GraphicsParams graphicsParams);
	~CGraphics();
	IDXGISwapChain1* swapChain;
	ID3D11Device1* device;
	ID3D11DeviceContext1* deviceContext;
	ID3D11RenderTargetView* renderTargetView;
	ID3D11DepthStencilView* depthStencilView;
	GraphicsHelper* graphicsHelper;

private:
	void InitializeDirectX(GraphicsParams graphicsParams);
	void CreateViewPort(float screenWidth, float screenHeight);
	ID3D11Texture2D* depthStencilBuffer;
};
