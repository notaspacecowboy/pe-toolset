#pragma once

#include <string>
#include <windows.h>

class Process
{
private:
	int m_process_id;

public:
    Process(DWORD pid);
    bool is_process_running();
    bool kill_process();
    std::wstring get_process_path();
};