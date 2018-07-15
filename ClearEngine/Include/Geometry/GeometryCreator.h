#pragma once

#include <vector>
#include <istream>
#include <map>
#include <fstream>
#include <iostream>
#include "Common/DirectX.h"
#include "Loading/GameAssets.h"
#include "Loading/Textures/DDSTextureLoader.h"
#include "Geometry/Models.h"
#include "Graphics/Rendering/GraphicsTypes.h"
#include "Common/GeneralUtils.h"
#include "Utils/GeometryUtils.h"


class GeometryCreator
{
public:
	GeometryCreator(ID3D11Device1* device);
	~GeometryCreator();
	void LoadTerrainFromBitmap(std::wstring fileName, RigidModel& model);
	void LoadTerrainFromBitmap(HeightMapInfo &hmInfo, RigidModel& model);
	void LoadHeightMap(std::wstring fileName, HeightMapInfo &hmInfo);
	void CreateCube(RigidModel& model);
	void CreateSimpleCube(RigidModel& model);
	void CreatePlane(RigidModel& model);
	void CreateSphere(RigidModel& model);

private:
	ID3D11Device1* device;
};

