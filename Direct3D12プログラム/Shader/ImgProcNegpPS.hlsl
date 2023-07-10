#include"Game.hlsli"

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

float4 ImgProcNegpPS(VS_OUTPUT input) : SV_TARGET
{
	float4 texel = tex.Sample(smp,input.uv);
	//float gray = (texel.r + texel.g + texel.b) / 3.0;
	//float3 sepia = float3(0.393 * texel.r + 0.769 * texel.g + 0.189 * texel.b,
	//	0.349 * texel.r + 0.686 * texel.g + 0.168 * texel.b,
	//	0.272 * texel.r + 0.534 * texel.g + 0.131 * texel.b);

	//return float4(sepia, 1.0);
	return float4(1.0 - texel.r, 1.0 - texel.g, 1.0 - texel.b, 1.0);
}