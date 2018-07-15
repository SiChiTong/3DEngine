#include "Graphics/Resources/PixelShader.h"

PixelShader* PixelShader::CreateFromFile(ID3D11Device1* device, LPCWSTR fileName, const D3D10_SHADER_MACRO* macroDefinitions, LPD3D10INCLUDE includeInterface, LPCSTR functionName, UINT compileProcedure)
{
	LPCSTR shaderModel;
	D3D_FEATURE_LEVEL featureLevel = device->GetFeatureLevel();
	switch (featureLevel)
    {
		case D3D_FEATURE_LEVEL_10_0:
		case D3D_FEATURE_LEVEL_10_1:
            shaderModel = "ps_4_0";
            break;
		default:
			shaderModel = "ps_5_0";
			break;
    }
	PixelShader* pixelShader = new PixelShader();
	ID3D10Blob* shaderBuffer = nullptr;
	ID3D10Blob* errors = nullptr;
	HRESULT hr = D3DCompileFromFile(fileName, 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, functionName, shaderModel, compileProcedure, 0, &shaderBuffer, &errors);
	hr = device->CreatePixelShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), NULL, &pixelShader->shader);
	shaderBuffer->Release();
	return pixelShader;
}

PixelShader::PixelShader() {}

PixelShader::~PixelShader()
{
	shader->Release();
}

void PixelShader::Bind(ID3D11DeviceContext1* devCon)
{
	devCon->PSSetShader(shader, 0, 0);
}

ID3D11PixelShader* PixelShader::GetShader()
{
	return shader;
}