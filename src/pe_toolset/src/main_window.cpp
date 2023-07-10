#include "main_window.h"

#include <CommCtrl.h>
#include <psapi.h>
#include <tchar.h>

#include "app.h"
#include "resource.h"
#include "log.h"


MainWindow::MainWindow(HWND handle_to_window)
	: AppWindow(handle_to_window)
	, m_process_list_view(GetControl<ListView>(IDC_MAIN_PROCESS_LIST))
	, m_module_list_view(GetControl<ListView>(IDC_MAIN_MODULE_LIST))
	, m_process_view_btn(GetControl<Button>(IDC_MAIN_PROCESS_VIEWER_BTN))
	, m_dll_injection_btn(GetControl<Button>(IDC_MAIN_DLL_INJECTION_BTN))
	, m_exe_pack_btn(GetControl<Button>(IDC_MAIN_EXE_PACK_BTN))
	, m_about_btn(GetControl<Button>(IDC_MAIN_ABOUT_BTN))
	, m_exit_btn(GetControl<Button>(IDC_MAIN_EXIT_BTN))
{

}

void MainWindow::OnCreate()
{
	m_process_view_btn->AddButtonClickListener([this]()
	{
		this->OnProcessViewBtnClick();
	});

	m_dll_injection_btn->AddButtonClickListener([this]()
	{
		this->OnDllInjectionBtnClick();
	});

	m_exe_pack_btn->AddButtonClickListener([this]()
	{
		this->OnExePackBtnClick();
	});

	m_about_btn->AddButtonClickListener([this]()
	{
		this->OnAboutBtnClick();
	});

	m_exit_btn->AddButtonClickListener([this]()
	{
		this->OnExitBtnClick();
	});

	m_process_list_view->AddItemSelectListener([this](int index)
	{
		this->OnProcessListItemSelect(index);
	});

	m_process_list_view->SetStyle(ListView::Style::k_full_row_select);
	m_process_list_view->AddColumn(TEXT("Process Name"), 300);
	m_process_list_view->AddColumn(TEXT("PID"), 150);
	m_process_list_view->AddColumn(TEXT("Image Base"), 190);
	m_process_list_view->AddColumn(TEXT("Image Size"), 190);

	RefreshListViews();
}


void MainWindow::OnClose()
{
	App::instance().Terminate();
}



void MainWindow::OnProcessViewBtnClick()
{
	Log::DebugPrintf(TEXT("Process view button clicked\n"));
}

void MainWindow::OnDllInjectionBtnClick()
{
	Log::DebugPrintf(TEXT("DLL injection button clicked\n"));
}

void MainWindow::OnExePackBtnClick()
{
	Log::DebugPrintf(TEXT("EXE pack button clicked\n"));
}

void MainWindow::OnAboutBtnClick()
{
	Log::DebugPrintf(TEXT("About button clicked\n"));
}

void MainWindow::OnExitBtnClick()
{
	Close();
}

void MainWindow::OnProcessListItemSelect(int index)
{
	Log::DebugPrintf(TEXT("Process list item selected: %d\n"), index);
}



void MainWindow::RefreshListViews()
{
	m_process_list_view->Clear();
	m_module_list_view->Clear();

	// Get the list of process identifiers.
	DWORD all_processes[2048], size_used;
	if (!EnumProcesses(all_processes, sizeof(all_processes), &size_used))
		return;

	DWORD total_processes = size_used / sizeof(DWORD);
	TCHAR process_name[MAX_PATH] = TEXT("<unknown>");
	HMODULE handle_to_module {0};
	MODULEINFO module_info {0};

	TCHAR process_id_str[20], image_base_ptr[20], image_size_ptr[20];
	for(int i = 0; i < total_processes; ++i)
	{
		memset(process_name, 0, sizeof(process_name));

		auto process_id = all_processes[i];
		auto handle_to_process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, process_id);
		if (handle_to_process == nullptr)
			continue;

		if (!EnumProcessModules(handle_to_process, &handle_to_module, sizeof(handle_to_module), &size_used))
			continue;

		GetModuleBaseName(handle_to_process, handle_to_module, process_name,
			sizeof(process_name) / sizeof(TCHAR));

		MODULEINFO module_info {0};

		if (!GetModuleInformation(handle_to_process, handle_to_module, &module_info, sizeof(MODULEINFO)))
			continue;

		wsprintf(process_id_str, _T("%08X"), process_id);
		wsprintf(image_base_ptr, _T("%08X"), (DWORD)module_info.lpBaseOfDll);
		wsprintf(image_size_ptr, _T("%08X"), module_info.SizeOfImage);

		ListViewItem item;
		item.AddField(process_name);
		item.AddField(process_id_str);
		item.AddField(image_base_ptr);
		item.AddField(image_size_ptr);

		m_process_list_view->AddItem(std::move(item));
	}
}

