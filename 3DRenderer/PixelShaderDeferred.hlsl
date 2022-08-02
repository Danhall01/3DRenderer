struct PSInput
{
    float4 position    : SV_POSITION;
    float4 normal      : NORMAL0;
    float4 wsPosition  : WSPOS0;
    float2 UV          : UV0;
};
struct PSOutput
{
    float4 color       : SV_TARGET0; // R G B Ka
    float4 wpos        : SV_TARGET1; // X Y Z W
    float4 normal      : SV_TARGET2; // X Y Z Ns
    float4 diffuseClr  : SV_TARGET3; // R G B Kd
    float4 specularClr : SV_TARGET4; // R G B Ks
};

cbuffer Material : register(b0)
{
    float  Ns; // Specular highlight
    float3 Ka; // Ambient constant
    float3 Kd; // Diffuse constant
    float3 Ks; // Specular constant
    float2 Padding;
};
Texture2D mapKa : register(t0);
Texture2D mapKd : register(t1);
Texture2D mapKs : register(t2);
SamplerState sState;


PSOutput main(PSInput input)
{
    PSOutput output;
    
    output.wpos = float4(input.wsPosition);
    output.normal = float4(input.normal.xyz, Ns);
    
    output.color = float4(mapKa.Sample(sState, input.UV).xyz, Ka.x);
    output.diffuseClr = float4(mapKd.Sample(sState, input.UV).xyz, Kd.x);
    output.specularClr = float4(mapKs.Sample(sState, input.UV).xyz, Ks.x);
	return output;
}