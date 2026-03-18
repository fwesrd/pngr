#pragma once
#ifndef _ISTRUI_H_
#define _ISTRUI_H_

#include "include.h"
#include "Class.h"
#include "IWinUI.h"

enum {
#undef _PRIVATE_
#define _PRIVATE_ \
	\
	bool _isClear; \
	\
	bool _isText; \
	const std::string _paintWindowClassName; \
	bool _isPaintRegister; \
	\
	COLORREF _putTextColor; \
	HBRUSH _putBackgroundBrush; \
	COLORREF _putBackgroundColor; \
	\
	HFONT _hFont; \
	int _fontHeight; \
	std::string _fontTypeName; \
	\
	double _areaPercentV; \
	double _areaPercentH; \
	\
	std::string _inputBuffer; \
	CEvent _inputEvent; \
	\
	std::thread _threadMainProc; \
	volatile std::atomic<bool> _endThreadMainProc; \
	\
	LRESULT(*_TextDefProc)(HWND, UINT, WPARAM, LPARAM); \
	LRESULT(*_InputDefProc)(HWND, UINT, WPARAM, LPARAM); \
	LRESULT(*_OutputDefProc)(HWND, UINT, WPARAM, LPARAM); \
	\
	bool WindowCreate(std::string, bool, HWND, DWORD, DWORD, int, int, int, int, HMENU) = delete; \
	using IWinUI::WindowCreate; \
	using IWinUI::WindowClear; \
	using IWinUI::WindowDestroy; \
	using IWinUI::PaintNow; \
	using IWinUI::_CreateTarget2DAndSource2D; \
	using IWinUI::_WindowProcess; \
	\
	static inline void _ThreadMainProc(IStrUI* pThis); \
	\
	inline void _Clear(); \
	\
	inline bool _WheelFontSize(WPARAM wParam); \
	\
	inline void _InputCommand(std::string input); \
	\
	static LRESULT _PaintProcLauncher(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam); \
	static LRESULT _TextProcLauncher(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam); \
	static LRESULT _InputProcLauncher(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam); \
	static LRESULT _OutputProcLauncher(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

class IStrUI : public IWinUI
{
protected:
	HWND hPaint;
	const int idPaint;

	HWND hInput;
	const int idInput;

	HWND hOutput;
	const int idOutput;

	RECT rectPaint;

public:
	bool WindowCreate(std::string name = "",
		bool endLoop = true,
		HWND hWndParent = nullptr,
		bool isText = true,
		std::string inputDefText = "",
		std::string outputDefText = "",
		COLORREF putTextColor = RGB(255, 255, 255),
		COLORREF putBackgroundColor = RGB(0, 0, 0),
		int fontHeight = 20,
		std::string fontTypeName = "NSimSun",
		DWORD style = (WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN),
		DWORD exStyle = 0,
		int x = CW_USEDEFAULT,
		int y = CW_USEDEFAULT,
		int width = CW_USEDEFAULT,
		int height = CW_USEDEFAULT,
		HMENU hMenu = nullptr);

	bool WindowClear();

	bool WindowDestroy();

protected:
	//main process access
	void MainProc(); //while need use IsEndThreadMainProc
	volatile bool IsEndThreadMainProc(); //check main process is end or not
	//input
	std::string GetInput();
	std::string GetInputN(); //no ""
	std::string GetInputNB(); //no "" and no only blank
	//output
	bool Output(std::string output);
	bool PaintNow(); //paint mode, InvalidateRect
	bool Text(std::string text); //text mode, do nothing if paint mode
	//output help
	bool OutputHelp();
	bool TextHelp();

protected:
	//virtual
	virtual WNDCLASSEXA PaintWindowClass();
	virtual LRESULT PaintWindowHandle(UINT uMsg, WPARAM wParam, LPARAM lParam);

	//default
	virtual bool CreateTarget2DAndSource2D();
	virtual LRESULT WindowProcess(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT PaintProcess(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT TextProcess(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT InputProcess(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OutputProcess(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	IStrUI(int idPaint = 0, int idInput = 0, int idOutput = 0, double areaPercentV = 0.1, double areaPercentH = 0.5);
	virtual ~IStrUI();

private:
	_PRIVATE_
#undef _PRIVATE_
};

#endif // !_ISTRUI_H_