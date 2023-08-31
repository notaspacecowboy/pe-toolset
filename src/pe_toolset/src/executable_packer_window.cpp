#include "executable_packer_window.h"
#include "button.h"
#include "file_browser.h"
#include "input_field.h"
#include "label.h"
#include "log.h"
#include "resource.h"
#include "pe_modifier.h"

ExecutablePackerWindow::ExecutablePackerWindow(HWND handle_to_window)
	: AppWindow(handle_to_window)
	, m_executable_packer_btn(GetControl<Button>(IDC_EXECUTABLE_BTN_PACKER_PATH))
	, m_source_executable_btn(GetControl<Button>(IDC_EXECUTABLE_BTN_SOURCE_PATH))
	, m_destination_executable_btn(GetControl<Button>(IDC_EXECUTABLE_BTN_DESTINATION_PATH))
	, m_start_packing_btn(GetControl<Button>(IDC_EXECUTABLE_BTN_START_PACKING))
	, m_executable_packer_label(GetControl<Label>(IDC_EXECUTABLE_LABEL_PACKER_PATH))
	, m_source_executable_label(GetControl<Label>(IDC_EXECUTABLE_LABEL_SOURCE_PATH))
	, m_destination_executable_label(GetControl<Label>(IDC_EXECUTABLE_LABEL_DESTINATION_PATH))
{
}


void ExecutablePackerWindow::OnCreate()
{
	m_executable_packer_btn->AddButtonClickListener([this]()
		{
			this->OnExecutablePackerBtnClicked();
		});

	m_source_executable_btn->AddButtonClickListener([this]()
		{
			this->OnSourceExecutableBtnClicked();
		});

	m_destination_executable_btn->AddButtonClickListener([this]()
		{
			this->OnDestinationExecutableBtnClicked();
		});

	m_start_packing_btn->AddButtonClickListener([this]()
		{
			this->OnStartPackingBtnClicked();
		});
}


void ExecutablePackerWindow::OnClose()
{
}


void ExecutablePackerWindow::OnExecutablePackerBtnClicked()
{
	auto result = FileBrowser::OpenFile(this, L"Executable\0*.exe\0");
	if (result.length() == 0)
		return;
	m_executable_packer_label->SetText(result);
}


void ExecutablePackerWindow::OnSourceExecutableBtnClicked()
{
	auto result = FileBrowser::OpenFile(this, L"Executable\0*.exe\0");
	if (result.length() == 0)
		return;
	m_source_executable_label->SetText(result);
}


void ExecutablePackerWindow::OnDestinationExecutableBtnClicked()
{
	auto result = FileBrowser::OpenFile(this, L"Executable\0*.exe\0");
	if (result.length() == 0)
		return;
	m_destination_executable_label->SetText(result);
}


void ExecutablePackerWindow::OnStartPackingBtnClicked()
{
	auto executable_packer_path = m_executable_packer_label->GetText();
	auto source_executable_path = m_source_executable_label->GetText();
	auto destination_executable_path = m_destination_executable_label->GetText();

	if(!CheckIfPathExists(executable_packer_path) || !CheckIfPathExists(source_executable_path) || !CheckIfPathExists(destination_executable_path))
	{
		MessageBox(nullptr, L"One or more of the file paths are invalid, please double check!", L"Error", MB_OK);
		return;
	}

	PEModifier pe_modifier;

}


bool ExecutablePackerWindow::CheckIfPathExists(const std::wstring& path)
{
	DWORD attributes = GetFileAttributes(path.c_str());
	if (attributes == INVALID_FILE_ATTRIBUTES)
		return false;

	return true;
}
