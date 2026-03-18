#include "head.hlsli"

[numthreads(NUMTHREADS_X_BODY, 1, 1)]
void main(uint3 dtID : SV_DispatchThreadID)
{
    uint idx = dtID.x;
    if (idx < body_count)
    {
        Body body = bodyList[idx];
        bodyList[idx].acceleration[0U] = (((body.acceleration[2U] + body.acceleration[3U]) * 2.F + body.acceleration[1U] + body.acceleration[4U]) / 6.F);
    }
}