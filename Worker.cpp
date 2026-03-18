#include "Worker.h"

bool Worker::Init()
{
    return (Worker::simulator.Init(NConst::bodyMaxCount, NConst::particleMaxCount) && Worker::light.Init() && Worker::sky.Init());
}
void Worker::CalculateAndAddDataInDraw(std::chrono::steady_clock::time_point lastFrameTime, std::chrono::steady_clock::time_point nextFrameTime, UINT width, UINT height)
{
    Worker::mutex.lock();
    {
        if (Worker::run)
        {
            if (Worker::simulator.Compute(std::chrono::duration_cast<std::chrono::microseconds>(nextFrameTime - lastFrameTime).count() / 1000000.F * NGlobal::scale_time))
            {
                Worker::simulator.GetResult();
            }
        }
        if (Worker::follow)
        {
            Worker::follow = false;
            const std::vector<BodyManager::BodyStruct>& bodies{ Worker::simulator.BodiesData() };
            for (const BodyManager::BodyStruct& body : bodies)
            {
                if (body.id == Worker::followId)
                {
                    Worker::follow = true;
                    Worker::pWindow->GetCamera().SetPosition(IWinUI::Position{ body.position[0][0], (body.position[0][1] + body.radius + 2.F * EPS), body.position[0][2] });
                    break;
                }
            }
        }
        if (Worker::focusBody)
        {
            Worker::focusBody = false;
            const std::vector<BodyManager::BodyStruct>& bodies{ Worker::simulator.BodiesData() };
            for (const BodyManager::BodyStruct& body : bodies)
            {
                if (body.id == Worker::focusBodyId)
                {
                    Worker::focusBody = true;
                    IWinUI::Position position{ Worker::pWindow->GetCamera().GetPosition() };
                    position.x = (body.position[0][0] - position.x);
                    position.y = (body.position[0][1] - position.y);
                    position.z = (body.position[0][2] - position.z);
                    Worker::pWindow->GetCamera().SetFocus(
                        DirectX::XMConvertToDegrees(::atan2(position.z, position.x)),
                        DirectX::XMConvertToDegrees(::atan2(position.y, ::sqrt(position.x * position.x + position.z * position.z))));
                    break;
                }
            }
        }
        else if (Worker::focusKerr)
        {
            if (Worker::simulator.KerrHas())
            {
                KerrManager::KerrStruct kerr{ Worker::simulator.KerrData() };
                IWinUI::Position position{ Worker::pWindow->GetCamera().GetPosition() };
                position.x = (kerr.kerr_position[0] - position.x);
                position.y = (kerr.kerr_position[1] - position.y);
                position.z = (kerr.kerr_position[2] - position.z);
                Worker::pWindow->GetCamera().SetFocus(
                    DirectX::XMConvertToDegrees(::atan2(position.z, position.x)),
                    DirectX::XMConvertToDegrees(::atan2(position.y, ::sqrt(position.x * position.x + position.z * position.z))));
            }
            else
            {
                Worker::focusKerr = false;
            }
        }
        if (Worker::light.GetIs())
        {
            Worker::light.Compute(width, height);
        }
        else
        {
            Worker::simulator.AddDataInDraw();
        }
    }
    Worker::mutex.unlock();
}
void Worker::LightDraw()
{
    Worker::mutex.lock();
    Worker::light.Draw();
    Worker::mutex.unlock();
}
void Worker::WmSize(UINT width, UINT height)
{
    Worker::mutex.lock();
    Worker::light.UpdataLightTexture(width, height);
    Worker::mutex.unlock();
}

void Worker::Stop()
{
    Worker::run = false;
}
void Worker::Run()
{
    Worker::run = true;
}

bool Worker::BodyAdd(_IN_ BodyManager::BodyStruct& bodyStruct)
{
    Worker::mutex.lock();
    bool result{ Worker::simulator.AddBody(bodyStruct) };
    Worker::mutex.unlock();
    return result;
}
bool Worker::BodyDelete(unsigned int id)
{
    Worker::mutex.lock();
    bool result{ Worker::simulator.DeleteBody(id) };
    Worker::mutex.unlock();
    return result;
}
void Worker::BodyClear()
{
    Worker::mutex.lock();
    Worker::simulator.ClearBody();
    Worker::mutex.unlock();
}
const std::vector<BodyManager::BodyStruct> Worker::BodiesData()
{
    Worker::mutex.lock();
    const std::vector<BodyManager::BodyStruct> result{ Worker::simulator.BodiesData() };
    Worker::mutex.unlock();
    return result;
}

void Worker::ParticleChange(int count, float distance, float offsetX, float offsetY, float offsetZ)
{
    Worker::mutex.lock();
    Worker::simulator.ChangeParticle(count, distance, offsetX, offsetY, offsetZ);
    Worker::mutex.unlock();
}
const ParticleManager::ParticleData Worker::ParticleData()
{
    Worker::mutex.lock();
    const ParticleManager::ParticleData result{ Worker::simulator.ParticleData() };
    Worker::mutex.unlock();
    return result;
}

bool Worker::KerrChange(bool has, double mass, float a_rate, float x, float y, float z, bool disk, float speed)
{
    Worker::mutex.lock();
    bool result{ Worker::simulator.ChangeKerr(has, mass, a_rate, x, y, z, disk, speed) };
    Worker::mutex.unlock();
    return result;
}
const KerrManager::KerrStruct Worker::KerrData()
{
    Worker::mutex.lock();
    const KerrManager::KerrStruct result{ Worker::simulator.KerrData() };
    Worker::mutex.unlock();
    return result;
}
bool Worker::KerrHas()
{
    Worker::mutex.lock();
    bool result{ Worker::simulator.KerrHas() };
    Worker::mutex.unlock();
    return result;
}
bool Worker::KerrDisk()
{
    Worker::mutex.lock();
    bool result{ Worker::simulator.KerrDisk() };
    Worker::mutex.unlock();
    return result;
}

void Worker::LightChange(unsigned int step_count, float step_min, float step_max)
{
    Worker::mutex.lock();
    Worker::light.Change(step_count, step_min, step_max);
    Worker::mutex.unlock();
}
void Worker::LightSetIs(bool is)
{
    Worker::mutex.lock();
    Worker::light.SetIs(is);
    Worker::mutex.unlock();
}
const LightTrace::LightStruct Worker::LightData()
{
    Worker::mutex.lock();
    const LightTrace::LightStruct result{ Worker::light.Data() };
    Worker::mutex.unlock();
    return result;
}
bool Worker::LightGetIs()
{
    Worker::mutex.lock();
    bool result{ Worker::light.GetIs() };
    Worker::mutex.unlock();
    return result;
}

bool Worker::SkyChange(bool has, std::string filePath, int density)
{
    Worker::mutex.lock();
    bool result{ Worker::sky.Change(has, filePath, static_cast<int>(density), NConst::sky_width, NConst::sky_height) };
    Worker::mutex.unlock();
    return result;
}
bool Worker::SkyHas()
{
    Worker::mutex.lock();
    bool result{ Worker::sky.Has() };
    Worker::mutex.unlock();
    return result;
}
std::string Worker::SkyPath()
{
    Worker::mutex.lock();
    std::string result{ Worker::sky.Path() };
    Worker::mutex.unlock();
    return result;
}
int Worker::SkyDensity()
{
    Worker::mutex.lock();
    int result{ static_cast<int>(Worker::sky.Density()) };
    Worker::mutex.unlock();
    return result;
}

void Worker::ChangeTimeRate(float rate)
{
    Worker::mutex.lock();
    if (NGlobal::time_calculate_rate != rate)
    {
        NGlobal::time_calculate_rate = rate;
        NGlobal::time_calculate_step = (NGlobal::time_calculate_rate * NGlobal::scale_time);
    }
    Worker::mutex.unlock();
}
void Worker::ChangeScaleTime(float scale_time)
{
    Worker::mutex.lock();
    if (NGlobal::scale_time != scale_time)
    {
        NGlobal::scale_time = scale_time;
        NGlobal::time_calculate_step = (NGlobal::time_calculate_rate * NGlobal::scale_time);
    }
    Worker::mutex.unlock();
}
bool Worker::ChangeScaleDistance(float scale_distance)
{
    Worker::mutex.lock();
    if (NGlobal::scale_distance == scale_distance)
    {
        Worker::mutex.unlock();
        return true;
    }
    NGlobal::scale_distance_old = NGlobal::scale_distance;
    NGlobal::scale_distance = scale_distance;
    const KerrManager::KerrStruct& kerr{ Worker::simulator.KerrData() };
    bool result{ Worker::simulator.ScaleBody() && (!Worker::simulator.KerrHas() ||
        Worker::simulator.ChangeKerr(Worker::simulator.KerrHas(), kerr.kerr_mass, kerr.kerr_a_rate,
            (kerr.kerr_position[0] * NGlobal::scale_distance_old),
            (kerr.kerr_position[1] * NGlobal::scale_distance_old),
            (kerr.kerr_position[2] * NGlobal::scale_distance_old),
            Worker::simulator.KerrDisk(), kerr.disk_speed)) };
    Worker::mutex.unlock();
    return result;
}

void Worker::Fov(float fov)
{
    Worker::mutex.lock();
    Worker::pWindow->GetCamera().SetFov(fov);
    Worker::mutex.unlock();
}
bool Worker::Follow(bool follow, unsigned int id)
{
    Worker::mutex.lock();
    Worker::follow = false;
    if (!follow)
    {
        Worker::mutex.unlock();
        return true;
    }
    const std::vector<BodyManager::BodyStruct>& bodies{ Worker::simulator.BodiesData() };
    for (const BodyManager::BodyStruct& body : bodies)
    {
        if (body.id == id)
        {
            Worker::follow = true;
            Worker::followId = id;
            Worker::mutex.unlock();
            return true;
        }
    }
    Worker::mutex.unlock();
    return false;
}
bool Worker::FocusBody(unsigned int id)
{
    Worker::mutex.lock();
    const std::vector<BodyManager::BodyStruct>& bodies{ Worker::simulator.BodiesData() };
    for (const BodyManager::BodyStruct& body : bodies)
    {
        if (body.id == id)
        {
            Worker::focusBody = true;
            Worker::focusKerr = false;
            Worker::focusBodyId = id;
            Worker::mutex.unlock();
            return true;
        }
    }
    Worker::focusBody = false;
    Worker::mutex.unlock();
    return false;
}
bool Worker::FocusKerr()
{
    Worker::mutex.lock();
    if (!Worker::simulator.KerrHas())
    {
        Worker::focusKerr = false;
        Worker::mutex.unlock();
        return false;
    }
    Worker::focusKerr = true;
    Worker::focusBody = false;
    Worker::mutex.unlock();
    return true;
}
void Worker::UnFocus()
{
    Worker::mutex.lock();
    Worker::focusBody = false;
    Worker::focusKerr = false;
    Worker::mutex.unlock();
}

Worker::Worker(IWinUI* pWindow) :
    pWindow{ pWindow }, simulator{ pWindow }, light{ pWindow }, sky{ pWindow }, run{ true },
    follow{ false }, followId{}, focusBody{ false }, focusBodyId{}, focusKerr{ false }
{}