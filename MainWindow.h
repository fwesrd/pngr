#pragma once
#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_

#include "C:\\MyDirC\\Code\\cpp\\usetool\\usetool7.1\\usetool.h"
#include "resource.h"
#include "ControlDialog.h"

class MainWindow : protected IWinUI
{
protected:
	Worker worker;
	ControlDialog controlDialog;

public:
	bool Start()
	{
		if (!IWinUI::WindowCreate("", true, nullptr, (WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN/* | WS_MAXIMIZE*/),
			0U, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
			::LoadMenuA(IWinUI::hInst, MAKEINTRESOURCEA(IDR_MENU_MAIN)))) return false;
		IWinUI::SynchronStart(10000.0);
		IWinUI::PeekLoop();
		return true;
	}

protected:
	void SynchronCalculate(std::chrono::steady_clock::time_point lastFrameTime, std::chrono::steady_clock::time_point nextFrameTime) override
	{
		IWinUI::ClearData3D();
		MainWindow::worker.CalculateAndAddDataInDraw(lastFrameTime, nextFrameTime, static_cast<UINT>(IWinUI::widthClient), static_cast<UINT>(IWinUI::heightClient));
	}
	void SynchronDraw(std::chrono::steady_clock::time_point lastPresentTime, std::chrono::steady_clock::time_point nowPresentTime) override
	{
		TESTA("draw");
		IWinUI::ClearFrame3D();
		if (MainWindow::worker.LightGetIs())
		{
			MainWindow::worker.LightDraw();
		}
		else
		{
			IWinUI::Draw3D();
		}
		IWinUI::Present3D();
	}

	void WmSize(UINT uMsg, WPARAM wParam, LPARAM lParam) override
	{
		MainWindow::worker.WmSize(static_cast<UINT>(IWinUI::widthClient), static_cast<UINT>(IWinUI::heightClient));
	}
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override
	{
		switch (uMsg)
		{
		case WM_CREATE:
		{
			if (!MainWindow::worker.Init())
			{
				::MessageBoxA(IWinUI::hWnd, "×ÅÉ«Æ÷È±Ê§!", "MainWindow", (MB_OK | MB_ICONERROR));
				::exit(-1);
			}
			//worker.KerrChange(true, 0, 0, 0, 0, 0, false, 0);
			//worker.LightSetIs(true);

			BodyManager::BodyStruct body8{ 8U,1E3,1E7,0,0,0,0,0,1E5,1,0,0 };
			MainWindow::worker.BodyAdd(body8);

			//worker.KerrChange(false, 0, 0, 0, 0, 0, false, 0);
			//worker.LightSetIs(false);



		} break;
		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				//file
			case ID_MENU_LEAD_OUT:
				MainWindow::LeadOut();
				break;
			case ID_MENU_LEAD_IN:
				MainWindow::LeadIn();
				break;
				//control
			case ID_MENU_CONTROL:
				MainWindow::Control();
				break;
			}
		} break;
		}
		return IWinUI::HandleMessage(uMsg, wParam, lParam);
	}

protected:
	void LeadOut()
	{
		TESTB("leadout");
	}
	void LeadIn()
	{
		TESTB("leadin");
	}

	void Control()
	{
		if (!MainWindow::controlDialog.HDlg())
		{
			MainWindow::controlDialog.Create(IWinUI::hWnd);
		}
	}

public:
	MainWindow() : IWinUI{ (UINT)4.2E8, 610000U, 610000U, 610000U, 610000U,
	 IWinUI::Position{ 0.F, 0.F, -1.F }, 90, 0, 0.1F, 0.2F, 45.F, EPS, 1E18F },
		worker { this }, controlDialog{ this, MainWindow::worker }
	{}
};


#endif // !_MAINWINDOW_H_