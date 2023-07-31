#pragma once
#include <string>
#include <windows.h>

class PEUtilities
{
public:
	static size_t ReadPEFile(const std::wstring& file_path, void*& file_buffer);

	static size_t CopyFileBufferToImageBuffer(const void* file_buffer, void*& image_buffer);

	static size_t CopyImageBufferToNewBuffer(const void* image_buffer, void*& new_buffer);

	static bool MemoryToFile(const char* file_path, const void* buffer, size_t buffer_size);

	static DWORD RvaToFileOffset(void* file_buffer, DWORD dwRva);

	static void* GetFunctionAddrByName(void* file_buffer, const char* function_name);

	static void* GetFunctionAddrByOrdinals(void* file_buffer, SHORT name_ordinal);

	static std::wstring GetSectionName(const IMAGE_SECTION_HEADER& section_header);
};