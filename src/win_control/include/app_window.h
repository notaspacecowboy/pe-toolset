#pragma once
#include <unordered_map>

#include "windows.h"
#include "eventcenter.h"

class AppWindow
{
public:
	AppWindow(HWND handle_to_window);
	virtual ~AppWindow() = default;
	virtual void OnCreate() = 0;
	virtual void OnClose() = 0;
	INT_PTR CALLBACK WindowEventProcess(UINT message, WPARAM wParam, LPARAM lParam);

	HWND handle_to_window() const { return m_handle_to_window; }

protected:
	void Close();

protected:
	HWND m_handle_to_window = nullptr;
	EventCenter<DWORD> m_event_center;
};
