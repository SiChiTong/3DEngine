struct Light
{
	float3 pos;
	float  range;
	float3 dir;
	float cone;
	float3 att;
	float4 ambient;
	float4 diffuse;
};

cbuffer LightCStruct
{
	Light light;
};

cbuffer ParticleCStruct
{
	float4x4 wvp;
    float4x4 world;
	float3 particleCentre;
};

Texture2D diffuseTexture;
SamplerState ObjSamplerState;

struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float4 worldPos : POSITION;
	float2 texCoord : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
};

VS_OUTPUT VS(float4 inPos : POSITION, float2 texCoord : TEXCOORD, float3 normal : NORMAL, float3 tangent : TANGENT)
{
    VS_OUTPUT output;

    output.Pos = mul(inPos, wvp);

	output.worldPos = mul(inPos, world);

	output.texCoord = texCoord;

	output.normal = mul(normal, world);

	output.tangent = tangent;

    return output;
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
	//input.normal = normalize(input.normal);

	float4 diffuse = diffuseTexture.Sample( ObjSamplerState, input.texCoord );
	
	//Return Final Color
	return float4(diffuse.x, diffuse.y, diffuse.z, 1.0f);
}