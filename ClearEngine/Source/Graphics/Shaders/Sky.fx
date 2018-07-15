
cbuffer SkyCStruct : register(b0)
{
	float4x4 wvp;
};

TextureCube skyMap : register(t0);
SamplerState objSamplerState : register(s0);

struct VS_OUTPUT	//output structure for skymap vertex shader
{
	float4 pos : SV_POSITION;
	float3 texCoord : TEXCOORD;
};

VS_OUTPUT VS(float3 inPos : POSITION)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	//Set pos to xyww instead of xyzw, so that z will always be 1 (furthest from camera)
	output.pos = mul(float4(inPos, 1.0f), wvp).xyww;
	output.texCoord = float3(inPos.x, inPos.y, inPos.z);

	return output;
}

float4 PS(VS_OUTPUT input) : SV_Target
{
	return skyMap.Sample(objSamplerState, input.texCoord);
}