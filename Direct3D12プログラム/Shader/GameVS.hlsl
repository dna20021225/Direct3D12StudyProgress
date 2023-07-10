#include"Game.hlsli"

cbuffer cbuff : register(b0)
{
	float4 eye;
	float4 lightDir;
	float4 lightCol;
	float4 kDiffuse;
	float4 kSpecular;
	float4 kAmbient;
	//float4 light;
	matrix world;
	matrix viewProj;
}

VS_OUTPUT GameVS(float4 pos : POSITION, float2 uv : TEXCOORD)
{
	VS_OUTPUT output;

	output.pos = mul(mul(viewProj, world), pos);
	output.uv = uv;

	return output;
}