#pragma once

#include "Common/DirectX.h"

class SamplerState 
{
public:
	static SamplerState* Create(ID3D11Device1* device, D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE textureAddressMode, FLOAT minLOD, FLOAT maxLOD, UINT maxAnisotropy);
	static SamplerState* CreateFromDesc(ID3D11Device1* device, D3D11_SAMPLER_DESC& samplerDesc);
	~SamplerState();
	ID3D11SamplerState* samplerState;
	
private:
	SamplerState();
	void Release();
};
