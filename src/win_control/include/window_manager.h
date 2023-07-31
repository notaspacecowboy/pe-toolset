#pragma once
#include <memory>
#include <strsafe.h>
#include <unordered_map>
#include <windows.h>

#include "singleton.h"
#include "app_window.h"


class WindowManager: public Singleton<WindowManager>
{
	friend class Singleton<WindowManager>;

public:
	void Init(HINSTANCE handle_to_app);

	template<typename TWindow, typename ...Args>
	void CreateAppWindow(int window_id, Args&&... args);

	void DestroyAppWindow(AppWindow* window);
	
	std::unordered_map<HWND, std::unique_ptr<AppWindow>>& active_windows() { return m_active_windows; }

private:
	WindowManager() = default;

private:
	HINSTANCE m_handle_to_app = nullptr;
	std::unordered_map<HWND, std::unique_ptr<AppWindow>> m_active_windows;

private:
	static INT_PTR CALLBACK WindowEventProcess(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

template<typename TWindow, typename ...Args>
void WindowManager::CreateAppWindow(int window_id, Args&&... args)
{
	HWND hDialog;
	hDialog = CreateDialog(m_handle_to_app, MAKEINTRESOURCE(window_id), NULL, (DLGPROC)WindowEventProcess);
	m_active_windows[hDialog] = std::make_unique<TWindow>(hDialog, args...);
	m_active_windows[hDialog]->OnCreate();

	ShowWindow(hDialog, SW_SHOW);
	UpdateWindow(hDialog);
}
