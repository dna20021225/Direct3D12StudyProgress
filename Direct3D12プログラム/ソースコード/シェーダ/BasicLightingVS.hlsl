#include"BasicLighting.hlsli"

VS_BASIC_OUTPUT BasicLightingVS(float4 pos : POSITION, float2 uv : TEXCOORD, float4 norm : NORMAL)
{
	VS_BASIC_OUTPUT output;

	float4 posW = mul(world, pos);
	output.pos = mul(viewProj, posW);
	output.uv = uv;
	output.posW = posW;
	norm.w = 0.0;
	output.normW = mul(world, norm);

	return output;
}