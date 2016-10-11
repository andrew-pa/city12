
struct PSInput
{
	float4 position	: SV_POSITION;
	float3 normal : NORMAL;
	float3 pos_world : POSITION;
	float3 pos_local : POSITION1;
	float2 uv		: TEXCOORD0;
#ifdef INSTANCED
	uint iid : ID;
#endif
};


struct PSOutput {
	float4 color : SV_TARGET0;
	float4 overflow: SV_TARGET1;
};

PSOutput OV(float4 col, PSInput i) {
	PSOutput o;
	float L = dot(col.rgb, float3(0.2126f, 0.7152f, 0.0722f));
	if (L > .9f)
		o.overflow = col;
	else
		o.overflow = float4(0.f, 0.f, 0.f, 0.f);
	o.color = col;
	return o;
}