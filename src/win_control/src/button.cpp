#include "button.h"




void Button::HandleEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
	for (auto func : m_call_backs)
	{
		func();
	}
}

void Button::AddButtonClickListener(ButtonEventCallBack&& call_back)
{
	m_call_backs.push_back(call_back);
}

void Button::ClearButtonClickListeners()
{
	m_call_backs.clear();
}

