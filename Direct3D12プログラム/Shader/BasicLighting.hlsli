struct VS_BASIC_OUTPUT
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD;
	float4 posW : WPOSITION;
	float4 normW : WNORM;
	//float4 brightness : BRIGHTNESS;
};
cbuffer cbuff : register(b0)
{
	float4 eye;
	float4 lightDir;
	float4 lightCol;
	float4 kDiffuse;
	float4 kSpecular;
	float4 kAmbient;
	matrix world;
	matrix viewProj;
}