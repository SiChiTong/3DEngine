#pragma once

#include "Common/DirectX.h"

class VertexShader
{
public:
	static VertexShader* CreateFromFile(ID3D11Device1* device, LPCWSTR fileName, const D3D10_SHADER_MACRO* macroDefinitions, LPD3D10INCLUDE includeInterface, LPCSTR functionName,
		D3D11_INPUT_ELEMENT_DESC* inputElementDescs, UINT numInputElements, UINT compileProcedure = 0);
	~VertexShader();
	void Bind(ID3D11DeviceContext1* devCon);
	ID3D11VertexShader* GetShader();
	ID3D11InputLayout* GetInputLayout();

private:
	VertexShader(); 
	ID3D11VertexShader* shader;
	ID3D11InputLayout* inputLayout;
};
