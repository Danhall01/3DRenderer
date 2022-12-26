struct VSOutput
{
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


cbuffer Data : register(b0)
{
    float4 camPos;    // X Y Z 0
    float4 meshPos;   // X Y Z 0
    float4 tesBaseFactor; // F F F F
};


#define NUM_CONTROL_POINTS 3
HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(
	InputPatch<VSOutput, NUM_CONTROL_POINTS> ip,
	uint PatchID : SV_PrimitiveID)
{
	HS_CONSTANT_DATA_OUTPUT Output;

    float tesFactor = 0;
	// Set tesFactor based on length between cam and mesh center
    float3 projectedVec = camPos.xyz - meshPos.xyz;
    float distance = sqrt(pow(projectedVec.x, 2) + pow(projectedVec.y, 2) + pow(projectedVec.z, 2));
	
    //float k = 0.058; // 71m
    float k = 0.017; // 242m
    tesFactor = exp( -k * distance ) * tesBaseFactor.x;
    tesFactor = clamp(tesFactor, 1.0f, tesBaseFactor.x);
	
    Output.EdgeTessFactor[0] =
		Output.EdgeTessFactor[1] =
		Output.EdgeTessFactor[2] =
		Output.InsideTessFactor = tesFactor;

	return Output;
}

[domain("tri")]
[partitioning("fractional_odd")] //Odd from 1 - 63
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("CalcHSPatchConstants")]
HS_CONTROL_POINT_OUTPUT main( 
	InputPatch<VSOutput, NUM_CONTROL_POINTS> ip,
	uint i : SV_OutputControlPointID,
	uint PatchID : SV_PrimitiveID )
{
	HS_CONTROL_POINT_OUTPUT Output;

	// Pass the data along
    Output.Normal = ip[i].Normal;
	Output.vPosition = ip[i].vPosition;
    Output.UV = ip[i].UV;
	
	return Output;
}
