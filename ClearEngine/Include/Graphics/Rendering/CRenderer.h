#pragma once

#include "Geometry/Models.h"
#include "Graphics/Actors/RigidGraphicsActor.h"
#include "Graphics/Actors/SkeletalGraphicsActor.h"
#include "Graphics/Rendering/CGraphics.h"
#include "Graphics/Rendering/ShaderLights.h"
#include "Graphics/Rendering/Sky.h"
#include "Graphics/Rendering/MeshDrawer.h"
#include "Graphics/Actors/GraphicsActor.h"
#include "Geometry/GeometryTypes.h"
#include "Lights/DirectionalLight.h"
#include "Lights/PointLight.h"
#include "Lights/SpotLight.h"
#include "Graphics/SceneGraph/ContainerGNode.h"

#define D3D_COMPILE_STANDARD_FILE_INCLUDE ((ID3DInclude*)(UINT_PTR)1)

class CRenderer : public CGraphics
{
public:
	CRenderer(GraphicsParams graphicsParams);
	virtual ~CRenderer();
	void Update(double frameTime, int fps);
	void Render(double frameTime, int fps);
	void UpdateDirectionalLightBuffer();
	void UpdatePointLightBuffer();
	void UpdateSpotLightBuffer();
	void AddGraphicsActor(GraphicsActor* graphicsActor);
	void RemoveGraphicsActor(GraphicsActor* graphicsActor);
	void CreateSpatialTree(const std::vector<DrawableEntity*>& drawableEntities);
	void CreateChildNode(uint32_t& nodeCount, ContainerGNode* parent, SpatialAABB aabb, const std::vector<DrawableEntity*>& drawableEntities);
	Camera* GetCamera() const;


private:
	struct SceneParams 
	{
		DirectX::XMMATRIX vp;
		DirectX::XMFLOAT3 camPos;
		float camPad = 0.0f;
		UINT dirLightCount;
		UINT pointLightCount;
		UINT spotLightCount;
		UINT pad = 0;
	};

	struct InstanceData
	{
		DirectX::XMMATRIX world;
	};

	void CreateDirectionalLightBuffer(UINT lightCount);
	void CreatePointLightBuffer(UINT lightCount);
	void CreateSpotLightBuffer(UINT lightCount);
	void DeleteLights();
	void RemoveGraphicsActors();

	MeshDrawer* meshDrawer;
	Sky* sky;
	DirectX::XMMATRIX GetVPMatrix() const;
	ID3D11ShaderResourceView* skyTexture = 0;
	std::vector<GraphicsActor*> graphicsActors;
	std::vector<DirectionalLight*> dirLights;
	std::vector<PointLight*> pointLights;
	std::vector<SpotLight*> spotLights;
	StructuredBuffer<ShaderDirectionalLight>* dirLightBuffer;
	StructuredBuffer<ShaderPointLight>* pointLightBuffer;
	StructuredBuffer<ShaderSpotLight>* spotLightBuffer;
	SceneParams sceneParams;
	ConstantBuffer<SceneParams>* sceneParamsCBuffer;
	VertexBuffer<InstanceData>* instanceBuffer;
	Camera* mCamera;
	GNode* spatialTree;
	DirectX::XMFLOAT4X4 vpMatrix;
};

