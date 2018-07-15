#pragma once

#include "Graphics/Rendering/GraphicsHelper.h"
#include "Geometry/Models.h"
#include "Graphics/Rendering/GraphicsTypes.h"
#include "Graphics/Resources/VertexShader.h"
#include "Graphics/Resources/PixelShader.h"
#include "Graphics/Resources/ConstantBuffer.h" 
#include "Graphics/Resources/SamplerState.h"
#include "Graphics/Resources/VertexBuffer.h"
#include "Graphics/Resources/IndexBuffer.h"

class Sky
{
	public:
		Sky(GraphicsHelper* graphicsHelper, int latLines, int longLines);
		~Sky();
		void Draw(double time, const DirectX::XMMATRIX& vp, const DirectX::XMFLOAT3& camPosition, ID3D11ShaderResourceView* texture = NULL);
		ID3D11ShaderResourceView* GetTexture();
		void SetTexture(ID3D11ShaderResourceView* texture);
		void CleanUp();
		float camDist;

	private:
		struct SkyVertex	//Overloaded DVertex Structure
		{
			template<class Archive>
			void serialize(Archive & ar, const unsigned int version)
			{
				ar & pos.x;
				ar & pos.y;
				ar & pos.z;
			}

			SkyVertex(){}
			SkyVertex(float x, float y, float z)
				: pos(x,y,z){}

			DirectX::XMFLOAT3 pos;
		};

		struct SkyCStruct
		{
			DirectX::XMMATRIX  wvp;
		};

		void Initialize();
		GraphicsHelper* graphicsHelper;
		ID3D11ShaderResourceView* texture;
		void CreateSkyData(int latLines, int longLines);
		VertexBuffer<SkyVertex>* vertexBuffer;
		IndexBuffer* indexBuffer;
		VertexShader* vertexShader;
		PixelShader* pixelShader;
		ConstantBuffer<SkyCStruct>* constantBuffer;
		SamplerState* samplerState;
		int indicesCount;
};
