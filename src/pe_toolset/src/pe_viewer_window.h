#pragma once
#include <string>
#include "app_window.h"
#include <pe_modifier.h>

class Button;
class InputField;

class PEViewerWindow : public AppWindow
{
public:
	PEViewerWindow(HWND handle_to_window, const std::wstring& file_path);
	~PEViewerWindow() override = default;

	void OnCreate() override;
	void OnClose() override;

	void RefreshPEData();

private:
	void OnExitBtnClick();
	void OnDataDirectoryBtnClick();
	void OnSectionTableBtnClick();

private:
	Button* m_exit_btn;
	Button* m_data_directory_btn;
	Button* m_section_table_btn;
	InputField* m_machine_number_input;
	InputField* m_pe_header_offset_input;
	InputField* m_time_date_stamp_input;
	InputField* m_number_of_sections_input;
	InputField* m_size_of_optional_header_input;
	InputField* m_characteristics_input;
	InputField* m_entry_point_input;
	InputField* m_image_base_input;
	InputField* m_section_alignment_input;
	InputField* m_file_alignment_input;
	InputField* m_size_of_image_input;
	InputField* m_size_of_headers_input;
	InputField* m_checksum_input;
	InputField* m_dll_characteristics_input;

	std::wstring m_file_path;
	std::unique_ptr<PEModifier> m_pe_modifier;
};
