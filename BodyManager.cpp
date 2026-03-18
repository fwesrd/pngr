#include "BodyManager.h"

void BodyManager::BodyStruct::AddSphere(_INOUT_ std::vector<IWinUI::Vertex>& vertices, _INOUT_ std::vector<IWinUI::Index>& indices) const
{
    const float& x{ BodyManager::BodyStruct::position[0][0] };
    const float& y{ BodyManager::BodyStruct::position[0][1] };
    const float& z{ BodyManager::BodyStruct::position[0][2] };
    const float& radius{ BodyManager::BodyStruct::radius };
    IWinUI::Color color{};
    color.x = BodyManager::BodyStruct::color[0];
    color.y = BodyManager::BodyStruct::color[1];
    color.z = BodyManager::BodyStruct::color[2];
    color.w = 1.F;
    static constexpr const IWinUI::Index latitudeCount{ NConst::body_sphere_fineness };
    static constexpr const IWinUI::Index longitudeCount{ NConst::body_sphere_fineness };
    IWinUI::Index offset{ static_cast<IWinUI::Index>(vertices.size()) };
    //vertex
    {
        IWinUI::Vertex northPoint{ IWinUI::Position{ x, (y + radius), z }, color };
        vertices.emplace_back(northPoint);
        //other vertex count = (latitudeCount - 1) * longitudeCount
        static constexpr const float latiStep{ DirectX::XM_PI / static_cast<float>(latitudeCount) };
        static constexpr const float longiStep{ DirectX::XM_2PI / static_cast<float>(longitudeCount) };
        for (IWinUI::Index latiTrav{ 1U }; latiTrav < latitudeCount; ++latiTrav)
        {
            for (IWinUI::Index longiTrav{ 0U }; longiTrav < longitudeCount; ++longiTrav)
            {
                IWinUI::Vertex point{ IWinUI::Position
                    {
                        (x + radius * ::sinf(latiTrav * latiStep) * ::cosf(longiTrav * longiStep)),
                        (y + radius * ::cosf(latiTrav * latiStep)),
                        (z + radius * ::sinf(latiTrav * latiStep) * ::sinf(longiTrav * longiStep))
                    }, color };
                vertices.emplace_back(point);
            }
        }
        IWinUI::Vertex southPoint{ IWinUI::Position{ x, (y - radius), z }, color };
        vertices.emplace_back(southPoint);
    }
    //index
    {
        //north
        for (IWinUI::Index longiTrav{ 0U }; longiTrav < longitudeCount; ++longiTrav)
        {
            indices.emplace_back(0U + offset);
            if (longiTrav == (longitudeCount - 1U))
            {
                indices.emplace_back(1U + offset);
            }
            else
            {
                indices.emplace_back(longiTrav + 2U + offset);
            }
            indices.emplace_back(longiTrav + 1U + offset);
        }
        //other
        for (IWinUI::Index latiTrav{ 1U }; latiTrav < (latitudeCount - 1U); ++latiTrav)
        {
            for (IWinUI::Index longiTrav{ 0U }; longiTrav < longitudeCount; ++longiTrav)
            {
                //  ___
                //  | /
                //  |/
                indices.emplace_back((latiTrav - 1U) * longitudeCount + 1U + longiTrav + offset);
                if (longiTrav == (longitudeCount - 1U))
                {
                    indices.emplace_back((latiTrav - 1U) * longitudeCount + 1U + offset);
                }
                else
                {
                    indices.emplace_back((latiTrav - 1U) * longitudeCount + 2U + longiTrav + offset);
                }
                indices.emplace_back(latiTrav * longitudeCount + 1U + longiTrav + offset);
                //   /|
                //  / |
                //  ~~~
                if (longiTrav == (longitudeCount - 1U))
                {
                    indices.emplace_back((latiTrav - 1U) * longitudeCount + 1U + offset);
                    indices.emplace_back(latiTrav * longitudeCount + 1U + offset);
                }
                else
                {
                    indices.emplace_back((latiTrav - 1U) * longitudeCount + 2U + longiTrav + offset);
                    indices.emplace_back(latiTrav * longitudeCount + 2U + longiTrav + offset);
                }
                indices.emplace_back(latiTrav * longitudeCount + 1U + longiTrav + offset);
            }
        }
        //south
        IWinUI::Index southIndex{ static_cast<IWinUI::Index>(vertices.size()) - 1U };
        for (IWinUI::Index longiTrav{ 0U }; longiTrav < longitudeCount; ++longiTrav)
        {
            indices.emplace_back(southIndex);
            indices.emplace_back(southIndex - longitudeCount + longiTrav);
            if (longiTrav == (longitudeCount - 1U))
            {
                indices.emplace_back(southIndex - longitudeCount);
            }
            else
            {
                indices.emplace_back(southIndex - longitudeCount + 1U + longiTrav);
            }
        }
    }
}

BodyManager::BodyStruct::BodyStruct(unsigned int id, double mass, float radius, float px, float py, float pz, float vx, float vy, float vz, float r, float g, float b) :
    id{ id },
    mass{ mass },
    radius{ (radius / NGlobal::scale_distance) },
    position{ (px / NGlobal::scale_distance), (py / NGlobal::scale_distance), (pz / NGlobal::scale_distance) },
    velocity{ (vx / NGlobal::scale_distance), (vy / NGlobal::scale_distance), (vz / NGlobal::scale_distance) },
    color{ r, g, b },
    acceleration{}
{}

bool BodyManager::Init(UINT bodyMaxCount)
{
    return (BodyManager::pWindow->CreateComputeShaderFromFileGPU("body_acceleration.hlsl", "main", &(BodyManager::pAccelerationCS)) &&
        BodyManager::pWindow->CreateComputeShaderFromFileGPU("body_euler.hlsl", "main", &(BodyManager::pEulerCS)) &&
        BodyManager::pWindow->CreateComputeShaderFromFileGPU("body_rungeKutta.hlsl", "main", &(BodyManager::pRungeKuttaCS)) &&
        BodyManager::pWindow->CreateBufferGPU(static_cast<UINT>(sizeof(BodyManager::BodyStruct)), bodyMaxCount, BodyManager::buffer) &&
        BodyManager::pWindow->BindBufferGPU(BodyManager::buffer, 0U));
}

bool BodyManager::Add(_IN_ BodyManager::BodyStruct& bodyStruct)
{
    for (BodyManager::BodyStruct& trav : BodyManager::bodyList)
    {
        if (bodyStruct.id == trav.id) return false;
    }
    BodyManager::bodyList.emplace_back(bodyStruct);
    BodyManager::TraceStruct traceStruct{};
    traceStruct.id = bodyStruct.id;
    BodyManager::traceList.emplace_back(traceStruct);
    return BodyManager::pWindow->MapBufferGPU(BodyManager::buffer, BodyManager::bodyList.data(), (BodyManager::bodyList.size() * sizeof(BodyManager::BodyStruct)));
}
bool BodyManager::Delete(unsigned int id)
{
    if (BodyManager::bodyList.empty()) return true;
    size_t size{ BodyManager::bodyList.size() };
    BodyManager::bodyList.erase(std::remove_if(BodyManager::bodyList.begin(), BodyManager::bodyList.end(), [id](_IN_ const BodyManager::BodyStruct& bodyStruct) -> bool
        {
            return (id == bodyStruct.id);
        }),
        BodyManager::bodyList.end());
    BodyManager::traceList.erase(std::remove_if(BodyManager::traceList.begin(), BodyManager::traceList.end(), [id](_IN_ const BodyManager::TraceStruct& traceStruct) -> bool
        {
            return (id == traceStruct.id);
        }),
        BodyManager::traceList.end());
    return ((size == BodyManager::bodyList.size()) || BodyManager::pWindow->MapBufferGPU(BodyManager::buffer, BodyManager::bodyList.data(), (BodyManager::bodyList.size() * sizeof(BodyManager::BodyStruct))));
}
void BodyManager::Clear()
{
    BodyManager::bodyList.clear();
    BodyManager::traceList.clear();
}
bool BodyManager::Scale()
{
    if ((NGlobal::scale_distance_old == NGlobal::scale_distance) || BodyManager::bodyList.empty()) return true;
    float temp{ NGlobal::scale_distance_old / NGlobal::scale_distance };
    for (BodyManager::BodyStruct& bodyStruct : BodyManager::bodyList)
    {
        bodyStruct.radius *= temp;
        bodyStruct.position[0][0] *= temp;
        bodyStruct.position[0][1] *= temp;
        bodyStruct.position[0][2] *= temp;
        bodyStruct.velocity[0][0] *= temp;
        bodyStruct.velocity[0][1] *= temp;
        bodyStruct.velocity[0][2] *= temp;
    }
    for (BodyManager::TraceStruct& traceStruct : BodyManager::traceList)
    {
        traceStruct.vertices.clear();
    }
    return BodyManager::pWindow->MapBufferGPU(BodyManager::buffer, BodyManager::bodyList.data(), (BodyManager::bodyList.size() * sizeof(BodyManager::BodyStruct)));
}

const std::vector<BodyManager::BodyStruct>& BodyManager::Data()
{
    return BodyManager::bodyList;
}

bool BodyManager::RunAcceleration()
{
    if (BodyManager::bodyList.empty()) return true;
    return (BodyManager::pWindow->RunGPU(BodyManager::pAccelerationCS.Get(), static_cast<UINT>(std::ceil(static_cast<float>(BodyManager::bodyList.size()) / NUMTHREADS_X_BODY)), 1U, 1U) &&
        BodyManager::pWindow->WaitGPU());
}
bool BodyManager::RunEuler()
{
    if (BodyManager::bodyList.empty()) return true;
    return (BodyManager::pWindow->RunGPU(BodyManager::pEulerCS.Get(), static_cast<UINT>(std::ceil(static_cast<float>(BodyManager::bodyList.size()) / NUMTHREADS_X_BODY)), 1U, 1U) &&
        BodyManager::pWindow->WaitGPU());
}
bool BodyManager::RunRungeKutta()
{
    if (BodyManager::bodyList.empty()) return true;
    return (BodyManager::pWindow->RunGPU(BodyManager::pRungeKuttaCS.Get(), static_cast<UINT>(std::ceil(static_cast<float>(BodyManager::bodyList.size()) / NUMTHREADS_X_BODY)), 1U, 1U) &&
        BodyManager::pWindow->WaitGPU());
}

bool BodyManager::GetResult()
{
    if (BodyManager::bodyList.empty()) return true;
    return BodyManager::pWindow->GetResultGPU(BodyManager::buffer, BodyManager::bodyList.data(), (BodyManager::bodyList.size() * sizeof(BodyManager::BodyStruct)));
}
bool BodyManager::AddDataInDraw()
{
    BodyManager::bodyVertices.clear();
    BodyManager::bodyIndices.clear();
    BodyManager::traceVertices.clear();
    if (BodyManager::bodyList.empty()) return true;
    for (const BodyManager::BodyStruct& bodyStruct : BodyManager::bodyList)
    {
        bodyStruct.AddSphere(BodyManager::bodyVertices, BodyManager::bodyIndices);
        for (BodyManager::TraceStruct& traceStruct : BodyManager::traceList)
        {
            if (traceStruct.id == bodyStruct.id)
            {
                BodyManager::traceVertex.position.x = bodyStruct.position[0][0];
                BodyManager::traceVertex.position.y = bodyStruct.position[0][1];
                BodyManager::traceVertex.position.z = bodyStruct.position[0][2];
                traceStruct.vertices.emplace_back(BodyManager::traceVertex);
                if (traceStruct.vertices.size() > NConst::trace_maxcount)
                {
                    traceStruct.vertices.pop_front();
                }
                BodyManager::traceVertices.insert(BodyManager::traceVertices.end(), traceStruct.vertices.begin(), traceStruct.vertices.end());
                break;
            }
        }
    }
    return (IWinUI::Object<NConst::body_type>::AddDataInDraw(BodyManager::pWindow, BodyManager::bodyVertices, BodyManager::bodyIndices) &&
        IWinUI::Object<NConst::trace_type>::AddDataInDraw(BodyManager::pWindow, BodyManager::traceVertices));
}

BodyManager::BodyManager(IWinUI* pWindow) : pWindow{ pWindow }, traceVertex{}
{
    BodyManager::traceVertex.color = NConst::trace_color;
}