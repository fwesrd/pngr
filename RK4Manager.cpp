#include "RK4Manager.h"

bool RK4Manager::Init()
{
    return (RK4Manager::pWindow->CreateConstantBufferGPU(static_cast<UINT>(sizeof(RK4Manager::RK4Struct)), &(RK4Manager::pBuffer)) &&
        RK4Manager::pWindow->BindConstantBufferGPU(RK4Manager::pBuffer.GetAddressOf(), 0U) &&
        RK4Manager::MapBuffer());
}

bool RK4Manager::MapBuffer()
{
    return RK4Manager::pWindow->MapBufferGPU(RK4Manager::pBuffer.Get(), &(RK4Manager::rk4), sizeof(RK4Manager::RK4Struct));
}

RK4Manager::RK4Manager(IWinUI* pWindow) : pWindow{ pWindow }, rk4{}
{
    RK4Manager::rk4.scale_distance = NGlobal::scale_distance;
    RK4Manager::rk4.scale_distance_3 = (NGlobal::scale_distance * NGlobal::scale_distance * NGlobal::scale_distance);
}