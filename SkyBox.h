#pragma once
#ifndef _SKYBOX_H_
#define _SKYBOX_H_

#include "head.h"

class SkyBox
{
protected:
    struct SkyStruct
    {
    public:
        unsigned int sky_width;
        unsigned int sky_height;
        int sky_has;
        int UNUSE3;
    };

protected:
    IWinUI* pWindow;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pSRV;

    SkyBox::SkyStruct skyStruct;
    Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;

    std::string path;
    long long density;

public:
    bool Init();

    bool Change(bool has, std::string filePath, long long density, UINT width, UINT height);

    bool Has();
    std::string Path();
    long long Density();

public:
    SkyBox(IWinUI* pWindow);
};

#endif // !_SKYBOX_H_