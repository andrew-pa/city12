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
cbuffer cb1 : register(b1)
{
	float time;
}

PSOutput main(PSInput input) : SV_TARGET
{
	float4 h = hash42(floor(input.pos_world.xz*.05f));
	float v = max(0.f, sin(time*2.5f + h.z*8.f));
	PSOutput o;
	o.color = float4(1.f, 0.04f+h.y*0.04f, 0.f, 1.f)*v*1.5f;
	o.overflow = v > 0.2f ? float4(1.f, 0.06f+h.y*0.04f, 0.f, 1.f) : float4(0.f, 0.f, 0.f, 0.f);
	return o;
}