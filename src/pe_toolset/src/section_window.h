#pragma once
#include "app_window.h"

class ListView;
class Button;
class PEModifier;



class SectionWindow : public AppWindow
{
public:
	SectionWindow(HWND handle_to_window, PEModifier* pe_modifier);
	~SectionWindow() override = default;

	void OnCreate() override;
	void OnClose() override;

private:
	void OnCloseBtnClick();
	void RefreshList();

private:
	PEModifier* m_pe_modifier;

	ListView* m_section_list_view;
	Button* m_close_btn;
};
