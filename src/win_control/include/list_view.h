#pragma once
#include <functional>
#include <vector>

#include "control.h"


class ListViewItem
{
public:
	ListViewItem() = default;
	~ListViewItem() = default;
	ListViewItem(ListViewItem&&) = default;
	ListViewItem& operator=(ListViewItem&&) = default;

	int index() const { return m_index; }
	void index(int index) { m_index = index; }
	LPTSTR GetField(int index) const { return m_fields[index]; }
	void AddField(LPTSTR field) { m_fields.push_back(field); }
	void SetField(int index, LPTSTR field) { m_fields[index] = field; }
	void RemoveField(int index) { m_fields.erase(m_fields.begin() + index); }
	void Clear() { m_fields.clear(); }

private:
	int m_index = 0;
	std::vector<LPTSTR> m_fields;
};

class ListView: public Control
{
	using ItemSelectCallBack = std::function<void(int)>;

public:
	enum class Style
	{
		k_full_row_select = 0x00000020,
	};

public:
	ListView(HWND handle_to_control);
	~ListView() override = default;

	void SetStyle(ListView::Style style);
	void AddColumn(LPCTSTR column_name, int width);
	void AddItem(ListViewItem&& item);
	void RemoveItem(int index);
	ListViewItem GetItem(int index);
	void SetItem(int index, ListViewItem&& item);
	void Clear();

	void AddItemSelectListener(ItemSelectCallBack);
	void ClearItemSelectListeners();

	void HandleEvent(UINT message, WPARAM wParam, LPARAM lParam) override;

private:
	std::vector<LPCTSTR> m_column_names;
	std::vector<ItemSelectCallBack> m_item_select_call_backs;
};
