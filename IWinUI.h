#pragma once
#ifndef _IWINUI_H_
#define _IWINUI_H_

#include "include.h"
#include "BWinHide.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//tips:
//use NOther::CoInit before and NOther::UnCoInit after IWinUI life area
//do not use NOther::UnCoInit in life area, if no will error CComPtr<IWICImagingFactory>.p->Release()
//so usually IWinUI in {}
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//how to use:
//window: WindowCreate, PeekLoop or MessageLoop, HandleMessage
//direct2d: CreateResource2D, ReleaseResource2D, DrawContent2D, Draw2D, ReSize
//direct3d: ClearData3D(), Object.AddDataIn, ClearFrame3D, Draw3D, Present3D
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//usually mode:
//static: WmPaint. draw only has message
//always: EachPeekLoop. draw in while(true)
//synchron: SynchronCalculateAndDraw, SynchronPresent, SynchronStart. frame time synchron reality
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

class IWinUI : protected BWinHide
{
public:
    using Vector = Vector;
    using Position = Position;
    using Color = Color;

    using Vertex = Vertex;
    using Index = Index;

    using Pixel = Pixel;

    struct BufferGPU
    {
        UINT maxByte;

        Microsoft::WRL::ComPtr<ID3D11Buffer> pCpuBuffer{};

        Microsoft::WRL::ComPtr<ID3D11Buffer> pUAVBuffer{};
        Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> pUAV{};

        Microsoft::WRL::ComPtr<ID3D11Buffer> pResultBuffer{};

        BufferGPU(UINT maxByte = 1U);
    };

public:
    enum ObjectType : unsigned char
    {
        POINTED, LINEED, TRIANGLEED
    };
    template <ObjectType Type>
    class Object
    {
    protected:
        std::vector<WinType::Vertex> vertices;
        std::vector<WinType::Index> indices;
    public:
        static bool AddDataInDraw(_IN_ IWinUI* pWindow, _IN_ const std::vector<WinType::Vertex>& vertices, _IN_ const std::vector<WinType::Index>& indices);
        static bool AddDataInDraw(_IN_ IWinUI* pWindow, _IN_ const std::vector<WinType::Vertex>& vertices);
        bool AddDataInDraw(_IN_ IWinUI* pWindow);
        void Clear();
    };

protected:
    const HINSTANCE hInst;
    HWND hWnd;
    HWND hWndParent;
    std::string name;
    bool endLoop;

    RECT rectClient; //size
    LONG widthClient;
    LONG heightClient;
    RECT rectWindow; //position

    float scale; //dpi

    CComPtr<ID2D1HwndRenderTarget> pTarget2d;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// window
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

public:
    const HWND& HWnd();
    const HWND& HWndParent();

    bool WindowCreate(std::string name = "",
        bool endLoop = true,
        HWND hWndParent = nullptr,
        DWORD style = (WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN),
        DWORD exStyle = 0U,
        int x = CW_USEDEFAULT,
        int y = CW_USEDEFAULT,
        int width = CW_USEDEFAULT,
        int height = CW_USEDEFAULT,
        HMENU hMenu = nullptr);
    bool WindowClear();
    bool WindowDestroy();

    bool ShowWindow(bool show);
    bool PaintNow(); //InvalidateRect for WM_PAINT

public:
    //return nExitCode from EndLoop, return -1 is error
    int PeekLoop();
    _VIRTUAL_EMPTY_ virtual void EachPeekLoop();
    static int MessageLoop();
    static void EndLoop(int nExitCode = 0);

protected:
    _VIRTUAL_EMPTY_ virtual WNDCLASSEXA WindowClass(); //class name is fixed
    _VIRTUAL_EMPTY_ virtual void WmSize(UINT uMsg, WPARAM wParam, LPARAM lParam);
    _VIRTUAL_EMPTY_ virtual void WmPaint(HDC hdc, PAINTSTRUCT paintStruct);
    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam); //WM_DESTROY

    //synchron
public:
    void SynchronStart(double fpsMax);
    void SynchronEnd();
protected:
    _VIRTUAL_EMPTY_ virtual void SynchronCalculate(std::chrono::steady_clock::time_point lastFrameTime, std::chrono::steady_clock::time_point nextFrameTime);
    _VIRTUAL_EMPTY_ virtual void SynchronDraw(std::chrono::steady_clock::time_point lastPresentTime, std::chrono::steady_clock::time_point nowPresentTime);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// direct2d
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

public:
    bool Draw2D();
    _VIRTUAL_EMPTY_ virtual void DrawContent2D();
    _VIRTUAL_EMPTY_ virtual bool CreateResource2D();
    _VIRTUAL_EMPTY_ virtual void ReleaseResource2D();
    //write
    bool CreateTextFormat2D(FLOAT fontSize, _OUT_ IDWriteTextFormat** ppTextFormat, bool hCenter = false, bool vCenter = false, std::string fontFamilyName = "");
    void DrawTextW2D(std::string text, D2D1_RECT_F layoutRect, _IN_ IDWriteTextFormat* pTextFormat, _IN_ ID2D1Brush* pBrush, bool clip = false); //clip is in rect area
    //image
    bool ReadImage(std::string filePath, _OUT_ std::vector<WinType::Pixel>& data, _OUT_ UINT* pWidth = nullptr, _OUT_ UINT* pHeight = nullptr);
    CComPtr<ID2D1Bitmap> CreateBitmap2D(int resourceID, std::string lpType); //fail return nullptr
    CComPtr<ID2D1Bitmap> CreateBitmap2D(std::string filePath); //fail return nullptr

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// direct3d
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

public:
    ID3D11Device* GetDevice();
    ID3D11DeviceContext* GetContext();
    ID3D11RenderTargetView* GetTarget3d();
    IWinUI::Camera& GetCamera();

protected:
    void ClearData3D();
    bool ClearFrame3D();
    bool Draw3D();
    bool Present3D();
    //default
    virtual void CameraControl3D(UINT uMsg, WPARAM wParam, LPARAM lParam);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// GPU
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

public:
    bool CreateComputeShaderFromCodeGPU(_IN_ const char* hlslCode, _IN_ const char* main, _OUT_ ID3D11ComputeShader** ppComputeShader);
    bool CreateComputeShaderFromFileGPU(std::string hlslPath, _IN_ const char* main, _OUT_ ID3D11ComputeShader** ppComputeShader);

    bool CreateBufferGPU(UINT typeByte, UINT maxCount, _OUT_ IWinUI::BufferGPU& bufferGPU);
    bool CreateConstantBufferGPU(UINT type16Byte, _OUT_ ID3D11Buffer** ppConstantBuffer); //must align 16 byte

    bool BindBufferGPU(_IN_ IWinUI::BufferGPU& bufferGPU, UINT slot);
    bool BindConstantBufferGPU(_IN_ ID3D11Buffer** ppConstantBuffer, UINT slot);

    bool MapBufferGPU(_IN_ IWinUI::BufferGPU& bufferGPU, _IN_OPT_ const void* pNewData, size_t byte);
    bool MapBufferGPU(_IN_ ID3D11Buffer* pBuffer, _IN_OPT_ const void* pNewData, size_t byte);

    bool RunGPU(_IN_ ID3D11ComputeShader* pComputeShader,
        UINT ThreadGroupCountX, UINT ThreadGroupCountY = 1U, UINT ThreadGroupCountZ = 1U);
    bool WaitGPU();
    bool GetResultGPU(_IN_ IWinUI::BufferGPU& bufferGPU, _OUT_ void* pOutput, size_t byte);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// tools
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

public:
    static POINT PointTurn(D2D1_POINT_2F point);
    static D2D1_POINT_2F PointTurn(POINT point);

    static RECT RectTurn(D2D1_RECT_F rect);
    static D2D1_RECT_F RectTurn(RECT rect);

protected:
    double PixelTurnDIP(double pixel);
    POINT PixelTurnDIP(POINT point);
    D2D1_POINT_2F PixelTurnDIP(D2D1_POINT_2F point);
    RECT PixelTurnDIP(RECT rect);
    D2D1_RECT_F PixelTurnDIP(D2D1_RECT_F rect);

    double DIPTurnPixel(double dip);
    POINT DIPTurnPixel(POINT point);
    D2D1_POINT_2F DIPTurnPixel(D2D1_POINT_2F point);
    RECT DIPTurnPixel(RECT rect);
    D2D1_RECT_F DIPTurnPixel(D2D1_RECT_F rect);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// structure
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

public:
	IWinUI(UINT pointMaxVertexCount = 1U,
        UINT lineMaxVertexCount = 1U, UINT lineMaxIndexCount = 1U,
        UINT triangleMaxVertexCount = 1U, UINT triangleMaxIndexCount = 1U,
        WinType::Position eyePosition = WinType::Position{ 0.F, 0.F, -10.F },
        float yaw = 90.F, float pitch = 0.F,
        float moveSpeed = 0.5F, float spinSpeed = 1.F,
        float fovAngleY = 45.F, float nearZ = 0.01F, float farZ = 10000.F);
	virtual ~IWinUI();

private:
	_PRIVATE_
#undef _PRIVATE_
};

#endif // !_IWINUI_H_