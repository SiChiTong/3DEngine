#include "Loading/AssetLoadingUtil.h"
#include "Common/GeneralUtils.h"
#include "Math/SIMDConversionUtils.h"

using namespace DirectX;
using namespace std;

void AssetLoadingUtil::ExtractMaterialsFromScene(const aiScene* scene, std::vector<SurfaceMaterial>& materials)
{
	if (scene->HasMaterials())
	{
		for (UINT i = 0; i < scene->mNumMaterials; i++)
		{
			SurfaceMaterial material;
			//aiTextureType_DIFFUSE aiTextureType_NORMALS aiTextureType_OPACITY 
			aiMaterial* aMaterial = scene->mMaterials[i];
			//aMaterial->mProperties[0]->
			aiString name;
			aMaterial->Get(AI_MATKEY_NAME, name);
			GeneralUtils::StringToWString(name.C_Str(), material.name);
			float opacity;
			aMaterial->Get(AI_MATKEY_OPACITY, opacity);
			aiColor3D diffuse;
			aMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
			aiColor3D specular;
			aMaterial->Get(AI_MATKEY_COLOR_SPECULAR, specular);
			aiColor3D ambient;
			aMaterial->Get(AI_MATKEY_COLOR_AMBIENT, ambient);

			material.hasTransparency = opacity != 1.0f;
			material.diffuse = XMFLOAT4(diffuse.r, diffuse.g, diffuse.b, opacity);
			material.specular = XMFLOAT3(specular.r, specular.g, specular.b);
			material.ambient = XMFLOAT3(ambient.r, ambient.g, ambient.b);

			for (MaterialTextureType textureType = (MaterialTextureType)0; textureType < TextureTypeEnd; textureType = (MaterialTextureType)(textureType + 1))
			{
				aiTextureType mappedTextureType = (aiTextureType)textureTypeMappings[textureType];
				UINT textureCount = aMaterial->GetTextureCount(mappedTextureType);
				if (textureCount > 0)
				{
					for (UINT textureIndex = 0; textureIndex < textureCount; textureIndex++)
					{
						aiString path;

						//We expect all textures to be in the textures folder specified by the application. 
						//We therefore trim anything before the texture name from the path Assimp gives us.
						if (aMaterial->GetTexture(mappedTextureType, textureIndex, &path) == AI_SUCCESS)
						{
							MaterialTexture texture;
							GeneralUtils::StringToWString(path.C_Str(), texture.name);
							texture.name = GeneralUtils::GetFileNameFromPath(texture.name);
							texture.type = textureType == TextureTypeHeightMap ? TextureTypeNormalMap : textureType;
							texture.data = nullptr;
							material.textures.push_back(texture);
						}
					}
				}
			}

			materials.push_back(material);
		}
	}
}

bool AssetLoadingUtil::MaterialHasTextureType(SurfaceMaterial& material, MaterialTextureType textureType)
{
	for (MaterialTexture texture : material.textures)
	{
		if (textureType == texture.type)
		{
			return true;
		}
	}
	return false;
}

void AssetLoadingUtil::ExtractRigidMeshesFromScene(const aiScene* scene, RigidModel& model)
{
	std::vector<UINT> meshIndices;
	ExtractRigidMeshesFromNode(scene, scene->mRootNode, model, meshIndices);
}

void AssetLoadingUtil::ExtractRigidMeshesFromNode(const aiScene* scene, const aiNode* node, RigidModel& model, std::vector<UINT>& meshIndices)
{
	//If the node has children, keep traversing down. Otherwise, a leaf has been reached and the mesh data can be extracted.
	if (node->mNumChildren > 0)
	{
		for (int i = 0; i < node->mNumChildren; ++i)
		{
			ExtractRigidMeshesFromNode(scene, node->mChildren[i], model, meshIndices);
		}
	}

	if (node->mNumMeshes < 1 || meshIndices.size() >= scene->mNumMeshes)
	{
		return;
	}

	//Parent transformation of node is already the product of the transformations of the parent nodes up the hierarchy.  
	//Assimp is column-major and DirectX is row-major. The transformation must then be transposed to perform this conversion.
	XMMATRIX transform = XMMatrixIdentity(); 
	if (node->mParent)
	{
		transform = XMMatrixTranspose(FM(XMFLOAT4X4(reinterpret_cast<const float*>(node->mTransformation[0]))));
	}
	//XMMATRIX inverseSceneWorldTransform = XMMatrixTranspose(FM(XMFLOAT4X4(reinterpret_cast<const float*>(scene->mRootNode->mTransformation[0]))));
	//inverseSceneWorldTransform = XMMatrixInverse(&XMMatrixDeterminant(inverseSceneWorldTransform), inverseSceneWorldTransform);
	//transform = XMMatrixMultiply(transform, inverseSceneWorldTransform);
	for (UINT index = 0; index < node->mNumMeshes; ++index)
	{
		UINT meshIndex = node->mMeshes[index];
		//Check if mesh has already been added from another node.
		bool meshAdded = false;
		for (UINT i : meshIndices)
		{
			if (i == meshIndex)
			{
				meshAdded = true;
				break;
			}
		}
		if (meshAdded)
		{
			continue;
		}
		meshIndices.push_back(meshIndex);
		model.subsets.push_back(ModelSubset());
		ModelSubset& subset = model.subsets[model.subsets.size() - 1];
		aiMesh* mesh = scene->mMeshes[meshIndex];
		subset.matIndex = mesh->mMaterialIndex;
		subset.vertexCount = mesh->mNumVertices;

		UINT startVertCount = model.vertices.size();
		for (UINT i = 0; i < mesh->mNumVertices; i++)
		{
			DVertex vertex;
			// Positions
			if (mesh->HasPositions())
			{
				vertex.pos = XMFLOAT3(reinterpret_cast<const float*>(&mesh->mVertices[i]));
				vertex.pos = VF3(XMVector3Transform(FV(vertex.pos), transform));
				subset.positions.push_back(vertex.pos);
			}
			// Normals
			if (mesh->HasNormals())
			{
				vertex.normal = XMFLOAT3(reinterpret_cast<const float*>(&mesh->mNormals[i]));
				vertex.normal = VF3(XMVector3TransformNormal(FV(vertex.normal), transform));
			}
			//NB. We do not take the biTangent here because we calculate it in the pixel shader.
			// Tangents
			if (mesh->HasTangentsAndBitangents())
			{
				vertex.tangent = XMFLOAT3(reinterpret_cast<const float*>(&mesh->mTangents[i]));
				vertex.tangent = VF3(XMVector3TransformNormal(FV(vertex.tangent), transform));
			}

			//NB. The engine does not support lightmaps because it uses a dynamic local lighting model.

			//Texture Coordinates
			UINT uvChannelCount = mesh->GetNumUVChannels();

			for (UINT j = 0; j < uvChannelCount; j++)
			{
				aiVector3D* aTextureCoordinates = mesh->mTextureCoords[j];	
				vertex.texCoord.x = aTextureCoordinates[i].x;
				vertex.texCoord.y = aTextureCoordinates[i].y;		
			}
			model.vertices.push_back(vertex);
		}
		if (mesh->HasFaces())
		{
			// Initialize index count. Should not be final value.
			subset.indexCount = 0;
			subset.vertStartIndex = model.indices.size();
			UINT faceCount = mesh->mNumFaces;
			for (UINT i = 0; i < faceCount; i++)
			{
				aiFace* face = &mesh->mFaces[i];

				for (UINT j = 0; j < face->mNumIndices; j++)
				{
					model.indices.push_back(startVertCount + face->mIndices[j]);
					subset.indexCount++;
				}
			}
		}
	}
}

void AssetLoadingUtil::ExtractSkeletalMeshesFromScene(const aiScene* scene, SkeletalModel& model)
{
	//Populate the bone data map.
	std::map<wstring, BoneInfo*> boneData;
	for (UINT index = 0; index < scene->mNumMeshes; index++)
	{
		model.subsets.push_back(ModelSubset());
		ModelSubset& subset = model.subsets[model.subsets.size() - 1];
		aiMesh* mesh = scene->mMeshes[index];
		subset.matIndex = mesh->mMaterialIndex;
		subset.vertexCount = mesh->mNumVertices;

		UINT startVertCount = model.vertices.size();
		for (UINT i = 0; i < mesh->mNumVertices; i++)
		{
			AnimVertex vertex;

			vertex.StartWeight = 0;
			vertex.WeightCount = 0;

			//NB. The engine does not support lightmaps but uses a dynamic local lighting model.

			//Texture Coordinates
			UINT uvChannelCount = mesh->GetNumUVChannels();

			for (UINT j = 0; j < uvChannelCount; j++)
			{
				aiVector3D* aTextureCoordinates = mesh->mTextureCoords[j];
				vertex.texCoord.x = aTextureCoordinates[i].x;
				vertex.texCoord.y = aTextureCoordinates[i].y;
			}
			model.vertices.push_back(vertex);
		}
		if (mesh->HasFaces())
		{
			// Initialize index count. Should not be final value.
			subset.indexCount = 0;
			subset.vertStartIndex = model.indices.size();
			UINT faceCount = mesh->mNumFaces;
			for (UINT i = 0; i < faceCount; i++)
			{
				aiFace* face = &mesh->mFaces[i];

				for (UINT j = 0; j < face->mNumIndices; j++)
				{
					model.indices.push_back(startVertCount + face->mIndices[j]);
					subset.indexCount++;
				}
			}
		}
		if (mesh->HasBones())
		{
			for (int i = 0; i < mesh->mNumBones; i++)
			{
				aiBone* bone = mesh->mBones[i];
				std::wstring boneName;
				GeneralUtils::StringToWString(bone->mName.C_Str(), boneName);
				std::map<std::wstring, BoneInfo*>::iterator it = boneData.find(boneName);
				if (it == boneData.end())
				{
					boneData[boneName] = new BoneInfo();
					//Index is set to just a place holder for now.
					boneData[boneName]->index = 0;
				}
				boneData[boneName]->data.push_back(BoneMeshInfo(bone, mesh, model.subsets.size(), startVertCount));
			}
		}
	}
	std::vector<std::vector<Weight>> weightData(model.vertices.size());
	//Perform bredth first search.  
	bool greaterDepth = true;
	for (UINT targetLevel = 0; greaterDepth; targetLevel++)
	{
		bool tempGreaterDepth = false;
		ExtractBonesFromScene(scene->mRootNode, nullptr, model, boneData, weightData, 0, targetLevel, -1, tempGreaterDepth);
		greaterDepth = tempGreaterDepth;
	}
	
	//Delete all bone information struct pointers from the bone data map.
	GeneralUtils::ClearMapWithSecondEntryPointers<wstring, BoneInfo>(boneData);

	//Fill in the model's weight vector.
	for (int i = 0; i < weightData.size(); i++)
	{
		AnimVertex& vertex = model.vertices[i];
		vertex.StartWeight = model.weights.size();
		vertex.WeightCount = weightData[i].size();
		model.weights.insert(model.weights.begin() + model.weights.size(), weightData[i].begin(), weightData[i].end());
	}
}

void AssetLoadingUtil::ExtractBonesFromScene(aiNode* node, aiNode* parentBone, SkeletalModel& model, std::map<std::wstring, BoneInfo*>& boneData, std::vector<std::vector<Weight>>& weightData, UINT level, UINT targetLevel,
	int parentBoneIndex, bool& greaterDepth)
{
	if (level > targetLevel)
	{
		return;
	}
	wstring nodeName;
	GeneralUtils::StringToWString(node->mName.C_Str(), nodeName);
	BoneInfo* boneInfo = nullptr;
	//Search to see if there is a bone in the map that has the same name as the node.
	std::map<std::wstring, BoneInfo*>::iterator it = boneData.find(nodeName);
	if (it != boneData.end())
	{
		boneInfo = it->second;
	}
	if (level < targetLevel)
	{
		//If this node is a bone, update the current parent index for use by children bones at the target depth.
		if (boneInfo)
		{
			parentBone = node;
			parentBoneIndex = boneInfo->index;
		}
		if (node->mNumChildren > 0)
		{
			level++;
			for (int i = 0; i < node->mNumChildren; i++)
			{
				ExtractBonesFromScene(node->mChildren[i], parentBone, model, boneData, weightData, level, targetLevel, parentBoneIndex, greaterDepth);
			}
		}
		return;
	}

	//If greaterDepth value is already true, then it will stay true as another node at the target depth level has children. Otherwise it will be assigned to true if this node has children.
	greaterDepth = greaterDepth || node->mNumChildren > 0;

	//If this node is not a bone, there is nothing else to do but return. 
	if (!boneInfo)
	{
		return;
	}

	//Otherwise, add a new joint to the model.
	XMMATRIX boneTransform = XMMatrixTranspose(FM(XMFLOAT4X4(reinterpret_cast<const float*>(node->mTransformation[0]))));

	Joint joint;
	joint.name = nodeName;
	//Offset matrix.
	XMVECTOR scale;
	XMVECTOR rotation;
	XMVECTOR translation;
	bool value = XMMatrixDecompose(&scale, &rotation, &translation, boneTransform);
	joint.orientation = VF4(XMQuaternionConjugate(rotation));
	joint.pos = VF3(translation);
	joint.parentID = parentBoneIndex;

	//Update the index of the bone inside the bone data map.
	boneInfo->index = model.joints.size();

	model.joints.push_back(joint);

	//Add the weights for each mesh that this joint affects.
	for (int index = 0; index < boneInfo->data.size(); index++)
	{
		BoneMeshInfo& boneMeshInfo = boneInfo->data[index];
		ModelSubset& subset = model.subsets[boneMeshInfo.meshIndex];
		subset.jointIndices.push_back(boneInfo->index);
		aiBone* bone = boneMeshInfo.bone;
		aiMesh* mesh = boneMeshInfo.mesh;

		//Matrix to transfrom vertex from mesh to bone space.
		XMMATRIX reverseBoneTransform = XMMatrixTranspose(FM(XMFLOAT4X4(reinterpret_cast<const float*>(bone->mOffsetMatrix[0]))));

		value = XMMatrixDecompose(&scale, &rotation, &translation, reverseBoneTransform);

		for (UINT i = 0; i < bone->mNumWeights; i++)
		{
			aiVertexWeight vertexWeight = bone->mWeights[i];
			Weight weight;
			weight.jointID = boneInfo->index;
			weight.bias = vertexWeight.mWeight;


			if (mesh->mNumVertices > 0)
			{
				weight.pos = XMFLOAT3(reinterpret_cast<const float*>(&mesh->mVertices[vertexWeight.mVertexId]));
				//Transform from mesh space to bone space.
				weight.pos = VF3(XMVector3Transform(FV(weight.pos), reverseBoneTransform));
			}
			if (mesh->mNormals && mesh->mNormals->Length() > 0)
			{
				weight.normal = XMFLOAT3(reinterpret_cast<const float*>(&mesh->mNormals[vertexWeight.mVertexId]));
				//Transform from mesh space to bone space.
				weight.normal = VF3(XMVector3Normalize(XMVector3TransformNormal(FV(weight.normal), reverseBoneTransform)));
			}
			if (mesh->mTangents && mesh->mTangents->Length() > 0)
			{
				weight.tangent = XMFLOAT3(reinterpret_cast<const float*>(&mesh->mTangents[vertexWeight.mVertexId]));
				//Transform from mesh space to bone space.
				weight.tangent = VF3(XMVector3Normalize(XMVector3TransformNormal(FV(weight.tangent), reverseBoneTransform)));
			}

			//The startVertCount is the number of vertices in all meshes prior to this one. 
			weightData[boneMeshInfo.startVertCount + vertexWeight.mVertexId].push_back(weight);
		}
	}
	
}

void AssetLoadingUtil::ExtractAnimationsFromScene(const aiScene* scene, std::map<std::wstring, ModelAnimation*>& animations, SkeletalModel& model, XMFLOAT4 rootBoneRotation)
{
	if (!scene->HasAnimations())
	{
		return;
	}

	for (UINT i = 0; i < scene->mNumAnimations; i++)
	{
		aiAnimation* aAnimation = scene->mAnimations[i];
		wstring animName;
		GeneralUtils::StringToWString(aAnimation->mName.C_Str(), animName);
		std::map<std::wstring, ModelAnimation*>::iterator iter = animations.find(animName);
		//Move on if there is already an animation loaded with the same name.
		if (iter != animations.end())
		{
			continue;
		}

		ModelAnimation* animation = new ModelAnimation();
		animation->numFrames = ceil(aAnimation->mDuration);
		animation->frameRate = aAnimation->mTicksPerSecond;
		animation->frameTime = 1.0f / animation->frameRate;
		animation->totalAnimTime = animation->numFrames * animation->frameTime;
		animation->numAnimatedComponents = aAnimation->mNumChannels;
		animation->numJoints = model.joints.size();
		animation->currAnimTime = 0.0f;	
		animations[animName] = animation;


		std::map<std::wstring, UINT> positionMap;
		std::map<std::wstring, UINT> rotationMap;
		//Scale is included here even though the engine does not support it.
		std::map<std::wstring, UINT> scaleMap;
		for (int j = 0; j < aAnimation->mNumChannels; j++)
		{
			aiNodeAnim* nodeAnim = aAnimation->mChannels[j];
			wstring boneName;
			GeneralUtils::StringToWString(nodeAnim->mNodeName.C_Str(), boneName);
			//Remove Assimp's string if it concatenates it to the bone name.
			boneName = FixBoneName(boneName);
			std::map<std::wstring, UINT>::iterator it = positionMap.find(boneName);
			if (it == positionMap.end())
			{
				//Add to all maps first. Assimp can split translations, rotations and scales between different nodeAnims. Thus three maps need to be used.
				positionMap[boneName] = j;
				rotationMap[boneName] = j;
				scaleMap[boneName] = j;
			}
			else
			{
				//Alter the nodeAnim the maps point two if there is more information in this nodeAnim than the one in the map for this bone name.
				aiNodeAnim* savedPositionNodeAnim = aAnimation->mChannels[positionMap[boneName]];
				if (nodeAnim->mNumPositionKeys > savedPositionNodeAnim->mNumPositionKeys)
				{
					positionMap[boneName] = j;
				}
				aiNodeAnim* savedRotationNodeAnim = aAnimation->mChannels[rotationMap[boneName]];
				if (nodeAnim->mNumRotationKeys > savedRotationNodeAnim->mNumRotationKeys)
				{
					rotationMap[boneName] = j;
				}
				aiNodeAnim* savedScaleNodeAnim = aAnimation->mChannels[scaleMap[boneName]];
				if (nodeAnim->mNumScalingKeys > savedScaleNodeAnim->mNumScalingKeys)
				{
					scaleMap[boneName] = j;
				}
			}
			
		}

		for (int j = 0; j < animation->numFrames; j++)
		{
			std::vector<Joint> frameJoints;
			for (int k = 0; k < model.joints.size(); k++)
			{
				//Make animation joint from the skeletal joint to get the name and parent ID.
				Joint joint = model.joints[k];
				bool rootBone = joint.parentID == -1;
				//If it is in one map, it is in all three.
				std::map<std::wstring, UINT>::iterator it = positionMap.find(joint.name);
				if (it != positionMap.end())
				{
					aiNodeAnim* positionNodeAnim = aAnimation->mChannels[positionMap[joint.name]];
					aiNodeAnim* rotationNodeAnim = aAnimation->mChannels[rotationMap[joint.name]];
					float posFactor = (float)positionNodeAnim->mNumPositionKeys * ((float)j / (float)animation->numFrames);
					float rotFactor = (float)rotationNodeAnim->mNumRotationKeys * ((float)j / (float)animation->numFrames);
					UINT pos1Index = (UINT)floorf(posFactor);
					UINT pos2Index = (UINT)posFactor == pos1Index ? pos1Index + 1 : (UINT)ceilf(posFactor);
					if (pos2Index > positionNodeAnim->mNumPositionKeys - 1)
					{
						pos2Index = pos1Index;
					}
					UINT rot1Index = (UINT)floorf(rotFactor);
					UINT rot2Index = (UINT)rotFactor == rot1Index ? rot1Index + 1 : (UINT)ceilf(rotFactor);
					if (rot2Index > rotationNodeAnim->mNumRotationKeys - 1)
					{
						rot2Index = rot1Index;
					}
					aiVectorKey position1 = positionNodeAnim->mPositionKeys[pos1Index];
					aiVectorKey position2 = positionNodeAnim->mPositionKeys[pos2Index];
					aiQuatKey orientation1 = rotationNodeAnim->mRotationKeys[rot1Index];
					aiQuatKey orientation2 = rotationNodeAnim->mRotationKeys[rot2Index];
					posFactor -= floorf(posFactor);
					rotFactor -= floorf(rotFactor);

					// Interpolate positions
					joint.pos.x = ((1 - posFactor) * position1.mValue.x) + (posFactor * position2.mValue.x);
					joint.pos.y = ((1 - posFactor) * position1.mValue.y) + (posFactor * position2.mValue.y);
					joint.pos.z = ((1 - posFactor) * position1.mValue.z) + (posFactor * position2.mValue.z);
					aiQuaternion orientation;
					//Interpolate orientations
					aiQuaternion::Interpolate(orientation, orientation1.mValue, orientation2.mValue, rotFactor);
					//orientation = orientation.Normalize();
					joint.orientation = XMFLOAT4(-orientation.x, -orientation.y, -orientation.z, orientation.w);
				}
				
				//If the joint is the root joint, its current position and orientation are correct.
				if (rootBone)
				{
					joint.orientation = VF4(XMQuaternionNormalize(XMQuaternionMultiply(FV(rootBoneRotation), FV(joint.orientation))));
				}
				else
				{
					Joint& parentJoint = frameJoints[joint.parentID];
					XMVECTOR parentOrientation = FV(parentJoint.orientation);
					joint.pos = VF3(XMQuaternionMultiply(XMQuaternionMultiply(parentOrientation, FV(joint.pos)), XMQuaternionConjugate(parentOrientation)));
					joint.pos.x += parentJoint.pos.x;
					joint.pos.y += parentJoint.pos.y;
					joint.pos.z += parentJoint.pos.z;
					joint.orientation = VF4(XMQuaternionNormalize(XMQuaternionMultiply(parentOrientation, FV(joint.orientation))));
				}
				frameJoints.push_back(joint);
			}
			animation->frameSkeleton.push_back(frameJoints);
		}
	}
}

std::wstring AssetLoadingUtil::FixBoneName(std::wstring name)
{
	UINT index = 0;
	UINT count = 0;
	UINT firstUnderScoreIndex = name.length();
	std::wstring::iterator pos = name.begin();
	while (pos != name.end())
	{
		if ((*pos == L'A' || *pos == L'a') && count == 0)
		{
			count++;
		}
		else if ((*pos == L'S' || *pos == L's') && (count == 1 || count == 2))
		{
			count++;
		}
		else if ((*pos == L'I' || *pos == L'i') && count == 3)
		{
			count++;
		}
		else if ((*pos == L'M' || *pos == L'm') && count == 4)
		{
			count++;
		}
		else if ((*pos == L'P' || *pos == L'p') && count == 5)
		{
			count++;
		}
		else if (*pos == L'_' && index < firstUnderScoreIndex)
		{
			firstUnderScoreIndex = index;
		}
		++pos;
		index++;
	}
	//Assimp can add a substring to the string that contains the substring "assimp" (ignore case). 
	//If the count is 6, then we know that the "assimp" substring is in the name. 
	//Assimp will always add an underscore a few characters before this.
	//Therefore the name string will be trimmed from the position of last underscore to the end.
	if (count == 6)
	{
		return name.substr(0, firstUnderScoreIndex);
	}
	return name;
}

std::map<MaterialTextureType, UINT> AssetLoadingUtil::textureTypeMappings;