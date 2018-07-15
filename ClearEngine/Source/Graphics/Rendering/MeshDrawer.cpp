#include "Graphics/Rendering/MeshDrawer.h"
using namespace DirectX;

MeshDrawer::MeshDrawer(GraphicsHelper* graphicsHelper) 
	: graphicsHelper(graphicsHelper)
{
	Initialize();
}

MeshDrawer::~MeshDrawer()
{
	delete skinningVS;
	delete rigidVS;
	delete rigidInstancingVS;
	delete dPS;
	delete modelBuffer;
	delete meshBuffer;
	delete dSS;
	delete mirrorSS;
}

void MeshDrawer::Initialize()
{
	devCon = graphicsHelper->GetDeviceContext();

	std::wstring temp = graphicsHelper->GetShaderPath() + L"Mesh.fx";
	LPCWSTR shaderLocation = temp.c_str();

	//**********************Default Vertex Shader*************************
	D3D11_INPUT_ELEMENT_DESC rigidLayout[] =
    {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },  
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },  
		{ "NORMAL",	 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	UINT numElements = ARRAYSIZE(rigidLayout);
	rigidVS = VertexShader::CreateFromFile(graphicsHelper->GetDevice(), shaderLocation, 0, 0, "VS", rigidLayout, numElements);

	//**********************Skeletal Animation Vertex Shader*************************
	D3D11_INPUT_ELEMENT_DESC animLayout[] =
    {
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },  
		{ "UINT", 0, DXGI_FORMAT_R32_UINT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "UINT", 1, DXGI_FORMAT_R32_UINT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	numElements = ARRAYSIZE(animLayout);
	skinningVS = VertexShader::CreateFromFile(graphicsHelper->GetDevice(), shaderLocation, 0, 0, "ANIM_VS", animLayout, numElements);

	D3D11_INPUT_ELEMENT_DESC rigidInstancingLayout[] =
	{
		// Data from the vertex buffer
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },

		// Data from the instance buffer
		{ "WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
	};
	numElements = ARRAYSIZE(rigidInstancingLayout);
	rigidInstancingVS = VertexShader::CreateFromFile(graphicsHelper->GetDevice(), shaderLocation, 0, 0, "INST_VS", rigidInstancingLayout, numElements);

	//**********************Default Pixel Shader*************************
	dPS = PixelShader::CreateFromFile(graphicsHelper->GetDevice(), shaderLocation, 0, 0, "PS");

	modelBuffer = ConstantBuffer<ShaderModel>::Create(graphicsHelper->GetDevice());

	meshBuffer = ConstantBuffer<ShaderMesh>::Create(graphicsHelper->GetDevice());

	dSS = SamplerState::Create(graphicsHelper->GetDevice(), D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_WRAP, 0, 0, 16);
	mirrorSS = SamplerState::Create(graphicsHelper->GetDevice(), D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_MIRROR, 0, 0, 16);
}

void MeshDrawer::BindResources()
{
	ID3D11Buffer* buffers[2] = { modelBuffer->GetBuffer(), meshBuffer->GetBuffer() };
	devCon->VSSetConstantBuffers(1, 2, &buffers[0]);
	devCon->PSSetConstantBuffers(1, 2, &buffers[0]);
	devCon->PSSetSamplers(0, 1, &dSS->samplerState);
	dPS->Bind(devCon);
}

void MeshDrawer::BindRigidResources()
{
	rigidVS->Bind(devCon);
}

void MeshDrawer::BindInstancingRigidResources()
{
	rigidInstancingVS->Bind(devCon);
}

void MeshDrawer::BindSkeletalResources()
{
	skinningVS->Bind(devCon);
}

void MeshDrawer::BindInstancingSkeletalResources()
{
	//TODO Add logic here when there is a vertex shader for skeletal instancing.
}

void MeshDrawer::SetModelParameters(XMMATRIX& world)
{
	ShaderModel shaderModel;
	ZeroMemory(&shaderModel, sizeof(ShaderModel));
	shaderModel.world = XMMatrixTranspose(world);
	modelBuffer->Update(devCon, &shaderModel);
}

void MeshDrawer::SetMeshMaterial(SurfaceMaterial& material)
{
	ID3D11ShaderResourceView* diffuseTexture = nullptr;
	ID3D11ShaderResourceView* normalMap = nullptr;
	ID3D11ShaderResourceView* specularMap = nullptr;
	ID3D11ShaderResourceView* glossMap = nullptr;
 	for (MaterialTexture& texture : material.textures)
	{
		switch (texture.type)
		{
		case TextureTypeDiffuse:
			diffuseTexture = texture.data;
			break;
		case TextureTypeNormalMap:
			normalMap = texture.data;
			break;
		case TextureTypeSpecularMap:
			specularMap = texture.data;
			break;
		case TextureTypeSpecularPowerMap:
			glossMap = texture.data;
			break;
		}
	}

	ShaderMesh shaderMesh;
	ZeroMemory(&shaderMesh, sizeof(ShaderMesh));
	shaderMesh.diffuse = material.diffuse;
	shaderMesh.specular = material.specular;
	shaderMesh.hasDiffuseTexture = diffuseTexture != nullptr;
	shaderMesh.hasNormalMap = normalMap != nullptr;
	shaderMesh.hasSpecular = specularMap != nullptr;
	shaderMesh.hasGloss = glossMap != nullptr;
	shaderMesh.hasTransparency = false;
	
	meshBuffer->Update(devCon, &shaderMesh);
	if (diffuseTexture != nullptr || normalMap != nullptr || specularMap != nullptr || glossMap != nullptr)
	{
		ID3D11ShaderResourceView* resources[4] = { diffuseTexture, normalMap, specularMap, glossMap };
		devCon->PSSetShaderResources(5, 4, resources);
	}
}

void MeshDrawer::SetSkeletalParameters(ID3D11ShaderResourceView* joints, ID3D11ShaderResourceView* weights)
{
	ID3D11ShaderResourceView* resources[2] = { joints, weights };
	devCon->VSSetShaderResources(3, 2, resources);
}

void MeshDrawer::Render(UINT indexCount, UINT startIndexLocation, UINT baseVertexLocation)
{
	devCon->DrawIndexed(indexCount, startIndexLocation, baseVertexLocation);
}

void MeshDrawer::RenderInstanced(UINT indexCountPerInstance, UINT instanceCount, UINT startIndexLocation, UINT baseVertexLocation, UINT startInstanceLocation)
{
	devCon->DrawIndexedInstanced(indexCountPerInstance, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
}