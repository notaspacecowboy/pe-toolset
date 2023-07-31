#pragma once
#include <functional>
#include <string>
#include <vector>

#include "control.h"


class ListViewItem
{
public:
	ListViewItem() = default;
	~ListViewItem() = default;
	ListViewItem(ListViewItem&&) = default;
	ListViewItem& operator=(ListViewItem&&) = default;

	int& index() { return m_index; }

	std::wstring& operator[](int index) { return m_fields[index]; }
	void AddField(std::wstring field) { m_fields.push_back(field); }
	void RemoveField(int index) { m_fields.erase(m_fields.begin() + index); }
	void Clear() { m_fields.clear(); }
	size_t Size() const { return m_fields.size(); }


private:
	int m_index = 0;
	std::vector<std::wstring> m_fields;
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
	void AddColumn(std::wstring column_name, int width);
	void AddItem(ListViewItem&& item);
	void RemoveItem(int index);
	ListViewItem GetItem(int index);
	void SetItem(int index, ListViewItem&& item);
	void Clear();

	void AddItemSelectListener(ItemSelectCallBack);
	void ClearItemSelectListeners();

	void HandleEvent(UINT message, WPARAM wParam, LPARAM lParam) override;

private:
	std::vector<std::wstring> m_column_names;
	std::vector<ItemSelectCallBack> m_item_select_call_backs;
};
