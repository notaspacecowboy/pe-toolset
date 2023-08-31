#include "file_browser.h"

#include "app_window.h"
#include "windows.h"

std::wstring FileBrowser::OpenFile(AppWindow* owner, std::wstring filter)
{
	OPENFILENAME ofn;
	WCHAR file[100];
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = owner->handle_to_window();  // Owner window
	ofn.lpstrFile = file;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(file);
	ofn.lpstrFilter = L"Executable\0*.exe\0";
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if(!GetOpenFileName(&ofn))
		return L"";

	std::wstring result = ofn.lpstrFile;
	DWORD attributes = GetFileAttributes(result.c_str());
	if (attributes == INVALID_FILE_ATTRIBUTES) {
		return L"";
	}

	return result;
}
