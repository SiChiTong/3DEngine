#include "Graphics/Rendering/Sky.h"
using namespace std;
using namespace DirectX;

Sky::Sky(GraphicsHelper* graphicsHelper, int latLines, int longLines)
	: graphicsHelper(graphicsHelper), texture(texture)
{	
	Initialize();
	CreateSkyData(latLines, longLines);
}

Sky::~Sky() 
{
	delete vertexShader;
	delete pixelShader;
	delete constantBuffer;
}

void Sky::Initialize() 
{
	ID3D11Device1* device = graphicsHelper->GetDevice();

	std::wstring temp = graphicsHelper->GetShaderPath() + L"Sky.fx";
	LPCWSTR shaderLocation = temp.c_str();

	//**********************Sky Vertex Shader*************************
	D3D11_INPUT_ELEMENT_DESC inputLayoutArray[] =
    {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT numElements = ARRAYSIZE(inputLayoutArray);
	vertexShader = VertexShader::CreateFromFile(graphicsHelper->GetDevice(), shaderLocation, 0, 0, "VS", inputLayoutArray, numElements);
	pixelShader = PixelShader::CreateFromFile(device, shaderLocation, 0, 0, "PS");
	constantBuffer = ConstantBuffer<SkyCStruct>::Create(device);
	samplerState = SamplerState::Create(device, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP, 0, D3D11_FLOAT32_MAX, 1);
	
}

void Sky::CreateSkyData(int latLines, int longLines)
{
	ID3D11Device1* device = graphicsHelper->GetDevice();
	HRESULT hr;
	int numSphereVertices = ((latLines-2) * longLines) + 2;
	int numSphereFaces = ((latLines-3)*(longLines)*2) + (longLines*2);

	float sphereYaw = 0.0f;
	float spherePitch = 0.0f;

	std::vector<SkyVertex> vertices(numSphereVertices);

	XMVECTOR currVertPos = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	vertices[0].pos.x = 0.0f;
	vertices[0].pos.y = 1.0f;
	vertices[0].pos.z = 0.0f;

	XMMATRIX rotationX;
	XMMATRIX rotationY;
	for(DWORD i = 0; i < latLines-2; ++i)
	{
		spherePitch = (i+1) * (3.14/(latLines-1));
		rotationX = XMMatrixRotationX(spherePitch);
		for(DWORD j = 0; j < longLines; ++j)
		{
			sphereYaw = j * (6.28/(longLines));
			rotationY = XMMatrixRotationY(sphereYaw);
			currVertPos = XMVector3TransformNormal( XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), (rotationX * rotationY) );	
			currVertPos = XMVector3Normalize( currVertPos );
			int index = i*longLines+j+1;
			vertices[index].pos.x = XMVectorGetX(currVertPos);
			vertices[index].pos.y = XMVectorGetY(currVertPos);
			vertices[index].pos.z = XMVectorGetZ(currVertPos);
		}
	}

	vertices[numSphereVertices-1].pos.x = 0.0f;
	vertices[numSphereVertices-1].pos.y = -1.0f;
	vertices[numSphereVertices-1].pos.z = 0.0f;

	DXBuffer::BUFFER_LIFETIME bufferLifeTime = DXBuffer::FOREVER;
	vertexBuffer = VertexBuffer<SkyVertex>::Create(device, bufferLifeTime, numSphereVertices, &vertices[0]);

	int numIndices = numSphereFaces * 3;
	std::vector<DWORD>indices(numIndices);

	int k = 0;
	for(DWORD l = 0; l < longLines-1; ++l)
	{
		indices[k] = 0;
		indices[k+1] = l+1;
		indices[k+2] = l+2;
		k += 3;
	}

	indices[k] = 0;
	indices[k+1] = longLines;
	indices[k+2] = 1;
	k += 3;

	for(DWORD i = 0; i < latLines-3; ++i)
	{
		for(DWORD j = 0; j < longLines-1; ++j)
		{
			indices[k]   = i*longLines+j+1;
			indices[k+1] = i*longLines+j+2;
			indices[k+2] = (i+1)*longLines+j+1;

			indices[k+3] = (i+1)*longLines+j+1;
			indices[k+4] = i*longLines+j+2;
			indices[k+5] = (i+1)*longLines+j+2;

			k += 6; // next quad
		}

		indices[k]   = (i*longLines)+longLines;
		indices[k+1] = (i*longLines)+1;
		indices[k+2] = ((i+1)*longLines)+longLines;

		indices[k+3] = ((i+1)*longLines)+longLines;
		indices[k+4] = (i*longLines)+1;
		indices[k+5] = ((i+1)*longLines)+1;

		k += 6;
	}

	for(DWORD l = 0; l < longLines-1; ++l)
	{
		indices[k] = numSphereVertices-1;
		indices[k+1] = (numSphereVertices-1)-(l+1);
		indices[k+2] = (numSphereVertices-1)-(l+2);
		k += 3;
	}

	indices[k] = numSphereVertices-1;
	indices[k+1] = (numSphereVertices-1)-longLines;
	indices[k+2] = numSphereVertices-2;

	indexBuffer = IndexBuffer::Create(device, bufferLifeTime, numIndices, &indices[0]);
	indicesCount = numIndices;
}

void Sky::Draw(double time, const XMMATRIX& vp, const XMFLOAT3& camPosition, ID3D11ShaderResourceView* texture)
{
	if (texture == NULL)
	{
		return;
	}
	ID3D11DeviceContext1* devCon = graphicsHelper->GetDeviceContext();
	graphicsHelper->EnableFullDepth();
	graphicsHelper->DisableCulling();
	graphicsHelper->DisableBlending();
	vertexShader->Bind(devCon);
	pixelShader->Bind(devCon);
	vertexBuffer->Bind(devCon, 0);
	indexBuffer->Bind(devCon);
	//Define sphereWorld's world space matrix
	XMMATRIX scale = XMMatrixScaling(10.0f, 10.0f, 10.0f);
	//Make sure the sphere is always centered around camera

	SkyCStruct shaderParams;
	//float amount = (1 - (fabs(XMVectorGetY(camPosition)) / 30000)) * 10;
	XMMATRIX translation = XMMatrixTranslation(camPosition.x, camPosition.y, camPosition.z);
	shaderParams.wvp = XMMatrixTranspose(scale * translation * vp);	

	constantBuffer->Update(devCon, &shaderParams);
	ID3D11Buffer* buffer = constantBuffer->GetBuffer();
	devCon->VSSetConstantBuffers(0, 1, &buffer);
	devCon->PSSetShaderResources( 0, 1, &texture);
	devCon->PSSetSamplers(0, 1, &samplerState->samplerState); 
	devCon->DrawIndexed(indexBuffer->GetElementCount(), 0, 0 );
}

ID3D11ShaderResourceView* Sky::GetTexture()
{
	return this->texture;
}
