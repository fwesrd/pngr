#include "LightTrace.h"

bool LightTrace::Init()
{
    return (LightTrace::pWindow->CreateComputeShaderFromFileGPU("light.hlsl", "main", &(LightTrace::pCS)) &&
        LightTrace::pWindow->CreateConstantBufferGPU(static_cast<UINT>(sizeof(LightTrace::LightStruct)), &(LightTrace::pBuffer)) &&
        LightTrace::pWindow->BindConstantBufferGPU(LightTrace::pBuffer.GetAddressOf(), 2U) &&
        LightTrace::pWindow->MapBufferGPU(LightTrace::pBuffer.Get(), &(LightTrace::light), sizeof(LightTrace::LightStruct)));
}

void LightTrace::Change(unsigned int step_count, float step_min, float step_max)
{
    LightTrace::light.light_step_count = step_count;
    LightTrace::light.light_step_min = step_min;
    LightTrace::light.light_step_max = step_max;
}
void LightTrace::SetIs(bool is)
{
    LightTrace::is = is;
}
const LightTrace::LightStruct& LightTrace::Data()
{
    return LightTrace::light;
}
bool LightTrace::GetIs()
{
    return LightTrace::is;
}

bool LightTrace::UpdataLightTexture(UINT width, UINT height)
{
    auto device{ LightTrace::pWindow->GetDevice() };
    auto context{ LightTrace::pWindow->GetContext() };
    if (!device || !context) return false;
    //texture
    D3D11_TEXTURE2D_DESC td{};
    {
        td.Width = width;
        td.Height = height;
        td.MipLevels = 1U;
        td.ArraySize = 1U;
        td.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
        td.SampleDesc.Count = 1U;
        td.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
        td.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_UNORDERED_ACCESS;
    }
    if (FAILED(device->CreateTexture2D(&td, nullptr, &(LightTrace::pTexture)))) return false;
    //UAV
    static D3D11_UNORDERED_ACCESS_VIEW_DESC uavd{};
    {
        uavd.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
        uavd.ViewDimension = D3D11_UAV_DIMENSION::D3D11_UAV_DIMENSION_TEXTURE2D;
    }
    if (FAILED(device->CreateUnorderedAccessView(LightTrace::pTexture.Get(), &uavd, &(LightTrace::pUAV)))) return false;
    context->CSSetUnorderedAccessViews(2U, 1U, LightTrace::pUAV.GetAddressOf(), nullptr);
    return true;
}

bool LightTrace::Compute(UINT width, UINT height)
{
    if (!LightTrace::is) return true;
    {
        auto& camera{ LightTrace::pWindow->GetCamera() };
        IWinUI::Position pos{ camera.GetPosition() };
        IWinUI::Vector focus{ camera.GetFocusDirection() };
        IWinUI::Vector left{ camera.GetLeftDirection() };
        IWinUI::Vector head{ camera.GetHeadDirection() };
        {
            LightTrace::light.camera_position[0] = pos.x;
            LightTrace::light.camera_position[1] = pos.y;
            LightTrace::light.camera_position[2] = pos.z;

            LightTrace::light.camera_focus[0] = focus.x;
            LightTrace::light.camera_focus[1] = focus.y;
            LightTrace::light.camera_focus[2] = focus.z;

            LightTrace::light.camera_left[0] = left.x;
            LightTrace::light.camera_left[1] = left.y;
            LightTrace::light.camera_left[2] = left.z;

            LightTrace::light.camera_head[0] = head.x;
            LightTrace::light.camera_head[1] = head.y;
            LightTrace::light.camera_head[2] = head.z;
        }
        LightTrace::light.screen_width = width;
        LightTrace::light.screen_height = height;
        LightTrace::light.fov_delta = (::tanf(DirectX::XMConvertToRadians(camera.GetFovAngleY() / 2.F)) * 2.F / height);
    }
    return (LightTrace::pWindow->MapBufferGPU(LightTrace::pBuffer.Get(), &(LightTrace::light), sizeof(LightTrace::LightStruct)) &&
        LightTrace::pWindow->RunGPU(LightTrace::pCS.Get(),
            static_cast<UINT>(std::ceil(static_cast<float>(LightTrace::light.screen_width) / NUMTHREADS_X_LIGHT)),
            static_cast<UINT>(std::ceil(static_cast<float>(LightTrace::light.screen_height) / NUMTHREADS_Y_LIGHT)), 1U) &&
        LightTrace::pWindow->WaitGPU());
}

void LightTrace::Draw()
{
    if (!LightTrace::is) return;
    ID3D11Resource* backBufferResource{};
    LightTrace::pWindow->GetTarget3d()->GetResource(&backBufferResource);
    if (backBufferResource)
    {
        LightTrace::pWindow->GetContext()->CopyResource(backBufferResource, LightTrace::pTexture.Get());
        backBufferResource->Release();
    }
}

LightTrace::LightTrace(IWinUI* pWindow) : pWindow{ pWindow }, light{}, is{ false }
{
    LightTrace::Change(NConst::light_init_step_count, NConst::light_init_step_min, NConst::light_init_step_max);
}