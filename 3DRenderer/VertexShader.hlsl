struct VSInput
{
    float3 position : POSITION0;
    float3 normal   : NORMAL0;
    float2 UV       : UV0;
};
struct VSOutput
{
    float4 outPosition : SV_POSITION;
    float4 outNormal   : NORMAL0;
    float2 outUV       : UV0;
};

cbuffer wvpMatrix : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

VSOutput main(VSInput input)
{
    VSOutput output;
    
    float4 pos = float4(input.position, 1.0f);
    float4 nor = float4(input.normal, 0.0f);
    
    matrix wvMatrix = mul(worldMatrix, viewMatrix);
    matrix wvpMatrix = mul(wvMatrix, projectionMatrix);
    
    pos = mul(pos, wvpMatrix);
    nor = mul(nor, worldMatrix);
    
    
    output.outPosition = pos;
    output.outNormal = nor;
    output.outUV = input.UV;
	return output;
}