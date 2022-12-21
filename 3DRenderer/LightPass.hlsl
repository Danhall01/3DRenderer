//
// Small read about compute shader threading optimization
// Source: https://developer.nvidia.com/blog/optimizing-compute-shaders-for-l2-locality-using-thread-group-id-swizzling/
//

#define WINDOW_WIDTH 1024.0f
#define WINDOW_HEIGHT 576.0f
sampler sState : register(s0);

// Output
RWTexture2D<float4> UAC : register(u0);

// Input
Texture2D<float4> in_clr         : register(t0); // R G B Ka
Texture2D<float4> in_wspos       : register(t1); // X Y Z W
Texture2D<float4> in_normal      : register(t2); // X Y Z Ns
Texture2D<float4> in_diffuseClr  : register(t3); // R G B Kd
Texture2D<float4> in_specularClr : register(t4); // R G B Ks

// Shadow
Texture2DArray<float> in_shadowMap : register(t6);
sampler shadowSampler : register(s1);

//Lights
struct light
{
    float4 lightPosType;
    float4 lightClrIntensity;
    float4 lightDirRange;
    float4 lightCosOuterInnerSMapCount;
};
StructuredBuffer<light> lightBuffer : register(t5);
cbuffer lightingData : register(b0)
{
    uint4 CamPosLightCount; // X Y Z Count
};


// Spot light
float3 SpotLight(float3 lightPos, float3 lightDir, float3 wsPos, float3 camPos, 
                float3 normal, float3 diffuseClr, float3 specularClr,
                float shininess, float intensity, float cosConeOuter, 
                float cosConeInner, float rangeMax, float3 lightClr)
{
    // Similar to a desk lamp, a point light restricted by a cone.
    // When computing light, need to check if we are inside the cone.
  
    float3 toLightVec = lightPos - wsPos;
    float3 toCamVec = normalize(camPos - wsPos);
    float distToLight = length(toLightVec);

    // Phong diffuse
    toLightVec /= distToLight;
    float NDotL = saturate(dot(toLightVec, normal));
    float3 finalColor = diffuseClr * NDotL;
    
    
   // Blinn specular
    float3 halfWay = normalize(toCamVec + toLightVec);
    float NDotH = saturate(dot(halfWay, normal));
    finalColor += specularClr * pow(NDotH, shininess) * intensity;
    

	// Cone attenuation
    float cosAng = dot(lightDir, toLightVec);
    float conAttn = saturate((cosAng - cosConeOuter) * (1 / cosConeInner));
    conAttn *= conAttn;
    
    // Attenuation
    float distToLightNorm = 1.0f - saturate(distToLight * (1 / rangeMax));
    float attn = distToLightNorm * distToLightNorm;
    finalColor *= lightClr * attn * conAttn;
    

    return finalColor;
}
// Directional light
float3 DirLight(float3 lightDir, float3 wsPos, float3 camPos, float3 normal, float3 diffuseClr,
                float shininess, float intensity, float3 lightClr)
{
    float3 toLightVec = normalize(lightDir);
    float3 toCamVec = normalize(camPos - wsPos);
    // Phong diffuse
    float NDotL = dot(toLightVec, normal);
    float3 finalColor = diffuseClr * saturate(NDotL);
   
	// Blinn specular
    float3 halfWay = normalize(toCamVec + toLightVec);
    float NDotH = saturate(dot(halfWay, normal));
    finalColor += lightClr * pow(NDotH, shininess) * intensity;

    return finalColor;
}
// Point light
float3 PhongLighting(float3 lightClr, float3 L, float3 N, float3 V, 
                    float intensity, float shininess)
{
    float3 R = normalize(2 * dot(N, L) * N - L);
    float diffuseI = dot(L, N);
    
    if (diffuseI < 0.0f)
    {
        return float3(0.0f, 0.0f, 0.0f);
    }
    float3 diffuse = intensity * max(diffuseI, 0.0f);
    float3 specular = max(intensity * pow(dot(V, R), shininess), float3(0.0f, 0.0f, 0.0f));
    
    
    return lightClr * (diffuse + specular);
}



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
        return;
    }
    ambientClr.xyz *= in_clr.SampleLevel(sState, texcoord, 0).w;
    
    // Sampling
    float4 wspos     = in_wspos.SampleLevel(sState, texcoord, 0);
    float3 normal    = in_normal.SampleLevel(sState, texcoord, 0).xyz;
    float  Shininess = in_normal.SampleLevel(sState, texcoord, 0).w;
    float  Kd        = in_diffuseClr.SampleLevel(sState, texcoord, 0).w;
    float  Ks        = in_specularClr.SampleLevel(sState, texcoord, 0).w;
    
    float3 diffuseClr  = Kd * in_diffuseClr.SampleLevel(sState, texcoord, 0).xyz;
    float3 specularClr = Ks * in_specularClr.SampleLevel(sState, texcoord, 0).xyz;
    
    // Gather lighting
    float3 lighting = float3(0.0f, 0.0f, 0.0f);
    for (uint i = 0; i < CamPosLightCount.w; i++)
    {
        // Sample light
        float3 lightpos       = lightBuffer[i].lightPosType.xyz;
        float3 lightclr       = lightBuffer[i].lightClrIntensity.xyz;
        float  lightIntensity = lightBuffer[i].lightClrIntensity.w;
        float  lightRange     = lightBuffer[i].lightDirRange.w;
        
        float  type = lightBuffer[i].lightPosType.w;
        if ( type == 0 ) // Spot Light
        {
            float3 lightDir = (-1) * normalize(lightBuffer[i].lightDirRange.xyz);
            float cosOuter = lightBuffer[i].lightCosOuterInnerSMapCount.x;
            float cosInner = lightBuffer[i].lightCosOuterInnerSMapCount.y;
            
            lighting += SpotLight(lightpos, lightDir, wspos.xyz, CamPosLightCount.xyz, 
                                    normal, diffuseClr, specularClr,
                                    Shininess, lightIntensity, cosOuter, cosInner,
                                    lightRange, lightclr);
        }
        else if (type == 1) // Directional Light
        {
            float3 lightDir = (-1) * normalize(lightBuffer[i].lightDirRange.xyz);
            lighting += DirLight(lightDir, wspos.xyz, CamPosLightCount.xyz, normal,
                                diffuseClr, Shininess, lightIntensity, lightclr);
        }
        else if (type == 2) // Point Light
        {
            float3 L = normalize(lightpos - wspos.xyz);
            float3 N = normalize(normal);
            float3 V = normalize(CamPosLightCount.xyz - wspos.xyz);
            lighting += PhongLighting(lightclr, L, N, V, lightIntensity, Shininess);
        }
    }
    float3 baseClr = ambientClr.xyz * diffuseClr * specularClr;
    UAC[threadID.xy] = saturate(float4(
        baseClr + lighting,
        1.0f
    ));
    return;
}