#include "ParticleManager.h"

ParticleManager::ParticleStruct::ParticleStruct(float px, float py, float pz) : orginalPosition{ px, py, pz }, position{ px, py, pz }, velocity{}
{}

bool ParticleManager::Init(UINT particleMaxCount)
{
    particleMaxCount = static_cast<UINT>(::pow((particleMaxCount * 2U + 1U), 3));
    return (ParticleManager::pWindow->CreateComputeShaderFromFileGPU("particle.hlsl", "main", &(ParticleManager::pParticleCS)) &&
        ParticleManager::pWindow->CreateComputeShaderFromFileGPU("particle_reset.hlsl", "main", &(ParticleManager::pResetCS)) &&
        ParticleManager::pWindow->CreateBufferGPU(static_cast<UINT>(sizeof(ParticleManager::ParticleStruct)), particleMaxCount, ParticleManager::buffer) &&
        ParticleManager::pWindow->BindBufferGPU(ParticleManager::buffer, 1U));
}

void ParticleManager::Change(int count, float distance, float offsetX, float offsetY, float offsetZ)
{
    ParticleManager::particleData.particle_count = count;
    ParticleManager::particleData.particle_distance = distance;
    ParticleManager::particleData.particle_offset_x = offsetX;
    ParticleManager::particleData.particle_offset_y = offsetY;
    ParticleManager::particleData.particle_offset_z = offsetZ;
    std::thread{ [this, count, distance, offsetX, offsetY, offsetZ]() -> void
        {
            std::vector<ParticleManager::ParticleStruct> tempList{};
            if (count)
            {
                for (int x{ -count }; x <= count; ++x)
                {
                    for (int y{ -count }; y <= count; ++y)
                    {
                        for (int z{ -count }; z <= count; ++z)
                        {
                            tempList.emplace_back(ParticleManager::ParticleStruct{ (x * distance + offsetX), (y * distance + offsetY), (z * distance + offsetZ) });
                        }
                    }
                }
            }
            ParticleManager::list.swap(tempList);
            if (ParticleManager::list.size())
            {
                ParticleManager::pWindow->MapBufferGPU(ParticleManager::buffer, ParticleManager::list.data(), (ParticleManager::list.size() * sizeof(ParticleManager::ParticleStruct)));
            }
        } }.detach();
}
const ParticleManager::ParticleData& ParticleManager::Data()
{
    return  ParticleManager::particleData;
}

bool ParticleManager::RunParticle()
{
    if (ParticleManager::list.empty()) return true;
    return (ParticleManager::pWindow->RunGPU(ParticleManager::pParticleCS.Get(), static_cast<UINT>(std::ceil(static_cast<float>(ParticleManager::list.size()) / NUMTHREADS_X_PARTICLE)), 1U, 1U) &&
        ParticleManager::pWindow->WaitGPU());
}
bool ParticleManager::RunReset(size_t bodyCount)
{
    if (ParticleManager::list.empty() || !bodyCount) return true;
    return (ParticleManager::pWindow->RunGPU(ParticleManager::pResetCS.Get(),
        static_cast<UINT>(std::ceil(static_cast<float>(ParticleManager::list.size()) / NUMTHREADS_X_PARTICLE)),
        static_cast<UINT>(std::ceil(static_cast<float>(bodyCount) / NUMTHREADS_Y_PARTICLE)), 1U) &&
        ParticleManager::pWindow->WaitGPU());
}

bool ParticleManager::GetResult()
{
    if (ParticleManager::list.empty()) return true;
    return ParticleManager::pWindow->GetResultGPU(ParticleManager::buffer, ParticleManager::list.data(), (ParticleManager::list.size() * sizeof(ParticleManager::ParticleStruct)));
}
bool ParticleManager::AddDataInDraw()
{
    ParticleManager::vertices.clear();
    if (ParticleManager::list.empty()) return true;
    for (const ParticleManager::ParticleStruct& particleStruct : ParticleManager::list)
    {
        ParticleManager::tempVertex.position.x = particleStruct.position[0];
        ParticleManager::tempVertex.position.y = particleStruct.position[1];
        ParticleManager::tempVertex.position.z = particleStruct.position[2];
        ParticleManager::vertices.emplace_back(ParticleManager::tempVertex);
    }
    return IWinUI::Object<NConst::particle_type>::AddDataInDraw(ParticleManager::pWindow, ParticleManager::vertices);
}

ParticleManager::ParticleManager(IWinUI* pWindow) : pWindow{ pWindow }, tempVertex{},
    particleData{ NConst::particle_init_count, NConst::particle_init_distance, NConst::particle_init_offset_x, NConst::particle_init_offset_y, NConst::particle_init_offset_z }
{
    ParticleManager::tempVertex.color = NConst::particle_color;
}