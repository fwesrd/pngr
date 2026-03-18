#pragma once
#ifndef _KERRMANAGER_H_
#define _KERRMANAGER_H_

#include "head.h"

class KerrManager
{
    friend class Simulator;

public:
    struct KerrStruct
    {
    public:
        double kerr_mass;
        float kerr_a_rate;
        int kerr_has;
        float kerr_position[3];
        float disk_speed;
        float disk_r_min;
        float disk_r_max;
        float disk_h_min;
        float disk_h_max;
    };

protected:
    IWinUI* pWindow;

    KerrStruct kerr;
    Microsoft::WRL::ComPtr<ID3D11Buffer> pKerrBuffer;

    std::vector<IWinUI::Vertex> diskVertices;
    std::vector<IWinUI::Index> diskIndices;

    bool disk;

public:
    bool Init();

    bool Change(bool has, double mass, float a_rate, float x, float y, float z, bool disk, float speed);

    const KerrStruct& Data();
    bool Has();
    bool Disk();

    bool AddDataInDraw();

public:
    KerrManager(IWinUI* pWindow);
};

#endif // !_KERRMANAGER_H_