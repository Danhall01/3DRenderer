struct PSInput
{
    float4 outPosition : SV_POSITION;
    float4 outNormal   : NORMAL0;
    float2 outUV       : UV0;
};


float4 main(PSInput input) : SV_TARGET
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}