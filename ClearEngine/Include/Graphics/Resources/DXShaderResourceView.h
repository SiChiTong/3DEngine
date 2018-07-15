#pragma once

#include "Common/DirectX.h"

class DXShaderResourceView 
{
public:
	static ID3D11ShaderResourceView* CreateShaderResourceView(ID3D11Device1* device, D3D11_SHADER_RESOURCE_VIEW_DESC& shaderResourceViewDesc, ID3D11Buffer* buffer = NULL);
	ID3D11ShaderResourceView* GetShaderResourceView();

protected:
	DXShaderResourceView();
	~DXShaderResourceView();
	void Update(ID3D11DeviceContext1* devCon, const void* dataSrc);
	ID3D11ShaderResourceView* shaderResourceView = 0;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
};

