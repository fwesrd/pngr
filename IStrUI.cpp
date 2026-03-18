#include "IStrUI.h"

#define _PAINT_WINDOWCLASSNAME "_PAINT_"

namespace NString
{
	extern std::string Trim(std::string str);
}

bool IStrUI::WindowCreate(std::string name, bool endLoop, HWND hWndParent, bool isText, std::string inputDefText, std::string outputDefText,
	COLORREF putTextColor, COLORREF putBackgroundColor, int fontHeight, std::string fontTypeName,
	DWORD style, DWORD exStyle, int x, int y, int width, int height, HMENU hMenu)
{
	//thread event
	if (!IStrUI::_inputEvent.Create(false, false)) return false;
	//main window
	if (!IWinUI::WindowCreate(name, endLoop, hWndParent, style, exStyle, x, y, width, height, hMenu)) return false;
	//input and output window
	IStrUI::_putTextColor = putTextColor;
	if (!(IStrUI::_putBackgroundBrush = ::CreateSolidBrush(IStrUI::_putBackgroundColor = putBackgroundColor))) return false;
	if ((IStrUI::_fontHeight = fontHeight) < 0) IStrUI::_fontHeight = 0;
	IStrUI::_hFont = ::CreateFontA(IStrUI::_fontHeight, 0, 0, 0, FW_NORMAL, false, false, false, DEFAULT_CHARSET,
		OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH, (IStrUI::_fontTypeName = fontTypeName).c_str()); //ignore font fail
	//print window
	if (IStrUI::_isText = isText)
	{
		if (!(IStrUI::hPaint = ::CreateWindowExA(0, "EDIT", nullptr,
			WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | ES_READONLY,
			0, 0, (IWinUI::rectClient.right - IWinUI::rectClient.left), static_cast<int>((IWinUI::rectClient.bottom - IWinUI::rectClient.top) * (1 - IStrUI::_areaPercentV)),
			IWinUI::hWnd, reinterpret_cast<HMENU>(static_cast<long long>(IStrUI::idPaint)), IWinUI::hInst, nullptr)) ||
			!(IStrUI::_TextDefProc = reinterpret_cast<LRESULT(*)(HWND, UINT, WPARAM, LPARAM)>(::GetWindowLongPtrA(IStrUI::hPaint, GWLP_WNDPROC))))
		{
			IStrUI::WindowClear();
			return false;
		}
		::SetWindowLongPtrA(IStrUI::hPaint, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(IStrUI::_TextProcLauncher));
		::SendMessageA(IStrUI::hPaint, WM_NCCREATE, 0, reinterpret_cast<LPARAM>(this)); //input this
		if (IStrUI::_hFont) ::SendMessageA(IStrUI::hPaint, WM_SETFONT, reinterpret_cast<WPARAM>(IStrUI::_hFont), true);
		::InvalidateRect(IStrUI::hPaint, nullptr, true);
	}
	else
	{
		if (!IStrUI::_isPaintRegister)
		{
			WNDCLASSEXA paintClass{ this->PaintWindowClass() };
			if (!paintClass.style) paintClass.style = (CS_HREDRAW | CS_VREDRAW);
			if (!paintClass.hbrBackground) paintClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
			if (!paintClass.cbSize) paintClass.cbSize = sizeof(WNDCLASSEXA);
			if (!paintClass.lpfnWndProc) paintClass.lpfnWndProc = IStrUI::_PaintProcLauncher;
			if (!paintClass.hInstance) paintClass.hInstance = IWinUI::hInst;
			paintClass.lpszClassName = IStrUI::_paintWindowClassName.c_str();
			if (::RegisterClassExA(&paintClass)) IStrUI::_isPaintRegister = true;
		}
		if (!(IStrUI::hPaint = ::CreateWindowExA(0, IStrUI::_paintWindowClassName.c_str(), "", WS_CHILD | WS_VISIBLE,
			0, 0, (IWinUI::rectClient.right - IWinUI::rectClient.left), static_cast<int>((IWinUI::rectClient.bottom - IWinUI::rectClient.top) * (1 - IStrUI::_areaPercentV)),
			IWinUI::hWnd, reinterpret_cast<HMENU>(static_cast<long long>(IStrUI::idPaint)), IWinUI::hInst, this)) ||
			!::GetClientRect(IStrUI::hPaint, &(IStrUI::rectPaint)))
		{
			IStrUI::WindowClear();
			return false;
		}
	}
	//input window
	std::string inputText{};
	for (const auto& ch : inputDefText)
	{
		if (ch == '\n') inputText += '\r';
		inputText += ch;
	}
	if (!(IStrUI::hInput = ::CreateWindowExA(WS_EX_ACCEPTFILES, "EDIT", inputText.c_str(),
		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | ES_READONLY, //only use function to allow input
		0, static_cast<int>((IWinUI::rectClient.bottom - IWinUI::rectClient.top) * (1 - IStrUI::_areaPercentV)),
		static_cast<int>((IWinUI::rectClient.right - IWinUI::rectClient.left) * IStrUI::_areaPercentH),
		static_cast<int>((IWinUI::rectClient.bottom - IWinUI::rectClient.top) * IStrUI::_areaPercentV),
		IWinUI::hWnd, reinterpret_cast<HMENU>(static_cast<long long>(IStrUI::idInput)), IWinUI::hInst, nullptr)) ||
		!(IStrUI::_InputDefProc = reinterpret_cast<LRESULT(*)(HWND, UINT, WPARAM, LPARAM)>(::GetWindowLongPtrA(IStrUI::hInput, GWLP_WNDPROC))))
	{
		IStrUI::WindowClear();
		return false;
	}
	::SetFocus(IStrUI::hInput);
	::SetWindowLongPtrA(IStrUI::hInput, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(IStrUI::_InputProcLauncher));
	::SendMessageA(IStrUI::hInput, WM_NCCREATE, 0, reinterpret_cast<LPARAM>(this)); //input this
	if (IStrUI::_hFont) ::SendMessageA(IStrUI::hInput, WM_SETFONT, reinterpret_cast<WPARAM>(IStrUI::_hFont), true);
	::InvalidateRect(IStrUI::hInput, nullptr, true);
	//output window
	std::string outputText{};
	for (const auto& ch : outputDefText)
	{
		if (ch == '\n') outputText += '\r';
		outputText += ch;
	}
	if (!(IStrUI::hOutput = ::CreateWindowExA(0, "EDIT", outputText.c_str(),
		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | ES_READONLY,
		static_cast<int>((IWinUI::rectClient.right - IWinUI::rectClient.left) * IStrUI::_areaPercentH),
		static_cast<int>((IWinUI::rectClient.bottom - IWinUI::rectClient.top) * (1 - IStrUI::_areaPercentV)),
		static_cast<int>((IWinUI::rectClient.right - IWinUI::rectClient.left) * (1 - IStrUI::_areaPercentH)),
		static_cast<int>((IWinUI::rectClient.bottom - IWinUI::rectClient.top) * IStrUI::_areaPercentV),
		IWinUI::hWnd, reinterpret_cast<HMENU>(static_cast<long long>(IStrUI::idOutput)), IWinUI::hInst, nullptr)) ||
		!(IStrUI::_OutputDefProc = reinterpret_cast<LRESULT(*)(HWND, UINT, WPARAM, LPARAM)>(::GetWindowLongPtrA(IStrUI::hOutput, GWLP_WNDPROC))))
	{
		IStrUI::WindowClear();
		return false;
	}
	::SetWindowLongPtrA(IStrUI::hOutput, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(IStrUI::_OutputProcLauncher));
	::SendMessageA(IStrUI::hOutput, WM_NCCREATE, 0, reinterpret_cast<LPARAM>(this)); //input this
	if (IStrUI::_hFont) ::SendMessageA(IStrUI::hOutput, WM_SETFONT, reinterpret_cast<WPARAM>(IStrUI::_hFont), true);
	::InvalidateRect(IStrUI::hOutput, nullptr, true);
	//main thread
	IStrUI::_threadMainProc = std::thread{ IStrUI::_ThreadMainProc, this };
	return true;
}

bool IStrUI::WindowClear()
{
	if (!IWinUI::WindowClear()) return false;
	IStrUI::_Clear();
	return true;
}

bool IStrUI::WindowDestroy()
{
	if (!IWinUI::WindowDestroy()) return false;
	IStrUI::_Clear();
	return true;
}

void IStrUI::MainProc()
{
	std::string input{};
	while (!IStrUI::IsEndThreadMainProc())
	{
		input = NString::Trim(IStrUI::GetInput());
		if (input == "help")
		{
			IStrUI::TextHelp();
		}
		else
		{
			IStrUI::Text("");
		}
	}
}
volatile bool IStrUI::IsEndThreadMainProc()
{
	return (IStrUI::_endThreadMainProc && IStrUI::_inputEvent.Set());
}
std::string IStrUI::GetInput()
{
	if (!IStrUI::hInput || !IStrUI::_inputEvent) return "";
	::SendMessageA(IStrUI::hInput, EM_SETREADONLY, false, 0);
	//wait event signal
	IStrUI::_inputEvent.Wait(INFINITE);
	std::string result{ IStrUI::_inputBuffer };
	IStrUI::_inputBuffer = "";
	::SendMessageA(IStrUI::hInput, EM_SETREADONLY, true, 0);
	::SetWindowTextA(IStrUI::hInput, nullptr);
	return result;
}
std::string IStrUI::GetInputN()
{
	std::string input{};
	while ((input = IStrUI::GetInput()) == "");
	return input;
}
std::string IStrUI::GetInputNB()
{
	std::string input{};
	while ((NString::Trim(input = IStrUI::GetInput()) == ""));
	return input;
}

bool IStrUI::Output(std::string output)
{
	if (!IStrUI::hOutput) return false;
	std::string outputText{};
	for (const char& ch : output)
	{
		if (ch == '\n') outputText += '\r';
		outputText += ch;
	}
	return ::SetWindowTextA(IStrUI::hOutput, outputText.c_str());
}
bool IStrUI::PaintNow()
{
	return (IStrUI::hPaint && ::InvalidateRect(IStrUI::hPaint, nullptr, false));
}
bool IStrUI::Text(std::string text)
{
	if (!IStrUI::_isText || !IStrUI::hPaint) return false;
	std::string outputText{};
	for (const char& ch : text)
	{
		if (ch == '\n') outputText += '\r';
		outputText += ch;
	}
	return ::SetWindowTextA(IStrUI::hPaint, outputText.c_str());
}
bool IStrUI::OutputHelp()
{
	return IStrUI::Output(
		"help:\n"
		"1.Ctrl + Q ..........exit\n"
		"2.Ctrl + Enter ......next line\n"
		"3.Ctrl + L ..........clear\n"
		"4.Ctrl + wheel ......font size\n"
		"5.drag file .........file path\n");
}
bool IStrUI::TextHelp()
{
	return IStrUI::Text(
		"help:\n"
		"1.Ctrl + Q ..........exit\n"
		"2.Ctrl + Enter ......next line\n"
		"3.Ctrl + L ..........clear\n"
		"4.Ctrl + wheel ......font size\n"
		"5.drag file .........file path\n");
}

WNDCLASSEXA IStrUI::PaintWindowClass()
{
	return WNDCLASSEXA{};
}
LRESULT IStrUI::PaintWindowHandle(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return (IStrUI::hPaint ? ::DefWindowProcA(IStrUI::hPaint, uMsg, wParam, lParam) : 0);
}

bool IStrUI::CreateTarget2DAndSource2D()
{
	if (IWinUI::pTarget2d) return true;
	D2D1_SIZE_U size{ D2D1::SizeU((IStrUI::rectPaint.right - IStrUI::rectPaint.left), (IStrUI::rectPaint.bottom - IStrUI::rectPaint.top)) };
	if (!IStrUI::hPaint || !IWinUI::pFactory2d || FAILED(IWinUI::pFactory2d->CreateHwndRenderTarget(D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(IStrUI::hPaint, size), &(IWinUI::pTarget2d)))) return false;
	this->ReleaseResource2D();
	return this->CreateResource2D();
}

LRESULT IStrUI::WindowProcess(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_NCCREATE)
	{
		IWinUI::hWnd = hWnd; //window creating means CreateWindowExA no return yet£¬so hWnd need manual input
		return this->HandleMessage(uMsg, wParam, lParam);
	}
	if (!IWinUI::hWnd) return this->HandleMessage(uMsg, wParam, lParam);
	switch (uMsg)
	{
	case WM_CTLCOLORSTATIC:
	case WM_CTLCOLOREDIT:
	{
		HWND tempHWnd{ reinterpret_cast<HWND>(lParam) };
		if (!tempHWnd) break;
		if ((!IStrUI::_isText || (tempHWnd != IStrUI::hPaint)) &&
			(tempHWnd != IStrUI::hInput) &&
			(tempHWnd != IStrUI::hOutput)) break;
		HDC hdcEdit{ reinterpret_cast<HDC>(wParam) };
		if (!hdcEdit) break;
		::SetTextColor(hdcEdit, IStrUI::_putTextColor);
		::SetBkColor(hdcEdit, IStrUI::_putBackgroundColor);
		return reinterpret_cast<LRESULT>(IStrUI::_putBackgroundBrush);
	}break;
	case WM_SIZE:
	{
		//main window
		if (!::GetClientRect(IWinUI::hWnd, &(IWinUI::rectClient))) return 0;
		::InvalidateRect(IWinUI::hWnd, nullptr, false);
		//print window
		if (IStrUI::hPaint)
		{
			if (!::SetWindowPos(IStrUI::hPaint, nullptr, 0, 0, (IWinUI::rectClient.right - IWinUI::rectClient.left),
				static_cast<int>((IWinUI::rectClient.bottom - IWinUI::rectClient.top) * (1 - IStrUI::_areaPercentV)), SWP_NOZORDER) ||
				!::GetClientRect(IStrUI::hPaint, &(IStrUI::rectPaint))) return 0;
			D2D1_SIZE_U size{ D2D1::SizeU((IStrUI::rectPaint.right - IStrUI::rectPaint.left), (IStrUI::rectPaint.bottom - IStrUI::rectPaint.top)) };
			if (IWinUI::pTarget2d && FAILED(IWinUI::pTarget2d->Resize(size))) return 0;
			::InvalidateRect(IStrUI::hPaint, nullptr, false);
		}
		//input window
		if (IStrUI::hInput)
		{
			if (!::SetWindowPos(IStrUI::hInput, nullptr, 0,
				static_cast<int>((IWinUI::rectClient.bottom - IWinUI::rectClient.top) * (1 - IStrUI::_areaPercentV)),
				static_cast<int>((IStrUI::rectPaint.right - IStrUI::rectPaint.left) * IStrUI::_areaPercentH),
				static_cast<int>((IWinUI::rectClient.bottom - IWinUI::rectClient.top) * IStrUI::_areaPercentV),
				SWP_NOZORDER)) return 0;
			::InvalidateRect(IStrUI::hInput, nullptr, false);
		}
		//output window
		if (IStrUI::hOutput)
		{
			if (!::SetWindowPos(IStrUI::hOutput, nullptr,
				static_cast<int>((IStrUI::rectPaint.right - IStrUI::rectPaint.left) * IStrUI::_areaPercentH),
				static_cast<int>((IWinUI::rectClient.bottom - IWinUI::rectClient.top) * (1 - IStrUI::_areaPercentV)),
				static_cast<int>((IStrUI::rectPaint.right - IStrUI::rectPaint.left) * (1 - IStrUI::_areaPercentH)),
				static_cast<int>((IWinUI::rectClient.bottom - IWinUI::rectClient.top) * IStrUI::_areaPercentV),
				SWP_NOZORDER)) return 0;
			::InvalidateRect(IStrUI::hOutput, nullptr, false);
		}
	} //no break
	case WM_MOVE:
	{
		if (!::GetWindowRect(IWinUI::hWnd, &(IWinUI::rectWindow))) return 0;
		::InvalidateRect(IWinUI::hWnd, nullptr, false);
	}break;
	case WM_PAINT:
	{
		PAINTSTRUCT paintStruct{};
		HDC hdc{ ::BeginPaint(IWinUI::hWnd, &paintStruct) };
		::EndPaint(IWinUI::hWnd, &paintStruct); //return === TRUE
	}break;
	case WM_ERASEBKGND: return 1; //no erase background
	}
	return this->HandleMessage(uMsg, wParam, lParam);
}
LRESULT IStrUI::PaintProcess(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_NCCREATE)
	{
		IStrUI::hPaint = hWnd;
		return this->PaintWindowHandle(uMsg, wParam, lParam);
	}
	if (!IStrUI::hPaint) return this->PaintWindowHandle(uMsg, wParam, lParam);
	switch (uMsg)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT paintStruct{};
		HDC hdc{ ::BeginPaint(IStrUI::hPaint, &paintStruct) };
		this->WmPaint(hdc, paintStruct);
		::EndPaint(IStrUI::hPaint, &paintStruct);
	}break;
	case WM_MOUSEWHEEL:
	{
		if (IStrUI::_WheelFontSize(wParam)) return 0;
	}break;
	}
	return this->PaintWindowHandle(uMsg, wParam, lParam);
}
LRESULT IStrUI::TextProcess(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_NCCREATE)
	{
		IStrUI::hPaint = hWnd;
		return ::DefWindowProcA(hWnd, uMsg, wParam, lParam);
	}
	if (!IStrUI::hPaint) return ::DefWindowProcA(hWnd, uMsg, wParam, lParam);
	switch (uMsg)
	{
	case WM_MOUSEWHEEL:
	{
		if (IStrUI::_WheelFontSize(wParam)) return 0;
	}break;
	case WM_KEYDOWN:
	{
		if ((wParam == 'Q') && (::GetKeyState(VK_CONTROL) & 0x8000))
		{
			IStrUI::WindowDestroy();
			return 0;
		}
	}break;
	}
	return IStrUI::_TextDefProc(hWnd, uMsg, wParam, lParam);
}
LRESULT IStrUI::InputProcess(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_NCCREATE)
	{
		IStrUI::hInput = hWnd;
		return ::DefWindowProcA(hWnd, uMsg, wParam, lParam);
	}
	if (!IStrUI::hInput) return ::DefWindowProcA(hWnd, uMsg, wParam, lParam);
	bool readOnly = (::GetWindowLongPtrA(IStrUI::hInput, GWL_STYLE) & ES_READONLY);
	switch (uMsg)
	{
	case WM_KEYDOWN:
	{
		if (wParam == VK_RETURN) //is input enter?
		{
			if (readOnly) return 0;
			if (::GetKeyState(VK_CONTROL) & 0x8000) break; //ctrl + enter is line feed
			auto length{ ::GetWindowTextLengthA(IStrUI::hInput) };
			char* buffer{ new char[length + 1] {} }; //user input
			if (!buffer) return 0;
			::GetWindowTextA(IStrUI::hInput, buffer, length + 1);
			std::string input{ buffer };
			delete[] buffer;
			::SetWindowTextA(IStrUI::hInput, nullptr);
			input.erase(std::remove_if(input.begin(), input.end(), [](char& ch) -> bool { return (ch == '\r'); }));
			IStrUI::_InputCommand(input);
			return 0;
		}
		else if (wParam == 'L')
		{
			if ((::GetKeyState(VK_CONTROL) & 0x8000) && !readOnly)
			{
				::SetWindowTextA(IStrUI::hInput, nullptr);
				return 0;
			}
		}
		else if ((wParam == 'Q') && (::GetKeyState(VK_CONTROL) & 0x8000))
		{
			IStrUI::WindowDestroy();
			return 0;
		}
	}break;
	case WM_CHAR: //message from edit default window process function auto sended
	{
		if (wParam == '\r') return 0; //stop all '\r', so that enter key decide all option
	}break;
	case WM_DROPFILES:
	{
		if (readOnly) return 0;
		HDROP hDrop{ reinterpret_cast<HDROP>(wParam) };
		auto length{ ::DragQueryFileA(hDrop, 0, nullptr, 0) };
		char* buffer{ new char[length + 1] {} }; //file path
		if (!buffer) break;
		if (::DragQueryFileA(hDrop, 0, buffer, length + 1))
		{
			auto length{ ::GetWindowTextLengthA(IStrUI::hInput) };
			::SendMessageA(IStrUI::hInput, EM_SETSEL, length, length);
			::SendMessageA(IStrUI::hInput, EM_REPLACESEL, false, reinterpret_cast<LPARAM>(buffer));
		}
		delete[] buffer;
		::SetFocus(IStrUI::hInput);
	}break;
	case WM_MOUSEWHEEL:
	{
		if (IStrUI::_WheelFontSize(wParam)) return 0;
	}break;
	}
	return IStrUI::_InputDefProc(hWnd, uMsg, wParam, lParam);
}
LRESULT IStrUI::OutputProcess(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_NCCREATE)
	{
		IStrUI::hOutput = hWnd;
		return ::DefWindowProcA(hWnd, uMsg, wParam, lParam);
	}
	if (!IStrUI::hOutput) return ::DefWindowProcA(hWnd, uMsg, wParam, lParam);
	switch (uMsg)
	{
	case WM_MOUSEWHEEL:
	{
		if (IStrUI::_WheelFontSize(wParam)) return 0;
	}break;
	case WM_KEYDOWN:
	{
		if ((wParam == 'Q') && (::GetKeyState(VK_CONTROL) & 0x8000))
		{
			IStrUI::WindowDestroy();
			return 0;
		}
	}break;
	}
	return IStrUI::_OutputDefProc(hWnd, uMsg, wParam, lParam);
}

IStrUI::IStrUI(int idPaint, int idInput, int idOutput, double areaPercentV, double areaPercentH) :
	IWinUI{},

	_isClear{ false },

	_isText{ false },

	hPaint{ nullptr }, idPaint{ idPaint },
	hInput{ nullptr }, idInput{ idInput },
	hOutput{ nullptr }, idOutput{ idOutput },
	rectPaint{},

	_paintWindowClassName{ std::to_string(reinterpret_cast<long long>(this)) + _PAINT_WINDOWCLASSNAME },
	_isPaintRegister{ false },

	_putTextColor{}, _putBackgroundBrush{ nullptr }, _putBackgroundColor{},

	_hFont{ nullptr }, _fontHeight{},

	_areaPercentV{ areaPercentV }, _areaPercentH{ areaPercentH },

	_inputEvent{},

	_endThreadMainProc{ false },

	_TextDefProc{ nullptr }, _InputDefProc{ nullptr }, _OutputDefProc{ nullptr }
{
	if (IStrUI::_areaPercentV < 0.0) IStrUI::_areaPercentV = 0.0;
	else if (IStrUI::_areaPercentV > 1.0) IStrUI::_areaPercentV = 1.0;
	if (IStrUI::_areaPercentH < 0.0) IStrUI::_areaPercentH = 0.0;
	else if (IStrUI::_areaPercentH > 1.0) IStrUI::_areaPercentH = 1.0;
}
IStrUI::~IStrUI()
{
	IStrUI::WindowDestroy();
	if (IWinUI::hInst) ::UnregisterClassA(IStrUI::_paintWindowClassName.c_str(), IWinUI::hInst);
}

inline void IStrUI::_ThreadMainProc(IStrUI* pThis)
{
	if (!pThis) return;
	pThis->IStrUI::_endThreadMainProc = false;
	pThis->MainProc();
}

inline void IStrUI::_Clear()
{
	IStrUI::_inputEvent.Set();

	if (IStrUI::_threadMainProc.joinable())
	{
		IStrUI::_endThreadMainProc = true;
		IStrUI::_threadMainProc.join();
	}
	IStrUI::_endThreadMainProc = false;

	IStrUI::hPaint = nullptr;
	IStrUI::hInput = nullptr;
	IStrUI::hOutput = nullptr;

	IStrUI::rectPaint = RECT{};

	IStrUI::_putTextColor = COLORREF{};
	if (IStrUI::_putBackgroundBrush)
	{
		::DeleteObject(IStrUI::_putBackgroundBrush);
		IStrUI::_putBackgroundBrush = nullptr;
	}
	IStrUI::_putBackgroundColor = COLORREF{};

	if (IStrUI::_hFont)
	{
		::CloseHandle(IStrUI::_hFont);
		IStrUI::_hFont = nullptr;
	}
	IStrUI::_fontHeight = 0;
	IStrUI::_fontTypeName = "";

	IStrUI::_areaPercentV = 0.0;
	IStrUI::_areaPercentH = 0.0;

	IStrUI::_inputBuffer = "";
	IStrUI::_inputEvent.Close();

	IStrUI::_TextDefProc = nullptr;
	IStrUI::_InputDefProc = nullptr;
	IStrUI::_OutputDefProc = nullptr;
}

inline bool IStrUI::_WheelFontSize(WPARAM wParam)
{
	if (!(GET_KEYSTATE_WPARAM(wParam) & MK_CONTROL)) return false;
	int fontSize{ static_cast<int>(IStrUI::_fontHeight + GET_WHEEL_DELTA_WPARAM(wParam) * 1.0 / WHEEL_DELTA) };
	if (fontSize > 0)
	{
		HFONT hFont{ ::CreateFontA(IStrUI::_fontHeight = fontSize, 0, 0, 0, FW_NORMAL, false, false, false, DEFAULT_CHARSET,
			OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH, IStrUI::_fontTypeName.c_str()) };
		if (hFont)
		{
			if (IStrUI::_isText && IStrUI::hPaint) ::SendMessageA(IStrUI::hPaint, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), true);
			if (IStrUI::hInput) ::SendMessageA(IStrUI::hInput, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), true);
			if (IStrUI::hOutput) ::SendMessageA(IStrUI::hOutput, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), true);
		}
	}
	return true;
}

inline void IStrUI::_InputCommand(std::string input)
{
	IStrUI::_inputBuffer = input;
	IStrUI::_inputEvent.Set();
}

LRESULT IStrUI::_PaintProcLauncher(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	IStrUI* pThis{};
	if (uMsg == WM_NCCREATE)
	{
		if (pThis = reinterpret_cast<IStrUI*>((reinterpret_cast<CREATESTRUCTA*>(lParam))->lpCreateParams))
		{
			::SetWindowLongPtrA(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
		}
	}
	else
	{
		pThis = reinterpret_cast<IStrUI*>(::GetWindowLongPtrA(hWnd, GWLP_USERDATA));
	}
	return (pThis ? pThis->PaintProcess(hWnd, uMsg, wParam, lParam) : ::DefWindowProcA(hWnd, uMsg, wParam, lParam));
}
LRESULT IStrUI::_TextProcLauncher(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	IStrUI* pThis{};
	if (uMsg == WM_NCCREATE)
	{
		if (pThis = reinterpret_cast<IStrUI*>(lParam))
		{
			::SetWindowLongPtrA(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
		}
	}
	else
	{
		pThis = reinterpret_cast<IStrUI*>(::GetWindowLongPtrA(hWnd, GWLP_USERDATA));
	}
	return (pThis ? pThis->TextProcess(hWnd, uMsg, wParam, lParam) : ::DefWindowProcA(hWnd, uMsg, wParam, lParam));
}
LRESULT IStrUI::_InputProcLauncher(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	IStrUI* pThis{};
	if (uMsg == WM_NCCREATE)
	{
		if (pThis = reinterpret_cast<IStrUI*>(lParam))
		{
			::SetWindowLongPtrA(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
		}
	}
	else
	{
		pThis = reinterpret_cast<IStrUI*>(::GetWindowLongPtrA(hWnd, GWLP_USERDATA));
	}
	return (pThis ? pThis->InputProcess(hWnd, uMsg, wParam, lParam) : ::DefWindowProcA(hWnd, uMsg, wParam, lParam));
}
LRESULT IStrUI::_OutputProcLauncher(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	IStrUI* pThis{};
	if (uMsg == WM_NCCREATE)
	{
		if (pThis = reinterpret_cast<IStrUI*>(lParam))
		{
			::SetWindowLongPtrA(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
		}
	}
	else
	{
		pThis = reinterpret_cast<IStrUI*>(::GetWindowLongPtrA(hWnd, GWLP_USERDATA));
	}
	return (pThis ? pThis->OutputProcess(hWnd, uMsg, wParam, lParam) : ::DefWindowProcA(hWnd, uMsg, wParam, lParam));
}