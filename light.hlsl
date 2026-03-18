#include "head.hlsli"

//#define UNROLL

/////////////////////////////////////////////////////////////////////////////////////////

//struct
struct Disk
{
    float r_min;
    float r_max;
    float h_min;
    float h_max;
    float k;
    float center;
    float R;
};

//constant
cbuffer LightChoose : register(b2)
{
    float3 camera_position;
    uint screen_width;

    float3 camera_focus;
    uint screen_height;

    float3 camera_left;
    float fov_delta;

    float3 camera_head;
    unsigned int light_step_count;
    
    float light_step_min;
    float light_step_max;
    
    int UNUSE2[2];
}
cbuffer Sky : register(b3)
{
    uint sky_width;
    uint sky_height;
    int sky_has;
    int UNUSE3;
}

//unorder
RWTexture2D<float4> lightTexture : register(u2);

//read
Texture2D<float4> skyboxTexture : register(t0);

/////////////////////////////////////////////////////////////////////////////////////////

typedef int Target;
#define TARGET_NO 0U
#define TARGET_BODY 1U
#define TARGET_HORIZON 2U
#define TARGET_DISK 3U

#define COLOR_BACKGROUND float4(0.F, 0.F, 0.F, 1.F)
#define COLOR_KERR float4(0.F, 0.F, 0.F, 1.F)
#define COLOR_DISK float4(1.F, 0.8F, 0.6F, 1.F)

/////////////////////////////////////////////////////////////////////////////////////////

static uint idx;
static uint idy;
static float m;
static float a;
static float horizonBlR;
static float ballNearR;
static float3 cameraX;
static Disk disk;
static const float4x4 MinkowskiMetric =
{
    1.F, 0.F, 0.F, 0.F,
    0.F, 1.F, 0.F, 0.F,
    0.F, 0.F, 1.F, 0.F,
    0.F, 0.F, 0.F, -1.F
};

/////////////////////////////////////////////////////////////////////////////////////////

float3 InitLightVelocity(in uint idx, in uint idy)
{
    return (C / scale_distance * normalize(camera_focus + fov_delta * ((screen_width * 0.5F - idx - 0.5F) * camera_left + (screen_height * 0.5F - idy - 0.5F) * camera_head)));
}

/////////////////////////////////////////////////////////////////////////////////////////

//scale_distance, geo, position - kerr_position, zy
float Root(in float _A, in float _B, in float _C)
{
    float temp = sqrt(max(0.F, (_B * _B - 4.F * _A * _C)));
    float root1 = (0.5F * (temp - _B) / max(EPS, _A));
    float root2 = (0.5F * (-temp - _B) / max(EPS, _A));
    return max(0.F, max(root1, root2));
}
float GetRadius(in float3 X)
{
    return max(EPS, sqrt(Root(1.F, (a * a - dot(X, X)), (-a * a * X.z * X.z))));
}
float GetHorizon(in float3 X)
{
    return sqrt(max((horizonBlR * horizonBlR + a * a * (1.F - pow((X.z / GetRadius(X)), 2.F))), 0.F));
}
float4x4 KsKerrMetric(in float3 X)
{
    float r = GetRadius(X);
    float r_r = (r * r);
    float a_a = (a * a);
    float f = (2.F * m * r / (r_r + a_a * X.z * X.z / r_r));
    float r_r_A_a_a = (r_r + a_a);
    float4 l = { ((r * X.x + a * X.y) / r_r_A_a_a), ((r * X.y - a * X.x) / r_r_A_a_a), (X.z / r), 1.F };
    return (MinkowskiMetric + f * float4x4((l * l[0U]), (l * l[1U]), (l * l[2U]), (l * l[3U])));
}
float4x4 KsKerrMetricInv(in float3 X)
{
    float r = GetRadius(X);
    float r_r = (r * r);
    float a_a = (a * a);
    float f = (2.F * m * r / (r_r + a_a * X.z * X.z / r_r));
    float r_r_A_a_a = (r_r + a_a);
    float4 l = { ((r * X.x + a * X.y) / r_r_A_a_a), ((r * X.y - a * X.x) / r_r_A_a_a), (X.z / r), -1.F };
    return (MinkowskiMetric - f * float4x4((l * l[0U]), (l * l[1U]), (l * l[2U]), (l * l[3U])));
}
void Christoffel(in float3 X, out float4x4 chris[4], in float epsilon)
{
    float4x4 gInv = KsKerrMetricInv(X);
    float4x4 dg[4U];
#ifdef UNROLL
    [unroll]
#endif
    for (uint d = 0U; d < 3U; ++d)
    {
        float3 delta = (float3) 0.F;
        delta[d] = epsilon;
        dg[d] = ((KsKerrMetric(X + delta) - KsKerrMetric(X - delta)) / (2.F * epsilon));
    }
    dg[3U] = (float4x4) 0.F;
#ifdef UNROLL
    [unroll]
#endif
    for (uint lambda = 0U; lambda < 4U; ++lambda)
    {
#ifdef UNROLL
    [unroll]
#endif
        for (uint mu = 0U; mu < 4U; ++mu)
        {
#ifdef UNROLL
    [unroll]
#endif
            for (uint nu = 0U; nu < 4U; ++nu)
            {
                float sum = 0.F;
#ifdef UNROLL
    [unroll]
#endif
                for (uint sigma = 0U; sigma < 4U; ++sigma)
                {
                    sum += (gInv[lambda][sigma] * (dg[mu][sigma][nu] + dg[nu][sigma][mu] - dg[sigma][mu][nu]));
                }
                chris[lambda][mu][nu] = (0.5F * sum);
            }
        }
    }
}
float3 GeodesicLine(in float3 X, in float3 V, in float epsilon)
{
    float4 U = { V, 1.F };
    float4x4 chris[4U];
    Christoffel(X, chris, epsilon);
    float at = 0.F;
#ifdef UNROLL
    [unroll]
#endif
    for (uint mu = 0U; mu < 4U; ++mu)
    {
#ifdef UNROLL
    [unroll]
#endif
        for (uint nu = 0U; nu < 4U; ++nu)
        {
            at += (chris[3U][mu][nu] * U[mu] * U[nu]);
        }
    }
    float3 A = (float3) 0.F;
#ifdef UNROLL
    [unroll]
#endif
    for (uint i = 0U; i < 3U; ++i)
    {
#ifdef UNROLL
    [unroll]
#endif
        for (uint mu = 0U; mu < 4U; ++mu)
        {
#ifdef UNROLL
    [unroll]
#endif
            for (uint nu = 0U; nu < 4U; ++nu)
            {
                A[i] -= (chris[i][mu][nu] * U[mu] * U[nu]);
            }
        }
        A[i] += (U[i] * at);
    }
    return A;
}
void RK4(inout float3 X, inout float3 V, in float dist)
{
    float epsilon = (0.5F * dist);
    float3 A1 = GeodesicLine(X, V, epsilon);
    float step = (0.5F * dist);
    float3 A2 = GeodesicLine((X + step * normalize(V + 0.5F * step * A1)), normalize(V + step * A1), epsilon);
    float3 A3 = GeodesicLine((X + step * normalize(V + 0.5F * step * A2)), normalize(V + step * A2), epsilon);
    step = dist;
    float3 A4 = GeodesicLine((X + step * normalize(V + 0.5F * step * A3)), normalize(V + step * A3), epsilon);
    float3 A = (((A2 + A3) * 2.F + A1 + A4) / 6.F);
    float3 newV = normalize(V + dist * A);
    X += (dist * normalize(0.5F * (V + newV)));
    V = newV;
}
void Euler(inout float3 X, in float3 V, in float dist)
{
    X += (dist * V);
}

/////////////////////////////////////////////////////////////////////////////////////////

void Init(out float3 X, out float3 V)
{
    if (kerr_has)
    {
        m = (float) (kerr_mass * MO);
        a = (kerr_a_rate * m);
        horizonBlR = (m * (1.F + sqrt(max((1.F - pow(kerr_a_rate, 2.F)), 0.F))));
        ballNearR = sqrt(pow((3.F * m * (1.F + cos(2.F / 3.F * acos(abs(kerr_a_rate))))), 2.F) + a * a);
        if (kerr_has == 2)
        {
            disk.r_min = (disk_r_min * scale_distance);
            disk.r_max = (disk_r_max * scale_distance);
            disk.h_min = (disk_h_min * scale_distance);
            disk.h_max = (disk_h_max * scale_distance);
            float deltar = (disk.r_max - disk.r_min);
            disk.k = ((disk.h_max - disk.h_min) / max(EPS, deltar));
            disk.center = (0.5F * (deltar + (disk.h_max * disk.h_max - disk.h_min * disk.h_min) / max(EPS, deltar)));
            disk.R = sqrt(disk.center * disk.center + disk.h_min * disk.h_min);
            disk.center += disk.r_min;
        }
    }
    cameraX = X = (scale_distance * (camera_position - kerr_position).xzy);
    V = normalize(scale_distance * VO * InitLightVelocity(idx, idy).xzy);
}
Target FindDist(in float3 X, out float dist, out uint bodyId)
{
    dist = light_step_max;
    if (kerr_has)
    {
        float lenX = length(X);
        if (lenX <= GetHorizon(X))
        {
            return TARGET_HORIZON;
        }
        dist = clamp((lenX - ballNearR), light_step_min, light_step_max);
        if (kerr_has == 2)
        {
            float r = length(X.xy);
            if ((r < disk.r_max) && (r > disk.r_min) && (abs(X.z) < abs(disk.h_min + disk.k * (r - disk.r_min))))
            {
                return TARGET_DISK;
            }
            dist = max(min(dist, (distance(X, float3((normalize(X.xy) * disk.center), 0.F)) - disk.R)), light_step_min);
        }
    }
    Body body;
    float temp;
    for (uint i = 0U; i < body_count; ++i)
    {
        body = bodyList[i];
        temp = ((distance((X.xzy / scale_distance + kerr_position), body.position[0U]) - body.radius) * scale_distance);
        if (temp < EPS)
        {
            bodyId = i;
            return TARGET_BODY;
        }
        dist = max(min(dist, temp), light_step_min);
    }
    return TARGET_NO;
}
Target RayTrace(inout float3 X, inout float3 V, out uint bodyId)
{
    bodyId = 0.F;
    Target target;
    float dist;
    for (uint i = 0U; i < light_step_count; ++i)
    {
        if (target = FindDist(X, dist, bodyId))
        {
            return target;
        }
        if (kerr_has)
        {
            RK4(X, V, dist);
        }
        else
        {
            Euler(X, V, dist);
        }
    }
    return TARGET_NO;
}
bool NoRunAway(in float3 X)
{
    return (kerr_has && (length(X) < ballNearR));
}

/////////////////////////////////////////////////////////////////////////////////////////

float2 SkyThetaPhi(in float3 V)
{
    V = normalize(V);
    float PI = acos(-1.F);
    return float2(min(((0.5F - 0.5F * atan2(V.y, V.x) / PI) * (sky_width - 1.F)), sky_width), min(((acos(V.z) / PI) * (sky_height - 1.F)), sky_height));
}
float2 SkyXYZ(in float3 V)
{
    V = normalize(V);
    float2 uv;
    float ax = abs(V.x);
    float ay = abs(V.y);
    float az = abs(V.z);
    if ((ax > ay) && (ax > az))
    {
        uv[0U] = V.y;
        uv[1U] = V.z;
    }
    else if ((ay > ax) && (ay > az))
    {
        uv[0U] = V.x;
        uv[1U] = V.z;
    }
    else
    {
        uv[0U] = V.x;
        uv[1U] = V.y;
    }
    uv = (0.5F - 0.5F * sqrt(2.F) * uv);
    return float2(min((uv[0U] * (sky_width - 1.F)), sky_width), min((uv[1U] * (sky_height - 1.F)), sky_height));
}

/////////////////////////////////////////////////////////////////////////////////////////

float ClockSlow(in float3 X)
{
    float r = length(X);
    return max(EPS, (1.F - 2.F * m * r / (r * r + a * a * pow((X.z / r), 2.F))));
}
float3 WavelengthToRgb(in float wavelength)
{
    float3 color = (float3) 0.F;
    if ((wavelength < 380.F) || (wavelength > 750.F))
    {
        return color;
    }
    if ((wavelength >= 380.F) && (wavelength < 440.F))
    {
        color.r = ((440.F - wavelength) / (440.F - 380.F));
        color.g = 0.F;
        color.b = 1.F;
    }
    else if ((wavelength >= 440.F) && (wavelength < 490.F))
    {
        color.r = 0.F;
        color.g = ((wavelength - 440.F) / (490.F - 440.F));
        color.b = 1.F;
    }
    else if ((wavelength >= 490.F) && (wavelength < 510.F))
    {
        color.r = 0.F;
        color.g = 1.F;
        color.b = ((510.F - wavelength) / (510.F - 490.F));
    }
    else if ((wavelength >= 510.F) && (wavelength < 580.F))
    {
        color.r = ((wavelength - 510.F) / (580.F - 510.F));
        color.g = 1.F;
        color.b = 0.F;
    }
    else if ((wavelength >= 580.F) && (wavelength < 645.F))
    {
        color.r = 1.F;
        color.g = ((645.F - wavelength) / (645.F - 580.F));
        color.b = 0.F;
    }
    else if ((wavelength >= 645.F) && (wavelength <= 750.F))
    {
        color.r = 1.F;
        color.g = 0.F;
        color.b = 0.F;
    }
    float factor = 0.F;
    if ((wavelength >= 380.F) && (wavelength < 420.F))
    {
        factor = (0.3F + 0.7F * (wavelength - 380.F) / (420.F - 380.F));
    }
    else if ((wavelength >= 420.F) && (wavelength < 645.F))
    {
        factor = 1.F;
    }
    else if ((wavelength >= 645.F) && (wavelength <= 750.F))
    {
        factor = (0.3F + 0.7F * (750.F - wavelength) / (750.F - 645.F));
    }
    return (factor * rsqrt(color.r * color.r + 2.25F * color.g * color.g + 0.36F * color.b * color.b) * (0.1F * (color.r + color.g + color.b) + 0.9F) * color);
}
float4 ChangeRgb(in float4 color, in float w)
{
    float3 colorS = ((color.r * 1.F * WavelengthToRgb(max(453.F, (645.F * w)))) + (color.g * 1.5F * WavelengthToRgb(max(416.F, (510.F * w)))) + (color.b * 0.6F * WavelengthToRgb(max(380.F, (440.F * w)))));
    colorS *= ((0.3F * color.r + 0.6F * color.g + 0.1F * color.b) / max((0.3F * colorS.r + 0.6F * colorS.g + 0.1F * colorS.b), EPS));
    return (float4(clamp(colorS, (float3) 0.F, (float3) 1.F), color.a) / max(pow(w, 4.F), EPS));
}
float4 DopplerAndShift(in float3 X)
{
    return ChangeRgb(COLOR_DISK, (ClockSlow(cameraX) / ClockSlow(X) * (1.F - dot(float3((normalize(float2(-X.y, X.x)) * disk_speed), 0.F), normalize(camera_position - kerr_position).xzy))));
}
float4 BlueShift(in float3 X, in float4 color)
{
    return (kerr_has ? ChangeRgb(color, ClockSlow(cameraX)) : color);
}

/////////////////////////////////////////////////////////////////////////////////////////

[numthreads(NUMTHREADS_X_LIGHT, NUMTHREADS_Y_LIGHT, 1)]
void main(uint3 dtID : SV_DispatchThreadID)
{
    idx = dtID.x;
    idy = dtID.y;
    if ((idx < screen_width) && (idy < screen_height))
    {
        float3 X;
        float3 V;
        Init(X, V);
        uint bodyId;
        switch (RayTrace(X, V, bodyId))
        {
            case TARGET_NO:
                lightTexture[dtID.xy] = (NoRunAway(X) ? COLOR_KERR : (sky_has ? BlueShift(X, skyboxTexture[SkyXYZ(V)]) : COLOR_BACKGROUND));
                return;
            case TARGET_BODY:
                lightTexture[dtID.xy] = BlueShift(X, float4(bodyList[bodyId].color, 1.F));
                return;
            case TARGET_HORIZON:
                lightTexture[dtID.xy] = COLOR_KERR;
                return;
            case TARGET_DISK:
                lightTexture[dtID.xy] = DopplerAndShift(X);
                return;
        }
    }
}