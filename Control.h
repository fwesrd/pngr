#pragma once
#ifndef _CONTROL_H_
#define _CONTROL_H_

#include "include.h"
#include "IWinUI.h"

enum {
#undef _PRIVATE_
#define _PRIVATE_ \
	bool _mouseMove; \
	int _tempPosY; \
	FLOAT _percent;
};

class _IControlBase
{
protected:
	const HINSTANCE hInst;
	const int id;

	HWND hWnd;
	HWND hWndParent;
	std::string name;

public:
	const HWND& HWnd();
	const int& Id();

	bool Show(bool show, bool parentPaint = true);
	bool PaintNow();
	bool Destroy();

protected:
	_IControlBase(int id);
	virtual ~_IControlBase();
};

class CButton : public _IControlBase
{
public:
	bool Create(std::string name, int x, int y, int width, int height, HWND hWndParent,
		DWORD dwStyle = (WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON),
		DWORD dwExStyle = 0);
	bool IsPush(UINT uMsg, LPARAM lParam);

public:
	CButton(int id = 0);
	virtual ~CButton();
};

class CEdit : public _IControlBase
{
protected:
	int maxByte;

public:
	bool Create(std::string initText, int maxByte, int x, int y, int width, int height, HWND hWndParent,
		DWORD dwStyle = (WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL),
		DWORD dwExStyle = 0);
	bool GetInput(_OUT_ std::string& text);
	bool SetWindowTextA(std::string text);
	bool IsChange(UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	CEdit(int id = 0);
	virtual ~CEdit();
};

class CStatic : public _IControlBase
{
public:
	bool Create(std::string text, int x, int y, int width, int height, HWND hWndParent,
		DWORD dwStyle = (WS_CHILD | WS_VISIBLE | WS_TABSTOP),
		DWORD dwExStyle = 0);
	bool SetWindowTextA(std::string text);

public:
	CStatic(int id = 0);
	virtual ~CStatic();
};

class CListBox : public _IControlBase
{
public:
	bool Create(std::vector<std::string> texts, int x, int y, int width, int height, HWND hWndParent,
		DWORD dwStyle = (WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL | LBS_NOTIFY),
		DWORD dwExStyle = 0);
	long long GetIndex(); //no select return -1
	bool IsChange(UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	CListBox(int id = 0);
	virtual ~CListBox();
};

class CComboBox : public _IControlBase
{
public:
	bool Create(std::vector<std::string> texts, int x, int y, int width, int height, HWND hWndParent,
		DWORD dwStyle = (WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL | CBS_DROPDOWNLIST),
		DWORD dwExStyle = 0);
	long long GetIndex(); //no select return -1
	bool IsChange(UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	CComboBox(int id = 0);
	virtual ~CComboBox();
};

class CTrackBar : public IWinUI
{
protected:
	FLOAT lineWidth;
	D2D1_RECT_F lineRect;

	FLOAT thumbRadius;
	D2D1_POINT_2F thumbPoint;

	D2D1_COLOR_F colorBackground;
	D2D1_COLOR_F colorLineOut;
	D2D1_COLOR_F colorLineIn;
	D2D1_COLOR_F colorThumb;

	CComPtr<ID2D1SolidColorBrush> pBrushLineOut;
	CComPtr<ID2D1SolidColorBrush> pBrushLineIn;
	CComPtr<ID2D1SolidColorBrush> pBrushThumb;

public:
	bool Create(FLOAT percent, int x, int y, int width, int height, HWND hWndParent,
		DWORD style = (WS_CHILD | WS_VISIBLE | WS_TABSTOP),
		DWORD exStyle = 0,
		FLOAT lineWidth = 8.F, FLOAT thumbRadius = 15.F,
		D2D1_COLOR_F colorBackground = D2D1_COLOR_F{ 1.F, 1.F, 1.F, 1.F },
		D2D1_COLOR_F colorLineOut = D2D1_COLOR_F{ 0.6F, 0.6F, 0.6F, 1.F },
		D2D1_COLOR_F colorLineIn = D2D1_COLOR_F{ 0.F, 0.F, 1.F, 1.F },
		D2D1_COLOR_F colorThumb = D2D1_COLOR_F{ 0.F, 0.F, 1.F, 0.5F },
		HMENU hMenu = nullptr);

	double GetPercent(); //fail return -1
	bool SetPercent(double percent);

	bool IsChange(UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	virtual bool SourceCreate();
	virtual void SourceRelease();
	virtual void ReSize(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void PaintCommand(HDC hdc, PAINTSTRUCT paintStruct);
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	CTrackBar();
	virtual ~CTrackBar();

private:
	_PRIVATE_
#undef _PRIVATE_
};

#endif // !_CONTROL_H_