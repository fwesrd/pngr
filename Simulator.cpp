#include "Simulator.h"

bool Simulator::Init(UINT bodyMaxCount, UINT particleMaxCount)
{
    return (Simulator::bodyManager.Init(bodyMaxCount) && Simulator::particleManager.Init(particleMaxCount) && Simulator::rk4Manager.Init() && Simulator::kerrManager.Init());
}
bool Simulator::Compute(double second)
{
    Simulator::rk4Manager.rk4.body_count = static_cast<unsigned int>(Simulator::bodyManager.bodyList.size());
    Simulator::rk4Manager.rk4.particle_count = static_cast<unsigned int>(Simulator::particleManager.list.size());
    Simulator::rk4Manager.rk4.scale_distance = NGlobal::scale_distance;
    Simulator::rk4Manager.rk4.scale_distance_3 = (NGlobal::scale_distance * NGlobal::scale_distance * NGlobal::scale_distance);
    float particle_distance{ Simulator::particleManager.particleData.particle_distance };
    Simulator::rk4Manager.rk4.particle_space_2 = (particle_distance * particle_distance * 3.F);
    if (!Simulator::rk4Manager.MapBuffer()) return false;
    //repret compute
    if (second <= 0.0) return true;
    TESTA("second");
    TESTA(second);
    long long repret{ static_cast<long long>(std::floor(second / NGlobal::time_calculate_step)) };
    if (repret < 0ULL) repret = 0ULL;
    for (long long i{ 0LL }; i < repret; ++i)
    {
        if (!Simulator::RungeKuttaIntegral(NGlobal::time_calculate_step)) return false;
    }
    if (!Simulator::RungeKuttaIntegral(static_cast<float>(second - repret * NGlobal::time_calculate_step))) return false;
    //particle reset
    return Simulator::particleManager.RunReset(Simulator::bodyManager.bodyList.size());
}
bool Simulator::GetResult()
{
    return (Simulator::bodyManager.GetResult() && Simulator::particleManager.GetResult());
}
bool Simulator::AddDataInDraw()
{
    return (Simulator::bodyManager.AddDataInDraw() && Simulator::particleManager.AddDataInDraw() && Simulator::kerrManager.AddDataInDraw());
}

bool Simulator::AddBody(_IN_ BodyManager::BodyStruct& bodyStruct)
{
    return Simulator::bodyManager.Add(bodyStruct);
}
bool Simulator::DeleteBody(unsigned int id)
{
    return Simulator::bodyManager.Delete(id);
}
void Simulator::ClearBody()
{
    Simulator::bodyManager.Clear();
}
bool Simulator::ScaleBody()
{
    return Simulator::bodyManager.Scale();
}
const std::vector<BodyManager::BodyStruct>& Simulator::BodiesData()
{
    return Simulator::bodyManager.Data();
}

void Simulator::ChangeParticle(int count, float distance, float offsetX, float offsetY, float offsetZ)
{
    Simulator::particleManager.Change(count, distance, offsetX, offsetY, offsetZ);
}
const ParticleManager::ParticleData& Simulator::ParticleData()
{
    return Simulator::particleManager.Data();
}

bool Simulator::ChangeKerr(bool has, double mass, float a_rate, float x, float y, float z, bool disk, float speed)
{
    return Simulator::kerrManager.Change(has, mass, a_rate, x, y, z, disk, speed);
}
const KerrManager::KerrStruct& Simulator::KerrData()
{
    return Simulator::kerrManager.Data();
}
bool Simulator::KerrHas()
{
    return Simulator::kerrManager.Has();
}
bool Simulator::KerrDisk()
{
    return Simulator::kerrManager.Disk();
}

bool Simulator::RungeKuttaIntegral(float step)
{
    if (step <= 0.F) return true;
    {
        //K1
        Simulator::rk4Manager.rk4.n = 0U;
        Simulator::rk4Manager.rk4.k = 1U;
        if (!Simulator::rk4Manager.MapBuffer() || !Simulator::bodyManager.RunAcceleration()) return false;
        //K2
        Simulator::rk4Manager.rk4.n = 1U;
        Simulator::rk4Manager.rk4.step = (0.5F * step);
        if (!Simulator::rk4Manager.MapBuffer() || !Simulator::bodyManager.RunEuler()) return false;
        Simulator::rk4Manager.rk4.k = 2U;
        if (!Simulator::rk4Manager.MapBuffer() || !Simulator::bodyManager.RunAcceleration()) return false;
        //K3
        if (!Simulator::bodyManager.RunEuler()) return false;
        Simulator::rk4Manager.rk4.k = 3U;
        Simulator::rk4Manager.rk4.step = step;
        if (!Simulator::rk4Manager.MapBuffer() || !Simulator::bodyManager.RunAcceleration()) return false;
        //K4
        if (!Simulator::bodyManager.RunEuler()) return false;
        Simulator::rk4Manager.rk4.k = 4U;
        if (!Simulator::rk4Manager.MapBuffer() || !Simulator::bodyManager.RunAcceleration()) return false;
        //K*
        if (!Simulator::bodyManager.RunRungeKutta()) return false;
    }
    //particle
    if (!Simulator::particleManager.RunParticle()) return false;
    //body
    Simulator::rk4Manager.rk4.k = 0U;
    Simulator::rk4Manager.rk4.n = 0U;
    Simulator::rk4Manager.rk4.step = step;
    return (Simulator::rk4Manager.MapBuffer() && Simulator::bodyManager.RunEuler());
}

Simulator::Simulator(IWinUI* pWindow) : pWindow{ pWindow }, bodyManager{ pWindow }, particleManager{ pWindow }, rk4Manager{ pWindow }, kerrManager{ pWindow }
{}