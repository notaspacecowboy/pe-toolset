#pragma once
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
public:
	ListView(HWND handle_to_control);
	~ListView() override = default;

	void AddItem(ListViewItem&& item);
	void RemoveItem(int index);
	ListViewItem GetItem(int index);
	void SetItem(int index, ListViewItem&& item);
	void Clear();
};
