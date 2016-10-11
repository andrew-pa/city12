#include "hash.hlsli"
#define INSTANCED
#include "psc.hlsli"

cbuffer cb0 : register(b0)
{
	float4x4 view_proj;
};

PSOutput main(PSInput input)
{
	if (abs(input.normal.y) > 0.f) return OV(float4(0.01f,0.01f,0.01f,1.f), input);
	float4 h = hash43(floor(input.pos_world));
	float w = pow(h.x,32.f)*.1f + pow(h.y, 256.f+h.z*64.f)*6.f;//pow(h.x+.01f, 32.f);
	return OV(float4(w,w,w, 1.f), input);
}