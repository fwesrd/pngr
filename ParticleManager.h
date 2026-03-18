#pragma once
#ifndef _PARTICLEMANAGER_H_
#define _PARTICLEMANAGER_H_

#include "head.h"

class ParticleManager
{
    friend class Simulator;

public:
    struct ParticleData
    {
    public:
        int particle_count;
        float particle_distance;
        float particle_offset_x;
        float particle_offset_y;
        float particle_offset_z;
    };

protected:
    struct ParticleStruct
    {
    public:
        const float orginalPosition[3];
        float position[3];
        float velocity[3];
    public:
        ParticleManager::ParticleStruct(float px, float py, float pz);
    };

protected:
    IWinUI* pWindow;

    Microsoft::WRL::ComPtr<ID3D11ComputeShader> pParticleCS{};
    Microsoft::WRL::ComPtr<ID3D11ComputeShader> pResetCS{};
    IWinUI::BufferGPU buffer;

    std::vector<ParticleManager::ParticleStruct> list;
    std::vector<IWinUI::Vertex> vertices;
    IWinUI::Vertex tempVertex;

    ParticleManager::ParticleData particleData;

public:
    bool Init(UINT particleMaxCount);

    void Change(int count, float distance, float offsetX, float offsetY, float offsetZ);

    const ParticleManager::ParticleData& Data();

    bool RunParticle();
    bool RunReset(size_t bodyCount);

    bool GetResult();
    bool AddDataInDraw();

public:
    ParticleManager(IWinUI* pWindow);
};

#endif // !_PARTICLEMANAGER_H_