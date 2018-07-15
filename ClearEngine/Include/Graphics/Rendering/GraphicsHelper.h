#pragma once

#include "Common/DirectX.h"
#include <vector>

class GraphicsHelper
{
public:
	GraphicsHelper(float screenWidth, float screenHeight, ID3D11DepthStencilView* depthStencilView, ID3D11RenderTargetView* renderTargetView, IDXGISwapChain1* swapChain,
	ID3D11Device1* device, ID3D11DeviceContext1* devCon);
	~GraphicsHelper();
	void SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY topology);
	IDXGISwapChain1* GetSwapChain();
	ID3D11Device1* GetDevice();
	ID3D11DeviceContext1* GetDeviceContext();
	ID3D11RenderTargetView* GetRenderTargetView();
	ID3D11DepthStencilView* GetDepthStencilView();
	void SetViewports(int numViewPorts, const D3D11_VIEWPORT* viewports);
	D3D11_VIEWPORT* GetViewport(int slot);
	std::wstring GetShaderPath();
	void EnableClockwiseCulling();
	void EnableCounterClockwiseCulling();
	void EnableWireframeRendering();
	void DisableCulling();
	void EnableAplhaBlending();
	void EnableColourBlending(float redFactor, float greenFactor, float blueFactor, float alphaFactor);
	void DisableBlending();
	void EnableFullDepth();
	void UseDefaultDpethStencilState();

private:
	void CreateBlendStates();
	void CreateRenderStates();
	void CreateDepthStencilStates();
	float screenWidth;
	float screenHeight;
	ID3D11DepthStencilView* depthStencilView;
	ID3D11RenderTargetView* renderTargetView;
	IDXGISwapChain1* swapChain;
	ID3D11Device1* device;
	ID3D11DeviceContext1* devCon;
	std::vector<D3D11_VIEWPORT> viewports;
	ID3D11BlendState* alphaBlend;
	ID3D11BlendState* colourBlend;
	ID3D11RasterizerState* cwCull;
	ID3D11RasterizerState* ccwCull;
	ID3D11RasterizerState* noCull;
	ID3D11RasterizerState* wireframe;
	ID3D11DepthStencilState* fullDepth;
};
