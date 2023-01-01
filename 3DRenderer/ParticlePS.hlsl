struct PSInput
{
    float4 position : SV_POSITION;
};


float4 main(PSInput input) : SV_TARGET0
{
    return float4(0.255f, 0.412f, 0.882f, 1.0f);
}