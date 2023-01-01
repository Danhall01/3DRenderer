struct Particle
{
    float3 pos;
};
RWStructuredBuffer<Particle> Particles : register(u0);

// If 32 changes, it must change on the CPU as well
[numthreads(32, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    Particle particle = Particles[DTid.x];
    
    if (particle.pos.y < -5)
    {
        particle.pos.y = 5;
    }
    
    particle.pos.y -= 0.4f;
    
    
    
    Particles[DTid.x] = particle;
}