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
TextureCube cubeMap : register(t3);

SamplerState sState : register(s0);

cbuffer CubeMap
{
    float4 enabled_camPos;
};

// To decide if the texture uses the texture cube or not
//cbuffer ...

PSOutput main(PSInput input)
{
    PSOutput output;
    
    output.wpos = input.wsPosition;
    output.normal = float4(input.normal.xyz, Ns);
    if (enabled_camPos.x == 1) // Cube mapping texture
    {
        float3 camToFragment = enabled_camPos.yzw - input.wsPosition.xyz;
        float3 reflectedVec = reflect(camToFragment, input.normal.xyz);
        float4 sampledView = cubeMap.Sample(sState, reflectedVec);
        
        output.color = sampledView;
        output.diffuseClr = sampledView;
        output.specularClr = sampledView;
    }
    else // Normal texturing
    {
        output.color = float4(mapKa.Sample(sState, input.UV).xyz, Ka.x);
        output.diffuseClr = float4(mapKd.Sample(sState, input.UV).xyz, Kd.x);
        output.specularClr = float4(mapKs.Sample(sState, input.UV).xyz, Ks.x);
    }
    return output;
}