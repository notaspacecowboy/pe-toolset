#include "window_manager.h"
#include "log.h"


void WindowManager::Init(HINSTANCE handle_to_app)
{
	m_handle_to_app = handle_to_app;
}


void WindowManager::DestroyAppWindow(AppWindow* window)
{
	auto& all_windows = WindowManager::instance().m_active_windows;
	auto handle_to_window = window->handle_to_window();
	if (all_windows.find(handle_to_window) == all_windows.end())
		return;

	window->OnClose();
	EndDialog(handle_to_window, 0);
	all_windows.erase(handle_to_window);
}


void WindowManager::GetAllActiveWindows(std::list<AppWindow*>& list)
{
	list.clear();
	for(auto& pair: WindowManager::instance().m_active_windows)
		list.push_back(pair.second.get());
}



INT_PTR WindowManager::WindowEventProcess(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_INITDIALOG)
		Log::DebugPrintf(L"new dialog created");

	auto& all_windows = WindowManager::instance().m_active_windows;
	if (all_windows.find(hWnd) == all_windows.end())
		return DefWindowProc(hWnd, message, wParam, lParam);

	all_windows[hWnd]->WindowEventProcess(message, wParam, lParam);
}

