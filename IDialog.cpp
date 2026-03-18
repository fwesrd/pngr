#include "IDialog.h"

const HWND& IDialog::HDlg()
{
	return IDialog::hDlg;
}
const int& IDialog::IdDlg()
{
	return IDialog::idDlg;
}

INT_PTR IDialog::CreateMode(HWND hWndParent, LPARAM lParam)
{
	if (!IDialog::hInst) return -1;
	IDialog::lParam = lParam;
	return ::DialogBoxParamA(IDialog::hInst, MAKEINTRESOURCEA(IDialog::idDlg), hWndParent, IDialog::_DialogProc, reinterpret_cast<LPARAM>(this));
}
bool IDialog::CreateNoMode(HWND hWndParent, LPARAM lParam)
{
	if (!IDialog::hInst) return false;
	IDialog::lParam = lParam;
	if (!(IDialog::hDlg = ::CreateDialogParamA(IDialog::hInst, MAKEINTRESOURCEA(IDialog::idDlg), hWndParent, IDialog::_DialogProc, reinterpret_cast<LPARAM>(this)))) return false;
	return ::ShowWindow(IDialog::hDlg, SW_SHOW);
}

bool IDialog::EndDialog(INT_PTR result)
{
	if (!IDialog::hDlg) return true;
	if (!::EndDialog(IDialog::hDlg, result)) return false;
	IDialog::hDlg = nullptr;
	return true;
}

HWND IDialog::GetDlgItem(int idItem)
{
	return (IDialog::hDlg ? ::GetDlgItem(IDialog::hDlg, idItem) : nullptr);
}
void IDialog::SetFocus(int idItem)
{
	if (IDialog::hDlg) ::SetFocus(::GetDlgItem(IDialog::hDlg, idItem));
}

bool IDialog::EditLimit(int idEdit, size_t maxByte)
{
	if (!IDialog::hDlg) return false;
	HWND editHWnd{ ::GetDlgItem(IDialog::hDlg, idEdit) };
	return (editHWnd && SUCCEEDED(::SendMessageA(editHWnd, EM_LIMITTEXT, maxByte, 0)));
}
bool IDialog::SetWindowTextA(HWND hWnd, std::string text)
{
	return (hWnd && ::SetWindowTextA(hWnd, text.c_str()));
}
bool IDialog::GetDlgItemTextA(int idItem, _OUT_ std::string& text, int maxCount)
{
	if (!IDialog::hDlg) return false;
	std::shared_ptr<char> buffer{ new char[maxCount + 1] {} };
	if ((!buffer) || (!::GetDlgItemTextA(IDialog::hDlg, idItem, buffer.get(), maxCount + 1))) return false;
	text = buffer.get();
	return true;
}

bool IDialog::ListBoxSetData(std::vector<std::string> texts, int idListBox)
{
	if (!IDialog::hDlg) return false;
	HWND hWnd{ ::GetDlgItem(IDialog::hDlg, idListBox) };
	if (!hWnd) return false;
	size_t count{ texts.size() };
	::SendMessageA(hWnd, LB_RESETCONTENT, 0, 0);
	for (size_t i{ 0ULL }; i < count; ++i)
	{
		::SendMessageA(hWnd, LB_SETITEMDATA,
			static_cast<WPARAM>(::SendMessageA(hWnd, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(texts.at(i).c_str()))),
			static_cast<LPARAM>(i));
	}
	return true;
}
long long IDialog::ListBoxGetIndex(int idListBox)
{
	if (!IDialog::hDlg) return -1LL;
	HWND hWnd{ ::GetDlgItem(IDialog::hDlg, idListBox) };
	if (!hWnd) return -1LL;
	long long index{ static_cast<long long>(::SendMessageA(hWnd, LB_GETITEMDATA,
		static_cast<WPARAM>(::SendMessageA(hWnd, LB_GETCURSEL, 0, 0)), 0)) };
	return (index >= 0LL ? index : -1LL);
}
bool IDialog::ListBoxIsChange(int idListBox, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (!IDialog::hDlg) return false;
	HWND hWnd{ ::GetDlgItem(IDialog::hDlg, idListBox) };
	if (!hWnd) return false;
	return (hWnd && (uMsg == WM_COMMAND) && (reinterpret_cast<HWND>(lParam) == hWnd) && (HIWORD(wParam) == LBN_SELCHANGE));
}
bool IDialog::DlgDirListA(std::string dirPath, int idListBox, int idStaticText, UINT fileType)
{
	return (IDialog::hDlg && ::DlgDirListA(IDialog::hDlg, const_cast<LPSTR>(dirPath.c_str()), idListBox, idStaticText, fileType));
}
std::string IDialog::DlgDirSelectExA(int idListBox, _OUT_ bool& isDir)
{
	if (!IDialog::hDlg) return std::string{ "" };
	char buffer[MAX_PATH]{};
	isDir = ::DlgDirSelectExA(IDialog::hDlg, buffer, MAX_PATH, idListBox);
	return std::string{ buffer };
}
std::string IDialog::DlgDirSelectExA(int idListBox)
{
	bool isDir{};
	return IDialog::DlgDirSelectExA(idListBox, isDir);
}

INT_PTR IDialog::DialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return 0LL;
}

INT_PTR IDialog::_DialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	IDialog* pThis{ nullptr };
	if (uMsg == WM_INITDIALOG)
	{
		if (pThis = reinterpret_cast<IDialog*>(lParam))
		{
			pThis->IDialog::hDlg = hDlg;
			::SetWindowLongPtrA(hDlg, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
			return pThis->DialogProc(hDlg, uMsg, wParam, pThis->IDialog::lParam);
		}
	}
	else
	{
		if (pThis = reinterpret_cast<IDialog*>(::GetWindowLongPtrA(hDlg, GWLP_USERDATA)))
		{
			return pThis->DialogProc(hDlg, uMsg, wParam, lParam);
		}
	}
	return 0LL;
}

IDialog::IDialog(int idDlg) : hInst{ ::GetModuleHandleA(nullptr) }, hDlg{ nullptr }, idDlg{ idDlg }, lParam{}
{}
IDialog::~IDialog()
{}