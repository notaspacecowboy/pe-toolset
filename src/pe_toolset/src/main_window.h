#pragma once
#include "app_window.h"

class MainWindow: public AppWindow
{
public:
	MainWindow(HWND handle_to_window);
	~MainWindow() override = default;

	void OnCreate() override;
	void OnClose() override;

private:
	void OnProcessViewBtnClick();
	void OnDllInjectionBtnClick();
	void OnExePackBtnClick();
	void OnAboutBtnClick();
	void OnExitBtnClick();

	void InitListViews();

private:
	HWND m_process_list_view = nullptr;
	HWND m_module_list_view = nullptr;
};
