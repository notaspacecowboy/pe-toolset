#include "label.h"

void Label::SetText(const std::wstring& text)
{
	SetWindowText(m_handle_to_control, text.c_str());
}

std::wstring Label::GetText()
{
	int length = GetWindowTextLength(m_handle_to_control);
	if (length == 0)
		return L"";

	WCHAR* buffer = new WCHAR[length + 1];
	GetWindowText(m_handle_to_control, buffer, length + 1);

	std::wstring result = buffer;
	delete[] buffer;
	return result;
}


