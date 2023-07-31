#pragma once
#include <string>

#include "app_window.h"
#include "windows.h"


class PEModifier;
class Button;
class InputField;

class DirectoryWindow: public AppWindow
{
public:
	DirectoryWindow(HWND handle_to_window, PEModifier* pe_modifier);
	~DirectoryWindow() override = default;

	void OnCreate() override;
	void OnClose() override;

private:
	void RefreshDirectoryInfo();

	void OnExitBtnClick();

private:
	Button* m_exit_btn;
	InputField* m_import_table_rva;
	InputField* m_import_table_size;

	InputField* m_export_table_rva;
	InputField* m_export_table_size;

	InputField* m_resource_table_rva;
	InputField* m_resource_table_size;

	InputField* m_exception_table_rva;
	InputField* m_exception_table_size;

	InputField* m_security_table_rva;
	InputField* m_security_table_size;

	InputField* m_base_relocation_table_rva;
	InputField* m_base_relocation_table_size;

	InputField* m_debug_table_rva;
	InputField* m_debug_table_size;

	InputField* m_asd_table_rva;
	InputField* m_asd_table_size;

	InputField* m_gpr_table_rva;
	InputField* m_gpr_table_size;
	
	InputField* m_tls_table_rva;
	InputField* m_tls_table_size;

	InputField* m_lct_table_rva;
	InputField* m_lct_table_size;

	InputField* m_bound_import_table_rva;
	InputField* m_bound_import_table_size;

	InputField* m_iat_table_rva;
	InputField* m_iat_table_size;

	InputField* m_delay_import_table_rva;
	InputField* m_delay_import_table_size;

	InputField* m_clr_table_rva;
	InputField* m_clr_table_size;

	InputField* m_reserved_table_rva;
	InputField* m_reserved_table_size;

	PEModifier* m_pe_modifier;
};
