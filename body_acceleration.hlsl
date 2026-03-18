#include "head.hlsli"

float3 PN2_5(in Body body1, in Body body2)
{
    float3 r12 = (body1.position[n] - body2.position[n]);
    if (dot(r12, r12) < EPS)
    {
        return (float3) 0.F;
    }
    
    float m1 = (float) (body1.mass * MO);
    float m2 = (float) (body2.mass * MO);
    r12 *= scale_distance;
    float3 v1 = (body1.velocity[n] * (scale_distance * VO));
    float3 v2 = (body2.velocity[n] * (scale_distance * VO));
    
    //temp
    float m1_m2 = (m1 * m2);
    float m2_m2 = (m2 * m2);
    float m1_m1_m2 = (m1 * m1_m2);
    float m1_m2_m2 = (m1 * m2_m2);
    float m2_m2_m2 = (m2 * m2_m2);

    float r12_r12 = dot(r12, r12);
    float r12_r12_r12 = (sqrt(r12_r12) * r12_r12);
    float r12_r12_r12_r12 = (r12_r12 * r12_r12);
    
    float v1_v1 = dot(v1, v1);
    float v2_v2 = dot(v2, v2);
    float v2_v2_v2_v2 = (v2_v2 * v2_v2);
    float v1_v2 = dot(v1, v2);
    float v1_v2_v1_v2 = (v1_v2 * v1_v2);
    float3 v12 = (v1 - v2);
    float v12_v12 = dot(v12, v12);
    
    float3 n12 = normalize(r12);
    float n12_v1 = dot(n12, v1);
    float n12_v1_n12_v1 = (n12_v1 * n12_v1);
    float n12_v2 = dot(n12, v2);
    float n12_v2_n12_v2 = (n12_v2 * n12_v2);
    float n12_v2_n12_v2_n12_v2 = (n12_v2 * n12_v2_n12_v2);
    float n12_v2_n12_v2_n12_v2_n12_v2 = (n12_v2_n12_v2 * n12_v2_n12_v2);
    float n12_v12 = dot(n12, v12);

    //0PN
    float3 a_0PN = (-m2 / r12_r12 * n12);
    //1PN
    float coeff_n12_c2 = ((5.F * m1_m2 + 4.F * m2_m2) / r12_r12_r12 + m2 / r12_r12 * (1.5F * n12_v2_n12_v2 - v1_v1 + 4.F * v1_v2 - 2.F * v2_v2));
    float coeff_v12_c2 = (m2 / r12_r12 * (4.F * n12_v1 - 3.F * n12_v2));
    float3 a_1PN = (coeff_n12_c2 * n12 + coeff_v12_c2 * v12);
    //2PN
    float coeff_n12_c4 =
        ((-14.25F * m1_m1_m2 - 34.5F * m1_m2_m2 - 9.F * m2_m2_m2) / r12_r12_r12_r12 +
        m2 / r12_r12 * (-1.875F * n12_v2_n12_v2_n12_v2_n12_v2 + 1.5F * n12_v2_n12_v2 * v1_v1 - 6.F * n12_v2_n12_v2 * v1_v2 - 2.F * v1_v2_v1_v2 + 4.5F * n12_v2_n12_v2 * v2_v2 + 4.F * v1_v2 * v2_v2 - 2.F * v2_v2_v2_v2) +
        m1_m2 / r12_r12_r12 * (19.5F * n12_v1_n12_v1 - 39.F * n12_v1 * n12_v2 + 8.5F * n12_v2_n12_v2 - 3.75F * v1_v1 - 2.5F * v1_v2 + 1.25F * v2_v2) +
        m2_m2 / r12_r12_r12 * (2.F * n12_v1_n12_v1 - 4.F * n12_v1 * n12_v2 - 6.F * n12_v2_n12_v2 - 8.F * v1_v2 + 4.F * v2_v2));
    float coeff_v12_c4 =
        (m2_m2 / r12_r12_r12 * (-2.F * n12_v1 - 2.F * n12_v2) +
        m1_m2 / r12_r12_r12 * (-15.75F * n12_v1 + 13.75F * n12_v2) +
        m2 / r12_r12 * (-6.F * n12_v1 * n12_v2_n12_v2 + 4.5F * n12_v2_n12_v2_n12_v2 + n12_v2 * v1_v1 - 4.F * n12_v1 * v1_v2 + 4.F * n12_v2 * v1_v2 + 4.F * n12_v1 * v2_v2 - 5.F * n12_v2 * v2_v2));
    float3 a_2PN = (coeff_n12_c4 * n12 + coeff_v12_c4 * v12);
    //2.5PN
    float coeff_n12_c5 = (((208.F / 15.F * m1_m2_m2 - 4.8F * m1_m1_m2) / r12_r12_r12_r12 + 2.4F * m1_m2 / r12_r12_r12 * v12_v12) * n12_v12);
    float coeff_v12_c5 = ((1.6F * m1_m1_m2 - 6.4F * m1_m2_m2) / r12_r12_r12_r12 - 0.8F * m1_m2 / r12_r12_r12 * v12_v12);
    float3 a_2_5PN = (coeff_n12_c5 * n12 + coeff_v12_c5 * v12);
    return ((a_0PN + a_1PN + a_2PN + a_2_5PN) / (scale_distance * AO));
}

float3 PN2_5_Kerr(in Body body)
{
    float3 r12 = (body.position[n] - kerr_position);
    if (dot(r12, r12) < EPS)
    {
        return (float3) 0.F;
    }
    
    float m1 = (float) (body.mass * MO);
    float m2 = (float) (kerr_mass * MO);
    r12 *= scale_distance;
    float3 v1 = (body.velocity[n] * (scale_distance * VO));
    
    //temp
    float m1_m2 = (m1 * m2);
    float m2_m2 = (m2 * m2);
    float m1_m1_m2 = (m1 * m1_m2);
    float m1_m2_m2 = (m1 * m2_m2);
    float m2_m2_m2 = (m2 * m2_m2);

    float r12_r12 = dot(r12, r12);
    float r12_r12_r12 = (sqrt(r12_r12) * r12_r12);
    float r12_r12_r12_r12 = (r12_r12 * r12_r12);
    
    float v1_v1 = dot(v1, v1);
    
    float3 n12 = normalize(r12);
    float n12_v1 = dot(n12, v1);
    float n12_v1_n12_v1 = (n12_v1 * n12_v1);

    //0PN
    float3 a_0PN = (-m2 / r12_r12 * n12);
    //1PN
    float coeff_n12_c2 = ((5.F * m1_m2 + 4.F * m2_m2) / r12_r12_r12 - m2 / r12_r12 * v1_v1);
    float coeff_v12_c2 = (m2 / r12_r12 * 4.F * n12_v1);
    float3 a_1PN = (coeff_n12_c2 * n12 + coeff_v12_c2 * v1);
    //2PN
    float coeff_n12_c4 =
        ((-14.25F * m1_m1_m2 - 34.5F * m1_m2_m2 - 9.F * m2_m2_m2) / r12_r12_r12_r12 +
        m1_m2 / r12_r12_r12 * (19.5F * n12_v1_n12_v1 - 3.75F * v1_v1) +
        m2_m2 / r12_r12_r12 * 2.F * n12_v1_n12_v1);
    float coeff_v12_c4 = (-m2_m2 / r12_r12_r12 * 2.F * n12_v1 - m1_m2 / r12_r12_r12 * 15.75F * n12_v1);
    float3 a_2PN = (coeff_n12_c4 * n12 + coeff_v12_c4 * v1);
    //2.5PN
    float coeff_n12_c5 = (((208.F / 15.F * m1_m2_m2 - 4.8F * m1_m1_m2) / r12_r12_r12_r12 + 2.4F * m1_m2 / r12_r12_r12 * v1_v1) * n12_v1);
    float coeff_v12_c5 = ((1.6F * m1_m1_m2 - 6.4F * m1_m2_m2) / r12_r12_r12_r12 - 0.8F * m1_m2 / r12_r12_r12 * v1_v1);
    float3 a_2_5PN = (coeff_n12_c5 * n12 + coeff_v12_c5 * v1);
    return ((a_0PN + a_1PN + a_2PN + a_2_5PN) / (scale_distance * AO));
}

[numthreads(NUMTHREADS_X_BODY, 1, 1)]
void main(uint3 dtID : SV_DispatchThreadID)
{
    uint idx = dtID.x;
    if (idx < body_count)
    {
        bodyList[idx].acceleration[k] = (float3) 0.F;
        for (uint trav = 0U; trav < body_count; ++trav)
        {
            if (trav != idx)
            {
                bodyList[idx].acceleration[k] += PN2_5(bodyList[idx], bodyList[trav]);
            }
        }
        if (kerr_has)
        {
            bodyList[idx].acceleration[k] += PN2_5_Kerr(bodyList[idx]);
        }
    }
}