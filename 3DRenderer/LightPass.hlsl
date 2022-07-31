//
// Small read about compute shader threading optimization
// Source: https://developer.nvidia.com/blog/optimizing-compute-shaders-for-l2-locality-using-thread-group-id-swizzling/
//

#define WINDOW_WIDTH 1024.0f
#define WINDOW_HEIGHT 576.0f

// Output
RWTexture2D<float4> UAC : register(u0);

// Input
Texture2D<float4> in_clr         : register(t0); // R G B Ns
Texture2D<float4> in_wspos       : register(t1); // X Y Z
Texture2D<float4> in_normal      : register(t2); // X Y Z
Texture2D<float4> in_diffuseClr  : register(t3); // R G B
Texture2D<float4> in_specularClr : register(t4); // R G B

sampler sState;
//Lights
//struct light
//{
    
//};
//StructuredBuffer<light> lightBuffer;


// Bind all the lights as buffers (StructuredBuffer) using an SRV to light array

[numthreads(16, 16, 1)]
void main( uint3 threadID : SV_DispatchThreadID )
{
    // Transform coordinates into UNORM coords
    float2 texcoord = float2(
        threadID.x / (WINDOW_WIDTH - 1.0f),
        threadID.y / (WINDOW_HEIGHT - 1.0f)
    );
    // Load all the data from samplers
    float4 finalColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float4 ambientClr = in_clr.SampleLevel(sState, texcoord, 0);
    if ((ambientClr.r + ambientClr.g + ambientClr.b) == 0)
    {
        UAC[threadID.xy] = finalColor;
        return;
    }
    
    float4 wspos = in_wspos.SampleLevel(sState, texcoord, 0);
    float3 normal = in_normal.SampleLevel(sState, texcoord, 0).xyz;
    float3 diffuseClr = in_diffuseClr.SampleLevel(sState, texcoord, 0).xyz;
    float3 specularClr = in_specularClr.SampleLevel(sState, texcoord, 0).xyz;
    float Shinyness = ambientClr.w;
    ambientClr.w = 1.0f;
    
    // Light pass
    finalColor += float4(ambientClr.xyz, 0.0f);
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
    
    
    UAC[threadID.xy] = finalColor;
}