#include "KerrManager.h"

bool KerrManager::Init()
{
    return (KerrManager::pWindow->CreateConstantBufferGPU(static_cast<UINT>(sizeof(KerrManager::KerrStruct)), &(KerrManager::pKerrBuffer)) &&
        KerrManager::pWindow->BindConstantBufferGPU(KerrManager::pKerrBuffer.GetAddressOf(), 1U) &&
        KerrManager::pWindow->MapBufferGPU(KerrManager::pKerrBuffer.Get(), &(KerrManager::kerr), sizeof(KerrManager::KerrStruct)));
}

bool KerrManager::Change(bool has, double mass, float a_rate, float x, float y, float z, bool disk, float speed)
{
    if (a_rate < 0.F) a_rate = 0.F;
    else if (a_rate > 1.F) a_rate = 1.F;
    if (speed < 0.F) speed = 0.F;
    else if (speed > 1.F) speed = 1.F;
    //kerr struct
    {
        KerrManager::kerr.kerr_mass = mass;
        KerrManager::kerr.kerr_a_rate = a_rate;
        KerrManager::kerr.kerr_has = (has ? (disk ? 2 : 1) : 0);
        KerrManager::kerr.kerr_position[0] = (x / NGlobal::scale_distance);
        KerrManager::kerr.kerr_position[1] = (y / NGlobal::scale_distance);
        KerrManager::kerr.kerr_position[2] = (z / NGlobal::scale_distance);
        KerrManager::kerr.disk_speed = speed;
        float mGeo{ static_cast<float>(G * mass / C / C) };
        KerrManager::kerr.disk_r_min = (NConst::disk_rate_r_min * mGeo / NGlobal::scale_distance);
        KerrManager::kerr.disk_r_max = (NConst::disk_rate_r_max * mGeo / NGlobal::scale_distance);
        KerrManager::kerr.disk_h_min = (NConst::disk_rate_h_min * KerrManager::kerr.disk_r_min);
        KerrManager::kerr.disk_h_max = (NConst::disk_rate_h_max * KerrManager::kerr.disk_r_max);
    }
    KerrManager::disk = disk;
    //disk
    if (has && disk)
    {
        static constexpr const float step{ DirectX::XM_2PI / static_cast<float>(NConst::disk_fineness) };
        KerrManager::diskVertices.clear();
        KerrManager::diskIndices.clear();
        IWinUI::Vertex vertex{ IWinUI::Position{}, NConst::disk_color };
        //vertex
        for (IWinUI::Index trav{ 0U }; trav < NConst::disk_fineness; ++trav)
        {
            vertex.position.x = (KerrManager::kerr.kerr_position[0] + KerrManager::kerr.disk_r_min * ::cosf(trav * step));
            vertex.position.y = (KerrManager::kerr.kerr_position[1] + KerrManager::kerr.disk_h_min);
            vertex.position.z = (KerrManager::kerr.kerr_position[2] + KerrManager::kerr.disk_r_min * ::sinf(trav * step));
            KerrManager::diskVertices.emplace_back(vertex);
            vertex.position.y = (KerrManager::kerr.kerr_position[1] - KerrManager::kerr.disk_h_min);
            KerrManager::diskVertices.emplace_back(vertex);
            vertex.position.x = (KerrManager::kerr.kerr_position[0] + KerrManager::kerr.disk_r_max * ::cosf(trav * step));
            vertex.position.y = (KerrManager::kerr.kerr_position[1] + KerrManager::kerr.disk_h_max);
            vertex.position.z = (KerrManager::kerr.kerr_position[2] + KerrManager::kerr.disk_r_max * ::sinf(trav * step));
            KerrManager::diskVertices.emplace_back(vertex);
            vertex.position.y = (KerrManager::kerr.kerr_position[1] - KerrManager::kerr.disk_h_max);
            KerrManager::diskVertices.emplace_back(vertex);
        }
        //index
        IWinUI::Index total{ NConst::disk_fineness - 1U };
        for (IWinUI::Index trav{ 0U }; trav < total; ++trav)
        {
            //  ___
            //  | /
            //  |/
            KerrManager::diskIndices.emplace_back(trav * 4U);
            KerrManager::diskIndices.emplace_back(trav * 4U + 1U);
            KerrManager::diskIndices.emplace_back((trav + 1U) * 4U);
            //   /|
            //  / |
            //  ~~~
            KerrManager::diskIndices.emplace_back((trav + 1U) * 4U);
            KerrManager::diskIndices.emplace_back(trav * 4U + 1U);
            KerrManager::diskIndices.emplace_back((trav + 1U) * 4U + 1U);

            KerrManager::diskIndices.emplace_back(trav * 4U + 2U);
            KerrManager::diskIndices.emplace_back((trav + 1U) * 4U + 2U);
            KerrManager::diskIndices.emplace_back(trav * 4U + 3U);
            KerrManager::diskIndices.emplace_back((trav + 1U) * 4U + 2U);
            KerrManager::diskIndices.emplace_back((trav + 1U) * 4U + 3U);
            KerrManager::diskIndices.emplace_back(trav * 4U + 3U);

            KerrManager::diskIndices.emplace_back(trav * 4U + 2U);
            KerrManager::diskIndices.emplace_back(trav * 4U);
            KerrManager::diskIndices.emplace_back((trav + 1U) * 4U + 2U);
            KerrManager::diskIndices.emplace_back((trav + 1U) * 4U + 2U);
            KerrManager::diskIndices.emplace_back(trav * 4U);
            KerrManager::diskIndices.emplace_back((trav + 1U) * 4U);

            KerrManager::diskIndices.emplace_back(trav * 4U + 3U);
            KerrManager::diskIndices.emplace_back((trav + 1U) * 4U + 3U);
            KerrManager::diskIndices.emplace_back(trav * 4U + 1U);
            KerrManager::diskIndices.emplace_back((trav + 1U) * 4U + 3U);
            KerrManager::diskIndices.emplace_back((trav + 1U) * 4U + 1U);
            KerrManager::diskIndices.emplace_back(trav * 4U + 1U);
        }
        {
            IWinUI::Index tail{ (total - 1U) * 4U };

            KerrManager::diskIndices.emplace_back(tail);
            KerrManager::diskIndices.emplace_back(tail + 1U);
            KerrManager::diskIndices.emplace_back(0U);
            KerrManager::diskIndices.emplace_back(0U);
            KerrManager::diskIndices.emplace_back(tail + 1U);
            KerrManager::diskIndices.emplace_back(1U);

            KerrManager::diskIndices.emplace_back(tail + 2U);
            KerrManager::diskIndices.emplace_back(2U);
            KerrManager::diskIndices.emplace_back(tail + 3U);
            KerrManager::diskIndices.emplace_back(2U);
            KerrManager::diskIndices.emplace_back(3U);
            KerrManager::diskIndices.emplace_back(tail + 3U);

            KerrManager::diskIndices.emplace_back(tail + 2U);
            KerrManager::diskIndices.emplace_back(tail);
            KerrManager::diskIndices.emplace_back(2U);
            KerrManager::diskIndices.emplace_back(2U);
            KerrManager::diskIndices.emplace_back(tail);
            KerrManager::diskIndices.emplace_back(0U);

            KerrManager::diskIndices.emplace_back(tail + 3U);
            KerrManager::diskIndices.emplace_back(3U);
            KerrManager::diskIndices.emplace_back(tail + 1U);
            KerrManager::diskIndices.emplace_back(3U);
            KerrManager::diskIndices.emplace_back(1U);
            KerrManager::diskIndices.emplace_back(tail + 1U);
        }
    }
    return KerrManager::pWindow->MapBufferGPU(KerrManager::pKerrBuffer.Get(), &(KerrManager::kerr), sizeof(KerrManager::KerrStruct));
}

const KerrManager::KerrStruct& KerrManager::Data()
{
    return KerrManager::kerr;
}
bool  KerrManager::Has()
{
    return (KerrManager::kerr.kerr_has != 0);
}
bool KerrManager::Disk()
{
    return KerrManager::disk;
}

bool KerrManager::AddDataInDraw()
{
    if (KerrManager::kerr.kerr_has != 2) return true;
    return IWinUI::Object<IWinUI::ObjectType::TRIANGLEED>::AddDataInDraw(KerrManager::pWindow, KerrManager::diskVertices, KerrManager::diskIndices);
}

KerrManager::KerrManager(IWinUI* pWindow) : pWindow{ pWindow }, kerr{}, disk{ false }
{}