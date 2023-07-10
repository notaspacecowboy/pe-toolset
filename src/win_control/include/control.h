#pragma once
#include "windows.h"

class Control
{
public:
	Control(HWND handle_to_control) : m_handle_to_control(handle_to_control) {}
	virtual ~Control() = default;
	Control(Control&&) = default;
	Control& operator=(Control&&) = default;
	virtual void HandleEvent(UINT message, WPARAM wParam, LPARAM lParam) {}

protected:
	HWND m_handle_to_control = nullptr;
};
