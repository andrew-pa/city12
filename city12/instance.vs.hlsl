#pragma pack_matrix(row_major)
struct VSInput
{
	float3 position	: POSITION;
	float3 normal	: NORMAL;
	float2 uv		: TEXCOORD0;
	float3 tangent	: TANGENT;
	float4x4 instanceT : TRANSFORM;
	uint instanceID : SV_InstanceID;
};

struct PSInput
{
	float4 position	: SV_POSITION;
	float3 normal : NORMAL;
	float3 pos_world : POSITION;
	float3 pos_local : POSITION1;
	float2 uv		: TEXCOORD0;
	uint iid : ID;
};

cbuffer cb0 : register(b0)
{
	float4x4 view_proj;
};

PSInput main(VSInput input)
{
	PSInput result;

	result.position = mul(float4(input.position, 1.0f), mul(input.instanceT, view_proj));
	result.pos_world = mul(float4(input.position, 1.f), input.instanceT);
	result.pos_local = input.position;
	result.normal = mul(float4(input.normal, 0.f), input.instanceT);
	result.uv = input.uv;
	result.iid = input.instanceID;

	return result;
}
