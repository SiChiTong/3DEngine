#include "Graphics/Resources/SamplerState.h"

SamplerState* SamplerState::Create(ID3D11Device1* device, D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE textureAddressMode, FLOAT minLOD, FLOAT maxLOD, UINT maxAnisotropy)
{
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory( &samplerDesc, sizeof(samplerDesc) );
	samplerDesc.Filter = filter;
	samplerDesc.AddressU = textureAddressMode;
	samplerDesc.AddressV = textureAddressMode;
	samplerDesc.AddressW = textureAddressMode;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = minLOD;
	samplerDesc.MaxLOD = maxLOD;
	samplerDesc.MaxAnisotropy = maxAnisotropy;
	return CreateFromDesc(device, samplerDesc);
}

SamplerState* SamplerState::CreateFromDesc(ID3D11Device1* device, D3D11_SAMPLER_DESC& samplerDesc)
{
	SamplerState* samplerState = new SamplerState();
	device->CreateSamplerState(&samplerDesc, &samplerState->samplerState);
	return samplerState;
}

SamplerState::SamplerState() {}

SamplerState::~SamplerState() 
{
	Release();
}

void SamplerState::Release()
{
	this->samplerState->Release();
}

