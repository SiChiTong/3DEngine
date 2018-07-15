#pragma once

#include <map>
#include "Graphics/Rendering/GraphicsHelper.h"
#include "Geometry/Models.h"
#include "Graphics/Resources/VertexShader.h"
#include "Graphics/Resources/PixelShader.h"
#include "Graphics/Resources/ConstantBuffer.h"
#include "Graphics/Resources/SamplerState.h"

class MeshDrawer {

public:
	MeshDrawer(GraphicsHelper* graphicsHelper);
	~MeshDrawer();
	void Initialize();
	void BindResources();
	void BindRigidResources();
	void BindInstancingRigidResources();
	void BindSkeletalResources();
	void BindInstancingSkeletalResources();
	void SetModelParameters(DirectX::XMMATRIX& world);
	void SetMeshMaterial(SurfaceMaterial& material);
	void SetSkeletalParameters(ID3D11ShaderResourceView* joints, ID3D11ShaderResourceView* weights);
	void Render(UINT indexCount, UINT startIndexLocation, UINT baseVertexLocation = 0);
	void RenderInstanced(UINT indexCountPerInstance, UINT instanceCount, UINT startIndexLocation, UINT baseVertexLocation, UINT startInstanceLocation);

private:
	//Structure of constant buffer. Sent to pixel shader for every mesh in an object.
	struct ShaderModel
	{
		DirectX::XMMATRIX world;
	};
	struct ShaderMesh
	{
		//Because of HLSL structure packing, we will use windows BOOL
		//instead of bool because HLSL packs things into 4 bytes, and
		//bool is only one byte, where BOOL is 4 bytes
		DirectX::XMFLOAT4 diffuse;
		DirectX::XMFLOAT3 specular;
		BOOL hasDiffuseTexture;
		BOOL hasNormalMap;
		BOOL hasSpecular;
		BOOL hasGloss;
		BOOL hasTransparency;
	};
	//dVS and dPS are the default vertex and pixel shaders.
	GraphicsHelper* graphicsHelper;
	ID3D11DeviceContext1* devCon;
	VertexShader* skinningVS;
	VertexShader* rigidVS;
	VertexShader* rigidInstancingVS;
	PixelShader* dPS;
	ConstantBuffer<ShaderModel>* modelBuffer;
	ConstantBuffer<ShaderMesh>* meshBuffer;
	SamplerState* dSS;
	SamplerState* mirrorSS;
};

