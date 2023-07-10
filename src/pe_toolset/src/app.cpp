#include "app.h"

#include "main_window.h"
#include "resource.h"
#include "log.h"
#include "window_manager.h"

void App::Init(HINSTANCE handle_to_app)
{
	if (m_status != AppStatus::k_uninitialized)
		return;

	m_handle_to_app = handle_to_app;
	m_status = AppStatus::k_ready;

	WindowManager::instance().Init(handle_to_app);
}

void App::Run()
{
	if (m_status != AppStatus::k_ready)
		return;

	WindowManager::instance().CreateAppWindow<MainWindow>(IDD_MAIN_WINDOW);

	m_status = AppStatus::k_running;
	MSG msg;
	while (m_status == AppStatus::k_running && GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		if (msg.message == WM_QUIT)
			break;
		DispatchMessage(&msg);
	}
}

void App::Terminate()
{
	if (m_status != AppStatus::k_running)
		return;
	m_status = AppStatus::k_terminated;
}


