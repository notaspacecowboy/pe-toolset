#pragma once
#include "windows.h"

class Control
{
public:
	Control(HWND handle_to_control);
	virtual ~Control() = default;
	Control(Control&&) = default;
	Control& operator=(Control&&) = default;

private:
	HWND handle_to_control = nullptr;
};
