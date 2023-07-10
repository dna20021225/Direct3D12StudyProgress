#include"BasicLighting.hlsli"

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

float4 BasicLightingPS(VS_BASIC_OUTPUT input) : SV_TARGET
{
	float3 norm = normalize(input.normW.xyz);
	float4 tcolor = tex.Sample(smp, input.uv);

	float3 lambert =
		max(dot(-lightDir.xyz, norm), 0.0) * lightCol.rgb * kDiffuse.rgb * tcolor.rgb;

	float3 e = normalize(eye.xyz - input.posW.xyz);
	float3 h = normalize(-lightDir.xyz + e);
	float3 specular =
		pow(saturate(dot(norm, h)), kSpecular.a) * lightCol.rgb * kSpecular.rgb;

	float3 ambient = lightCol.rgb * kAmbient.rgb * tcolor.rgb;

	return max(float4(lambert, 1.0) + float4(specular, 0.0), float4(ambient, 1.0));
}