#pragma once
#pragma once
#ifndef _BWINHIDE_H_
#define _BWINHIDE_H_

#include "include.h"

enum {
#undef _PRIVATE_
#define _PRIVATE_ \
    friend class IStrUI; \
\
    std::string _windowClassName; \
    bool _isRegister; \
    bool _isClear; \
\
    std::mutex _mutex3D; \
\
	static LRESULT _WindowProcLauncher(HWND, UINT, WPARAM, LPARAM); \
    virtual LRESULT _WindowProcess(HWND, UINT, WPARAM, LPARAM); \
\
    static void _SynchronCalculateLauncher(IWinUI*, _IN_ std::chrono::steady_clock::time_point&, _IN_ std::chrono::steady_clock::time_point&); \
    static void _SynchronDrawLauncher(IWinUI*, _IN_ std::chrono::steady_clock::time_point&, _IN_ std::chrono::steady_clock::time_point&); \
\
    bool _Create2D(); \
    virtual bool _CreateTarget2DAndSource2D(); \
    virtual void _ReleaseTarget2DAndSource2D(); \
\
    bool _Create3D(); \
    bool _CreateViewPortAndDepthStencilAndTarget3D(); \
    bool _Resize3D(WPARAM wParam); \
    void _Release3D(); \
\
    bool _CreateGPU();
};

class WinType
{
public:
    using Vector = DirectX::XMFLOAT3;
    using Position = WinType::Vector;
    using Color = DirectX::XMFLOAT4;

    struct Vertex
    {
    public:
        WinType::Position position;
        WinType::Color color;
    public:
        Vertex(WinType::Position position = WinType::Position{}, WinType::Color color = WinType::Color{});
    };

    using Index = unsigned int;

    struct Pixel
    {
    public:
        union
        {
            BYTE rgba[4];
            struct
            {
                BYTE r;
                BYTE g;
                BYTE b;
                BYTE a;
            };
        };
    public:
        BYTE operator[](int x);
    public:
        Pixel(BYTE r = 0U, BYTE g = 0U, BYTE b = 0U, BYTE a = 1U);
    };
};

class IWinUI;

class BWinHide : public WinType
{
protected:
    class Synchronizer
    {
    protected:
        IWinUI* pWindow;
        std::function<void(IWinUI*, std::chrono::steady_clock::time_point&, std::chrono::steady_clock::time_point&)> synchronCalculateLauncher;
        std::function<void(IWinUI*, std::chrono::steady_clock::time_point&, std::chrono::steady_clock::time_point&)> synchronDrawLauncher;
        std::atomic<bool> running;
        std::thread thread;
    public:
        bool Start(double fpsMax);
        void End();
    public:
        Synchronizer(IWinUI* pWindow,
            std::function<void(IWinUI*, std::chrono::steady_clock::time_point&, std::chrono::steady_clock::time_point&)> synchronCalculateLauncher,
            std::function<void(IWinUI*, std::chrono::steady_clock::time_point&, std::chrono::steady_clock::time_point&)> synchronDrawLauncher);
        virtual ~Synchronizer();
    };

    class Mouse
    {
    protected:
        int originX;
        int originY;
        int lastX;
        int lastY;
        int nowX;
        int nowY;
        bool isCapture;
    public:
        static int TurnX(LPARAM lParam);
        static int TurnY(LPARAM lParam);
    public:
        int GetX();
        int GetY();
        bool Wheel(UINT uMsg, WPARAM wParam, _OUT_ short& value);
        void StartCapture(LPARAM lParam);
        bool Capturing(LPARAM lParam, _OUT_OPT_ int* pDeltaX = nullptr, _OUT_OPT_ int* pDeltaY = nullptr); //each event
        bool EndCapture(LPARAM lParam, _OUT_OPT_ int* pDeltaX = nullptr, _OUT_OPT_ int* pDeltaY = nullptr); //summary
        bool IsCapture();
    public:
        Mouse();
    };

    struct CameraConstantBuffer
    {
        DirectX::XMMATRIX view;
        DirectX::XMMATRIX projection;
    };

    class Camera
    {
        friend class IWinUI;
    protected:
        WinType::Position eyePosition;
        float yaw; //T = 360
        float pitch; //[-89, 89]

        float moveSpeed;
        float spinSpeed;

        float fovAngleY; //[1, 179]
        float nearZ; //nearZ > 0
        float farZ; //farZ > nearZ

        std::mutex mutex;

        WinType::Vector focusDirection;
        WinType::Vector leftDirection;
        WinType::Vector headDirection;
        WinType::Vector const upDirection;
    public:
        WinType::Position GetPosition();
        float GetYaw();
        float GetPitch();
        float GetFovAngleY();
        WinType::Vector GetFocusDirection();
        WinType::Vector GetLeftDirection();
        WinType::Vector GetHeadDirection();
        float GetMoveSpeed();
        float GetSpinSpeed();

        void SetPosition(WinType::Position eyePosition);
        void SetFocus(float yaw, float pitch);
        void SetFov(float fovAngleY);
        void SetMoveSpeed(float moveSpeed);
        void SetSpinSpeed(float spinSpeed);
        //move
        void Forward();
        void Backward();
        void Left();
        void Right();
        void Up();
        void Down();
        //spin
        void SpinLeft();
        void SpinRight();
        void SpinUp();
        void SpinDown();
    public:
        Camera(WinType::Position eyePosition,
            float yaw, float pitch,
            float moveSpeed, float spinSpeed,
            float fovAngleY, float nearZ, float farZ);
    };

    class PointsData
    {
        friend class IWinUI;
    protected:
        std::vector<WinType::Vertex> vertices;
        Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
        UINT const maxVertexCount; //can not 0
    public:
        void Clear();
        bool Add(_IN_ const std::vector<WinType::Vertex>& vertices);
    public:
        PointsData(UINT maxVertexCount = 1U);
    };
    class LinesData
    {
        friend class IWinUI;
    protected:
        std::vector<WinType::Vertex> vertices;
        std::vector<WinType::Index> indices;
        Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;
        UINT const maxVertexCount; //can not 0
        UINT const maxIndexCount; //can not 0
    public:
        void Clear();
        bool Add(_IN_ const std::vector<WinType::Vertex>& vertices, _IN_ const std::vector<WinType::Index>& indices);
    public:
        LinesData(UINT maxVertexCount = 1U, UINT maxIndexCount = 1U);
    };
    class TrianglesData
    {
        friend class IWinUI;
    protected:
        std::vector<WinType::Vertex> vertices;
        std::vector<WinType::Index> indices;
        Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;
        UINT const maxVertexCount; //can not 0
        UINT const maxIndexCount; //can not 0
    public:
        void Clear();
        bool Add(_IN_ const std::vector<WinType::Vertex>& vertices, _IN_ const std::vector<WinType::Index>& indices);
    public:
        TrianglesData(UINT maxVertexCount = 1U, UINT maxIndexCount = 1U);
    };

protected:
    CComPtr<ID2D1Factory> pFactory2d;
    CComPtr<IDWriteFactory> pFactoryWrite;
    CComPtr<IWICImagingFactory> pFactoryImaging;

protected:
    Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
    Microsoft::WRL::ComPtr<IDXGISwapChain> pSwapChain;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> pDepthBuffer{};
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDepthStencil;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget3d;

    Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;

    Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;

    Microsoft::WRL::ComPtr<ID3D11Query> pQuery;

    BWinHide::Synchronizer synchronizer;

    BWinHide::Mouse mouse;

    BWinHide::Camera camera;

    BWinHide::PointsData pointsData;
    BWinHide::LinesData linesData;
    BWinHide::TrianglesData trianglesData;

public:
    BWinHide(IWinUI* pWindow,
        std::function<void(IWinUI*, std::chrono::steady_clock::time_point&, std::chrono::steady_clock::time_point&)> synchronCalculateLauncher,
        std::function<void(IWinUI*, std::chrono::steady_clock::time_point&, std::chrono::steady_clock::time_point&)> synchronDrawLauncher,
        UINT pointMaxVertexCount,
        UINT lineMaxVertexCount, UINT lineMaxIndexCount,
        UINT triangleMaxVertexCount, UINT triangleMaxIndexCount,
        WinType::Position eyePosition,
        float yaw, float pitch,
        float moveSpeed, float spinSpeed,
        float fovAngleY, float nearZ, float farZ);
    virtual ~BWinHide();
};

#endif // !_BWINHIDE_H_