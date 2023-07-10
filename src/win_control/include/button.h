#pragma once
#include <functional>
#include <vector>

#include "control.h"


class Button: public Control
{
	using ButtonEventCallBack = std::function<void()>;

public:
	Button(HWND handle_to_control): Control(handle_to_control) {}
	~Button() override = default;
	void AddButtonClickListener(ButtonEventCallBack&& call_back);
	void ClearButtonClickListeners();

	void HandleEvent(UINT message, WPARAM wParam, LPARAM lParam) override;	

private:
	std::vector<ButtonEventCallBack> m_call_backs;
};
