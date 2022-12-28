#include "enables.h"

struct VSInput
{
    float3 position : POSITION0;
    float3 normal   : NORMAL0;
    float2 UV       : UV0;
};
struct VSOutput
{
#if !LOD
    float4 outPosition : SV_POSITION;
#endif
    float4 outNormal    : NORMAL0;
    float4 outWPosition : WSPOS0;
    float2 outUV        : UV0;
};

cbuffer wvpMatrix : register(b0)
{
    matrix worldMatrix;
    matrix normalWMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

VSOutput main(VSInput input)
{
    VSOutput output;
    matrix wvMatrix = mul(worldMatrix, viewMatrix);
    matrix wvpMatrix = mul(wvMatrix, projectionMatrix);
   
    float4 pos = float4(input.position, 1.0f);
    
    output.outWPosition = mul(pos, worldMatrix);
    float4 normal = mul(float4(input.normal, 0.0f), normalWMatrix);
    output.outNormal = normalize(normal);
    
#if !LOD
    output.outPosition = mul(pos, wvpMatrix);
#endif
    
    output.outUV = input.UV;
	return output;
}