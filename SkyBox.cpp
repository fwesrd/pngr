#include "SkyBox.h"

bool SkyBox::Init()
{
    return (SkyBox::pWindow->CreateConstantBufferGPU(static_cast<UINT>(sizeof(SkyBox::SkyStruct)), &(SkyBox::buffer)) &&
        SkyBox::pWindow->BindConstantBufferGPU(SkyBox::buffer.GetAddressOf(), 3U) &&
        SkyBox::pWindow->MapBufferGPU(SkyBox::buffer.Get(), &(SkyBox::skyStruct), sizeof(SkyBox::SkyStruct)));
}

bool SkyBox::Change(bool has, std::string filePath, long long density, UINT width, UINT height)
{
    auto device{ SkyBox::pWindow->GetDevice() };
    auto context{ SkyBox::pWindow->GetContext() };
    if (!device || !context) return false;
    SkyBox::skyStruct.sky_has = (has ? 1 : 0);
    SkyBox::path = filePath;
    SkyBox::density = density;
    if (!has) return SkyBox::pWindow->MapBufferGPU(SkyBox::buffer.Get(), &(SkyBox::skyStruct), sizeof(SkyBox::SkyStruct));
    //pixels
    std::vector<IWinUI::Pixel> pixels{};
    {
        if ((filePath == "") || !SkyBox::pWindow->ReadImage(filePath, pixels, &width, &height))
        {
            SkyBox::path = "";
            pixels.clear();
            IWinUI::Pixel pixel{ 0, 0, 0 };
            for (UINT y{ 0U }; y < height; ++y)
            {
                for (UINT x{ 0U }; x < width; ++x)
                {
                    pixels.emplace_back(NMath::Random(0LL, SkyBox::density) ? pixel :
                        IWinUI::Pixel{
                            static_cast<BYTE>(NMath::Random(0LL, 255LL)),
                            static_cast<BYTE>(NMath::Random(0LL, 255LL)),
                            static_cast<BYTE>(NMath::Random(0LL, 255LL)) });
                }
            }
        }
        SkyBox::skyStruct.sky_width = width;
        SkyBox::skyStruct.sky_height = height;
        if (!SkyBox::pWindow->MapBufferGPU(SkyBox::buffer.Get(), &(SkyBox::skyStruct), sizeof(SkyBox::SkyStruct))) return false;
    }
    //texture
    D3D11_TEXTURE2D_DESC td{};
    {
        td.Width = width;
        td.Height = height;
        td.MipLevels = 1U;
        td.ArraySize = 1U;
        td.Format = DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM;
        td.SampleDesc.Count = 1U;
        td.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
        td.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE;
    }
    D3D11_SUBRESOURCE_DATA data{};
    {
        data.pSysMem = pixels.data();
        data.SysMemPitch = (width * sizeof(IWinUI::Pixel));
    }
    if (FAILED(device->CreateTexture2D(&td, &data, &(SkyBox::pTexture)))) return false;
    //SRV
    D3D11_SHADER_RESOURCE_VIEW_DESC srvd{};
    {
        srvd.Format = DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM;
        srvd.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D;
        srvd.Texture2D.MipLevels = td.MipLevels;
    }
    if (FAILED(device->CreateShaderResourceView(SkyBox::pTexture.Get(), &srvd, &(SkyBox::pSRV)))) return false;
    context->CSSetShaderResources(0U, 1U, SkyBox::pSRV.GetAddressOf());
    return true;
}

bool SkyBox::Has()
{
    return (SkyBox::skyStruct.sky_has == 1);
}
std::string SkyBox::Path()
{
    return SkyBox::path;
}
long long SkyBox::Density()
{
    return SkyBox::density;
}

SkyBox::SkyBox(IWinUI* pWindow) : pWindow{ pWindow }, skyStruct{}, density{ NConst::sky_init_density }
{}