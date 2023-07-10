//頂点シェーダからの出力、ピクセルシェーダからの入力

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD;
};

//struct VS_OUTPUT
//{
//	float4 pos : SV_POSITION;
//	float2 uv : TEXCOORD;
//	float4 brightness : BRIGHTNESS;
//};