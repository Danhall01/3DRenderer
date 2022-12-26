struct DS_OUTPUT
{
    float4 Position  : SV_POSITION;
    float4 Normal    : NORMAL0;
    float4 vPosition : WSPOS0;
    float2 UV        : UV0;
};
struct HS_CONTROL_POINT_OUTPUT
{
    float4 Normal    : NORMAL0;
    float4 vPosition : WSPOS0;
    float2 UV        : UV0;
};
struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[3]			: SV_TessFactor;
	float InsideTessFactor			: SV_InsideTessFactor;
};


cbuffer wvpMatrix : register(b0)
{
    matrix worldMatrix;
    matrix normalWMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};


#define NUM_CONTROL_POINTS 3
[domain("tri")]
DS_OUTPUT main(
	HS_CONSTANT_DATA_OUTPUT input,
	float3 domain : SV_DomainLocation,
	const OutputPatch<HS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> patch)
{
	DS_OUTPUT Output;    
    Output.Normal    = normalize(patch[0].Normal * domain.x + patch[1].Normal * domain.y + patch[2].Normal * domain.z);
    Output.UV        = patch[0].UV * domain.x + patch[1].UV * domain.y + patch[2].UV * domain.z;
    
    
    // Math from: http://www.klayge.org/material/4_0/PhongTess/PhongTessellation.pdf
    // Additional information: NVIDIA
    
    // LERP
    float3 posVertex = domain.x * patch[0].vPosition.xyz + 
                        domain.y * patch[1].vPosition.xyz + 
                         domain.z * patch[2].vPosition.xyz;
    
    // Projected vectors to the nodes (tangent planes)
    float3 projectedVec1 = dot(patch[0].vPosition.xyz - posVertex, patch[0].Normal.xyz) * patch[0].Normal.xyz;
    float3 projectedVec2 = dot(patch[1].vPosition.xyz - posVertex, patch[1].Normal.xyz) * patch[1].Normal.xyz;
    float3 projectedVec3 = dot(patch[2].vPosition.xyz - posVertex, patch[2].Normal.xyz) * patch[2].Normal.xyz;
    
    // LERP
    float3 offSet = domain.x * projectedVec1 + 
                     domain.y * projectedVec2 + 
                      domain.z * projectedVec3;
    
    float alpha = 1.0f;
    posVertex += alpha * offSet;
    
    
    matrix vpMatrix = mul(viewMatrix, projectionMatrix);
    Output.vPosition = float4(posVertex, 1.0f);
    Output.Position = mul(float4(posVertex, 1.0f), vpMatrix);
    
	return Output;
}
