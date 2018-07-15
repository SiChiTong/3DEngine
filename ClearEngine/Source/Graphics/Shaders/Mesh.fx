#include "LightUtil.fx"

struct ModelInstance
{
	float4x4 world;
};

struct Mesh
{
	float4 diffuse;
	float3 specular;
	bool hasDiffuseTexture;
	bool hasNormalMap;
	bool hasSpecular;
	bool hasGloss;
	bool hasTransparency;
};

cbuffer SceneParamsCBuffer : register(b0)
{
	float4x4 VP;
	float3 camPos;
	float ambient;
	uint dirLightCount;
	uint pointLightCount;
	uint spotLightCount;
	uint pad3;
}

cbuffer ModelCBuffer : register(b1)
{
	ModelInstance model;
};

cbuffer MeshCBuffer : register(b2)
{
	Mesh mesh;
};

struct Joint
{
	float3 pos;
	float parentID;
	float4 orientation;
};

struct Weight
{
	float3 pos;
	int jointID;
	float3 normal;
	float bias;
	float3 tangent;
	float pad;
};

StructuredBuffer<ShaderDirectionalLight> dirLights : register(t0);
StructuredBuffer<ShaderPointLight> pointLights : register(t1);
StructuredBuffer<ShaderSpotLight> spotLights : register(t2);
StructuredBuffer<Joint> joints : register(t3);
StructuredBuffer<Weight> weights : register(t4);
Texture2D diffuseTexture : register(t5);
Texture2D bumpMap : register(t6);
Texture2D specularMap : register(t7);
Texture2D glossMap : register(t8);

SamplerState ObjSamplerState : register(s0);

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float4 worldPos : POSITION;
	float2 texCoord : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
};

float4 GetQuaternionProduct(float4 q1, float4 q2)
{
	float4 result;
	result.x = (q2.w * q1.x) + (q2.x * q1.w) + (q2.y * q1.z) - (q2.z * q1.y);
	result.y = (q2.w * q1.y) - (q2.x * q1.z) + (q2.y * q1.w) + (q2.z * q1.x);
	result.z = (q2.w * q1.z) + (q2.x * q1.y) - (q2.y * q1.x) + (q2.z * q1.w);
	result.w = (q2.w * q1.w) - (q2.x * q1.x) - (q2.y * q1.y) - (q2.z * q1.z);
	return result;
}

VS_OUTPUT VS(float4 pos : POSITION, float2 texCoord : TEXCOORD, float3 normal : NORMAL, float3 tangent : TANGENT)
{
	float4x4 wvp = mul(model.world, VP);

	VS_OUTPUT output = (VS_OUTPUT)0;
	output.pos = mul(pos, wvp);
	output.worldPos = mul(pos, model.world);

	output.normal = mul(normal, model.world);

	if (mesh.hasNormalMap)
	{
		output.tangent = mul(tangent, model.world);
	}
	else
	{
		output.tangent = tangent;
	}

	output.texCoord = texCoord;
	return output;
}

VS_OUTPUT ANIM_VS(float2 texCoord : TEXCOORD, int startWeight : UINT0, int weightCount : UINT1)
{
	float4x4 wvp = mul(model.world, VP);

	VS_OUTPUT output = (VS_OUTPUT)0;

	output.pos = float4(0, 0, 0, 0);
	output.normal = float3(0, 0, 0);
	output.tangent = float3(0, 0, 0);

	Weight weight;
	Joint joint;
	float4 orientationConjugate;

	int amount = startWeight + weightCount;
	for (int i = startWeight; i < amount; i++)
	{
		weight = weights[i];
		joint = joints[weight.jointID];
		orientationConjugate = float4(-joint.orientation.x, -joint.orientation.y, -joint.orientation.z, joint.orientation.w);

		float4 temp = float4(weight.pos.x, weight.pos.y, weight.pos.z, 0);
		temp = GetQuaternionProduct(GetQuaternionProduct(joint.orientation, temp), orientationConjugate);

		output.pos.x += (joint.pos.x + temp.x) * weight.bias;
		output.pos.y += (joint.pos.y + temp.y) * weight.bias;
		output.pos.z += (joint.pos.z + temp.z) * weight.bias;
		output.pos.w = 0.0f;

		temp = float4(weight.normal.x, weight.normal.y, weight.normal.z, 0.0f);

		temp = GetQuaternionProduct(GetQuaternionProduct(joint.orientation, temp), orientationConjugate);
		output.normal.x += temp.x * weight.bias;
		output.normal.y += temp.y * weight.bias;
		output.normal.z += temp.z * weight.bias;

		if (mesh.hasNormalMap)
		{
			temp = float4(weight.tangent.x, weight.tangent.y, weight.tangent.z, 0.0f);

			temp = GetQuaternionProduct(GetQuaternionProduct(joint.orientation, temp), orientationConjugate);
			output.tangent.x += temp.x * weight.bias;
			output.tangent.y += temp.y * weight.bias;
			output.tangent.z += temp.z * weight.bias;
		}
	}
	output.normal = normalize(output.normal);

	output.texCoord = texCoord;

	output.worldPos = mul(output.pos, model.world);
	output.pos = mul(output.pos, wvp);
	output.normal = mul(output.normal, model.world);

	if (mesh.hasNormalMap)
	{
		output.tangent = normalize(output.tangent);
		output.tangent = mul(output.tangent, model.world);
	}
	return output;
}

VS_OUTPUT INST_VS(float4 pos : POSITION, float2 texCoord : TEXCOORD, float3 normal : NORMAL, float3 tangent : TANGENT, float4x4 instanceWorld : WORLD)
{
	float4x4 wvp = mul(instanceWorld, VP);

	VS_OUTPUT output = (VS_OUTPUT)0;
	output.pos = mul(pos, wvp);
	output.worldPos = mul(pos, instanceWorld);

	output.normal = mul(normal, instanceWorld);

	if (mesh.hasNormalMap)
	{
		output.tangent = mul(tangent, instanceWorld);
	}
	else
	{
		output.tangent = tangent;
	}

	output.texCoord = texCoord;
	return output;
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
	input.normal = normalize(input.normal);
	input.tangent = normalize(input.tangent);

	//The ambient light effecting the material should be calculated in the renderer before the material is drawn.
	Material mat;

	//If material has a diffuse texture, set it now.
	if (mesh.hasDiffuseTexture)
	{
		mat.diffuse = diffuseTexture.Sample(ObjSamplerState, input.texCoord);
	}
	else
	{
		//If no diffuse texture, use diffuse colour.
		mat.diffuse = mesh.diffuse;
	}
	//saturate(mat.diffuse.a);
	//If material has a specular map, set it now.
	if (mesh.hasSpecular)
	{
		mat.specular = specularMap.Sample(ObjSamplerState, input.texCoord);
	}
	else
	{
		mat.specular = float4(mesh.specular, 1.0f);
	}

	if (mesh.hasGloss)
	{
		mat.gloss = glossMap.Sample(ObjSamplerState, input.texCoord).w;
	}

	//If material has a normal map, set it now.
	if (mesh.hasNormalMap)
	{
		//Load normal from normal map
		float4 normalMap = bumpMap.Sample(ObjSamplerState, input.texCoord);

		//Change normal map range from [0, 1] to [-1, 1]
		normalMap = (2.0f*normalMap) - 1.0f;

		//Make sure tangent is completely orthogonal to normal
		input.tangent = normalize(input.tangent - dot(input.tangent, input.normal)*input.normal);

		//Create the biTangent
		float3 biTangent = cross(input.normal, input.tangent);

		//Create the "Texture Space"
		float3x3 texSpace = float3x3(input.tangent, biTangent, input.normal);

		//Convert normal from normal map to texture space and store in input.normal
		input.normal = normalize(mul(normalMap, texSpace));
	}

	//Create a float3 of the input position. Easier to use.
	float3 pixPos = float3(input.pos.x, input.pos.y, input.pos.z);
	//Calculate the direction from the pixel to the camera.
	float3 toEyeVec = normalize(camPos - pixPos);

	float4 colour = float4(0.0f, 0.0f, 0.0f, 0.0f);

	//These variables are reassigned every time a light computational function is called.
	float4 specular;

	for (int i = 0; i < dirLightCount; i++)
	{
		if (colour.x >= 1 && colour.y >= 1 && colour.z >= 1)
		{
			break;
		}

		ShaderDirectionalLight light = dirLights[i];

		ComputeShaderDirectionalLight(light, mat, input.normal, toEyeVec, diffuse, specular);
		colour += saturate(diffuse + specular);
	}

	for (int i = 0; i < pointLightCount; i++)
	{
		if (colour.x >= 1 && colour.y >= 1 && colour.z >= 1)
		{
			break;
		}

		ShaderPointLight light = pointLights[i];

		ComputeShaderPointLight(light, mat, pixPos, input.normal, toEyeVec, diffuse, specular);
		colour += saturate(diffuse + specular);
	}

	for (int i = 0; i < spotLightCount; i++)
	{
		if (colour.x >= 1 && colour.y >= 1 && colour.z >= 1)
		{
			break;
		}

		ShaderSpotLight light = spotLights[i];

		ComputeShaderSpotLight(light, mat, pixPos, input.normal, toEyeVec, diffuse, specular);
		colour += saturate(diffuse + specular);
	}

	// Add ambient light value
	colour.x += ambient;
	colour.y += ambient;
	colour.z += ambient;
	//Make sure the values are between 0 and 1
	colour = saturate(colour);

	//Return Final Color
	return float4(colour.x, colour.y, colour.z, mesh.hasTransparency ? colour.w : 1.0f);
}