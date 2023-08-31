#pragma once
#include <string>

class AppWindow;

class FileBrowser
{
public:
	static std::wstring OpenFile(AppWindow* owner, std::wstring filter);
};
