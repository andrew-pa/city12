#include "hash.hlsli"
#include "psc.hlsli"

struct PSInput
{
	float4 position	: SV_POSITION;
	float3 normal : NORMAL;
	float3 pos_world : POSITION;
	float3 pos_local : POSITION1;
	float2 uv		: TEXCOORD0;
};

cbuffer cb0 : register(b0)
{
	float4x4 view_proj;
};

PSOutput main(PSInput input) : SV_TARGET
{
	if (abs(input.normal.y) > 0.f) return OV(float4(0.01f,0.01f,0.01f,1.f));
	float4 h = hash43(floor(input.pos_world));
	float w = pow(h.x,32.f)*.1f + pow(h.y,128.f)*8.f;//pow(h.x+.01f, 32.f);
	return OV(float4(w,w,w, 1.f));
}