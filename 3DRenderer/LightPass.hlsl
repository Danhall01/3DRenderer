//
// Small read about compute shader threading optimization
// Source: https://developer.nvidia.com/blog/optimizing-compute-shaders-for-l2-locality-using-thread-group-id-swizzling/
//

#define WINDOW_WIDTH 1024.0f
#define WINDOW_HEIGHT 576.0f
sampler sState : register(s0);

// Output
RWTexture2D<float4> UAC : register(u0);
RWTexture2DArray<float4> UAV : register(u1);

// Input
Texture2D<float4> in_clr         : register(t0); // mapKa Ka
Texture2D<float4> in_wspos       : register(t1); // X Y Z W
Texture2D<float4> in_normal      : register(t2); // X Y Z Ns

Texture2D<float4> in_diffuseClr  : register(t3); // mapKd Kd
Texture2D<float4> in_specularClr : register(t4); // mapKs Ks

//Lights
struct light
{
    float4 lightPosType;
    float4 lightClrIntensity;
    float4 lightDirRange;
    float4 lightCosOuterInnerSMapCount;
    matrix VPMatrix;
};
StructuredBuffer<light> lightBuffer : register(t5);
cbuffer lightingData : register(b0)
{
    uint4 CamPosLightCount; // X Y Z Amount of lights
};


// Shadow
Texture2DArray<float> in_shadowMap : register(t6);
sampler shadowSampler : register(s1);


float3 lightCalc(uint index, float3 wsPos, float3 normal, float3 diffuseCLR, float3 specularCLR);
bool HasShadow(uint index, float4 wsPos);

[numthreads(16, 16, 1)]
void main( uint3 threadID : SV_DispatchThreadID )
{
    // Transform coordinates into normalised coords
    float2 texcoord = float2(
        threadID.x / (WINDOW_WIDTH - 1.0f),
        threadID.y / (WINDOW_HEIGHT - 1.0f)
    );
    // Early return
    float4 ambientClr = in_clr.SampleLevel(sState, texcoord, 0);
    if ( (ambientClr.r + ambientClr.g + ambientClr.b) == 0)
    {
        UAC[threadID.xy] = float4(0.0f, 0.0f, 0.0f, 1.0f);
        UAV[uint3(threadID.xy, 0)] = float4(0.0f, 0.0f, 0.0f, 1.0f);
        return;
    }
    ambientClr.xyz *= in_clr.SampleLevel(sState, texcoord, 0).w;
    
    // Sampling
    float4 wspos     = in_wspos.SampleLevel(sState, texcoord, 0);
    float3 normal    = in_normal.SampleLevel(sState, texcoord, 0).xyz;
    
    float  Kd        = in_diffuseClr.SampleLevel(sState, texcoord, 0).w;
    float  Ks        = in_specularClr.SampleLevel(sState, texcoord, 0).w;
    
    float3 diffuseClr  = Kd * in_diffuseClr.SampleLevel(sState, texcoord, 0).xyz;
    float3 specularClr = Ks * in_specularClr.SampleLevel(sState, texcoord, 0).xyz;
    
    // Gather lighting
    float3 lighting = float3(0.0f, 0.0f, 0.0f);
    for (uint i = 0; i < CamPosLightCount.w; i++)
    {
        bool enabledShadow = lightBuffer[i].lightCosOuterInnerSMapCount.z == 1.0f;
        if (enabledShadow && HasShadow(i, wspos))
        {
            continue;
        }
        lighting += lightCalc(i, wspos.xyz, normal, diffuseClr, specularClr);
    }
    
    float3 baseClr = ambientClr.xyz;
    float ambientModifier = 0.5f;
    UAC[threadID.xy] = saturate(float4(
        baseClr * ambientModifier + lighting,
        1.0f
    ));
    UAV[uint3(threadID.xy, 0)] = saturate(float4(
        baseClr + lighting,
        1.0f
    ));
    return;
}



bool HasShadow(uint index, float4 wsPos)
{
    float4 pos = wsPos;
    float shadowLightIndex = lightBuffer[index].lightCosOuterInnerSMapCount.w;
    matrix vpMatrix = lightBuffer[index].VPMatrix;
    
    pos = mul(pos, vpMatrix);
    
    // Normaliced Device Coordinate
    float4 NDCPos = pos / pos.w;
    
    // Early return
    if (abs(NDCPos.x) + abs(NDCPos.y) + abs(NDCPos.z) > 3.0f)
    {
        return false;
    }
    
    float depthCalc = NDCPos.z;
    
    
    
    
    float3 UV = float3(NDCPos.x * 0.5f + 0.5f, NDCPos.y * -0.5f + 0.5f, shadowLightIndex);
    
    float3 depthSample = in_shadowMap.SampleLevel(shadowSampler, UV, 0);
    
    if (depthCalc >= (depthSample.x + 0.001f))
    {
        return true;
    }
    return false;
}

float3 lightCalc(uint index, float3 wsPos, float3 normal, float3 diffuseCLR, float3 specularCLR)
{
    float3 finalColor = float3(0.0f, 0.0f, 0.0f);
    float3 L = float3(0.0f, 0.0f, 0.0f);
    float attenuation = 1.0f;
    
    
    float3 lightPos       = lightBuffer[index].lightPosType.xyz;
    float3 lightDirection = lightBuffer[index].lightDirRange.xyz;
    float3 cameraPos      = CamPosLightCount.xyz;
    
    float lightRange      = lightBuffer[index].lightDirRange.w;
    float specularPower   = lightBuffer[index].lightClrIntensity.w;
    
    float3 lightColor     = lightBuffer[index].lightClrIntensity.xyz;
    
    
    
    if (lightBuffer[index].lightPosType.w == 0) // Spotlight
    {
        float2 spotlightAngels = lightBuffer[index].lightCosOuterInnerSMapCount.xy;
        
        L = lightPos - wsPos;
        
        float dist = length(L);
        attenuation = max(0, 1.0f - (dist / lightRange));
        
        L /= dist;
        
        float3 L2 = lightDirection;
        float rho = dot(-L, L2);
        attenuation *= saturate(
                    (rho - spotlightAngels.x) / (spotlightAngels.y - spotlightAngels.x)
            );

    }
    else if (lightBuffer[index].lightPosType.w == 1) // Directional Light
    {
        L = -lightDirection;
    }
    else // Pointlight
    {
        L = lightPos - wsPos;
        
        float dist = length(L);
        attenuation = max(0, 1.0f - (dist / lightRange));
        
        L /= dist;
    }
    
    float nDotL = saturate( dot(normal, L) );
    float3 diffuse = nDotL * lightColor * attenuation;
    finalColor += diffuse * diffuseCLR;
    
    //Calculate specular
    float3 V = cameraPos - wsPos;
    float3 H = normalize(L + V);
    
    float specular = pow( saturate( dot(normal, H) ), specularPower ) * attenuation * nDotL;
    finalColor += specular * specularCLR;
    
        
    return finalColor;
}