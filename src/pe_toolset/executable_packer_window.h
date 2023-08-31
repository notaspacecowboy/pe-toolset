#pragma once
#include <string>
#include "app_window.h"


class Button;
class InputField;
class Label;

class ExecutablePackerWindow: public AppWindow
{
public:
	ExecutablePackerWindow(HWND handle_to_window);
	~ExecutablePackerWindow() override = default;

	void OnCreate() override;
	void OnClose() override;

private:
	void OnExecutablePackerBtnClicked();
	void OnSourceExecutableBtnClicked();
	void OnDestinationExecutableBtnClicked();
	void OnStartPackingBtnClicked();
	bool CheckIfPathExists(const std::wstring& path);


private:
	Button* m_executable_packer_btn;
	Button* m_source_executable_btn;
	Button* m_destination_executable_btn;
	Button* m_start_packing_btn;
	Label*	m_executable_packer_label;
	Label*	m_source_executable_label;
	Label*	m_destination_executable_label;
};