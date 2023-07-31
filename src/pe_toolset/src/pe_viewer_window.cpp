#include "pe_viewer_window.h"
#include "button.h"
#include "input_field.h"
#include "log.h"
#include "resource.h"
#include <sstream>
#include <ctime>
#include <iomanip>

#include "directory_window.h"
#include "section_window.h"
#include "window_manager.h"

PEViewerWindow::PEViewerWindow(HWND handle_to_window, const std::wstring& file_path)
	: AppWindow(handle_to_window)
	, m_exit_btn(GetControl<Button>(IDC_PE_VIEWER_EXIT_BUTTON))
	, m_data_directory_btn(GetControl<Button>(IDC_PE_VIEWER_DATA_DIR_BUTTON))
	, m_section_table_btn(GetControl<Button>(IDC_PE_VIEWER_SECTION_BUTTON))
	, m_machine_number_input(GetControl<InputField>(IDC_PE_VIEWER_MACHINE_NUMBER))
	, m_pe_header_offset_input(GetControl<InputField>(IDC_PE_VIEWER_PE_HEADER_OFFSET))
	, m_time_date_stamp_input(GetControl<InputField>(IDC_PE_VIEWER_TIMESTAMP))
	, m_number_of_sections_input(GetControl<InputField>(IDC_PE_VIEWER_NUMBER_OF_SECTIONS))
	, m_size_of_optional_header_input(GetControl<InputField>(IDC_PE_VIEWER_OH_SIZE))
	, m_characteristics_input(GetControl<InputField>(IDC_PE_VIEWER_CHARACTERISTICS))
	, m_entry_point_input(GetControl<InputField>(IDC_PE_VIEWER_ENTRY_POINT))
	, m_image_base_input(GetControl<InputField>(IDC_PE_VIEWER_IMAGE_BASE))
	, m_section_alignment_input(GetControl<InputField>(IDC_PE_VIEWER_SECTION_ALIGNMENT))
	, m_file_alignment_input(GetControl<InputField>(IDC_PE_VIEWER_FILE_ALIGNMENT))
	, m_size_of_image_input(GetControl<InputField>(IDC_PE_VIEWER_IMAGE_SIZE))
	, m_size_of_headers_input(GetControl<InputField>(IDC_PE_VIEWER_HEADER_SIZE))
	, m_checksum_input(GetControl<InputField>(IDC_PE_VIEWER_CHECKSUM))
	, m_dll_characteristics_input(GetControl<InputField>(IDC_PE_VIEWER_DLL_CHARACTERISTICS))
	, m_file_path(file_path)
	, m_pe_modifier(std::make_unique<PEModifier>())
{
}

void PEViewerWindow::OnCreate()
{
	m_exit_btn->AddButtonClickListener([this]()
	{
		this->OnExitBtnClick();
	});

	m_data_directory_btn->AddButtonClickListener([this]()
	{
				this->OnDataDirectoryBtnClick();
	});

	m_section_table_btn->AddButtonClickListener([this]()
	{
		this->OnSectionTableBtnClick();
	});

	//load pe modifier
	m_pe_modifier->LoadPEFile(m_file_path);
	RefreshPEData();
}

void PEViewerWindow::OnClose()
{
}


void PEViewerWindow::OnExitBtnClick()
{
	Close();
}

void PEViewerWindow::OnDataDirectoryBtnClick()
{
	WindowManager::instance().CreateAppWindow<DirectoryWindow>(IDD_DIRECTORY_WINDOW, m_pe_modifier.get());
}


void PEViewerWindow::OnSectionTableBtnClick()
{
	WindowManager::instance().CreateAppWindow<SectionWindow>(IDD_SECTION_WINDOW, m_pe_modifier.get());
}


void PEViewerWindow::RefreshPEData()
{
	std::wstringstream ss;
	ss << std::hex << std::setw(sizeof(IMAGE_FILE_HEADER::Machine) * 2) << std::setfill(L'0') << m_pe_modifier->file_header().Machine;
	m_machine_number_input->SetText(ss.str());

	ss.str(L"");
	ss << std::hex << std::setw(sizeof(IMAGE_DOS_HEADER::e_lfanew) * 2) << std::setfill(L'0') << m_pe_modifier->dos_header().e_lfanew;
	m_pe_header_offset_input->SetText(ss.str());

	auto timestamp = static_cast<std::time_t>(m_pe_modifier->file_header().TimeDateStamp);
	wchar_t time_buffer[100];
	std::tm tm_result;
	localtime_s(&tm_result, &timestamp);
	std::wcsftime(time_buffer, sizeof(time_buffer) / sizeof(wchar_t), L"%Y-%m-%d %H:%M:%S", &tm_result);
	m_time_date_stamp_input->SetText(time_buffer);

	ss.str(L"");
	ss << std::dec << m_pe_modifier->file_header().NumberOfSections;
	m_number_of_sections_input->SetText(ss.str());

	ss.str(L"");
	ss << std::hex << std::setw(sizeof(IMAGE_FILE_HEADER::SizeOfOptionalHeader) * 2) << std::setfill(L'0') << m_pe_modifier->file_header().SizeOfOptionalHeader;
	m_size_of_optional_header_input->SetText(ss.str());

	ss.str(L"");
	ss << std::hex << std::setw(sizeof(IMAGE_FILE_HEADER::Characteristics) * 2) << std::setfill(L'0') << m_pe_modifier->file_header().Characteristics;
	m_characteristics_input->SetText(ss.str());

	ss.str(L"");
	ss << std::hex << std::setw(sizeof(IMAGE_OPTIONAL_HEADER::AddressOfEntryPoint) * 2) << std::setfill(L'0') << m_pe_modifier->optional_header().AddressOfEntryPoint;
	m_entry_point_input->SetText(ss.str());

	ss.str(L"");
	ss << std::hex << std::setw(sizeof(IMAGE_OPTIONAL_HEADER::ImageBase) * 2) << std::setfill(L'0') << m_pe_modifier->optional_header().ImageBase;
	m_image_base_input->SetText(ss.str());

	ss.str(L"");
	ss << std::hex << std::setw(sizeof(IMAGE_OPTIONAL_HEADER::SectionAlignment) * 2) << std::setfill(L'0') << m_pe_modifier->optional_header().SectionAlignment;
	m_section_alignment_input->SetText(ss.str());

	ss.str(L"");
	ss << std::hex << std::setw(sizeof(IMAGE_OPTIONAL_HEADER::FileAlignment) * 2) << std::setfill(L'0') << m_pe_modifier->optional_header().FileAlignment;
	m_file_alignment_input->SetText(ss.str());

	ss.str(L"");
	ss << std::hex << std::setw(sizeof(IMAGE_OPTIONAL_HEADER::SizeOfImage) * 2) << std::setfill(L'0') << m_pe_modifier->optional_header().SizeOfImage;
	m_size_of_image_input->SetText(ss.str());

	ss.str(L"");
	ss << std::hex << std::setw(sizeof(IMAGE_OPTIONAL_HEADER::SizeOfHeaders) * 2) << std::setfill(L'0') << m_pe_modifier->optional_header().SizeOfHeaders;
	m_size_of_headers_input->SetText(ss.str());

	ss.str(L"");
	ss << std::hex << std::setw(sizeof(IMAGE_OPTIONAL_HEADER::CheckSum) * 2) << std::setfill(L'0') << m_pe_modifier->optional_header().CheckSum;
	m_checksum_input->SetText(ss.str());

	ss.str(L"");
	ss << std::hex << std::setw(sizeof(IMAGE_OPTIONAL_HEADER::DllCharacteristics) *2) << std::setfill(L'0') << m_pe_modifier->optional_header().DllCharacteristics;
	m_dll_characteristics_input->SetText(ss.str());
}
