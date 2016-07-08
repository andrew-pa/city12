
cbuffer cb0 : register(b0) {
	float2 resolution;
}


SamplerState samp : register(s0);
Texture2D src : register(t0);
Texture2D src_bloom : register(t1);


float4 main(float4 pos : SV_POSITION) : SV_TARGET
{
	float2 uv = pos.xy / resolution;
	return pow(src.Sample(samp, uv)+
		src_bloom.Sample(samp, uv), 1.f/2.2f);
}