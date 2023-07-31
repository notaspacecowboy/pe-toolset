#include "input_field.h"

#include "log.h"


void InputField::HandleEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
    if (HIWORD(wParam) == EN_KILLFOCUS)
    {
		auto text = GetText();
		for(int i = 0; i < m_call_backs.size(); i++)
			m_call_backs[i](text);
    }
}

void InputField::AddUserInputListener(InputFieldEventCallBack&& call_back)
{
	m_call_backs.push_back(call_back);
}

void InputField::ClearUserInputListeners()
{
	m_call_backs.clear();
}

void InputField::SetText(const std::wstring& text)
{
    SetWindowTextW(m_handle_to_control, text.c_str());
}


std::wstring InputField::GetText() const
{
	int text_length = GetWindowTextLengthW(m_handle_to_control);
	std::wstring text(text_length, L'\0');
	GetWindowTextW(m_handle_to_control, (LPWSTR)text.data(), text_length + 1);
	return text;
}
