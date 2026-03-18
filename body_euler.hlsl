#include "head.hlsli"

[numthreads(NUMTHREADS_X_BODY, 1, 1)]
void main(uint3 dtID : SV_DispatchThreadID)
{
    uint idx = dtID.x;
    if (idx < body_count)
    {
        Body body = bodyList[idx];
        bodyList[idx].position[n] = ((0.5F * step * body.acceleration[k] + body.velocity[0U]) * step + body.position[0U]);
        bodyList[idx].velocity[n] = (step * body.acceleration[k] + body.velocity[0U]);
    }
}