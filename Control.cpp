#include "Control.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// _IControlBase
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

const HWND& _IControlBase::HWnd()
{
	return _IControlBase::hWnd;
}
const int& _IControlBase::Id()
{
	return _IControlBase::id;
}

bool _IControlBase::Show(bool show, bool parentPaint)
{
	if (!_IControlBase::hWnd) return false;
	::ShowWindow(_IControlBase::hWnd, show ? SW_SHOW : SW_HIDE); //return last is show or not
	return (!parentPaint || !_IControlBase::hWndParent || ::InvalidateRect(_IControlBase::hWndParent, nullptr, false));
}
bool _IControlBase::PaintNow()
{
	return (_IControlBase::hWnd && ::InvalidateRect(_IControlBase::hWnd, nullptr, false));
}
bool _IControlBase::Destroy()
{
	if (!_IControlBase::hWnd) return true;
	bool result{ static_cast<bool>(::DestroyWindow(_IControlBase::hWnd)) }; //block
	_IControlBase::hWnd = nullptr;
	_IControlBase::hWndParent = nullptr;
	_IControlBase::name.clear();
	return result;
}

_IControlBase::_IControlBase(int id) : hInst{ ::GetModuleHandleA(nullptr) }, id{ id }, hWnd{ nullptr }, hWndParent{ nullptr }
{}
_IControlBase::~_IControlBase()
{
	_IControlBase::Destroy();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// CButton
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

bool CButton::Create(std::string name, int x, int y, int width, int height, HWND hWndParent, DWORD dwStyle, DWORD dwExStyle)
{
	return (_IControlBase::hInst && _IControlBase::Destroy() && (_IControlBase::hWnd = ::CreateWindowExA(dwExStyle, WC_BUTTONA,
		(_IControlBase::name = name).c_str(), dwStyle, x, y, width, height, (_IControlBase::hWndParent = hWndParent),
		reinterpret_cast<HMENU>(static_cast<unsigned long long>(_IControlBase::id)), _IControlBase::hInst, nullptr)));
}
bool CButton::IsPush(UINT uMsg, LPARAM lParam)
{
	return (_IControlBase::hWnd && (uMsg == WM_COMMAND) && (reinterpret_cast<HWND>(lParam) == _IControlBase::hWnd));
}

CButton::CButton(int id) : _IControlBase{ id }
{}
CButton::~CButton()
{}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// CEdit
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

bool CEdit::Create(std::string initText, int maxByte, int x, int y, int width, int height, HWND hWndParent, DWORD dwStyle, DWORD dwExStyle)
{
	return (_IControlBase::hInst && (maxByte > 0) && _IControlBase::Destroy() && (_IControlBase::hWnd = ::CreateWindowExA(dwExStyle, WC_EDITA,
		(_IControlBase::name = initText.substr(0, maxByte)).c_str(), dwStyle, x, y, width, height, (_IControlBase::hWndParent = hWndParent),
		reinterpret_cast<HMENU>(static_cast<unsigned long long>(_IControlBase::id)), _IControlBase::hInst, nullptr)) &&
		SUCCEEDED(::SendMessageA(_IControlBase::hWnd, EM_LIMITTEXT, CEdit::maxByte = maxByte, 0)));
}
bool CEdit::GetInput(_OUT_ std::string& text)
{
	if (!_IControlBase::hWnd) return false;
	std::shared_ptr<char> buffer{ new char[CEdit::maxByte + 1] {} };
	if (!buffer) return false;
	text = (::GetWindowTextA(_IControlBase::hWnd, buffer.get(), CEdit::maxByte + 1) ? buffer.get() : "");
	return true;
}
bool CEdit::SetWindowTextA(std::string text)
{
	return (_IControlBase::hWnd && ::SetWindowTextA(_IControlBase::hWnd, text.c_str()));
}
bool CEdit::IsChange(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return (_IControlBase::hWnd && (uMsg == WM_COMMAND) && (reinterpret_cast<HWND>(lParam) == _IControlBase::hWnd) && (HIWORD(wParam) == EN_CHANGE));
}

CEdit::CEdit(int id) : _IControlBase{ id }, maxByte{}
{}
CEdit::~CEdit()
{}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// CStatic
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

bool CStatic::Create(std::string text, int x, int y, int width, int height, HWND hWndParent, DWORD dwStyle, DWORD dwExStyle)
{
	return (_IControlBase::hInst && _IControlBase::Destroy() && (_IControlBase::hWnd = ::CreateWindowExA(dwExStyle, WC_STATICA,
		(_IControlBase::name = text).c_str(), dwStyle, x, y, width, height, (_IControlBase::hWndParent = hWndParent),
		reinterpret_cast<HMENU>(static_cast<unsigned long long>(_IControlBase::id)), _IControlBase::hInst, nullptr)));
}
bool CStatic::SetWindowTextA(std::string text)
{
	return (_IControlBase::hWnd && ::SetWindowTextA(_IControlBase::hWnd, text.c_str()));
}

CStatic::CStatic(int id) : _IControlBase{ id }
{}
CStatic::~CStatic()
{}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// CListBox
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

bool CListBox::Create(std::vector<std::string> texts, int x, int y, int width, int height, HWND hWndParent, DWORD dwStyle, DWORD dwExStyle)
{
	if (!_IControlBase::hInst || !_IControlBase::Destroy() || !(_IControlBase::hWnd = ::CreateWindowExA(dwExStyle, WC_LISTBOXA,
		"", dwStyle, x, y, width, height, (_IControlBase::hWndParent = hWndParent),
		reinterpret_cast<HMENU>(static_cast<unsigned long long>(_IControlBase::id)), _IControlBase::hInst, nullptr)))
	{
		return false;
	}
	size_t count{ texts.size() };
	for (size_t i{ 0 }; i < count; ++i)
	{
		::SendMessageA(_IControlBase::hWnd, LB_SETITEMDATA,
			static_cast<WPARAM>(::SendMessageA(_IControlBase::hWnd, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(texts.at(i).c_str()))),
			static_cast<LPARAM>(i));
	}
	return true;
}
long long CListBox::GetIndex()
{
	if (!_IControlBase::hWnd) return -1LL;
	long long index{ static_cast<long long>(::SendMessageA(_IControlBase::hWnd, LB_GETITEMDATA,
		static_cast<WPARAM>(::SendMessageA(_IControlBase::hWnd, LB_GETCURSEL, 0, 0)), 0)) };
	return (index >= 0LL ? index : -1LL);
}
bool CListBox::IsChange(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return (_IControlBase::hWnd && (uMsg == WM_COMMAND) && (reinterpret_cast<HWND>(lParam) == _IControlBase::hWnd) && (HIWORD(wParam) == LBN_SELCHANGE));
}

CListBox::CListBox(int id) : _IControlBase{ id }
{}
CListBox::~CListBox()
{}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// CComboBox
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

bool CComboBox::Create(std::vector<std::string> texts, int x, int y, int width, int height, HWND hWndParent, DWORD dwStyle, DWORD dwExStyle)
{
	if (!_IControlBase::hInst || !_IControlBase::Destroy() || !(_IControlBase::hWnd = ::CreateWindowExA(dwExStyle, WC_COMBOBOXA,
		"", dwStyle, x, y, width, height, (_IControlBase::hWndParent = hWndParent),
		reinterpret_cast<HMENU>(static_cast<unsigned long long>(_IControlBase::id)), _IControlBase::hInst, nullptr)))
	{
		return false;
	}
	size_t count{ texts.size() };
	for (size_t i{ 0 }; i < count; ++i)
	{
		::SendMessageA(_IControlBase::hWnd, CB_SETITEMDATA,
			static_cast<WPARAM>(::SendMessageA(CComboBox::hWnd, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(texts.at(i).c_str()))),
			static_cast<LPARAM>(i));
	}
	return true;
}
long long CComboBox::GetIndex()
{
	if (!_IControlBase::hWnd) return -1LL;
	long long index{ static_cast<long long>(::SendMessageA(_IControlBase::hWnd, CB_GETITEMDATA,
		static_cast<WPARAM>(::SendMessageA(_IControlBase::hWnd, CB_GETCURSEL, 0, 0)), 0)) };
	return (index >= 0LL ? index : -1LL);
}
bool CComboBox::IsChange(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return (_IControlBase::hWnd && (uMsg == WM_COMMAND) && (reinterpret_cast<HWND>(lParam) == _IControlBase::hWnd) && (HIWORD(wParam) == CBN_SELCHANGE));
}

CComboBox::CComboBox(int id) : _IControlBase{ id }
{}
CComboBox::~CComboBox()
{}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// CTrackBar
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

bool CTrackBar::Create(FLOAT percent, int x, int y, int width, int height, HWND hWndParent,
	DWORD style, DWORD exStyle, FLOAT lineWidth, FLOAT thumbRadius,
	D2D1_COLOR_F colorBackground, D2D1_COLOR_F colorLineOut, D2D1_COLOR_F colorLineIn, D2D1_COLOR_F colorThumb,
	HMENU hMenu)
{
	if (!IWinUI::WindowCreate("", false, hWndParent, style, exStyle, x, y, width, height, hMenu)) return false;
	CTrackBar::lineWidth = lineWidth;

	CTrackBar::thumbRadius = thumbRadius;
	CTrackBar::thumbPoint.x = ((IWinUI::rectClient.right + IWinUI::rectClient.left) / 2.F);

	auto lineHalf{ static_cast<FLOAT>(IWinUI::DIPTurnPixel(CTrackBar::lineWidth / 2.F)) };
	auto radius{ static_cast<FLOAT>(IWinUI::DIPTurnPixel(CTrackBar::thumbRadius)) };
	CTrackBar::lineRect.left = (CTrackBar::thumbPoint.x - lineHalf);
	CTrackBar::lineRect.right = (CTrackBar::thumbPoint.x + lineHalf);
	CTrackBar::lineRect.top = (IWinUI::rectClient.top + radius);
	CTrackBar::lineRect.bottom = (IWinUI::rectClient.bottom - radius);

	CTrackBar::colorBackground = colorBackground;
	CTrackBar::colorLineOut = colorLineOut;
	CTrackBar::colorLineIn = colorLineIn;
	CTrackBar::colorThumb = colorThumb;

	CTrackBar::SetPercent(CTrackBar::_percent = percent);
	return true;
}

double CTrackBar::GetPercent()
{
	if (!IWinUI::hWnd) return -1.0;
	return ((CTrackBar::lineRect.bottom - CTrackBar::thumbPoint.y) / (CTrackBar::lineRect.bottom - CTrackBar::lineRect.top));
}
bool CTrackBar::SetPercent(double percent)
{
	if (!IWinUI::hWnd) return false;
	CTrackBar::thumbPoint.y = (((CTrackBar::lineRect.bottom - CTrackBar::lineRect.top) * (1.F - (CTrackBar::_percent = static_cast<FLOAT>(percent)))) + CTrackBar::lineRect.top);
	IWinUI::PaintNow();
	return true;
}

bool CTrackBar::IsChange(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return ((uMsg == WM_MOUSEMOVE) && CTrackBar::_mouseMove && (wParam & MK_LBUTTON));
}

bool CTrackBar::SourceCreate()
{
	if (FAILED(IWinUI::pTarget2d->CreateSolidColorBrush(CTrackBar::colorLineOut, &(CTrackBar::pBrushLineOut)))) return false;
	if (FAILED(IWinUI::pTarget2d->CreateSolidColorBrush(CTrackBar::colorLineIn, &(CTrackBar::pBrushLineIn)))) return false;
	if (FAILED(IWinUI::pTarget2d->CreateSolidColorBrush(CTrackBar::colorThumb, &(CTrackBar::pBrushThumb)))) return false;
	return true;
}
void CTrackBar::SourceRelease()
{
	CTrackBar::pBrushLineOut.Release();
	CTrackBar::pBrushLineIn.Release();
	CTrackBar::pBrushThumb.Release();
}
void CTrackBar::ReSize(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CTrackBar::thumbPoint.x = ((IWinUI::rectClient.right + IWinUI::rectClient.left) / 2.F);

	auto lineHalf{ static_cast<FLOAT>(IWinUI::DIPTurnPixel(CTrackBar::lineWidth / 2.F)) };
	auto radius{ static_cast<FLOAT>(IWinUI::DIPTurnPixel(CTrackBar::thumbRadius)) };
	CTrackBar::lineRect.left = (CTrackBar::thumbPoint.x - lineHalf);
	CTrackBar::lineRect.right = (CTrackBar::thumbPoint.x + lineHalf);
	CTrackBar::lineRect.top = (IWinUI::rectClient.top + radius);
	CTrackBar::lineRect.bottom = (IWinUI::rectClient.bottom - radius);

	CTrackBar::thumbPoint.y = (((CTrackBar::lineRect.bottom - CTrackBar::lineRect.top) * (1.F - CTrackBar::_percent)) + CTrackBar::lineRect.top);
}
void CTrackBar::PaintCommand(HDC hdc, PAINTSTRUCT paintStruct)
{
	IWinUI::pTarget2d->Clear(CTrackBar::colorBackground);
	//line out
	D2D1_ROUNDED_RECT roundedRect{};
	roundedRect.rect = IWinUI::PixelTurnDIP(CTrackBar::lineRect);
	roundedRect.radiusY = roundedRect.radiusX = (CTrackBar::lineWidth / 2.F);
	IWinUI::pTarget2d->FillRoundedRectangle(roundedRect, CTrackBar::pBrushLineOut);
	//line in
	roundedRect.rect.top = static_cast<FLOAT>(IWinUI::PixelTurnDIP(CTrackBar::thumbPoint.y));
	IWinUI::pTarget2d->FillRoundedRectangle(roundedRect, CTrackBar::pBrushLineIn);
	//thumb
	D2D1_ELLIPSE thumb{};
	thumb.point = IWinUI::PixelTurnDIP(CTrackBar::thumbPoint);
	thumb.radiusY = thumb.radiusX = CTrackBar::thumbRadius;
	IWinUI::pTarget2d->FillEllipse(thumb, CTrackBar::pBrushThumb);
}
LRESULT CTrackBar::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_LBUTTONDOWN:
	{
		int posX{ GET_X_LPARAM(lParam) };
		int posY{ GET_Y_LPARAM(lParam) };
		if ((::pow((posX - CTrackBar::thumbPoint.x), 2.F) + ::pow((posY - CTrackBar::thumbPoint.y), 2.F)) <= ::pow(IWinUI::DIPTurnPixel(CTrackBar::thumbRadius), 2.F))
		{
			CTrackBar::_mouseMove = true;
			if (IWinUI::hWnd) ::SetCapture(IWinUI::hWnd);
			IWinUI::PaintNow();
		}
	}break;
	case WM_MOUSEMOVE:
	{
		if (CTrackBar::_mouseMove && (wParam & MK_LBUTTON))
		{
			int posY{ GET_Y_LPARAM(lParam) };
			if (posY < CTrackBar::lineRect.top) posY = static_cast<int>(CTrackBar::lineRect.top);
			else if (posY > CTrackBar::lineRect.bottom) posY = static_cast<int>(CTrackBar::lineRect.bottom);
			if (CTrackBar::_tempPosY == posY) break;
			CTrackBar::thumbPoint.y = static_cast<FLOAT>(CTrackBar::_tempPosY = posY);
			IWinUI::PaintNow();
			if (IWinUI::hWndParent) ::PostMessageA(IWinUI::hWndParent, WM_MOUSEMOVE, wParam, lParam);
		}
	}break;
	case WM_LBUTTONUP:
	{
		if (CTrackBar::_mouseMove)
		{
			CTrackBar::_mouseMove = false;
			::ReleaseCapture();
			IWinUI::PaintNow();
		}
	}break;
	case WM_DESTROY:
	{
		CTrackBar::lineWidth = 0.F;
		CTrackBar::lineRect = D2D1_RECT_F{};

		CTrackBar::thumbRadius = 0.F;
		CTrackBar::thumbPoint = D2D1_POINT_2F{};

		CTrackBar::colorBackground = D2D1_COLOR_F{};
		CTrackBar::colorLineOut = D2D1_COLOR_F{};
		CTrackBar::colorLineIn = D2D1_COLOR_F{};
		CTrackBar::colorThumb = D2D1_COLOR_F{};

		CTrackBar::_mouseMove = false;
		CTrackBar::_tempPosY = 0;
		CTrackBar::_percent = 0.F;
	}break;
	}
	return IWinUI::HandleMessage(uMsg, wParam, lParam);
}

CTrackBar::CTrackBar() : IWinUI{},
lineWidth{}, lineRect{},
thumbRadius{}, thumbPoint{},
colorBackground{}, colorLineOut{}, colorLineIn{}, colorThumb{},
_mouseMove{ false }, _tempPosY{}, _percent{}
{}
CTrackBar::~CTrackBar()
{}