#pragma once

#include <singleton.h>
#include <windows.h>
#include <unordered_map>
#include <memory>

#include "app_window.h"

using WindowEventCallBack = 

class App: public Singleton<App>
{
	friend class Singleton<App>;

public:
	enum class AppStatus
	{
		k_uninitialized					= 0,
		k_ready							= 1,
		k_running						= 2,
		k_terminated					= 3
	};

public:
	void Init(HINSTANCE handle_to_app);
	void Run();
	void Terminate();

	HINSTANCE handle_to_app() const { return m_handle_to_app; }

private:
	App()								= default;

private:
	AppStatus m_status					= AppStatus::k_uninitialized;
	HINSTANCE m_handle_to_app			= nullptr;
};