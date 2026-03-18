#include "head.hlsli"

float3 NewtonianGravitation(in float3 particlePosition, in Body body)
{
    float3 r = (body.position[0] - particlePosition);
    float r_r = dot(r, r);
    return (float3) ((r_r < EPS) ? 0.F : (G * body.mass / scale_distance_3 / sqrt(r_r) / r_r * r));
}

[numthreads(NUMTHREADS_X_PARTICLE, 1, 1)]
void main(uint3 dtID : SV_DispatchThreadID)
{
    uint idx = dtID.x;
    if (idx < particle_count)
    {
        Particle particle = particleList[idx];
        float3 acceleration = (float3) 0.F;
        for (uint trav = 0U; trav < body_count; ++trav)
        {
            acceleration += NewtonianGravitation(particle.position, bodyList[trav]);
        }
        particleList[idx].position = ((0.5F * step * acceleration + particle.velocity) * step + particle.position);
        particleList[idx].velocity = (step * acceleration + particle.velocity);
    }
}