#include "list_view.h"
#include <CommCtrl.h>
#pragma comment(lib, "comctl32.lib")

ListView::ListView(HWND handle_to_control) : Control(handle_to_control)
{
}

void ListView::SetStyle(ListView::Style style)
{
	ListView_SetExtendedListViewStyle(m_handle_to_control, (DWORD)style);
}


void ListView::AddColumn(std::wstring column_name, int width)
{
	ListView_SetExtendedListViewStyle(m_handle_to_control, LVS_EX_FULLROWSELECT);

	LVCOLUMN column;
	column.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	column.fmt = LVCFMT_LEFT;
	column.cx = width;
	column.pszText = (LPTSTR)column_name.c_str();
	ListView_InsertColumn(m_handle_to_control, m_column_names.size(), &column);
	m_column_names.push_back(column_name);
}

void ListView::AddItem(ListViewItem&& item)
{
	LVITEM lv_item;
	lv_item.mask = LVIF_TEXT;
	lv_item.iItem = item.index();
	lv_item.iSubItem = 0;
	lv_item.pszText = (LPWSTR)item[0].c_str();
	ListView_InsertItem(m_handle_to_control, &lv_item);
	for (int i = 1; i < min(item.Size(), m_column_names.size()); ++i)
	{
		ListView_SetItemText(m_handle_to_control, lv_item.iItem, i, (LPWSTR)item[i].c_str());
	}
}

void ListView::RemoveItem(int index)
{
}

void ListView::SetItem(int index, ListViewItem&& item)
{
}

ListViewItem ListView::GetItem(int index)
{
	ListViewItem item;
	LVITEM lvi;
	memset(&lvi, 0, sizeof(LVITEM));
	for(int i = 0; i < m_column_names.size(); ++i)
	{
		TCHAR buffer[100] = { 0 };
		ListView_GetItemText(m_handle_to_control, index, i, buffer, sizeof(buffer));
		item.AddField(buffer);
	}

	return item;
}


void ListView::Clear()
{
	ListView_DeleteAllItems(m_handle_to_control);
}


void ListView::HandleEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message != WM_NOTIFY)
		return;

	auto pnmhdr = (LPNMHDR)lParam;
	if (pnmhdr->hwndFrom != m_handle_to_control || pnmhdr->code != NM_CLICK)	
		return;

	int index = ListView_GetNextItem(m_handle_to_control, -1, LVNI_SELECTED);
	if (index == -1)
		return;

	for (auto& func : m_item_select_call_backs)
	{
		func(index);
	}


}

void ListView::AddItemSelectListener(ItemSelectCallBack func)
{
	m_item_select_call_backs.push_back(func);
}

void ListView::ClearItemSelectListeners()
{
	m_item_select_call_backs.clear();
}

