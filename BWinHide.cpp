#include "BWinHide.h"

namespace NDebug
{
	extern bool Assert(bool expression, std::string text);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// WinType
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

WinType::Vertex::Vertex(WinType::Position position, WinType::Color color) : position{ position }, color{ color }
{}

BYTE WinType::Pixel::operator[](int x)
{
	return WinType::Pixel::rgba[x];
}
WinType::Pixel::Pixel(BYTE r, BYTE g, BYTE b, BYTE a) : rgba{ r, g, b, a }
{}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Synchronizer
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

bool BWinHide::Synchronizer::Start(double fpsMax)
{
	if (!(BWinHide::Synchronizer::pWindow)) return false;
	BWinHide::Synchronizer::End();
	BWinHide::Synchronizer::thread = std::move(std::thread{ [this, fpsMax]() -> void
		{
			BWinHide::Synchronizer::running.store(true);
			double microSecond{ 1000000.0 / fpsMax };
			long long usedMicroSecond{ static_cast<long long>(std::ceil(microSecond)) };
			long long duration{};
			std::chrono::steady_clock::time_point currentTime{ std::chrono::steady_clock::now() };
			std::chrono::steady_clock::time_point frameTime{ currentTime };
			std::chrono::steady_clock::time_point lastFrameTime{ frameTime };
			std::chrono::steady_clock::time_point lastPresentTime{ currentTime };
			std::chrono::steady_clock::time_point temp{};
			while (BWinHide::Synchronizer::running.load())
			{
				frameTime += std::chrono::microseconds(usedMicroSecond);
				BWinHide::Synchronizer::synchronCalculateLauncher(BWinHide::Synchronizer::pWindow, lastFrameTime, frameTime);
				lastFrameTime = frameTime;
				temp = std::chrono::steady_clock::now();
				usedMicroSecond = static_cast<long long>(std::ceil(std::ceil(static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(temp - currentTime).count()) / microSecond) * microSecond));
				TESTA("used");
				TESTA(usedMicroSecond);
				duration = std::chrono::duration_cast<std::chrono::microseconds>(temp - frameTime).count();
				if (duration >= 0LL)
				{
					frameTime += std::chrono::microseconds(static_cast<long long>(std::ceil(static_cast<double>(duration) / microSecond) * microSecond));
				}
				else
				{
					std::this_thread::sleep_until(frameTime);
				}
				currentTime = std::chrono::steady_clock::now();
				BWinHide::Synchronizer::synchronDrawLauncher(BWinHide::Synchronizer::pWindow, lastPresentTime, currentTime);
				lastPresentTime = currentTime;
			}
		} });
	return true;
}
void BWinHide::Synchronizer::End()
{
	if (!(BWinHide::Synchronizer::thread.joinable())) return;
	BWinHide::Synchronizer::running.store(false);
	BWinHide::Synchronizer::thread.join();
}

BWinHide::Synchronizer::Synchronizer(IWinUI* pWindow,
	std::function<void(IWinUI*, std::chrono::steady_clock::time_point&, std::chrono::steady_clock::time_point&)> synchronCalculateLauncher,
	std::function<void(IWinUI*, std::chrono::steady_clock::time_point&, std::chrono::steady_clock::time_point&)> synchronDrawLauncher) :
	pWindow{ pWindow },
	synchronCalculateLauncher{ synchronCalculateLauncher },
	synchronDrawLauncher{ synchronDrawLauncher }
{}
BWinHide::Synchronizer::~Synchronizer()
{
	BWinHide::Synchronizer::End();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Mouse
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

int BWinHide::Mouse::TurnX(LPARAM lParam)
{
	return GET_X_LPARAM(lParam);
}
int BWinHide::Mouse::TurnY(LPARAM lParam)
{
	return GET_Y_LPARAM(lParam);
}

int BWinHide::Mouse::GetX()
{
	return BWinHide::Mouse::nowX;
}
int BWinHide::Mouse::GetY()
{
	return BWinHide::Mouse::nowY;
}
bool BWinHide::Mouse::Wheel(UINT uMsg, WPARAM wParam, _OUT_ short& value)
{
	if (uMsg != WM_MOUSEWHEEL) return false;
	value = (GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA);
	return true;
}
void BWinHide::Mouse::StartCapture(LPARAM lParam)
{
	if (BWinHide::Mouse::isCapture) return;
	BWinHide::Mouse::originX = BWinHide::Mouse::lastX = BWinHide::Mouse::nowX = GET_X_LPARAM(lParam);
	BWinHide::Mouse::originY = BWinHide::Mouse::lastY = BWinHide::Mouse::nowY = GET_Y_LPARAM(lParam);
	BWinHide::Mouse::isCapture = true;
}
bool BWinHide::Mouse::Capturing(LPARAM lParam, _OUT_OPT_ int* pDeltaX, _OUT_OPT_ int* pDeltaY)
{
	if (!(BWinHide::Mouse::isCapture)) return false;
	BWinHide::Mouse::nowX = GET_X_LPARAM(lParam);
	BWinHide::Mouse::nowY = GET_Y_LPARAM(lParam);
	if (pDeltaX) *pDeltaX = (BWinHide::Mouse::nowX - BWinHide::Mouse::lastX);
	if (pDeltaY) *pDeltaY = (BWinHide::Mouse::nowY - BWinHide::Mouse::lastY);
	BWinHide::Mouse::lastX = BWinHide::Mouse::nowX;
	BWinHide::Mouse::lastY = BWinHide::Mouse::nowY;
	return true;
}
bool BWinHide::Mouse::EndCapture(LPARAM lParam, _OUT_OPT_ int* pDeltaX, _OUT_OPT_ int* pDeltaY)
{
	if (!(BWinHide::Mouse::isCapture)) return false;
	BWinHide::Mouse::isCapture = false;
	BWinHide::Mouse::nowX = GET_X_LPARAM(lParam);
	BWinHide::Mouse::nowY = GET_Y_LPARAM(lParam);
	if (pDeltaX) *pDeltaX = (BWinHide::Mouse::nowX - BWinHide::Mouse::originX);
	if (pDeltaY) *pDeltaY = (BWinHide::Mouse::nowY - BWinHide::Mouse::originY);
	BWinHide::Mouse::originX = BWinHide::Mouse::lastX = BWinHide::Mouse::nowX;
	BWinHide::Mouse::originY = BWinHide::Mouse::lastY = BWinHide::Mouse::nowY;
	return true;
}
bool BWinHide::Mouse::IsCapture()
{
	return BWinHide::Mouse::isCapture;
}
BWinHide::Mouse::Mouse() : originX{}, originY{}, lastX{}, lastY{}, nowX{}, nowY{}, isCapture{ false }
{}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Camera
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

WinType::Position BWinHide::Camera::GetPosition()
{
	BWinHide::Camera::mutex.lock();
	WinType::Position result{ BWinHide::Camera::eyePosition };
	BWinHide::Camera::mutex.unlock();
	return result;
}
float BWinHide::Camera::GetYaw()
{
	BWinHide::Camera::mutex.lock();
	float result{ BWinHide::Camera::yaw };
	BWinHide::Camera::mutex.unlock();
	return result;
}
float BWinHide::Camera::GetPitch()
{
	BWinHide::Camera::mutex.lock();
	float result{ BWinHide::Camera::pitch };
	BWinHide::Camera::mutex.unlock();
	return result;
}
float BWinHide::Camera::GetFovAngleY()
{
	BWinHide::Camera::mutex.lock();
	float result{ BWinHide::Camera::fovAngleY };
	BWinHide::Camera::mutex.unlock();
	return result;
}
WinType::Vector BWinHide::Camera::GetFocusDirection()
{
	BWinHide::Camera::mutex.lock();
	WinType::Vector result{ BWinHide::Camera::focusDirection };
	BWinHide::Camera::mutex.unlock();
	return result;
}
WinType::Vector BWinHide::Camera::GetLeftDirection()
{
	BWinHide::Camera::mutex.lock();
	WinType::Vector result{ BWinHide::Camera::leftDirection };
	BWinHide::Camera::mutex.unlock();
	return result;
}
WinType::Vector BWinHide::Camera::GetHeadDirection()
{
	BWinHide::Camera::mutex.lock();
	WinType::Vector result{ BWinHide::Camera::headDirection };
	BWinHide::Camera::mutex.unlock();
	return result;
}
float BWinHide::Camera::GetMoveSpeed()
{
	return BWinHide::Camera::moveSpeed;
}
float BWinHide::Camera::GetSpinSpeed()
{
	return BWinHide::Camera::spinSpeed;
}

void BWinHide::Camera::SetPosition(WinType::Position eyePosition)
{
	BWinHide::Camera::mutex.lock();
	BWinHide::Camera::eyePosition = eyePosition;
	BWinHide::Camera::mutex.unlock();
}
void BWinHide::Camera::SetFocus(float yaw, float pitch)
{
	BWinHide::Camera::mutex.lock();
	yaw = ::fmodf(yaw, 360.F);
	if (yaw < 0.F) yaw += 360.F;
	if (pitch > 89.F) pitch = 89.F;
	if (pitch < -89.F) pitch = -89.F;
	BWinHide::Camera::yaw = yaw;
	BWinHide::Camera::pitch = pitch;
	yaw = DirectX::XMConvertToRadians(yaw);
	pitch = DirectX::XMConvertToRadians(pitch);
	float sinYaw{ ::sinf(yaw) };
	float cosYaw{ ::cosf(yaw) };
	float sinPitch{ ::sinf(pitch) };
	float cosPitch{ ::cosf(pitch) };
	//forward
	BWinHide::Camera::focusDirection.x = (cosPitch * cosYaw);
	BWinHide::Camera::focusDirection.y = sinPitch;
	BWinHide::Camera::focusDirection.z = (cosPitch * sinYaw);
	//left
	BWinHide::Camera::leftDirection.x = -sinYaw;
	BWinHide::Camera::leftDirection.y = 0.F;
	BWinHide::Camera::leftDirection.z = cosYaw;
	//head
	BWinHide::Camera::headDirection.x = (-sinPitch * cosYaw);
	BWinHide::Camera::headDirection.y = cosPitch;
	BWinHide::Camera::headDirection.z = (-sinPitch * sinYaw);
	BWinHide::Camera::mutex.unlock();
}
void BWinHide::Camera::SetFov(float fovAngleY)
{
	BWinHide::Camera::mutex.lock();
	if (fovAngleY > 179.F) fovAngleY = 179.F;
	if (fovAngleY < 1.F) fovAngleY = 1.F;
	BWinHide::Camera::fovAngleY = fovAngleY;
	BWinHide::Camera::mutex.unlock();
}
void BWinHide::Camera::SetMoveSpeed(float moveSpeed)
{
	BWinHide::Camera::moveSpeed = moveSpeed;
}
void BWinHide::Camera::SetSpinSpeed(float spinSpeed)
{
	BWinHide::Camera::spinSpeed = spinSpeed;
}

void BWinHide::Camera::Forward()
{
	BWinHide::Camera::mutex.lock();
	BWinHide::Camera::eyePosition.x += (BWinHide::Camera::focusDirection.x * BWinHide::Camera::moveSpeed);
	BWinHide::Camera::eyePosition.y += (BWinHide::Camera::focusDirection.y * BWinHide::Camera::moveSpeed);
	BWinHide::Camera::eyePosition.z += (BWinHide::Camera::focusDirection.z * BWinHide::Camera::moveSpeed);
	BWinHide::Camera::mutex.unlock();
}
void BWinHide::Camera::Backward()
{
	BWinHide::Camera::mutex.lock();
	BWinHide::Camera::eyePosition.x -= (BWinHide::Camera::focusDirection.x * BWinHide::Camera::moveSpeed);
	BWinHide::Camera::eyePosition.y -= (BWinHide::Camera::focusDirection.y * BWinHide::Camera::moveSpeed);
	BWinHide::Camera::eyePosition.z -= (BWinHide::Camera::focusDirection.z * BWinHide::Camera::moveSpeed);
	BWinHide::Camera::mutex.unlock();
}
void BWinHide::Camera::Left()
{
	BWinHide::Camera::mutex.lock();
	BWinHide::Camera::eyePosition.x += (BWinHide::Camera::leftDirection.x * BWinHide::Camera::moveSpeed);
	BWinHide::Camera::eyePosition.y += (BWinHide::Camera::leftDirection.y * BWinHide::Camera::moveSpeed);
	BWinHide::Camera::eyePosition.z += (BWinHide::Camera::leftDirection.z * BWinHide::Camera::moveSpeed);
	BWinHide::Camera::mutex.unlock();
}
void BWinHide::Camera::Right()
{
	BWinHide::Camera::mutex.lock();
	BWinHide::Camera::eyePosition.x -= (BWinHide::Camera::leftDirection.x * BWinHide::Camera::moveSpeed);
	BWinHide::Camera::eyePosition.y -= (BWinHide::Camera::leftDirection.y * BWinHide::Camera::moveSpeed);
	BWinHide::Camera::eyePosition.z -= (BWinHide::Camera::leftDirection.z * BWinHide::Camera::moveSpeed);
	BWinHide::Camera::mutex.unlock();
}
void BWinHide::Camera::Up()
{
	BWinHide::Camera::mutex.lock();
	BWinHide::Camera::eyePosition.x += (BWinHide::Camera::upDirection.x * BWinHide::Camera::moveSpeed);
	BWinHide::Camera::eyePosition.y += (BWinHide::Camera::upDirection.y * BWinHide::Camera::moveSpeed);
	BWinHide::Camera::eyePosition.z += (BWinHide::Camera::upDirection.z * BWinHide::Camera::moveSpeed);
	BWinHide::Camera::mutex.unlock();
}
void BWinHide::Camera::Down()
{
	BWinHide::Camera::mutex.lock();
	BWinHide::Camera::eyePosition.x -= (BWinHide::Camera::upDirection.x * BWinHide::Camera::moveSpeed);
	BWinHide::Camera::eyePosition.y -= (BWinHide::Camera::upDirection.y * BWinHide::Camera::moveSpeed);
	BWinHide::Camera::eyePosition.z -= (BWinHide::Camera::upDirection.z * BWinHide::Camera::moveSpeed);
	BWinHide::Camera::mutex.unlock();
}

void BWinHide::Camera::SpinLeft()
{
	BWinHide::Camera::SetFocus(BWinHide::Camera::yaw + BWinHide::Camera::spinSpeed, BWinHide::Camera::pitch);
}
void BWinHide::Camera::SpinRight()
{
	BWinHide::Camera::SetFocus(BWinHide::Camera::yaw - BWinHide::Camera::spinSpeed, BWinHide::Camera::pitch);
}
void BWinHide::Camera::SpinUp()
{
	BWinHide::Camera::SetFocus(BWinHide::Camera::yaw, BWinHide::Camera::pitch + BWinHide::Camera::spinSpeed);
}
void BWinHide::Camera::SpinDown()
{
	BWinHide::Camera::SetFocus(BWinHide::Camera::yaw, BWinHide::Camera::pitch - BWinHide::Camera::spinSpeed);
}
BWinHide::Camera::Camera(WinType::Position eyePosition, float yaw, float pitch, float moveSpeed, float spinSpeed, float fovAngleY, float nearZ, float farZ) :
	eyePosition{ eyePosition },
	yaw{ yaw }, pitch{ pitch },
	moveSpeed{ moveSpeed }, spinSpeed{ spinSpeed },
	fovAngleY{ fovAngleY }, nearZ{ nearZ }, farZ{ farZ },
	focusDirection{ WinType::Vector{} }, leftDirection{ WinType::Vector{} }, headDirection{ WinType::Vector{} },
	upDirection{ WinType::Vector{ 0.F, 1.F, 0.F } }
{
	BWinHide::Camera::SetFocus(yaw, pitch);
	BWinHide::Camera::SetFov(fovAngleY);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// PointsData
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void BWinHide::PointsData::Clear()
{
	BWinHide::PointsData::vertices.clear();
}
bool BWinHide::PointsData::Add(_IN_ const std::vector<WinType::Vertex>& vertices)
{
	if (vertices.empty()) return true;
	if (!NDebug::Assert(((BWinHide::PointsData::vertices.size() + vertices.size()) <= static_cast<size_t>(BWinHide::PointsData::maxVertexCount)),
		__FUNCTION__ ": vertex count > maxVertexCount")) return false;
	BWinHide::PointsData::vertices.insert(BWinHide::PointsData::vertices.end(), vertices.begin(), vertices.end());
	return true;
}
BWinHide::PointsData::PointsData(UINT maxVertexCount) : maxVertexCount{ maxVertexCount ? maxVertexCount : 1U }
{}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// LinesData
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void BWinHide::LinesData::Clear()
{
	BWinHide::LinesData::vertices.clear();
	BWinHide::LinesData::indices.clear();
}
bool BWinHide::LinesData::Add(_IN_ const std::vector<WinType::Vertex>& vertices, _IN_ const std::vector<WinType::Index>& indices)
{
	if (vertices.empty() || indices.empty()) return true;
	if (!NDebug::Assert(((BWinHide::LinesData::vertices.size() + vertices.size()) <= static_cast<size_t>(BWinHide::LinesData::maxVertexCount)),
		__FUNCTION__ ": vertex count > maxVertexCount") ||
		!NDebug::Assert(((BWinHide::LinesData::indices.size() + indices.size()) <= static_cast<size_t>(BWinHide::LinesData::maxIndexCount)),
			__FUNCTION__ ": index count > maxIndexCount")) return false;
	WinType::Index offset{ static_cast<WinType::Index>(BWinHide::LinesData::vertices.size()) };
	BWinHide::LinesData::vertices.insert(BWinHide::LinesData::vertices.end(), vertices.begin(), vertices.end());
	for (const auto& iterator : indices)
	{
		BWinHide::LinesData::indices.emplace_back(iterator + offset);
	}
	return true;
}
BWinHide::LinesData::LinesData(UINT maxVertexCount, UINT maxIndexCount) :
	maxVertexCount{ maxVertexCount ? maxVertexCount : 1U },
	maxIndexCount{ maxIndexCount ? maxIndexCount : 1U }
{}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// TrianglesData
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void BWinHide::TrianglesData::Clear()
{
	BWinHide::TrianglesData::vertices.clear();
	BWinHide::TrianglesData::indices.clear();
}
bool BWinHide::TrianglesData::Add(_IN_ const std::vector<WinType::Vertex>& vertices, _IN_ const std::vector<WinType::Index>& indices)
{
	if (vertices.empty() || indices.empty()) return true;
	if (!NDebug::Assert(((BWinHide::TrianglesData::vertices.size() + vertices.size()) <= static_cast<size_t>(BWinHide::TrianglesData::maxVertexCount)),
		__FUNCTION__ ": vertex count > maxVertexCount") ||
		!NDebug::Assert(((BWinHide::TrianglesData::indices.size() + indices.size()) <= static_cast<size_t>(BWinHide::TrianglesData::maxIndexCount)),
			__FUNCTION__ ": index count > maxIndexCount")) return false;
	WinType::Index offset{ static_cast<WinType::Index>(BWinHide::TrianglesData::vertices.size()) };
	BWinHide::TrianglesData::vertices.insert(BWinHide::TrianglesData::vertices.end(), vertices.begin(), vertices.end());
	for (const auto& iterator : indices)
	{
		BWinHide::TrianglesData::indices.emplace_back(iterator + offset);
	}
	return true;
}
BWinHide::TrianglesData::TrianglesData(UINT maxVertexCount, UINT maxIndexCount) :
	maxVertexCount{ maxVertexCount ? maxVertexCount : 1U },
	maxIndexCount{ maxIndexCount ? maxIndexCount : 1U }
{}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// structure
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

BWinHide::BWinHide(IWinUI* pWindow,
	std::function<void(IWinUI*, std::chrono::steady_clock::time_point&, std::chrono::steady_clock::time_point&)> synchronCalculateLauncher,
	std::function<void(IWinUI*, std::chrono::steady_clock::time_point&, std::chrono::steady_clock::time_point&)> synchronDrawLauncher,
	UINT pointMaxVertexCount,
	UINT lineMaxVertexCount, UINT lineMaxIndexCount,
	UINT triangleMaxVertexCount, UINT triangleMaxIndexCount,
	WinType::Position eyePosition,
	float yaw, float pitch,
	float moveSpeed, float spinSpeed,
	float fovAngleY, float nearZ, float farZ) :
	synchronizer{ pWindow, synchronCalculateLauncher, synchronDrawLauncher },
	pointsData{ pointMaxVertexCount },
	linesData{ lineMaxVertexCount , lineMaxIndexCount },
	trianglesData{ triangleMaxVertexCount, triangleMaxIndexCount },
	camera{ eyePosition, yaw, pitch, moveSpeed, spinSpeed, fovAngleY, nearZ, farZ }
{}
BWinHide::~BWinHide()
{}