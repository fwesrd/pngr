#pragma once
#ifndef _IDIALOG_H_
#define _IDIALOG_H_

#include "include.h"

class IDialog
{
protected:
	HINSTANCE hInst;
	HWND hDlg;
	int idDlg;
	LPARAM lParam;

public:
	const HWND& HDlg();
	const int& IdDlg();

	INT_PTR CreateMode(HWND hWndParent, LPARAM lParam = LPARAM{}); //return: param "result" in EndDialog, 0(hWndParent unvalid), -1(fail)
	bool CreateNoMode(HWND hWndParent, LPARAM lParam = LPARAM{}); //no block

protected:
	bool EndDialog(INT_PTR result); //CreateMode return param "result"

	//child control
	HWND GetDlgItem(int idItem);
	void SetFocus(int idItem);
	//edit
	bool EditLimit(int idEdit, size_t maxByte); //send EM_LIMITTEXT, maxByte 0 is unlimit
	bool SetWindowTextA(HWND hWnd, std::string text); //send WM_SETTEXT
	bool GetDlgItemTextA(int idItem, _OUT_ std::string& text, int maxCount); //send WM_GETTEXT, maxCount no include '\0'
	//listbox
	bool ListBoxSetData(std::vector<std::string> texts, int idListBox);
	long long ListBoxGetIndex(int idListBox);
	bool ListBoxIsChange(int idListBox, UINT uMsg, WPARAM wParam, LPARAM lParam);
	bool DlgDirListA(std::string dirPath, int idListBox, int idStaticText = 0, UINT fileType = 0);
	std::string DlgDirSelectExA(int idListBox, _OUT_ bool& isDir); //return listbox select path
	std::string DlgDirSelectExA(int idListBox);

protected:
	_VIRTUAL_EMPTY_ virtual INT_PTR DialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

private: //unuse
	static INT_PTR _DialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	IDialog(int id);
	virtual ~IDialog();
};

#endif // !_IDIALOG_H_