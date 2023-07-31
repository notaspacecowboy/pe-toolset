#include "directory_window.h"
#include "input_field.h"
#include "button.h"
#include "pe_modifier.h"
#include "resource.h"

DirectoryWindow::DirectoryWindow(HWND handle_to_window, PEModifier* pe_modifier)
	: AppWindow(handle_to_window)
	, m_pe_modifier(pe_modifier)
	, m_exit_btn(GetControl<Button>(IDC_DIRECTORY_EXIT_BTN))
	, m_import_table_rva(GetControl<InputField>(IDC_DIRECTORY_IMPORT_TABLE_RVA))
	, m_import_table_size(GetControl<InputField>(IDC_DIRECTORY_IMPORT_TABLE_SIZE))
	, m_export_table_rva(GetControl<InputField>(IDC_DIRECTORY_EXPORT_TABLE_RVA))
	, m_export_table_size(GetControl<InputField>(IDC_DIRECTORY_EXPORT_TABLE_SIZE))
	, m_resource_table_rva(GetControl<InputField>(IDC_DIRECTORY_RESOURCE_TABLE_RVA))
	, m_resource_table_size(GetControl<InputField>(IDC_DIRECTORY_RESOURCE_TABLE_SIZE))
	, m_exception_table_rva(GetControl<InputField>(IDC_DIRECTORY_EXCEPTION_TABLE_RVA))
	, m_exception_table_size(GetControl<InputField>(IDC_DIRECTORY_EXCEPTION_TABLE_SIZE))
	, m_security_table_rva(GetControl<InputField>(IDC_DIRECTORY_SECURITY_TABLE_RVA))
	, m_security_table_size(GetControl<InputField>(IDC_DIRECTORY_SECURITY_TABLE_SIZE))
	, m_base_relocation_table_rva(GetControl<InputField>(IDC_DIRECTORY_BASE_RELOCATION_TABLE_RVA))
	, m_base_relocation_table_size(GetControl<InputField>(IDC_DIRECTORY_BASE_RELOCATION_TABLE_SIZE))
	, m_debug_table_rva(GetControl<InputField>(IDC_DIRECTORY_DEBUG_TABLE_RVA))
	, m_debug_table_size(GetControl<InputField>(IDC_DIRECTORY_DEBUG_TABLE_SIZE))
	, m_asd_table_rva(GetControl<InputField>(IDC_DIRECTORY_ASD_TABLE_RVA))
	, m_asd_table_size(GetControl<InputField>(IDC_DIRECTORY_ASD_TABLE_SIZE))
	, m_gpr_table_rva(GetControl<InputField>(IDC_DIRECTORY_GPR_TABLE_RVA))
	, m_gpr_table_size(GetControl<InputField>(IDC_DIRECTORY_GPR_TABLE_SIZE))
	, m_tls_table_rva(GetControl<InputField>(IDC_DIRECTORY_TLS_TABLE_RVA))
	, m_tls_table_size(GetControl<InputField>(IDC_DIRECTORY_TLS_TABLE_SIZE))
	, m_lct_table_rva(GetControl<InputField>(IDC_DIRECTORY_LCT_TABLE_RVA))
	, m_lct_table_size(GetControl<InputField>(IDC_DIRECTORY_LCT_TABLE_SIZE))
	, m_bound_import_table_rva(GetControl<InputField>(IDC_DIRECTORY_BIT_TABLE_RVA))
	, m_bound_import_table_size(GetControl<InputField>(IDC_DIRECTORY_BIT_TABLE_SIZE))
	, m_iat_table_rva(GetControl<InputField>(IDC_DIRECTORY_IAT_TABLE_RVA))
	, m_iat_table_size(GetControl<InputField>(IDC_DIRECTORY_IAT_TABLE_SIZE))
	, m_delay_import_table_rva(GetControl<InputField>(IDC_DIRECTORY_DELAY_IMPORT_TABLE_RVA))
	, m_delay_import_table_size(GetControl<InputField>(IDC_DIRECTORY_DELAY_IMPORT_TABLE_SIZE))
	, m_clr_table_rva(GetControl<InputField>(IDC_DIRECTORY_CLR_TABLE_RVA))
	, m_clr_table_size(GetControl<InputField>(IDC_DIRECTORY_CLR_TABLE_SIZE))
	, m_reserved_table_rva(GetControl<InputField>(IDC_DIRECTORY_RESERVED_TABLE_RVA))
	, m_reserved_table_size(GetControl<InputField>(IDC_DIRECTORY_RESERVED_TABLE_SIZE))
{
}

void DirectoryWindow::OnCreate()
{
	m_exit_btn->AddButtonClickListener([this]()
	{
		this->OnExitBtnClick();
	});

	RefreshDirectoryInfo();
}


void DirectoryWindow::RefreshDirectoryInfo()
{
	PIMAGE_DATA_DIRECTORY directories = m_pe_modifier->optional_header().DataDirectory;

	wchar_t buffer[9];

	//export table
	int export_table_rva = directories[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
	int export_table_size = directories[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;
	wsprintf(buffer, L"%08X", export_table_rva);
	m_export_table_rva->SetText(buffer);
	wsprintf(buffer, L"%08X", export_table_size);
	m_export_table_size->SetText(buffer);

	// Import table
	int import_table_rva = directories[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
	int import_table_size = directories[IMAGE_DIRECTORY_ENTRY_IMPORT].Size;
	wsprintf(buffer, L"%08X", import_table_rva);
	m_import_table_rva->SetText(buffer);
	wsprintf(buffer, L"%08X", import_table_size);
	m_import_table_size->SetText(buffer);

	// Resource table
	int resource_table_rva = directories[IMAGE_DIRECTORY_ENTRY_RESOURCE].VirtualAddress;
	int resource_table_size = directories[IMAGE_DIRECTORY_ENTRY_RESOURCE].Size;
	wsprintf(buffer, L"%08X", resource_table_rva);
	m_resource_table_rva->SetText(buffer);
	wsprintf(buffer, L"%08X", resource_table_size);
	m_resource_table_size->SetText(buffer);

	// Exception table
	int exception_table_rva = directories[IMAGE_DIRECTORY_ENTRY_EXCEPTION].VirtualAddress;
	int exception_table_size = directories[IMAGE_DIRECTORY_ENTRY_EXCEPTION].Size;
	wsprintf(buffer, L"%08X", exception_table_rva);
	m_exception_table_rva->SetText(buffer);
	wsprintf(buffer, L"%08X", exception_table_size);
	m_exception_table_size->SetText(buffer);

	// Security table
	int security_table_rva = directories[IMAGE_DIRECTORY_ENTRY_SECURITY].VirtualAddress;
	int security_table_size = directories[IMAGE_DIRECTORY_ENTRY_SECURITY].Size;
	wsprintf(buffer, L"%08X", security_table_rva);
	m_security_table_rva->SetText(buffer);
	wsprintf(buffer, L"%08X", security_table_size);
	m_security_table_size->SetText(buffer);

	// Base relocation table
	int base_relocation_table_rva = directories[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress;
	int base_relocation_table_size = directories[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size;
	wsprintf(buffer, L"%08X", base_relocation_table_rva);
	m_base_relocation_table_rva->SetText(buffer);
	wsprintf(buffer, L"%08X", base_relocation_table_size);
	m_base_relocation_table_size->SetText(buffer);

	// Debug table
	int debug_table_rva = directories[IMAGE_DIRECTORY_ENTRY_DEBUG].VirtualAddress;
	int debug_table_size = directories[IMAGE_DIRECTORY_ENTRY_DEBUG].Size;
	wsprintf(buffer, L"%08X", debug_table_rva);
	m_debug_table_rva->SetText(buffer);
	wsprintf(buffer, L"%08X", debug_table_size);
	m_debug_table_size->SetText(buffer);

	// Architecture-specific data table
	int asd_table_rva = directories[IMAGE_DIRECTORY_ENTRY_ARCHITECTURE].VirtualAddress;
	int asd_table_size = directories[IMAGE_DIRECTORY_ENTRY_ARCHITECTURE].Size;
	wsprintf(buffer, L"%08X", asd_table_rva);
	m_asd_table_rva->SetText(buffer);
	wsprintf(buffer, L"%08X", asd_table_size);
	m_asd_table_size->SetText(buffer);

	// Global pointer register table
	int gpr_table_rva = directories[IMAGE_DIRECTORY_ENTRY_GLOBALPTR].VirtualAddress;
	int gpr_table_size = directories[IMAGE_DIRECTORY_ENTRY_GLOBALPTR].Size;
	wsprintf(buffer, L"%08X", gpr_table_rva);
	m_gpr_table_rva->SetText(buffer);
	wsprintf(buffer, L"%08X", gpr_table_size);
	m_gpr_table_size->SetText(buffer);

	// Thread local storage table
	int tls_table_rva = directories[IMAGE_DIRECTORY_ENTRY_TLS].VirtualAddress;
	int tls_table_size = directories[IMAGE_DIRECTORY_ENTRY_TLS].Size;
	wsprintf(buffer, L"%08X", tls_table_rva);
	m_tls_table_rva->SetText(buffer);
	wsprintf(buffer, L"%08X", tls_table_size);
	m_tls_table_size->SetText(buffer);

	// Load configuration table
	int load_config_table_rva = directories[IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG].VirtualAddress;
	int load_config_table_size = directories[IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG].Size;
	wsprintf(buffer, L"%08X", load_config_table_rva);
	m_lct_table_rva->SetText(buffer);
	wsprintf(buffer, L"%08X", load_config_table_size);
	m_lct_table_size->SetText(buffer);

	// Bound import table
	int bound_import_table_rva = directories[IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT].VirtualAddress;
	int bound_import_table_size = directories[IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT].Size;
	wsprintf(buffer, L"%08X", bound_import_table_rva);
	m_bound_import_table_rva->SetText(buffer);
	wsprintf(buffer, L"%08X", bound_import_table_size);
	m_bound_import_table_size->SetText(buffer);

	// Import address table
	int iat_table_rva = directories[IMAGE_DIRECTORY_ENTRY_IAT].VirtualAddress;
	int iat_table_size = directories[IMAGE_DIRECTORY_ENTRY_IAT].Size;
	wsprintf(buffer, L"%08X", iat_table_rva);
	m_iat_table_rva->SetText(buffer);
	wsprintf(buffer, L"%08X", iat_table_size);
	m_iat_table_size->SetText(buffer);

	// Delay import descriptor table
	int delay_import_descriptor_table_rva = directories[IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT].VirtualAddress;
	int delay_import_descriptor_table_size = directories[IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT].Size;
	wsprintf(buffer, L"%08X", delay_import_descriptor_table_rva);
	m_delay_import_table_rva->SetText(buffer);
	wsprintf(buffer, L"%08X", delay_import_descriptor_table_size);
	m_delay_import_table_size->SetText(buffer);

	// COM runtime descriptor table
	int com_runtime_descriptor_table_rva = directories[IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR].VirtualAddress;
	int com_runtime_descriptor_table_size = directories[IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR].Size;
	wsprintf(buffer, L"%08X", com_runtime_descriptor_table_rva);
	m_clr_table_rva->SetText(buffer);
	wsprintf(buffer, L"%08X", com_runtime_descriptor_table_size);
	m_clr_table_size->SetText(buffer);

	// Reserved
	int reserved_table_rva = directories[15].VirtualAddress;
	int reserved_table_size = directories[15].Size;
	wsprintf(buffer, L"%08X", reserved_table_rva);
	m_reserved_table_rva->SetText(buffer);
	wsprintf(buffer, L"%08X", reserved_table_size);
	m_reserved_table_size->SetText(buffer);
		
}

void DirectoryWindow::OnClose()
{
}


void DirectoryWindow::OnExitBtnClick()
{
	Close();
}
