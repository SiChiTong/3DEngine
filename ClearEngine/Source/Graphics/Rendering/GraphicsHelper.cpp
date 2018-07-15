#include "Graphics/Rendering/GraphicsHelper.h"

GraphicsHelper::GraphicsHelper(float screenWidth, float screenHeight, ID3D11DepthStencilView* depthStencilView, ID3D11RenderTargetView* renderTargetView, IDXGISwapChain1* swapChain,
	ID3D11Device1* device, ID3D11DeviceContext1* devCon)
	: screenWidth(screenWidth), screenHeight(screenHeight), depthStencilView(depthStencilView), renderTargetView(renderTargetView), swapChain(swapChain), device(device), devCon(devCon)
	{	
		CreateBlendStates();
		CreateRenderStates();
		CreateDepthStencilStates();
	}

	GraphicsHelper::~GraphicsHelper()
	{
		swapChain->SetFullscreenState(false, NULL);
		alphaBlend->Release();
		colourBlend->Release();
		cwCull->Release();
		ccwCull->Release();
		noCull->Release();
		wireframe->Release();
		fullDepth->Release();
	}
	
	IDXGISwapChain1* GraphicsHelper::GetSwapChain()
	{
		return swapChain;
	}

	ID3D11Device1* GraphicsHelper::GetDevice()
	{
		return device;
	}

	ID3D11DeviceContext1* GraphicsHelper::GetDeviceContext()
	{
		return devCon;
	}

	ID3D11RenderTargetView* GraphicsHelper::GetRenderTargetView()
	{
		return renderTargetView;
	}

	ID3D11DepthStencilView* GraphicsHelper::GetDepthStencilView()
	{
		return depthStencilView;
	}

	void GraphicsHelper::SetViewports(int numViewPorts, const D3D11_VIEWPORT* viewports)
	{
		this->viewports.clear();
		for (int i = 0; i < numViewPorts; i++) {
			const D3D11_VIEWPORT viewport = viewports[i];
			this->viewports.push_back(viewport);
		}

		devCon->RSSetViewports(numViewPorts, viewports);
	}

	D3D11_VIEWPORT* GraphicsHelper::GetViewport(int slot)
	{
		return &this->viewports.at(slot);
	}

	std::wstring GraphicsHelper::GetShaderPath()
	{
		return L"../ClearEngine/Graphics/Shaders/";
	}

	void GraphicsHelper::CreateBlendStates()
	{
		D3D11_BLEND_DESC blendDesc;

		//Alpha blend state
		ZeroMemory( &blendDesc, sizeof(blendDesc) );
		blendDesc.AlphaToCoverageEnable = true;
		blendDesc.IndependentBlendEnable = false;
		blendDesc.RenderTarget[0].BlendEnable = true;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;
		ID3D11BlendState* blendState;
		device->CreateBlendState(&blendDesc, &alphaBlend);
	
		//Colour blend state
		ZeroMemory( &blendDesc, sizeof(blendDesc) );
		blendDesc.AlphaToCoverageEnable = true;
		blendDesc.IndependentBlendEnable = false;
		blendDesc.RenderTarget[0].BlendEnable = true;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_COLOR;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_BLEND_FACTOR;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;
		device->CreateBlendState(&blendDesc, &colourBlend);
	}

	void GraphicsHelper::CreateRenderStates()
	{
		D3D11_RASTERIZER_DESC rastDesc;

		//Clock-wise culling
		ZeroMemory(&rastDesc, sizeof(D3D11_RASTERIZER_DESC));
		rastDesc.FillMode = D3D11_FILL_SOLID;
		rastDesc.CullMode = D3D11_CULL_BACK;
		rastDesc.FrontCounterClockwise = FALSE;
		rastDesc.DepthBias = 0;
		rastDesc.DepthBiasClamp = 0.0f; 
		rastDesc.SlopeScaledDepthBias = 0.0f;
		rastDesc.DepthClipEnable = TRUE;
		rastDesc.ScissorEnable = FALSE;
		rastDesc.MultisampleEnable = TRUE;
		rastDesc.AntialiasedLineEnable = TRUE;
		device->CreateRasterizerState(&rastDesc, &cwCull);
		
		//Counter-clockwise culling
		ZeroMemory(&rastDesc, sizeof(D3D11_RASTERIZER_DESC));
		rastDesc.FillMode = D3D11_FILL_SOLID;
		rastDesc.CullMode = D3D11_CULL_BACK;
		rastDesc.FrontCounterClockwise = TRUE;
		rastDesc.DepthBias = 0;
		rastDesc.DepthBiasClamp = 0.0f; 
		rastDesc.SlopeScaledDepthBias = 0.0f;
		rastDesc.DepthClipEnable = TRUE;
		rastDesc.ScissorEnable = FALSE;
		rastDesc.MultisampleEnable = TRUE;
		rastDesc.AntialiasedLineEnable = TRUE;
		device->CreateRasterizerState(&rastDesc, &ccwCull);
	
		//No culling
		ZeroMemory(&rastDesc, sizeof(D3D11_RASTERIZER_DESC));
		rastDesc.FillMode = D3D11_FILL_SOLID;
		rastDesc.CullMode = D3D11_CULL_NONE;
		rastDesc.FrontCounterClockwise = FALSE;
		rastDesc.DepthBias = 0;
		rastDesc.DepthBiasClamp = 0.0f; 
		rastDesc.SlopeScaledDepthBias = 0.0f;
		rastDesc.DepthClipEnable = TRUE;
		rastDesc.ScissorEnable = FALSE;
		rastDesc.MultisampleEnable = TRUE;
		rastDesc.AntialiasedLineEnable = TRUE;
		device->CreateRasterizerState(&rastDesc, &noCull);
	
		//Wireframe
		ZeroMemory(&rastDesc, sizeof(D3D11_RASTERIZER_DESC));
		rastDesc.FillMode = D3D11_FILL_WIREFRAME;
		rastDesc.CullMode = D3D11_CULL_NONE;
		rastDesc.FrontCounterClockwise = FALSE;
		rastDesc.DepthBias = 0;
		rastDesc.DepthBiasClamp = 0.0f; 
		rastDesc.SlopeScaledDepthBias = 0.0f;
		rastDesc.DepthClipEnable = TRUE;
		rastDesc.ScissorEnable = FALSE;
		rastDesc.MultisampleEnable = TRUE;
		rastDesc.AntialiasedLineEnable = TRUE;
		device->CreateRasterizerState(&rastDesc, &wireframe);
	}

	void GraphicsHelper::CreateDepthStencilStates() 
	{
		D3D11_DEPTH_STENCIL_DESC dssDesc;

		//Create depth stencil state used for sky.
		ZeroMemory(&dssDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
		dssDesc.DepthEnable = true;
		dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dssDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		dssDesc.StencilEnable = FALSE;
		dssDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
		dssDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

		device->CreateDepthStencilState(&dssDesc, &fullDepth);
	}

	void GraphicsHelper::SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY topology)
	{
		devCon->IASetPrimitiveTopology(topology);
	}

	//Cull counterclockwise polygons.
	void GraphicsHelper::EnableClockwiseCulling()
	{
		devCon->RSSetState(cwCull);
	}

	//Cull clockwise polygons.
	void GraphicsHelper::EnableCounterClockwiseCulling()
	{
		devCon->RSSetState(ccwCull);
	}

	void GraphicsHelper::DisableCulling()
	{
		devCon->RSSetState(noCull);
	}

	void GraphicsHelper::EnableWireframeRendering()
	{
		devCon->RSSetState(wireframe);
	}

	void GraphicsHelper::EnableAplhaBlending()
	{
		devCon->OMSetBlendState(alphaBlend, 0, 0xffffffff);	
	}

	void GraphicsHelper::EnableColourBlending(float redFactor, float greenFactor, float blueFactor, float alphaFactor)
	{
		float blendFactor[] = {redFactor, greenFactor, blueFactor, alphaFactor};
		devCon->OMSetBlendState(colourBlend, blendFactor, 0xffffffff);	
	}

	void GraphicsHelper::DisableBlending()
	{
		devCon->OMSetBlendState(0, 0, 0xffffffff);	
	}

	void GraphicsHelper::EnableFullDepth()
	{
		devCon->OMSetDepthStencilState(fullDepth, 0);
	}

	void GraphicsHelper::UseDefaultDpethStencilState()
	{
		devCon->OMSetDepthStencilState(NULL, 0);
	}


	
	

	