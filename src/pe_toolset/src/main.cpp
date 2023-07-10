#include <windows.h>
#include <tchar.h>
#include "app.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	App::instance().Init(hInstance);
	App::instance().Run();

	return 0;
}