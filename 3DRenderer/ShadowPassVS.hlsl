struct VSInput
{
    float3 position : POSITION0;
    float3 normal : NORMAL0;
    float2 UV : UV0;
};
cbuffer wvpMatrix : register(b0)
{
    matrix worldMatrix;
    matrix normalWMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

float4 main(VSInput input) : SV_POSITION
{
    matrix wvMatrix = mul(worldMatrix, viewMatrix);
    matrix wvpMatrix = mul(wvMatrix, projectionMatrix);
    
    float4 pos = float4(input.position, 1.0f);
    return mul(pos, wvpMatrix);
}