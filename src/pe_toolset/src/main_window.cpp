#include "main_window.h"
#include "app.h"
#include "resource.h"
#include "log.h"


MainWindow::MainWindow(HWND handle_to_window)
	: AppWindow(handle_to_window)
{

}

void MainWindow::OnCreate()
{
	m_event_center.RegisterCallback(IDC_MAIN_PROCESS_VIEWER_BTN, [this]()
	{
		this->OnProcessViewBtnClick();
	});

	m_event_center.RegisterCallback(IDC_MAIN_DLL_INJECTION_BTN, [this]()
	{
		this->OnDllInjectionBtnClick();
	});

	m_event_center.RegisterCallback(IDC_MAIN_EXE_PACK_BTN, [this]()
	{
		this->OnExePackBtnClick();
	});

	m_event_center.RegisterCallback(IDC_MAIN_ABOUT_BTN, [this]()
	{
		this->OnAboutBtnClick();
	});

	m_event_center.RegisterCallback(IDC_MAIN_EXIT_BTN, [this]()
	{
		this->OnExitBtnClick();
	});
}


void MainWindow::OnClose()
{
	App::instance().Terminate();
}






void MainWindow::OnProcessViewBtnClick()
{
	Log::DebugPrintf(TEXT("Process view button clicked\n"));
}

void MainWindow::OnDllInjectionBtnClick()
{
	Log::DebugPrintf(TEXT("DLL injection button clicked\n"));
}

void MainWindow::OnExePackBtnClick()
{
	Log::DebugPrintf(TEXT("EXE pack button clicked\n"));
}

void MainWindow::OnAboutBtnClick()
{
	Log::DebugPrintf(TEXT("About button clicked\n"));
}

void MainWindow::OnExitBtnClick()
{
	Close();
}

