#pragma once

#include "Common/DirectX.h"

class PixelShader
{
public:
	static PixelShader* CreateFromFile(ID3D11Device1* device, LPCWSTR fileName, const D3D10_SHADER_MACRO* macroDefinitions, LPD3D10INCLUDE includeInterface, LPCSTR functionName, UINT compileProcedure = 0);
	~PixelShader();
	void Bind(ID3D11DeviceContext1* devCon);
	ID3D11PixelShader* GetShader();
private:
	PixelShader(); 
	ID3D11PixelShader* shader;
};
