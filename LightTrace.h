#pragma once
#ifndef _LIGHTTRACE_H_
#define _LIGHTTRACE_H_

#include "head.h"

class LightTrace
{
public:
    struct LightStruct
    {
    public:
        float camera_position[3];
        unsigned int screen_width;

        float camera_focus[3];
        unsigned int screen_height;

        float camera_left[3];
        float fov_delta;

        float camera_head[3];
        unsigned int light_step_count;

        float light_step_min;
        float light_step_max;

        int UNUSE2[2];
    };

protected:
    IWinUI* pWindow;

    Microsoft::WRL::ComPtr<ID3D11ComputeShader> pCS{};

    Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> pUAV;

    LightTrace::LightStruct light;
    Microsoft::WRL::ComPtr<ID3D11Buffer> pBuffer;

    bool is;

public:
    bool Init();

    void Change(unsigned int step_count, float step_min, float step_max);
    void SetIs(bool is);
    const LightTrace::LightStruct& Data();
    bool GetIs();

    bool UpdataLightTexture(UINT width, UINT height);

    bool Compute(UINT width, UINT height);

    void Draw();

public:
    LightTrace(IWinUI* pWindow);
};

#endif // !_LIGHTTRACE_H_