#include"Game.hlsli"

cbuffer cbuff : register(b0)
{
	matrix mat;
}

VS_OUTPUT SpriteVS(float4 pos : POSITION, float2 uv : TEXCOORD)
{
	VS_OUTPUT output;

	output.pos = mul(mat, pos);
	output.uv = uv;

	return output;
}