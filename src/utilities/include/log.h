#pragma once
#include <cstdarg>
#include <debugapi.h>
#include <winnt.h>
#include <strsafe.h>

class Log
{
public:
    static void DebugPrintf(const TCHAR* format, ...)
    {
        TCHAR buffer[1024];
        va_list argptr;
        va_start(argptr, format);
        StringCchVPrintf(buffer, ARRAYSIZE(buffer), format, argptr);
        va_end(argptr);
        OutputDebugString(buffer);
    }
};
