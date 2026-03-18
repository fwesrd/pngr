#pragma once
#ifndef _SIMULATOR_H_
#define _SIMULATOR_H_

#include "head.h"

#include "BodyManager.h"
#include "ParticleManager.h"
#include "RK4Manager.h"
#include "KerrManager.h"

class Simulator
{
protected:
    IWinUI* pWindow;

    BodyManager bodyManager;
    ParticleManager particleManager;
    RK4Manager rk4Manager;
    KerrManager kerrManager;

public:
    bool Init(UINT bodyMaxCount, UINT particleMaxCount);
    bool Compute(double second);
    bool GetResult();
    bool AddDataInDraw();

    bool AddBody(_IN_ BodyManager::BodyStruct& bodyStruct);
    bool DeleteBody(unsigned int id);
    void ClearBody();
    bool ScaleBody();
    const std::vector<BodyManager::BodyStruct>& BodiesData();

    void ChangeParticle(int count, float distance, float offsetX, float offsetY, float offsetZ);
    const ParticleManager::ParticleData& ParticleData();

    bool ChangeKerr(bool has, double mass, float a_rate, float x, float y, float z, bool disk, float speed);
    const KerrManager::KerrStruct& KerrData();
    bool KerrHas();
    bool KerrDisk();

protected:
    bool RungeKuttaIntegral(float step);

public:
    Simulator(IWinUI* pWindow);
};

#endif // !_SIMULATOR_H_