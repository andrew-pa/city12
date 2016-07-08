// *** Change these to suit your range of random numbers..

// *** Use this for integer stepped ranges, ie Value-Noise/Perlin noise functions.
//#define HASHSCALE1 .1031
//#define HASHSCALE3 float3(.1031, .1030, .0973)
//#define HASHSCALE4 float4(1031, .1030, .0973, .1099)

// For smaller input rangers like audio tick or 0-1 UVs use these...
#define HASHSCALE1 443.8975
#define HASHSCALE3 float3(443.897, 441.423, 437.195)
#define HASHSCALE4 float4(443.897, 441.423, 437.195, 444.129)



//----------------------------------------------------------------------------------------
//  1 out, 1 in...
float hash11(float p)
{
	float3 p3 = frac(float3(p,p,p) * HASHSCALE1);
	p3 += dot(p3, p3.yzx + 19.19);
	return frac((p3.x + p3.y) * p3.z);
}

//----------------------------------------------------------------------------------------
//  1 out, 2 in...
float hash12(float2 p)
{
	float3 p3 = frac(float3(p.xyx) * HASHSCALE1);
	p3 += dot(p3, p3.yzx + 19.19);
	return frac((p3.x + p3.y) * p3.z);
}

//----------------------------------------------------------------------------------------
//  1 out, 3 in...
float hash13(float3 p3)
{
	p3 = frac(p3 * HASHSCALE1);
	p3 += dot(p3, p3.yzx + 19.19);
	return frac((p3.x + p3.y) * p3.z);
}

//----------------------------------------------------------------------------------------
//  2 out, 1 in...
float2 hash21(float p)
{
	float3 p3 = frac(float3(p,p,p) * HASHSCALE3);
	p3 += dot(p3, p3.yzx + 19.19);
	return frac(float2((p3.x + p3.y)*p3.z, (p3.x + p3.z)*p3.y));
}

//----------------------------------------------------------------------------------------
///  2 out, 2 in...
float2 hash22(float2 p)
{
	float3 p3 = frac(float3(p.xyx) * HASHSCALE3);
	p3 += dot(p3, p3.yzx + 19.19);
	return frac(float2((p3.x + p3.y)*p3.z, (p3.x + p3.z)*p3.y));
}

//----------------------------------------------------------------------------------------
///  2 out, 3 in...
float2 hash23(float3 p3)
{
	p3 = frac(p3 * HASHSCALE3);
	p3 += dot(p3, p3.yzx + 19.19);
	return frac(float2((p3.x + p3.y)*p3.z, (p3.x + p3.z)*p3.y));
}

//----------------------------------------------------------------------------------------
//  3 out, 1 in...
float3 hash31(float p)
{
	float3 p3 = frac(float3(p,p,p) * HASHSCALE3);
	p3 += dot(p3, p3.yzx + 19.19);
	return frac(float3((p3.x + p3.y)*p3.z, (p3.x + p3.z)*p3.y, (p3.y + p3.z)*p3.x));
}


//----------------------------------------------------------------------------------------
///  3 out, 2 in...
float3 hash32(float2 p)
{
	float3 p3 = frac(float3(p.xyx) * HASHSCALE3);
	p3 += dot(p3, p3.yxz + 19.19);
	return frac(float3((p3.x + p3.y)*p3.z, (p3.x + p3.z)*p3.y, (p3.y + p3.z)*p3.x));
}

//----------------------------------------------------------------------------------------
///  3 out, 3 in...
float3 hash33(float3 p3)
{
	p3 = frac(p3 * HASHSCALE3);
	p3 += dot(p3, p3.yxz + 19.19);
	return frac(float3((p3.x + p3.y)*p3.z, (p3.x + p3.z)*p3.y, (p3.y + p3.z)*p3.x));
}

//----------------------------------------------------------------------------------------
// 4 out, 1 in...
float4 hash41(float p)
{
	float4 p4 = frac(float4(p,p,p,p) * HASHSCALE4);
	p4 += dot(p4, p4.wzxy + 19.19);
	return frac(float4((p4.x + p4.y)*p4.z, (p4.x + p4.z)*p4.y, (p4.y + p4.z)*p4.w, (p4.z + p4.w)*p4.x));

}

//----------------------------------------------------------------------------------------
// 4 out, 2 in...
float4 hash42(float2 p)
{
	float4 p4 = frac(float4(p.xyxy) * HASHSCALE4);
	p4 += dot(p4, p4.wzxy + 19.19);
	return frac(float4((p4.x + p4.y)*p4.z, (p4.x + p4.z)*p4.y, (p4.y + p4.z)*p4.w, (p4.z + p4.w)*p4.x));
}

//----------------------------------------------------------------------------------------
// 4 out, 3 in...
float4 hash43(float3 p)
{
	float4 p4 = frac(float4(p.xyzx)  * HASHSCALE4);
	p4 += dot(p4, p4.wzxy + 19.19);
	return frac(float4((p4.x + p4.y)*p4.z, (p4.x + p4.z)*p4.y, (p4.y + p4.z)*p4.w, (p4.z + p4.w)*p4.x));
}

//----------------------------------------------------------------------------------------
// 4 out, 4 in...
float4 hash44(float4 p4)
{
	p4 = frac(p4  * HASHSCALE4);
	p4 += dot(p4, p4.wzxy + 19.19);
	return frac(float4((p4.x + p4.y)*p4.z, (p4.x + p4.z)*p4.y, (p4.y + p4.z)*p4.w, (p4.z + p4.w)*p4.x));
}

//###############################################################################