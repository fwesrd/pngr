#include "head.hlsli"

[numthreads(NUMTHREADS_X_PARTICLE, NUMTHREADS_Y_PARTICLE, 1)]
void main(uint3 dtID : SV_DispatchThreadID)
{
    uint idx = dtID.x;
    uint idy = dtID.y;
    if ((idx < particle_count) && (idy < body_count))
    {
        Particle particle = particleList[idx];
        Body body = bodyList[idy];
        float3 r1 = (particle.position - particle.orginalPosition);
        float3 r2 = (particle.position - body.position[0U]);
        if ((dot(r1, r1) > particle_space_2) || (dot(r2, r2) < (body.radius * body.radius)))
        {
            particleList[idx].position = particle.orginalPosition;
            particleList[idx].velocity = (float3) 0.F;
        }
    }
}