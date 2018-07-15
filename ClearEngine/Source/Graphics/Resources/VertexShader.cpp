#include "Graphics/Resources/VertexShader.h"

VertexShader* VertexShader::CreateFromFile(ID3D11Device1* device, LPCWSTR fileName, const D3D10_SHADER_MACRO* macroDefinitions, LPD3D10INCLUDE includeInterface, LPCSTR functionName,
	D3D11_INPUT_ELEMENT_DESC* inputElementDescs, UINT numInputElements, UINT compileProcedure)
{
	LPCSTR shaderModel;
	D3D_FEATURE_LEVEL featureLevel = device->GetFeatureLevel();
	switch (featureLevel)
    {
		case D3D_FEATURE_LEVEL_10_0:
		case D3D_FEATURE_LEVEL_10_1:
            shaderModel = "vs_4_0";
            break;
		default:
			shaderModel = "vs_5_0";
			break;
    }
	VertexShader* vertexShader = new VertexShader();
	ID3D10Blob* shaderBuffer = nullptr;
	ID3D10Blob* errors = nullptr;
	HRESULT hr = D3DCompileFromFile(fileName, 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, functionName, shaderModel, compileProcedure, 0, &shaderBuffer, &errors);
	vertexShader->shader = nullptr;
	vertexShader->inputLayout = nullptr;
	hr = device->CreateVertexShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), nullptr, &vertexShader->shader);
	hr = device->CreateInputLayout(inputElementDescs, numInputElements, shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), &vertexShader->inputLayout);
	shaderBuffer->Release();
	return vertexShader;
}

VertexShader::VertexShader() {}

VertexShader::~VertexShader()
{
	shader->Release();
	inputLayout->Release();
}

void VertexShader::Bind(ID3D11DeviceContext1* devCon)
{
	devCon->VSSetShader(shader, 0, 0);
	devCon->IASetInputLayout(inputLayout);
}

ID3D11VertexShader* VertexShader::GetShader()
{
	return shader;
}
	
ID3D11InputLayout* VertexShader::GetInputLayout()
{
	return inputLayout;
}

