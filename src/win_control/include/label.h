#pragma once
#include <string>

#include "control.h"


class Label : public Control
{
public:
	Label(HWND handle_to_control) : Control(handle_to_control) {}
	~Label() override = default;

	std::wstring GetText();
	void SetText(const std::wstring& text);
};
