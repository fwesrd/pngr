#include "share.h"

#pragma warning(disable: 4000)

//geo
#define MO (G / C / C)
#define VO (1.F / C)
#define TO C
#define AO (1.F / C / C)

//struct
struct Body
{
    float3 position[2];
    float3 velocity[2];
    float3 color;
    float3 acceleration[5];
    uint id;
    float radius;
    int UNUSE1[2];
    double mass;
};
struct Particle
{
    float3 orginalPosition;
    float3 position;
    float3 velocity;
};

//constant
cbuffer ChooseRK4 : register(b0)
{
    uint body_count;
    uint particle_count;

    uint k;
    uint n;

    float step;

    float scale_distance;
    float scale_distance_3;

    float particle_space_2;
};
cbuffer Kerr : register(b1)
{
    double kerr_mass;
    float kerr_a_rate;
    int kerr_has;

    float3 kerr_position;
    float disk_speed;
    
    float disk_r_min;
    float disk_r_max;
    float disk_h_min;
    float disk_h_max;
}

//unorder
RWStructuredBuffer<Body> bodyList : register(u0);
RWStructuredBuffer<Particle> particleList : register(u1);