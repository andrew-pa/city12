

cbuffer cb0 : register(b0)
{
	float4x4 view_proj;
};

#define block_world_size 50.f
#define street_width 15.f

// This helper function returns 1.0 if the current pixel is on a grid line, 0.0 otherwise
float IsGridLine(float2 fragCoord)
{
	// Define the size we want each grid square in pixels
	float2 vPixelsPerGridSquare = float2(block_world_size, block_world_size);

	// fragCoord is an input to the shader, it defines the pixel co-ordinate of the current pixel
	float2 vScreenPixelCoordinate = fragCoord.xy;

	// Get a value in the range 0->1 based on where we are in each grid square
	// fract() returns the fractional part of the value and throws away the whole number part
	// This helpfully wraps numbers around in the 0->1 range
	float2 vGridSquareCoords = frac(vScreenPixelCoordinate / vPixelsPerGridSquare);

	// Convert the 0->1 co-ordinates of where we are within the grid square
	// back into pixel co-ordinates within the grid square 
	float2 vGridSquarePixelCoords = vGridSquareCoords * vPixelsPerGridSquare;

	// step() returns 0.0 if the second parmeter is less than the first, 1.0 otherwise
	// so we get 1.0 if we are on a grid line, 0.0 otherwise
	float2 vIsGridLine = step(vGridSquarePixelCoords, float2(street_width, street_width));

	// Combine the x and y gridlines by taking the maximum of the two values
	float fIsGridLine = max(vIsGridLine.x, vIsGridLine.y);

	// return the result
	return fIsGridLine;
}

#include "psc.hlsli"

float stripes(float2 p) {
	return step(frac((p.x - (street_width*.5f)) / block_world_size), .01f)*step(abs(sin(p.y)), 0.8f)* (1.f - step(frac(p.y / block_world_size)*block_world_size, street_width));
}

float intersection(float2 p) {
	float2 gp = frac(p / block_world_size)*block_world_size;
	float2 v = step(gp, street_width);
	float mask = 1.f-abs(v.x-v.y);
	gp -= street_width*.5f;
	float d = max(abs(gp.x), abs(gp.y)) - 6.;
	return mask * 1.-step(d, 0.f);
}

PSOutput main(PSInput input) : SV_TARGET
{
	float g = IsGridLine(input.pos_world.xz);
	float2 p = input.pos_world.xz;
	float4 street_col = float4(0.02f, 0.02f, 0.02f, 1.f) +
		float4(0.3f, 0.3f, 0.3f, 0.f) * (stripes(p) + stripes(p.yx) + intersection(p));
	return OV(g*street_col + float4(0.02f,0.02f,0.02f,1.f));
}