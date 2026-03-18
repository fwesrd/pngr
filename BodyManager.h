#pragma once
#ifndef _BODYMANAGER_H_
#define _BODYMANAGER_H_

#include "head.h"

class BodyManager
{
    friend class Simulator;

public:
    struct BodyStruct
    {
    public:
        float position[2][3];
        float velocity[2][3];
        float color[3];
        float acceleration[5][3];
        unsigned int id;
        float radius;
        int UNUSE1[2];
        double mass;
    public:
        void AddSphere(_INOUT_ std::vector<IWinUI::Vertex>& vertices, _INOUT_ std::vector<IWinUI::Index>& indices) const;
    public:
        BodyManager::BodyStruct(unsigned int id, double mass, float radius, float px, float py, float pz, float vx, float vy, float vz, float r, float g, float b);
    };

    struct TraceStruct
    {
    public:
        unsigned int id;
        std::deque<IWinUI::Vertex> vertices;
    };

protected:
    IWinUI* pWindow;

    Microsoft::WRL::ComPtr<ID3D11ComputeShader> pAccelerationCS{};
    Microsoft::WRL::ComPtr<ID3D11ComputeShader> pEulerCS{};
    Microsoft::WRL::ComPtr<ID3D11ComputeShader> pRungeKuttaCS{};
    IWinUI::BufferGPU buffer;

    std::vector<BodyManager::BodyStruct> bodyList;
    std::vector<IWinUI::Vertex> bodyVertices;
    std::vector<IWinUI::Index> bodyIndices;
    std::list<BodyManager::TraceStruct> traceList;
    std::vector<IWinUI::Vertex> traceVertices;
    IWinUI::Vertex traceVertex{};

public:
    bool Init(UINT bodyMaxCount);

    bool Add(_IN_ BodyManager::BodyStruct& bodyStruct);
    bool Delete(unsigned int id);
    void Clear();
    bool Scale();

    const std::vector<BodyManager::BodyStruct>& Data();

    bool RunAcceleration();
    bool RunEuler();
    bool RunRungeKutta();

    bool GetResult();
    bool AddDataInDraw();

public:
    BodyManager(IWinUI* pWindow);
};

#endif // !_BODYMANAGER_H_