#pragma once
#ifndef _RK4MANAGER_H_
#define _RK4MANAGER_H_

#include "head.h"

class RK4Manager
{
    friend class Simulator;

protected:
    struct RK4Struct
    {
    public:
        unsigned int body_count;
        unsigned int particle_count;

        unsigned int k;
        unsigned int n;

        float step;

        float scale_distance;
        float scale_distance_3;

        float particle_space_2;
    };

protected:
    IWinUI* pWindow;

    RK4Manager::RK4Struct rk4;
    Microsoft::WRL::ComPtr<ID3D11Buffer> pBuffer{};

public:
    bool Init();

    bool MapBuffer();

public:
    RK4Manager(IWinUI* pWindow);
};

#endif // !_RK4MANAGER_H_