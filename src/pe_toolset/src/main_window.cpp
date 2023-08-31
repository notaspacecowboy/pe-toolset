#include "main_window.h"

#include <CommCtrl.h>
#include <ios>
#include <psapi.h>
#include <string.h>
#include <tchar.h>
#include "app.h"
#include "resource.h"
#include "log.h"
#include "window_manager.h"
#include "pe_viewer_window.h"
#include "executable_packer_window.h"
#include <sstream>

MainWindow::MainWindow(HWND handle_to_window)
	: AppWindow(handle_to_window)
	, m_process_list_view(GetControl<ListView>(IDC_MAIN_PROCESS_LIST))
	, m_module_list_view(GetControl<ListView>(IDC_MAIN_MODULE_LIST))
	, m_process_view_btn(GetControl<Button>(IDC_MAIN_PROCESS_VIEWER_BTN))
	, m_dll_injection_btn(GetControl<Button>(IDC_MAIN_DLL_INJECTION_BTN))
	, m_exe_pack_btn(GetControl<Button>(IDC_MAIN_EXE_PACK_BTN))
	, m_about_btn(GetControl<Button>(IDC_MAIN_ABOUT_BTN))
	, m_exit_btn(GetControl<Button>(IDC_MAIN_EXIT_BTN))
	, m_selected_index(-1)
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
	m_process_list_view->AddColumn(L"Process Name", 300);
	m_process_list_view->AddColumn(L"PID", 150);
	m_process_list_view->AddColumn(L"Image Base", 190);
	m_process_list_view->AddColumn(L"Image Size", 190);

	
	m_module_list_view->SetStyle(ListView::Style::k_full_row_select);
	m_module_list_view->AddColumn(L"Module Name", 500);
	m_module_list_view->AddColumn(L"Module Address", 300);

	RefreshListViews();
}


void MainWindow::OnClose()
{
	std::list<AppWindow*> all_windows;
	WindowManager::instance().GetAllActiveWindows(all_windows);
	for(auto window: all_windows)
	{
		if (window == this)
			continue;
		window->Close();
	}

	App::instance().Terminate();
}



void MainWindow::OnProcessViewBtnClick()
{
	Log::DebugPrintf(L"Process view button clicked\n");

	if (m_selected_index < 0)
		return;

	m_selected_index = m_selected_index;
	auto item = m_process_list_view->GetItem(m_selected_index);
	auto field_str = item[1];
	std::wstringstream ws;
    ws << std::hex << field_str;
	int process_id;
	ws >> process_id;

	auto handle_to_process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, process_id);
	if (handle_to_process == nullptr)
		return;

	// Get the process name.

	HMODULE handle_to_module;
	DWORD cb_needed, path_len;
	TCHAR process_path[MAX_PATH];

	if (EnumProcessModules(handle_to_process, &handle_to_module, sizeof(handle_to_module), &cb_needed))
	{
		path_len = GetModuleFileNameEx(handle_to_process, handle_to_module, process_path, MAX_PATH);
	}
	

	CloseHandle(handle_to_process);

	WindowManager::instance().CreateAppWindow<PEViewerWindow>(IDD_PE_VIEWER_WINDOW, process_path);
}

void MainWindow::OnDllInjectionBtnClick()
{
	Log::DebugPrintf(L"DLL injection button clicked\n");
}

void MainWindow::OnExePackBtnClick()
{
	Log::DebugPrintf(L"EXE pack button clicked\n");

	WindowManager::instance().CreateAppWindow<ExecutablePackerWindow>(IDD_EXECUTABLE_PACKING_WINDOW);
}

void MainWindow::OnAboutBtnClick()
{
	Log::DebugPrintf(L"About button clicked\n");
}

void MainWindow::OnExitBtnClick()
{
	Close();
}

void MainWindow::OnProcessListItemSelect(int index)
{
	Log::DebugPrintf(L"Process list item selected: %d\n", index);

	if (m_selected_index == index)
		return;

	m_module_list_view->Clear();

	m_selected_index = index;
	auto item = m_process_list_view->GetItem(index);
	auto field_str = item[1];
	std::wstringstream ws;
	ws << std::hex << field_str;
	int process_id;
	ws >> process_id;

	auto handle_to_process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, process_id);
	if(handle_to_process == nullptr)
		return;

	HMODULE handle_to_modules[1024];
	DWORD size_used;
	// Get a list of all the modules in this process.
	if (EnumProcessModules(handle_to_process, handle_to_modules, sizeof(handle_to_modules), &size_used))
	{
		for (int i = 0; i < (size_used / sizeof(HMODULE)); i++)
		{
			wchar_t module_name[MAX_PATH];

			// Get the full path to the module's file.
			if (GetModuleFileNameEx(handle_to_process, handle_to_modules[i], module_name, sizeof(module_name) / sizeof(TCHAR)))
			{
				// Print the module name and handle value.
				wchar_t address_base[20];
				wsprintf(address_base, L"%08X", (DWORD)handle_to_modules[i]);

				ListViewItem item;
				item.AddField(module_name);
				item.AddField(address_base);

				m_module_list_view->AddItem(std::move(item));
			}
		}
	}

	CloseHandle(handle_to_process);
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
	int count = 0;
	for(int i = 0; i < total_processes; ++i)
	{
		memset(process_name, 0, sizeof(process_name));

		auto process_id = all_processes[i];
		auto handle_to_process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, process_id);
		if (handle_to_process == nullptr) 
			continue;

		if (!EnumProcessModules(handle_to_process, &handle_to_module, sizeof(handle_to_module), &size_used))
		{
			CloseHandle(handle_to_process);
			continue;
		}

		GetModuleBaseName(handle_to_process, handle_to_module, process_name,
			sizeof(process_name) / sizeof(TCHAR));

		MODULEINFO module_info {0};

		if (!GetModuleInformation(handle_to_process, handle_to_module, &module_info, sizeof(MODULEINFO)))
		{
			CloseHandle(handle_to_process);
			continue;
		}

		wsprintf(process_id_str, _T("%08X"), process_id);
		wsprintf(image_base_ptr, _T("%08X"), (DWORD)module_info.lpBaseOfDll);
		wsprintf(image_size_ptr, _T("%08X"), module_info.SizeOfImage);

		ListViewItem item;
		item.AddField(process_name);
		item.AddField(process_id_str);
		item.AddField(image_base_ptr);
		item.AddField(image_size_ptr);
		item.index() = count++;

		m_process_list_view->AddItem(std::move(item));

		CloseHandle(handle_to_process);
	}
}

