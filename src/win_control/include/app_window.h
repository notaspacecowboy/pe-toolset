#pragma once
#include <unordered_map>

#include "windows.h"
#include "eventcenter.h"
#include "cassert"
#include "control.h"

class AppWindow
{
public:
	AppWindow(HWND handle_to_window);
	virtual ~AppWindow() = default;
	virtual void OnCreate() = 0;
	virtual void OnClose() = 0;
	INT_PTR CALLBACK WindowEventProcess(UINT message, WPARAM wParam, LPARAM lParam);
	template<typename TControl>
	TControl* GetControl(DWORD control_id);

	HWND handle_to_window() const { return m_handle_to_window; }

protected:
	void Close();

protected:
	HWND m_handle_to_window = nullptr;
	std::unordered_map<DWORD, std::unique_ptr<Control>> m_controls;
};


template<typename TControl>
TControl* AppWindow::GetControl(DWORD control_id)
{
	HWND handle_to_control = GetDlgItem(m_handle_to_window, control_id);
	assert(handle_to_control != nullptr);
	auto control = new TControl(handle_to_control);
	if(m_controls.find(control_id) == m_controls.end())
		m_controls[control_id] = std::unique_ptr<Control>(control);

	return control;
}