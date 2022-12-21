struct VSInput
{
    float3 position : POSITION0;
    float3 normal : NORMAL0;
    float2 UV : UV0;
};
cbuffer wvpMatrix : register(b0)
{
    matrix WorldViewProjectionMatrix;
};

float4 main(VSInput input) : SV_POSITION
{
    return mul(float4(input.position, 1.0f), WorldViewProjectionMatrix);
}