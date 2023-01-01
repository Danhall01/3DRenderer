struct Particle
{
    float3 pos; // Stored in World Space to begin with
};
StructuredBuffer<Particle> Particles : register(t0);

float3 main(uint vertexID : SV_VertexID) : POSITION
{
    return Particles[vertexID].pos;
}