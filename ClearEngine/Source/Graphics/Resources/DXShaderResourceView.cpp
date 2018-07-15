#include "Graphics/Resources/DXShaderResourceView.h"

DXShaderResourceView::DXShaderResourceView() {}

DXShaderResourceView::~DXShaderResourceView()
{
	shaderResourceView->Release();
}

ID3D11ShaderResourceView* DXShaderResourceView::CreateShaderResourceView(ID3D11Device1* device, D3D11_SHADER_RESOURCE_VIEW_DESC& shaderResourceViewDesc, ID3D11Buffer* buffer)
{
	ID3D11ShaderResourceView* shaderResourceView; 
	HRESULT hr = device->CreateShaderResourceView(buffer, &shaderResourceViewDesc, &shaderResourceView);
	return shaderResourceView;
}

ID3D11ShaderResourceView* DXShaderResourceView::GetShaderResourceView()
{
	return shaderResourceView;
}