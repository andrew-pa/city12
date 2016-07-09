#include "hash.hlsli"
#define INSTANCED
#include "psc.hlsli"

cbuffer cb0 : register(b0)
{
	float4x4 view_proj;
};

PSOutput main(PSInput input)
{
	float3 col = float3(0.11f, 0.11f, 0.1f);
	return OV(float4(col, 1.f));
}