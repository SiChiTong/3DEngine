#pragma once

#include <vector>
#include <list>
#include "Common/DirectX.h"

struct DVertex
{
	//serialize

	DVertex(){}
	DVertex(float x, float y, float z,
		float u, float v)
		: pos(x, y, z), texCoord(u, v), normal(0, 1, 0),
		tangent(1, 0, 0){}
	DVertex(float x, float y, float z,
		float u, float v,
		float nx, float ny, float nz)
		: pos(x, y, z), texCoord(u, v), normal(nx, ny, nz),
		tangent(1, 0, 0){}
	DVertex(float x, float y, float z,
		float u, float v,
		float nx, float ny, float nz,
		float tx, float ty, float tz)
		: pos(x, y, z), texCoord(u, v), normal(nx, ny, nz),
		tangent(tx, ty, tz){}

	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT2 texCoord;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT3 tangent;
};

struct AnimVertex	
{
	//serialize

	AnimVertex(){}
	AnimVertex(float u, float v,
		float tx, float ty, float tz)
		: texCoord(u, v){}

	DirectX::XMFLOAT2 texCoord;
	UINT StartWeight = 0;
	UINT WeightCount = 0;
};

struct MeshPolygon
{
	UINT index1;
	UINT index2;
	UINT index3;
	DirectX::XMFLOAT3 normal;
};

struct ShaderJoint
{
	DirectX::XMFLOAT3 pos;
	int parentID;
	DirectX::XMFLOAT4 orientation;
};

enum ShaderType
{
	VERTEX, HULL, GEOMETRY, PIXEL
};

//Leave in this order.
enum MaterialTextureType
{
	TextureTypeDiffuse = 0,
	TextureTypeSpecularMap,
	TextureTypeAmbient,
	TextureTypeEmissive,
	TextureTypeHeightMap,
	TextureTypeNormalMap,
	TextureTypeSpecularPowerMap,
	TextureTypeDisplacementMap,
	TextureTypeEnd
};

struct MaterialTexture
{
	//serialize
	std::wstring name;
	MaterialTextureType type;
	ID3D11ShaderResourceView* data = nullptr;
};

struct SurfaceMaterial
{
	//serialize
	std::wstring name = L"";
	DirectX::XMFLOAT4 diffuse = DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	DirectX::XMFLOAT3 specular = DirectX::XMFLOAT3(0.5f, 0.5f, 0.5f);
	DirectX::XMFLOAT3 ambient = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	std::vector<MaterialTexture> textures;
	bool hasTransparency = false;
};

struct ParticleSystemGraphicsDesc
{
	ID3D11SamplerState* samplerState;
	ID3D11DepthStencilState* depthStencilState;
	ID3D11VertexShader* VS;
	ID3D11PixelShader* PS;
	ID3D11Buffer* vertBuff;
	ID3D11Buffer* indexBuff;
	ID3D11Buffer* particleCStruct;
	ID3D11ShaderResourceView* texture;
	ID3D11InputLayout* particleVertLayout;
	int numIndices;
	DirectX::XMFLOAT3 normal;
};

struct Weight
{
	//serialize

	DirectX::XMFLOAT3 pos;
	int jointID;
	DirectX::XMFLOAT3 normal;
	float bias;
	DirectX::XMFLOAT3 tangent;
	float pad;
};
