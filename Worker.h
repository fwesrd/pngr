#pragma once
#ifndef _WORKER_H_
#define _WORKER_H_

#include "head.h"

#include "Simulator.h"
#include "LightTrace.h"
#include "SkyBox.h"

class Worker
{
protected:
    IWinUI* pWindow;

    std::mutex mutex;

    Simulator simulator;
    LightTrace light;
    SkyBox sky;

    bool run;

    bool follow;
    unsigned int followId;

    bool focusBody;
    unsigned int focusBodyId;

    bool focusKerr;

public:
    bool Init();
    void CalculateAndAddDataInDraw(std::chrono::steady_clock::time_point lastFrameTime, std::chrono::steady_clock::time_point nextFrameTime, UINT width, UINT height);
    void LightDraw();
    void WmSize(UINT width, UINT height);

public:
    void Stop();
    void Run();

public:
    //body
    bool BodyAdd(_IN_ BodyManager::BodyStruct& bodyStruct);
    bool BodyDelete(unsigned int id);
    void BodyClear();
    const std::vector<BodyManager::BodyStruct> BodiesData();

    //particle
    void ParticleChange(int count, float distance, float offsetX, float offsetY, float offsetZ);
    const ParticleManager::ParticleData ParticleData();

    //kerr
    bool KerrChange(bool has, double mass, float a_rate, float x, float y, float z, bool disk, float speed);
    const KerrManager::KerrStruct KerrData();
    bool KerrHas();
    bool KerrDisk();

    //light
    void LightChange(unsigned int step_count, float step_min, float step_max);
    void LightSetIs(bool is);
    const LightTrace::LightStruct LightData();
    bool LightGetIs();

    //sky
    bool SkyChange(bool has, std::string filePath, int density);
    bool SkyHas();
    std::string SkyPath();
    int SkyDensity();

    //scale
    void ChangeTimeRate(float rate);
    void ChangeScaleTime(float scale_time);
    bool ChangeScaleDistance(float scale_distance);

    //camera
    void Fov(float fov);
    bool Follow(bool follow, unsigned int id);
    bool FocusBody(unsigned int id);
    bool FocusKerr();
    void UnFocus();

public:
    Worker(IWinUI* pWindow);
};

#endif // !_WORKER_H_