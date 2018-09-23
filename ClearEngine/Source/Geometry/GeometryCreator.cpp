#include "Math/SIMDConversionUtils.h"
#include "Geometry/GeometryCreator.h"
#include "Graphics/Resources/VertexBuffer.h"
#include "Graphics/Resources/IndexBuffer.h"
#include "Graphics/Resources/StructuredBuffer.h"

using namespace DirectX;

GeometryCreator::GeometryCreator(ID3D11Device1* device)
: device(device) {}

GeometryCreator::~GeometryCreator() {}

void GeometryCreator::LoadTerrainFromBitmap(std::wstring fileName, RigidModel& model)
{
	HeightMapInfo hmInfo;
	LoadHeightMap(fileName, hmInfo);
	LoadTerrainFromBitmap(hmInfo, model);
}

void GeometryCreator::LoadTerrainFromBitmap(HeightMapInfo& hmInfo, RigidModel& model)
{
	int cols = hmInfo.terrainWidth;
	int rows = hmInfo.terrainHeight;

	//Create the grid
	UINT numVertices = rows * cols;
	UINT numTriangles = (rows - 1)*(cols - 1) * 2;

	model.vertices = std::vector<DVertex>(numVertices);

	for (DWORD i = 0; i < rows; ++i)
	{
		for (DWORD j = 0; j < cols; ++j)
		{
			//Adjust to make sure minimum height of terrain is zero.
			XMFLOAT3 pos = hmInfo.heightMap[i*cols + j];
			pos.y -= hmInfo.minHeight;
			model.vertices[i*cols + j].pos = pos;
			model.vertices[i*cols + j].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			model.vertices[i*cols + j].tangent = XMFLOAT3(0.0f, 1.0f, 0.0f);
		}
	}

	model.indices = std::vector<DWORD>(numTriangles * 3);

	int k = 0;
	int texUIndex = 0;
	int texVIndex = 0;
	for (DWORD i = 0; i < rows - 1; i++)
	{
		for (DWORD j = 0; j < cols - 1; j++)
		{
			model.indices[k] = i*cols + j;		// Bottom left of quad
			model.vertices[i*cols + j].texCoord = XMFLOAT2(texUIndex + 0.0f, texVIndex + 1.0f);

			model.indices[k + 1] = (i + 1)*cols + j;	// Top left of quad
			model.vertices[(i + 1)*cols + j].texCoord = XMFLOAT2(texUIndex + 0.0f, texVIndex + 0.0f);

			model.indices[k + 2] = (i + 1)*cols + j + 1;	// Top right of quad
			model.vertices[(i + 1)*cols + j + 1].texCoord = XMFLOAT2(texUIndex + 1.0f, texVIndex + 0.0f);

			model.indices[k + 3] = i*cols + j;		// Bottom left of quad
			model.vertices[i*cols + j].texCoord = XMFLOAT2(texUIndex + 0.0f, texVIndex + 1.0f);

			model.indices[k + 4] = (i + 1)*cols + j + 1;	// Top right of quad
			model.vertices[(i + 1)*cols + j + 1].texCoord = XMFLOAT2(texUIndex + 1.0f, texVIndex + 0.0f);

			model.indices[k + 5] = i*cols + j + 1;		// Bottom right of quad
			model.vertices[i*cols + j + 1].texCoord = XMFLOAT2(texUIndex + 1.0f, texVIndex + 1.0f);

			k += 6; // next quad

			texUIndex++;
		}
		texUIndex = 0;
		texVIndex++;
	}

	SurfaceMaterial material;
	material.diffuse = XMFLOAT4(0.0f, 0.0f, 0.75f, 0.0f);
	material.name = L"default";
	model.materials.push_back(material);

	ModelSubset subset;
	subset.indexCount = model.indices.size();
	subset.matIndex = 0;
	subset.vertStartIndex = 0;
	model.subsets.push_back(subset);

	//Compute normals and tangents
	GeometryUtils::ComputeRigidModelData(model);

	GeometryUtils::CreateRigidModelObjects(device, model);
}

void GeometryCreator::LoadHeightMap(std::wstring fileName, HeightMapInfo &hmInfo)
{
	FILE *filePtr;							// Point to the current position in the file
	BITMAPFILEHEADER bitmapFileHeader;		// Structure which stores information about file
	BITMAPINFOHEADER bitmapInfoHeader;		// Structure which stores information about image
	int imageSize, index;
	unsigned char height;

	std::string temp;
	GeneralUtils::WStringToString(fileName, temp);
	const char* fileLocation = temp.c_str();
	// Open the file
	errno_t error = fopen_s(&filePtr, fileLocation, "r");

	assert(filePtr != NULL);

	// Read bitmaps header
	fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);

	// Read the info header
	fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);

	// Get the width and height (width and length) of the image
	hmInfo.terrainWidth = bitmapInfoHeader.biWidth;
	hmInfo.terrainHeight = bitmapInfoHeader.biHeight;

	// Size of the image in bytes. the 3 represents RBG (byte, byte, byte) for each pixel
	imageSize = hmInfo.terrainWidth * hmInfo.terrainHeight * 3;

	// Initialize the array which stores the image data
	unsigned char* bitmapImage = new unsigned char[imageSize];

	// Set the file pointer to the beginning of the image data
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// Store image data in bitmapImage
	fread(bitmapImage, 1, imageSize, filePtr);

	// Close file
	fclose(filePtr);

	// Initialize the heightMap array (stores the vertices of our terrain)
	hmInfo.heightMap = new XMFLOAT3[hmInfo.terrainWidth * hmInfo.terrainHeight];

	hmInfo.minHeight = 0;

	// We use a greyscale image, so all 3 rgb values are the same, but we only need one for the height
	// So we use this counter to skip the next two components in the image data (we read R, then skip BG)
	int k = 0;

	// We divide the height by this number to "water down" the terrains height, otherwise the terrain will
	// appear to be "spikey" and not so smooth.
	float heightFactor = 10.0f;

	// Read the image data into our heightMap array
	for (int j = 0; j< hmInfo.terrainHeight; j++)
	{
		for (int i = 0; i< hmInfo.terrainWidth; i++)
		{
			height = bitmapImage[k];

			index = (hmInfo.terrainWidth * j) + i;

			float actualHeight = (float)height / heightFactor;

			if (actualHeight < hmInfo.minHeight)
			{
				hmInfo.minHeight = actualHeight;
			}

			hmInfo.heightMap[index].x = (float)i;
			hmInfo.heightMap[index].y = actualHeight;
			hmInfo.heightMap[index].z = (float)j;

			k += 3;
		}
	}
	
	delete[] bitmapImage;
	bitmapImage = 0;
}

//Creates a unit cube
void GeometryCreator::CreateCube(RigidModel& model)
{	
	//Normals below will be overwritten and calculated later in the ComputeRigidModelData function based on the average of the normals of the polygons they belong to.
	DVertex vertices[] =
	{
		// Front Face
		DVertex(-1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f),
		DVertex(-1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f),
		DVertex(1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f),
		DVertex(1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f),

		// Back Face
		DVertex(-1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f),
		DVertex(1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f),
		DVertex(1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f),
		DVertex(-1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f),

		// Top Face
		DVertex(-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f),
		DVertex(-1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f),
		DVertex(1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f),
		DVertex(1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f),

		// Bottom Face
		DVertex(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f),
		DVertex(1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f),
		DVertex(1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f),
		DVertex(-1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f),

		// Left Face
		DVertex(-1.0f, -1.0f, 1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f),
		DVertex(-1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f),
		DVertex(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f),
		DVertex(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f),

		// Right Face
		DVertex(1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f),
		DVertex(1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f),
		DVertex(1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f),
		DVertex(1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f),
	};

	model.vertices = std::vector<DVertex>(std::begin(vertices), std::end(vertices));

	DWORD indices[] = {
		// Front Face
		0, 1, 2,
		0, 2, 3,

		// Back Face
		4, 5, 6,
		4, 6, 7,

		// Top Face
		8, 9, 10,
		8, 10, 11,

		// Bottom Face
		12, 13, 14,
		12, 14, 15,

		// Left Face
		16, 17, 18,
		16, 18, 19,

		// Right Face
		20, 21, 22,
		20, 22, 23
	};

	model.indices = std::vector<DWORD>(std::begin(indices), std::end(indices));

	SurfaceMaterial material;
	material.name = L"default";
	material.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	material.hasTransparency = true;
	
	model.materials.push_back(material);

	ModelSubset subset;
	subset.indexCount = model.indices.size();
	subset.matIndex = 0;
	subset.vertStartIndex = 0;
	model.subsets.push_back(subset);

	//Compute normals and tangents
	GeometryUtils::ComputeRigidModelData(model);

	GeometryUtils::CreateRigidModelObjects(device, model);
}

//Creates a unit cube
void GeometryCreator::CreateSimpleCube(RigidModel& model)
{
	//Normals below will be overwritten and calculated later in the ComputeRigidModelData function based on the average of the normals of the polygons they belong to.
	DVertex vertices[] =
	{
		// Front Face
		DVertex(-1.0f, -1.0f, -1.0f, 0.0f, 1.0f),
		DVertex(-1.0f, 1.0f, -1.0f, 0.0f, 0.0f),
		DVertex(1.0f, 1.0f, -1.0f, 1.0f, 0.0f),
		DVertex(1.0f, -1.0f, -1.0f, 1.0f, 1.0f),

		//Back Face
		DVertex(1.0f, -1.0f, 1.0f, 0.0f, 1.0f),
		DVertex(1.0f, 1.0f, 1.0f, 0.0f, 0.0f),
		DVertex(-1.0f, 1.0f, 1.0f, 1.0f, 0.0f),
		DVertex(-1.0f, -1.0f, 1.0f, 1.0f, 1.0f)
	};

	model.vertices = std::vector<DVertex>(std::begin(vertices), std::end(vertices));

	DWORD indices[] = {
		// Front Face
		0, 1, 2,
		0, 2, 3,

		// Back Face
		4, 5, 6,
		4, 6, 7,

		// Top Face
		6, 5, 2,
		6, 2, 1,

		// Bottom Face
		4, 7, 0,
		4, 0, 3,

		// Left Face
		7, 6, 1,
		7, 1, 0,

		// Right Face
		3, 2, 5,
		3, 5, 4
	};

	model.indices = std::vector<DWORD>(std::begin(indices), std::end(indices));

	SurfaceMaterial material;
	material.name = L"default";
	material.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	material.hasTransparency = true;

	model.materials.push_back(material);

	ModelSubset subset;
	subset.indexCount = model.indices.size();
	subset.matIndex = 0;
	subset.vertStartIndex = 0;
	model.subsets.push_back(subset);

	//Compute normals and tangents
	GeometryUtils::ComputeRigidModelData(model);

	GeometryUtils::CreateRigidModelObjects(device, model);
}

//Creates a unit plane
void GeometryCreator::CreatePlane(RigidModel& model)
{
	//Normals below will be overwritten and calculated later in the ComputeRigidModelData function based on the average of the normals of the polygons they belong to.
	DVertex vertices[] =
	{
		// Facing up
		DVertex(-1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f),
		DVertex(-1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f),
		DVertex(1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f),
		DVertex(1.0f, 0.0f, -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f),
	};

	model.vertices = std::vector<DVertex>(std::begin(vertices), std::end(vertices));

	DWORD indices[] = {
		0, 1, 2,
		0, 2, 3,
	};

	model.indices = std::vector<DWORD>(std::begin(indices), std::end(indices));

	SurfaceMaterial material;
	material.name = L"default";
	material.diffuse = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	material.hasTransparency = false;

	model.materials.push_back(material);

	ModelSubset subset;
	subset.indexCount = model.indices.size();
	subset.matIndex = 0;
	subset.vertStartIndex = 0;
	model.subsets.push_back(subset);

	//Compute normals and tangents
	GeometryUtils::ComputeRigidModelData(model);

	GeometryUtils::CreateRigidModelObjects(device, model);
}

//Creates a unit sphere
void GeometryCreator::CreateSphere(RigidModel& model)
{
	//For the sphere, there is one vertex at the front and one at the back. 
	//The longitude lines are around the x-axis and the latitude lines are around the z-axis.
	int latLines = 30.0f;
	int longLines = 30.0f;

	int numSphereVertices = ((latLines - 2) * longLines) + 2;
	int numSphereFaces = ((latLines - 3)*(longLines)* 2) + (longLines * 2);

	float sphereYaw = 0.0f;
	float spherePitch = 0.0f;

	std::vector<DVertex> vertices(numSphereVertices);
	XMVECTOR upNormal = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR rightNormal = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR currVertPos = upNormal;
	XMVECTOR pitchNormal;
	XMVECTOR yawNormal;
	vertices[0].pos.x = 0.0f;
	vertices[0].pos.y = 1.0f;
	vertices[0].pos.z = 0.0f;
	vertices[0].texCoord.x = 0.5f;
	vertices[0].texCoord.y = 1.0f;

	XMMATRIX rotationX;
	XMMATRIX rotationY;
	for (DWORD i = 0; i < latLines - 2; ++i)
	{
		spherePitch = (i + 1) * (3.14 / (latLines - 1));
		rotationX = XMMatrixRotationX(spherePitch);
		//yawNormal is really around z axis.
		for (DWORD j = 0; j < longLines; ++j)
		{
			sphereYaw = j * (6.28 / (longLines));
			rotationY = XMMatrixRotationY(sphereYaw);
			currVertPos = XMVector3TransformNormal(upNormal, (rotationX * rotationY));
			currVertPos = XMVector3Normalize(currVertPos);
			pitchNormal = XMVector3Normalize(XMVector3TransformNormal(upNormal, rotationX));
			yawNormal = XMVector3Normalize(XMVector3TransformNormal(upNormal, rotationY));

			int index = i*longLines + j + 1;
			vertices[index].pos.x = XMVectorGetX(currVertPos);
			vertices[index].pos.y = XMVectorGetY(currVertPos);
			vertices[index].pos.z = XMVectorGetZ(currVertPos);

			float dot = XMVectorGetX(XMVector3Dot(pitchNormal, upNormal));
			vertices[index].texCoord.y = dot;
			dot = XMVectorGetX(XMVector3Dot(yawNormal, rightNormal));
			vertices[index].texCoord.x = dot;
		}
	}

	vertices[numSphereVertices - 1].pos.x = 0.0f;
	vertices[numSphereVertices - 1].pos.y = -1.0f;
	vertices[numSphereVertices - 1].pos.z = 0.0f;
	vertices[numSphereVertices - 1].texCoord.x = 0.5f;
	vertices[numSphereVertices - 1].texCoord.y = 0.0f;

	model.vertices = std::vector<DVertex>(std::begin(vertices), std::end(vertices));

	int numIndices = numSphereFaces * 3;
	std::vector<DWORD>indices(numIndices);

	int k = 0;

	//*Indices for the back vertices of the sphere.
	for (DWORD l = 0; l < longLines - 1; ++l)
	{
		indices[k] = 0;
		indices[k + 1] = l + 1;
		indices[k + 2] = l + 2;
		k += 3;
	}

	indices[k] = 0;
	indices[k + 1] = longLines;
	indices[k + 2] = 1;
	k += 3;

	//Indices for the main vertices of the sphere.
	for (DWORD i = 0; i < latLines - 3; ++i)
	{
		for (DWORD j = 0; j < longLines - 1; ++j)
		{
			indices[k] = i*longLines + j + 1;
			indices[k + 1] = (i + 1)*longLines + j + 1;
			indices[k + 2] = i*longLines + j + 2; 

			indices[k + 3] = (i + 1)*longLines + j + 1;
			indices[k + 4] = (i + 1)*longLines + j + 2; 
			indices[k + 5] = i*longLines + j + 2;

			k += 6; // next quad
		}

		indices[k] = (i*longLines) + longLines;
		indices[k + 1] = ((i + 1)*longLines) + longLines;
		indices[k + 2] = (i*longLines) + 1; 

		indices[k + 3] = ((i + 1)*longLines) + longLines;
		indices[k + 4] = ((i + 1)*longLines) + 1;
		indices[k + 5] = (i*longLines) + 1; 

		k += 6;
	}

	//Indices for the front vertices of the sphere.
	for (DWORD l = 0; l < longLines - 1; ++l)
	{
		indices[k] = numSphereVertices - 1;
		indices[k + 1] = (numSphereVertices - 1) - (l + 1);
		indices[k + 2] = (numSphereVertices - 1) - (l + 2);
		k += 3;
	}

	indices[k] = numSphereVertices - 1;
	indices[k + 1] = (numSphereVertices - 1) - longLines;
	indices[k + 2] = numSphereVertices - 2;

	model.indices = std::vector<DWORD>(std::begin(indices), std::end(indices));

	SurfaceMaterial material;
	material.name = L"default";
	material.diffuse = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	material.hasTransparency = false;

	model.materials.push_back(material);

	ModelSubset subset;
	subset.indexCount = model.indices.size();
	subset.matIndex = 0;
	subset.vertStartIndex = 0;
	model.subsets.push_back(subset);

	//Compute normals and tangents
	GeometryUtils::ComputeRigidModelData(model);

	GeometryUtils::CreateRigidModelObjects(device, model);
}


