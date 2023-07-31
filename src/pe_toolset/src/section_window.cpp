#include "section_window.h"
#include "list_view.h"
#include "button.h"
#include "log.h"
#include "pe_modifier.h"
#include "pe_utilities.h"
#include "resource.h"


SectionWindow::SectionWindow(HWND handle_to_window, PEModifier* pe_modifier)
	: AppWindow(handle_to_window)
	, m_pe_modifier(pe_modifier)
	, m_section_list_view(GetControl<ListView>(IDC_SECTION_LIST))
	, m_close_btn(GetControl<Button>(IDC_SECTION_CLOSE_BTN))
{
}


void SectionWindow::OnCreate()
{
	m_close_btn->AddButtonClickListener([this]()
	{
		this->OnCloseBtnClick();
	});

	m_section_list_view->SetStyle(ListView::Style::k_full_row_select);
	m_section_list_view->AddColumn(L"#", 100);
	m_section_list_view->AddColumn(L"Name", 150);
	m_section_list_view->AddColumn(L"Virtual Size", 150);
	m_section_list_view->AddColumn(L"Virtual Offset", 150);
	m_section_list_view->AddColumn(L"Raw Size", 150);
	m_section_list_view->AddColumn(L"Raw Offset", 150);
	m_section_list_view->AddColumn(L"Characteristics", 150);

	RefreshList();
}

void SectionWindow::OnClose()
{
	
}

void SectionWindow::OnCloseBtnClick()
{
	Close();
}



void SectionWindow::RefreshList()
{
	m_section_list_view->Clear();

	auto sections = m_pe_modifier->section_headers();
	for(int i = 0; i < m_pe_modifier->GetNumberOfSections(); i++)
	{
		auto& section = sections[i];

		wchar_t buffer[9];
		ListViewItem item;

		//item index
		item.AddField(std::to_wstring(i));

		//item name 
		std::wstring section_name = PEUtilities::GetSectionName(section);
		Log::DebugPrintf(L"Section Name is: %ls \n", section_name.c_str());
		item.AddField(section_name);

		//virtual size
		int virtual_size = section.Misc.VirtualSize;
		wsprintf(buffer, L"%08X", virtual_size);
		item.AddField(buffer);

		//virtual offset
		int virtual_offset = section.VirtualAddress;
		wsprintf(buffer, L"%08X", virtual_offset);
		item.AddField(buffer);


		//raw size
		int raw_size = section.SizeOfRawData;
		wsprintf(buffer, L"%08X", raw_size);
		item.AddField(buffer);


		//raw offset
		int raw_offset = section.PointerToRawData;
		wsprintf(buffer, L"%08X", raw_offset);
		item.AddField(buffer);


		//characteristics
		int characteristics = section.Characteristics;
		wsprintf(buffer, L"%08X", characteristics);
		item.AddField(buffer);

		item.index() = i;

		m_section_list_view->AddItem(std::move(item));
	}
}



