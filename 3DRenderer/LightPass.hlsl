//
// Small read about compute shader threading optimization
// Source: https://developer.nvidia.com/blog/optimizing-compute-shaders-for-l2-locality-using-thread-group-id-swizzling/
//

#define WINDOW_WIDTH 1024.0f
#define WINDOW_HEIGHT 576.0f
sampler sState;

// Output
RWTexture2D<float4> UAC : register(u0);

// Input
Texture2D<float4> in_clr         : register(t0); // R G B Ns
Texture2D<float4> in_wspos       : register(t1); // X Y Z
Texture2D<float4> in_normal      : register(t2); // X Y Z
Texture2D<float4> in_diffuseClr  : register(t3); // R G B
Texture2D<float4> in_specularClr : register(t4); // R G B

//Lights
struct light
{
    float4 lightPosType;
    float4 lightClrIntensity;
    float4 lightDirRange;
};
StructuredBuffer<light> lightBuffer : register(t5);
cbuffer lightingData : register(b0)
{
    uint4 count;
};


float2 PhongLightingSpot(uint dataIndex, float2 coord)
{
    // Light pass
    float ambient;
    float specular;
    float2 finalColor = float2(0.0f, 0.0f);
    
    return finalColor;
    
    //float4 wspos          = in_wspos.SampleLevel(sState, coord, 0);
    //float3 normal         = in_normal.SampleLevel(sState, coord, 0);
    //float  Shinyness      = in_clr.SampleLevel(sState, coord, 0).w;
    //float3 lightpos       = lightBuffer[dataIndex].lightPosType.xyz;
    //float3 lightclr       = lightBuffer[dataIndex].lightClrIntensity.xyz;
    //float  lightIntensity = lightBuffer[dataIndex].lightClrIntensity.w;
    //float3 lightDir       = lightBuffer[dataIndex].lightDirRange.xyz;
    //float  lightRange     = lightBuffer[dataIndex].lightDirRange.w;
    
    // ============================= OLD PS SHADER ===================================
    //float4 finalColor;
    //float3 L = normalize((lightPos - input.wsPos).xyz);
    //float3 N = normalize(input.normal);
    //float3 V = normalize(input.vecToCam.xyz);
    //float3 R = normalize(2 * dot(N, L) * N - L);
    //float diffuseI = dot(L, N);


    //float3 ambient = ka.x * lightColor.xyz; // I = K * L
    //if (diffuseI < 0.0f)
    //{
    //    return tex.Sample(sState, input.textureCord) * float4(ambient, 1.0f);
    //}

    //float3 diffuse = kd.x * lightColor.xyz * max(diffuseI, 0.0f); // I = K * L * cos(A)

    //float3 specular = max(ks.x * lightColor.xyz * pow(dot(V, R), shininess.x), float3(0, 0, 0)); // I = K * L * dot(R,V)^s

    
    //finalColor = tex.Sample(sState, input.textureCord) * float4((ambient + diffuse + specular), 1.0f);
    //return saturate(finalColor);
    // ===============================================================================
}
float2 PhongLightingDirectional(uint dataIndex, float2 coord)
{
    // Light pass
    float ambient;
    float specular;
    float2 finalColor = float2(0.0f, 0.0f);
    
    return finalColor;
    
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
    if ((ambientClr.r + ambientClr.g + ambientClr.b) == 0)
    {
        UAC[threadID.xy] = float4(0.0f, 0.0f, 0.0f, 1.0f);
        return;
    }
    
    float3 diffuseClr = in_diffuseClr.SampleLevel(sState, texcoord, 0).xyz;
    float3 specularClr = in_specularClr.SampleLevel(sState, texcoord, 0).xyz;
       
    float4 lighting = float4(0.0f, 0.0f, 0.0f, 1.0f);
    for (uint i = 0; i < count.x; i++)
    {
        // Sample light
        float type = lightBuffer[i].lightPosType.w;
        
        
        if ( type == 0 )
        {
            lighting.yz += PhongLightingSpot(i, texcoord);
        }
        else if (type == 1)
        {
            lighting.yz += PhongLightingDirectional(i, texcoord);
        }
    }
    UAC[threadID.xy] = float4(
        ambientClr.xyz +
        diffuseClr * lighting.r +
        specularClr * lighting.g,
    1.0f
    );;
}