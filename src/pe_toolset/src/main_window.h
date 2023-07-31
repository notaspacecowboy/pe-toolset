#pragma once
#include "app_window.h"
#include "button.h"
#include "list_view.h"

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
	void OnProcessListItemSelect(int index);

	void RefreshListViews();

private:
	ListView* m_process_list_view;
	ListView* m_module_list_view;
	Button* m_process_view_btn;
	Button* m_dll_injection_btn;
	Button* m_exe_pack_btn;
	Button* m_about_btn;
	Button* m_exit_btn;

	int m_selected_index;
};
