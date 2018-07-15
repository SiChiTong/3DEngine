#include "Graphics/Rendering/CRenderer.h"
#include "Common/GeneralUtils.h"
#include "Math/SIMDConversionUtils.h"
#include "Graphics/SceneGraph/DataGNode.h"

using namespace DirectX;

CRenderer::CRenderer(GraphicsParams graphicsParams)
: CGraphics(graphicsParams)
{
	D3D11_VIEWPORT* viewport = graphicsHelper->GetViewport(0);

	mCamera = new Camera(XMFLOAT3(0, 0, 0), XMFLOAT3(0, 1, 0), XMFLOAT3(0, 0, 100), 0.4f*3.14f, graphicsParams.screenWidth, graphicsParams.screenHeight, 0.01f, 2000.0f);

	vpMatrix = MF(mCamera->GetViewProjection());
	
	sceneParams.dirLightCount = 0;
	sceneParams.pointLightCount = 0;
	sceneParams.spotLightCount = 0;

	sceneParamsCBuffer = ConstantBuffer<SceneParams>::Create(device, &sceneParams);

	DXBuffer::BUFFER_LIFETIME bufferTime = DXBuffer::TEMPORARY;
	instanceBuffer = VertexBuffer<InstanceData>::Create(device, bufferTime, 20 * 40 * 10);

	meshDrawer = new MeshDrawer(graphicsHelper);
	sky = new Sky(graphicsHelper, 30, 30);
}


CRenderer::~CRenderer()
{
	DeleteLights();
	RemoveGraphicsActors();
	delete meshDrawer;
	delete sky;
	delete dirLightBuffer;
	delete pointLightBuffer;
	delete spotLightBuffer;
	delete sceneParamsCBuffer;
	delete instanceBuffer;
	delete mCamera;
}

void CRenderer::Update(double frameTime, int fps)
{
	
}

void CRenderer::Render(double frameTime, int fps)
{
	GraphicsHelper* graphicsHelper = GetGraphicsHelper();

	XMStoreFloat4x4(&vpMatrix, mCamera->GetViewProjection());

	UpdateDirectionalLightBuffer();
	UpdatePointLightBuffer();
	UpdateSpotLightBuffer();

	//std::sort(rigidActors.begin(), rigidActors.end(), &SortPredicate);
	//std::sort(skeletalActors.begin(), skeletalActors.end(), &SortPredicate);

	ID3D11DeviceContext1* devCon = graphicsHelper->GetDeviceContext();
	ID3D11RenderTargetView* renderTargetView = graphicsHelper->GetRenderTargetView();
	ID3D11DepthStencilView* depthStencilView = graphicsHelper->GetDepthStencilView();

	//spotLightCStruct.spotLight.pos.x += 0.01f;
	//spotLightCStruct.spotLight.pos.y += 0.01f;
	//spotLightCStruct.spotLight.pos.z += 0.01f;
	//spotLightCStruct.spotLight.pos = mCamera->GetPosition();
	//spotLightCStruct.spotLight.dir = VF3(mCamera->GetForward());
	//spotLightCBuffer->Update(devCon, &spotLightCStruct);

	//dirLightCBuffer->Update(devCon, &dirLightCStruct);

	//Clear our backbuffer
	float bgColor[4] = { (0.0f, 0.0f, 0.0f, 0.0f) };
	devCon->ClearRenderTargetView(renderTargetView, bgColor);

	//Refresh the Depth/Stencil view
	devCon->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//Set our Render Target and bind depth stencil view to OM stage of pipeline.
	devCon->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

	XMMATRIX xmVPMatrix = GetVPMatrix();

	sceneParams.camPos = mCamera->GetPosition();
	sky->Draw(frameTime, xmVPMatrix, sceneParams.camPos, skyTexture);

	sceneParams.vp = XMMatrixTranspose(xmVPMatrix);
	sceneParamsCBuffer->Update(devCon, &sceneParams);

	ID3D11Buffer* buffers[1] = { sceneParamsCBuffer->GetBuffer() };
	devCon->VSSetConstantBuffers(0, 1, buffers);
	devCon->PSSetConstantBuffers(0, 1, buffers);

	ID3D11ShaderResourceView* resources[3] = { dirLightBuffer->GetShaderResourceView(), pointLightBuffer->GetShaderResourceView(), spotLightBuffer->GetShaderResourceView() };
	devCon->PSSetShaderResources(0, 3, resources);

	graphicsHelper->EnableClockwiseCulling();

	DrawPhysicalGeometry();

	//Present the backbuffer to the screen
	graphicsHelper->GetSwapChain()->Present(0, 0);
}

void CRenderer::CreateSpatialTree(const std::vector<DrawableEntity*>& drawableEntities)
{
	//Create aabb that encompasses all the spheres.
	float lowest = std::numeric_limits<float>::lowest();
	float infinity = std::numeric_limits<float>::infinity();
	AABB aabb;
	aabb.min = XMFLOAT3(infinity, infinity, infinity);
	aabb.max = XMFLOAT3(lowest, lowest, lowest);
	XMFLOAT3 min;
	XMFLOAT3 max;
	for (const DrawableEntity* component : drawableEntities)
	{
		const BoundingSphere& sphere = component->boundingVolume;
		min.x = sphere.centre.x - sphere.radius;
		min.y = sphere.centre.y - sphere.radius;
		min.z = sphere.centre.z - sphere.radius;
		max.x = sphere.centre.x + sphere.radius;
		max.y = sphere.centre.y + sphere.radius;
		max.z = sphere.centre.z + sphere.radius;
		if (min.x < aabb.min.x)
		{
			aabb.min.x = min.x;
		}
		if (min.y < aabb.min.y)
		{
			aabb.min.y = min.y;
		}
		if (min.z < aabb.min.z)
		{
			aabb.min.z = min.z;
		}
		if (max.x > aabb.max.x)
		{
			aabb.max.x = max.x;
		}
		if (max.y > aabb.max.y)
		{
			aabb.max.y = max.y;
		}
		if (max.z > aabb.max.z)
		{
			aabb.max.z = max.z;
		}
	}
	//Create spatial aabb used for creating the spatial hierarchy. 
	SpatialAABB spatialAABB;
	XMVECTOR minVec = FV(min);
	XMVECTOR halfExtents = XMVectorSubtract(FV(max), minVec) * 0.5f;
	spatialAABB.halfExtents = VF3(halfExtents);
	spatialAABB.centre = VF3(XMVectorAdd(minVec, halfExtents));

	if (spatialTree)
	{
		delete spatialTree;
	}
	uint32_t id = 0;
	auto spatialTree = new ContainerGNode(id);
	CreateChildNode(id, spatialTree, spatialAABB, drawableEntities);
}

void CRenderer::CreateChildNode(uint32_t& nodeCount, ContainerGNode* parent, SpatialAABB aabb, const std::vector<DrawableEntity*>& drawableEntities)
{
	std::vector<std::vector<DrawableEntity*>> childComponents(8);
	for (auto component : drawableEntities)
	{
		const BoundingSphere& sphere = component->boundingVolume;

		//Front Bottom Left
		if (sphere.centre.x < aabb.centre.x && sphere.centre.y < aabb.centre.y && sphere.centre.z < aabb.centre.z)
		{
			childComponents[0].push_back(component);
		}
		//Front Top Left
		if (sphere.centre.x < aabb.centre.x && sphere.centre.y > aabb.centre.y && sphere.centre.z < aabb.centre.z)
		{
			childComponents[1].push_back(component);
		}
		//Front Top Right
		if (sphere.centre.x > aabb.centre.x && sphere.centre.y > aabb.centre.y && sphere.centre.z < aabb.centre.z)
		{
			childComponents[2].push_back(component);
		}
		//Front Bottom Right
		if (sphere.centre.x > aabb.centre.x && sphere.centre.y < aabb.centre.y && sphere.centre.z < aabb.centre.z)
		{
			childComponents[3].push_back(component);
		}
		//Back Bottom Left
		if (sphere.centre.x < aabb.centre.x && sphere.centre.y < aabb.centre.y && sphere.centre.z > aabb.centre.z)
		{
			childComponents[4].push_back(component);
		}
		//Back Top Left
		if (sphere.centre.x < aabb.centre.x && sphere.centre.y > aabb.centre.y && sphere.centre.z > aabb.centre.z)
		{
			childComponents[5].push_back(component);
		}
		//Back Top Right
		if (sphere.centre.x > aabb.centre.x && sphere.centre.y > aabb.centre.y && sphere.centre.z > aabb.centre.z)
		{
			childComponents[6].push_back(component);
		}
		//Back Bottom Right
		if (sphere.centre.x > aabb.centre.x && sphere.centre.y < aabb.centre.y && sphere.centre.z > aabb.centre.z)
		{
			childComponents[7].push_back(component);
		}
	}
	//Overwrite aabb with new extents.
	aabb.halfExtents = VF3(FV(aabb.halfExtents ) * 0.5f);
	for (uint32_t i = 0; i < childComponents.size(); ++i)
	{
		auto components = childComponents[i];
		if (components.size() == 0)
		{
			continue;
		}
		if (components.size() == 1)
		{
			auto node = new DataGNode(nodeCount++, components[0]);
			parent->AddChild(node);
			node->SetParent(parent);
		}
		else
		{
			switch (i)
			{
				//Front Bottom Left
				case 0:
					aabb.centre.x -= aabb.halfExtents.x;
					aabb.centre.y -= aabb.halfExtents.y;
					aabb.centre.z -= aabb.halfExtents.z;
					break;
				//Front Top Left
				case 1:
					aabb.centre.x -= aabb.halfExtents.x;
					aabb.centre.y += aabb.halfExtents.y;
					aabb.centre.z -= aabb.halfExtents.z;
					break;
				//Front Top Right
				case 2:
					aabb.centre.x += aabb.halfExtents.x;
					aabb.centre.y += aabb.halfExtents.y;
					aabb.centre.z -= aabb.halfExtents.z;
					break;
				//Front Bottom Right
				case 3:
					aabb.centre.x += aabb.halfExtents.x;
					aabb.centre.y -= aabb.halfExtents.y;
					aabb.centre.z -= aabb.halfExtents.z;
					break;
				//Back Bottom Left
				case 4:
					aabb.centre.x -= aabb.halfExtents.x;
					aabb.centre.y -= aabb.halfExtents.y;
					aabb.centre.z += aabb.halfExtents.z;
					break;
				//Back Top Left
				case 5:
					aabb.centre.x -= aabb.halfExtents.x;
					aabb.centre.y += aabb.halfExtents.y;
					aabb.centre.z += aabb.halfExtents.z;
					break;
				//Back Top Right
				case 6:
					aabb.centre.x += aabb.halfExtents.x;
					aabb.centre.y += aabb.halfExtents.y;
					aabb.centre.z += aabb.halfExtents.z;
					break;
				//Back Bottom Right
				case 7:
					aabb.centre.x += aabb.halfExtents.x;
					aabb.centre.y -= aabb.halfExtents.y;
					aabb.centre.z += aabb.halfExtents.z;
					break;
			}
			auto node = new ContainerGNode(nodeCount++);
			parent->AddChild(node);
			node->SetParent(parent);
			CreateChildNode(nodeCount, node, aabb, components);
		}
	}
	//Calculate bounding sphere for the parent node by merging the spheres of the children.
	parent->MergeExistngChildSpheres();
}

void CRenderer::DrawPhysicalGeometry()
{
	GraphicsHelper* graphicsHelper = GetGraphicsHelper();
	graphicsHelper->UseDefaultDpethStencilState();

	meshDrawer->BindResources();

	meshDrawer->BindRigidResources();
	DrawRigidActors(normalActors, false);

	//meshDrawer->BindInstancingRigidResources();
	//DrawRigidActors(instancedActors, true);

	meshDrawer->BindSkeletalResources();
	DrawSkeletalActors(skeletalActors, false);
}

//We draw our terrain in this method because it is rigid.
void CRenderer::DrawRigidActors(std::vector<RigidActor*>& actors, boolean instancing)
{
	GraphicsHelper* graphicsHelper = GetGraphicsHelper();
	if (actors.empty())
	{
		return;
	}
	int index = 0;
	std::vector<InstanceData> instancingData;
	ID3D11DeviceContext1* devCon = graphicsHelper->GetDeviceContext();
	std::vector<RigidActor*>::iterator iter;
	for (iter = actors.begin(); iter != actors.end(); iter++)
	{
		RigidActor* entity = *iter;
		RigidModel* model = entity->GetModel();
		
		if(instancing)
		{
			InstanceData data;
			data.world = entity->GetTransposedXMWorldMatrix();
			instancingData.push_back(data);
			if (index != actors.size() - 1) 
			{
				index++;
				continue;
			}
			instanceBuffer->Update(devCon, &instancingData[0]);
			//Bind the vertex and index buffers the entity is using.
			entity->Render(devCon, instanceBuffer->GetBuffer(), instanceBuffer->GetElementSize());
		}
		else 
		{
			//Bind the vertex and index buffers the entity is using.
			entity->Render(devCon);
		}
		
		meshDrawer->SetModelParameters(entity->GetXMWorldMatrix());

		for(int i = 0; i < model->subsets.size(); i++)
		{
			ModelSubset* subset = entity->GetModelSubset(i);
			SurfaceMaterial& material = model->materials[subset->matIndex];
			if(material.hasTransparency)
			{
				//Set the blend state to transparency.
				graphicsHelper->EnableAplhaBlending();
			}
			else
			{
				//Set the default blend state (no blending). 
				graphicsHelper->DisableBlending();
			}
			
			meshDrawer->SetMeshMaterial(material);
			if (instancing) 
			{
				meshDrawer->RenderInstanced(subset->indexCount, actors.size(), subset->vertStartIndex, 0, 0);
			} 
			else
			{
				meshDrawer->Render(subset->indexCount, subset->vertStartIndex);
			}
		}
	}
}

void CRenderer::DrawSkeletalActors(std::vector<SkeletalActor*>& actors, boolean instancing)
{
	GraphicsHelper* graphicsHelper = GetGraphicsHelper();
	if (actors.empty())
	{
		return;
	}
	int index = 0;
	std::vector<InstanceData> instancingData;
	ID3D11DeviceContext1* devCon = graphicsHelper->GetDeviceContext();
	std::vector<SkeletalActor*>::iterator iter;
	for (iter = actors.begin(); iter != actors.end(); iter++)
	{
		SkeletalActor* entity = *iter;
		SkeletalModel* model = entity->GetModel();
		
		if (instancing)
		{
			InstanceData data;
			data.world = entity->GetTransposedXMWorldMatrix();
			instancingData.push_back(data);
			if (index != actors.size() - 1)
			{
				index++;
				continue;
			}
			instanceBuffer->Update(devCon, &instancingData[0]);
		} 

		meshDrawer->SetModelParameters(entity->GetXMWorldMatrix());
		meshDrawer->SetSkeletalParameters(model->jointSB->GetShaderResourceView(), model->weightSB->GetShaderResourceView());

		for(int i = 0; i < model->subsets.size(); i++)
		{
			//Bind the vertex and index buffers the entity is using.
			ModelSubset* subset = entity->GetModelSubset(i);

			if (instancing)
			{
				//Bind the vertex and index buffers the entity is using.
				entity->Render(devCon, instanceBuffer->GetBuffer(), instanceBuffer->GetElementSize());
			}
			else
			{
				//Bind the vertex and index buffers the entity is using.
				entity->Render(devCon);
			}
			SurfaceMaterial& material = model->materials[subset->matIndex];
		
			if(material.hasTransparency)
			{
				//Set the blend state to transparency.
				graphicsHelper->EnableColourBlending(0.5f, 0.5f, 0.5f, 1.0f);
			}
			else
			{
				//Set the default blend state (no blending). 
				graphicsHelper->DisableBlending();
			}

			meshDrawer->SetMeshMaterial(material);
			if (instancing)
			{
				meshDrawer->RenderInstanced(subset->indexCount, actors.size(), subset->vertStartIndex, 0, 0);
			}
			else
			{
				meshDrawer->Render(subset->indexCount, subset->vertStartIndex);
			}
		}
		index++;
	}
}

void CRenderer::AddGraphicsActor(GraphicsActor* actor)
{
	graphicsActors.push_back(actor);
}

void CRenderer::RemoveGraphicsActor(GraphicsActor* actor)
{
	GeneralUtils::RemoveVectorPointer(graphicsActors, actor, false);
}

void CRenderer::RemoveGraphicsActors()
{
	GeneralUtils::RemoveVectorPointers(graphicsActors, &graphicsActors[0], graphicsActors.size(), false);
}

void CRenderer::CreateDirectionalLightBuffer(UINT lightCount)
{
	if (dirLightBuffer)
	{
		delete dirLightBuffer;
	}
	DXBuffer::BUFFER_TYPE bufferType = DXBuffer::DYNAMIC;
	dirLightBuffer = StructuredBuffer<ShaderDirectionalLight>::Create(device, bufferType, lightCount);
}

void CRenderer::CreatePointLightBuffer(UINT lightCount)
{
	if (pointLightBuffer)
	{
		delete pointLightBuffer;
	}
	DXBuffer::BUFFER_TYPE bufferType = DXBuffer::DYNAMIC;
	pointLightBuffer = StructuredBuffer<ShaderPointLight>::Create(device, bufferType, lightCount);
	spotLightBuffer = StructuredBuffer<ShaderSpotLight>::Create(device, bufferType, lightCount);
}

void CRenderer::CreateSpotLightBuffer(UINT lightCount)
{
	if (spotLightBuffer)
	{
		delete spotLightBuffer;
	}
	DXBuffer::BUFFER_TYPE bufferType = DXBuffer::DYNAMIC;
	spotLightBuffer = StructuredBuffer<ShaderSpotLight>::Create(device, bufferType, lightCount);
}

void CRenderer::UpdateDirectionalLightBuffer()
{
	sceneParams.dirLightCount = dirLights.size();
	if (sceneParams.dirLightCount < 1)
	{
		return;
	}
	std::vector<ShaderDirectionalLight> lights(dirLightBuffer->GetElementCount());
	for (int i = 0; i < sceneParams.dirLightCount; ++i)
	{
		lights[i] = *dirLights[i];
	}
	dirLightBuffer->Update(deviceContext, &lights[0]);
}

void CRenderer::UpdatePointLightBuffer()
{
	sceneParams.pointLightCount = pointLights.size();
	if (sceneParams.pointLightCount < 1)
	{
		return;
	}
	std::vector<ShaderPointLight> lights(pointLightBuffer->GetElementCount());
	for (int i = 0; i < sceneParams.pointLightCount; ++i)
	{
		lights[i] = *pointLights[i];
	}
	pointLightBuffer->Update(deviceContext, &lights[0]);
}

void CRenderer::UpdateSpotLightBuffer()
{
	sceneParams.spotLightCount = spotLights.size();
	if (sceneParams.spotLightCount < 1)
	{
		return;
	}
	std::vector<ShaderSpotLight> lights(spotLightBuffer->GetElementCount());
	for (int i = 0; i < sceneParams.spotLightCount; ++i)
	{
		lights[i] = *spotLights[i];
	}
	spotLightBuffer->Update(deviceContext, &lights[0]);
}

void CRenderer::DeleteLights()
{
	GeneralUtils::DeleteAllVectorPointers(dirLights);
	GeneralUtils::DeleteAllVectorPointers(pointLights);
	GeneralUtils::DeleteAllVectorPointers(spotLights);
}

Camera* CRenderer::GetCamera() const
{
	return mCamera;
}

XMMATRIX CRenderer::GetVPMatrix() const
{
	return XMLoadFloat4x4(&vpMatrix);
}

