#include "app_window.h"
#include "window_manager.h"

AppWindow::AppWindow(HWND handle_to_window)
	: m_handle_to_window(handle_to_window)
{
}


INT_PTR CALLBACK AppWindow::WindowEventProcess(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_COMMAND:
		case WM_NOTIFY:
		{
			auto control_id = LOWORD(wParam);
			if(m_controls.find(control_id) == m_controls.end())
				return (INT_PTR)FALSE;
			auto control = m_controls[control_id].get();
			control->HandleEvent(message, wParam, lParam);
			return (INT_PTR)TRUE;
		}

		case WM_CLOSE:
		{
			Close();
			return (INT_PTR)TRUE;
		}
		}

		return (INT_PTR)FALSE;
}


void AppWindow::Close()
{
	WindowManager::instance().DestroyAppWindow(this);
}
