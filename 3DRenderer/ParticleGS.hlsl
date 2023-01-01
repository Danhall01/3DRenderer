cbuffer Camera : register(b0)
{
    float4 camPosition;
    matrix viewMatrix;
    matrix projectionMatrix;
}

struct VSOutput
{
    float4 position : SV_POSITION;
};

[maxvertexcount(6)]
void main(
	point float3 input[1] : POSITION,
	inout TriangleStream<VSOutput> output
)
{
    matrix vpMatrix = mul(viewMatrix, projectionMatrix);
    
    
    float3 upVec = float3(0.0f, 1.0f, 0.0f);
    float3 frontVec = normalize((camPosition.xyz - input[0])); // a - b = ba
    float3 rightVec = cross(frontVec, upVec); // Right hand rule: pekfinger(a), mittfinger(b), tumme(a X b) 
    //upVec = normalize(cross(rightVec, frontVec));
    

    
    //Scale based on preference
    upVec *= 0.15f;
    rightVec *= 0.04f;
    
    
    VSOutput toAppend;
    // top left
    toAppend.position = mul(float4(input[0] - rightVec + upVec, 1.0f), vpMatrix);
    output.Append(toAppend);
        
    // bottom right
    toAppend.position = mul(float4(input[0] + rightVec - upVec, 1.0f), vpMatrix);
    output.Append(toAppend);
    
    // bottom left
    toAppend.position = mul(float4(input[0] - rightVec - upVec, 1.0f), vpMatrix);
    output.Append(toAppend);
  
    output.RestartStrip(); // Done with first triangle
    
    // top left
    toAppend.position = mul(float4(input[0] - rightVec + upVec, 1.0f), vpMatrix);
    output.Append(toAppend);
    
    // top right
    toAppend.position = mul(float4(input[0] + rightVec + upVec, 1.0f), vpMatrix);
    output.Append(toAppend);
    
    // bottom right
    toAppend.position = mul(float4(input[0] + rightVec - upVec, 1.0f), vpMatrix);
    output.Append(toAppend);
}