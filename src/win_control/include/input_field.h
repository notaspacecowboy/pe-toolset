#pragma once
#include <functional>
#include <string>
#include <vector>
#include "control.h"


class InputField : public Control
{
	using InputFieldEventCallBack = std::function<void(std::wstring)>;

public:
	InputField(HWND handle_to_control) : Control(handle_to_control) {}
	~InputField() override = default;
	void AddUserInputListener(InputFieldEventCallBack&& call_back);
	void ClearUserInputListeners();
	void HandleEvent(UINT message, WPARAM wParam, LPARAM lParam) override;
	void SetText(const std::wstring& text);
	std::wstring GetText() const;

private:
	std::vector<InputFieldEventCallBack> m_call_backs;
};
