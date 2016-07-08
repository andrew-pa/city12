
cbuffer cb0 : register(b0) {
	float2 resolution;
	bool horiz;
}


SamplerState samp : register(s0);
Texture2D src : register(t0);

float4 main(float4 pos : SV_POSITION) : SV_TARGET
{
	//const float weight[5] = { 0.227027f, 0.1945946f, 0.1216216f, 0.054054f, 0.016216f };
	const float weight[8] = { 0.197448, 0.174697, 0.120999, 0.065602, 0.02784, 0.009246, 0.002403, 0.000489, };
	const float2 txls = 1.f / resolution;
	float2 uv = pos.xy * txls;
	float3 res = src.Sample(samp, uv).rgb * weight[0];
	float2 offset = horiz ? float2(txls.x, 0.f) : float2(0.f, txls.y);
	for (int i = 1; i < 8; ++i) {
		res += src.Sample(samp, uv + offset*((float)i)).rgb * weight[i];
		res += src.Sample(samp, uv - offset*((float)i)).rgb * weight[i];
	}
	return float4(res, 1.0f);
}