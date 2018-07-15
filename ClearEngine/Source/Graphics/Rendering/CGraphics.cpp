#include "Graphics/Rendering/CGraphics.h"
#include <list>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <vector>

using namespace std;

CGraphics::CGraphics(GraphicsParams graphicsParams)
{
	InitializeDirectX(graphicsParams);
}

CGraphics::~CGraphics()
{
	delete graphicsHelper;
	depthStencilView->Release();
	depthStencilBuffer->Release();
	renderTargetView->Release();
	swapChain->Release();
	device->Release();
	deviceContext->Release();
}

void CGraphics::InitializeDirectX(GraphicsParams graphicsParams)
{
	ID3D11Device* tempDevice = nullptr;
	ID3D11DeviceContext* tempDevCon = nullptr;

	D3D_FEATURE_LEVEL dxFeatureLevel = D3D_FEATURE_LEVEL_11_1;

	HRESULT hr;
	
	//Create our Direct3D 11 Device and SwapChain
	if (FAILED(hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG, NULL, NULL, D3D11_SDK_VERSION, &tempDevice, &dxFeatureLevel, &tempDevCon)))
	{
		MessageBox(NULL, L"D3D11CreateDevice() failed.",
			L"Error", MB_OK | MB_ICONERROR);
		return;
	}
	if (FAILED(hr = tempDevice->QueryInterface(__uuidof(ID3D11Device1), reinterpret_cast<void**>(&device))))
	{
		tempDevice->Release();
		tempDevCon->Release();
		MessageBox(NULL, L"ID3D11Device::QueryInterface() failed.",
			L"Error", MB_OK | MB_ICONERROR);
		return;
	}

	if (FAILED(hr = tempDevCon->QueryInterface(__uuidof(ID3D11DeviceContext1), reinterpret_cast<void**>(&deviceContext))))
	{
		tempDevice->Release();
		tempDevCon->Release();
		MessageBox(NULL, L"ID3D11DeviceContect::QueryInterface() failed.",
			L"Error", MB_OK | MB_ICONERROR);
		return;
	}

	tempDevice->Release();
	tempDevCon->Release();

	IDXGIDevice* dxgiDevice = nullptr;
	if (FAILED(hr = device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice))))
	{
		MessageBox(NULL, L"ID3D11Device::QueryInterface() failed.",
			L"Error", MB_OK | MB_ICONERROR);
		return;
	}

	IDXGIAdapter* dxgiAdapter = nullptr;
	if (FAILED(hr = dxgiDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&dxgiAdapter))))
	{
		dxgiDevice->Release();
		MessageBox(NULL, L"IDXGIDevice::GetParent() failed retrieving adapter.",
			L"Error", MB_OK | MB_ICONERROR);
		return;

	}

	IDXGIFactory2* dxgiFactory = nullptr;
	if (FAILED(hr = dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(&dxgiFactory))))
	{
		dxgiDevice->Release();
		dxgiAdapter->Release();
		MessageBox(NULL, L"IDXGIAdapter::GetParent() failed retrieving factory.",
			L"Error", MB_OK | MB_ICONERROR);
		return;
	}

	//Describe our SwapChain
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC1));
	swapChainDesc.Width = graphicsParams.screenWidth;
	swapChainDesc.Height = graphicsParams.screenHeight;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	//Multisampling quality level
	UINT qualityLevel;
	if (graphicsParams.multiSamplingenabled)
	{
		device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, graphicsParams.multiSamplingCount, &qualityLevel);
		if (qualityLevel == 0)
		{
			dxgiDevice->Release();
			dxgiAdapter->Release();
			dxgiFactory->Release();
			MessageBox(NULL, L"Unsupported multisampling quality level.",
				L"Error", MB_OK | MB_ICONERROR);
			return;
		}
		swapChainDesc.SampleDesc.Count = graphicsParams.multiSamplingCount;
		swapChainDesc.SampleDesc.Quality = qualityLevel - 1;
	}
	else
	{
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
	}

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullScreenDesc;
	ZeroMemory(&fullScreenDesc, sizeof(fullScreenDesc));
	fullScreenDesc.RefreshRate.Denominator = 1;
	fullScreenDesc.RefreshRate.Numerator = graphicsParams.refreshRate;
	fullScreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	fullScreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	fullScreenDesc.Windowed = graphicsParams.windowed;

	if (FAILED(hr = dxgiFactory->CreateSwapChainForHwnd(dxgiDevice, mWindowHandle, &swapChainDesc, &fullScreenDesc, NULL, &swapChain)))
	{
		dxgiDevice->Release();
		dxgiAdapter->Release();
		dxgiFactory->Release();
		MessageBox(NULL, L"IDXGIDevice::CreateSwapChainForHwnd() failed.",
			L"Error", MB_OK | MB_ICONERROR);
		return;
	}

	dxgiDevice->Release();
	dxgiAdapter->Release();
	dxgiFactory->Release();

	ID3D11Texture2D* backBuffer;
	//Create our BackBuffer and Render Target
	hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
	hr = device->CreateRenderTargetView(backBuffer, NULL, &renderTargetView);

	backBuffer->Release();

	//Describe our Depth/Stencil Buffer
	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = graphicsParams.screenWidth;
	depthStencilDesc.Height = graphicsParams.screenHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; //24 bits for the depth and 8 bits for the stencil.
	depthStencilDesc.SampleDesc.Count = swapChainDesc.SampleDesc.Count;
	depthStencilDesc.SampleDesc.Quality = swapChainDesc.SampleDesc.Quality;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;
	device->CreateTexture2D(&depthStencilDesc, NULL, &depthStencilBuffer);

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	device->CreateDepthStencilView(depthStencilBuffer, &depthStencilViewDesc, &depthStencilView);

	graphicsHelper = new GraphicsHelper(graphicsParams.screenWidth, graphicsParams.screenHeight, depthStencilView, renderTargetView, swapChain, device, deviceContext);

	graphicsHelper->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	CreateViewPort(graphicsParams.screenWidth, graphicsParams.screenHeight);
}

void CGraphics::CreateViewPort(float screenWidth, float screenHeight)
{
	//Create the Viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = screenWidth;
	viewport.Height = screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	//Set the Viewport
	graphicsHelper->SetViewports(1, &viewport);
}

IDXGISwapChain1* CGraphics::GetSwapChain()
{
	return swapChain;
}

ID3D11Device1* CGraphics::GetDevice()
{
	return device;
}

ID3D11DeviceContext1* CGraphics::GetDeviceContext()
{
	return deviceContext;
}

ID3D11RenderTargetView* CGraphics::GetRenderTargetView()
{
	return renderTargetView;
}

ID3D11DepthStencilView* CGraphics::GetDepthStencilView()
{
	return depthStencilView;
}

