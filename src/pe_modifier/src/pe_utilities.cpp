#include "pe_utilities.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <array>

size_t PEUtilities::ReadPEFile(const std::wstring& file_path, void*& file_buffer)
{
	std::fstream file;
	file.open(file_path, std::ios::binary | std::ios::in);
	if (!file)
		return 0;

	//get file size
	file.seekg(0, std::ios::end);
	size_t size = file.tellg();

	//allocate enough memory space
	file_buffer = static_cast<void*>(new char[size]);
	memset(file_buffer, '\0', size);

	//read everything
	file.seekg(0, std::ios::beg);
	file.read(static_cast<char*>(file_buffer), size);

	file.close();
	return size;
}


size_t PEUtilities::CopyFileBufferToImageBuffer(const void* file_buffer, void*& image_buffer)
{
	using namespace std;

	const char* file_buffer_c = static_cast<const char*>(file_buffer);

	//find pe header
	ULONG e_lfanew = 0;
	memcpy_s(&e_lfanew, 4, file_buffer_c + 0x3C, 4);

	//read important fields in pe header
	file_buffer_c += e_lfanew;
	USHORT number_of_sections = 0, size_of_optional_header = 0;
	memcpy_s(&number_of_sections, 2, file_buffer_c + 0x6, 2);
	memcpy_s(&size_of_optional_header, 2, file_buffer_c + 0x14, 2);

	//read important fields in optional header
	ULONG section_alignment = 0, file_alignment = 0, size_of_header = 0, size_of_image = 0;
	memcpy_s(&section_alignment, 4, file_buffer_c + 0x38, 4);
	memcpy_s(&file_alignment, 4, file_buffer_c + 0x3C, 4);
	memcpy_s(&size_of_image, 4, file_buffer_c + 0x50, 4);
	memcpy_s(&size_of_header, 4, file_buffer_c + 0x54, 4);

	//allocate memory space for image buffer
	image_buffer = new char[size_of_image];
	memset(image_buffer, '\0', size_of_image);

	//copy header
	memcpy_s(image_buffer, size_of_header, file_buffer, size_of_header);

	//copy each section
	file_buffer_c += (0x18 + size_of_optional_header);
	for (int i = 0; i < number_of_sections; i++)
	{
		_IMAGE_SECTION_HEADER section_header;
		memcpy_s(&section_header, sizeof(section_header), file_buffer_c, sizeof(section_header));
		memcpy_s(static_cast<char*>(image_buffer) + section_header.VirtualAddress, section_header.SizeOfRawData, static_cast<const char*>(file_buffer) + section_header.PointerToRawData, section_header.SizeOfRawData);
		file_buffer_c += sizeof(section_header);
	}

	return size_of_image;
}


size_t PEUtilities::CopyImageBufferToNewBuffer(const void* image_buffer, void*& new_buffer)
{
	using namespace std;

	const char* image_buffer_c = static_cast<const char*>(image_buffer);

	//find pe header
	ULONG e_lfanew = 0;
	memcpy_s(&e_lfanew, 4, image_buffer_c + 0x3C, 4);

	//read important fields in pe header
	image_buffer_c += e_lfanew;
	USHORT number_of_sections = 0, size_of_optional_header = 0;
	memcpy_s(&number_of_sections, 2, image_buffer_c + 0x6, 2);
	memcpy_s(&size_of_optional_header, 2, image_buffer_c + 0x14, 2);


	//read important fields in optional header
	ULONG section_alignment = 0, file_alignment = 0, size_of_header = 0, size_of_image = 0;
	memcpy_s(&section_alignment, 4, image_buffer_c + 0x38, 4);
	memcpy_s(&file_alignment, 4, image_buffer_c + 0x3C, 4);
	memcpy_s(&size_of_image, 4, image_buffer_c + 0x50, 4);
	memcpy_s(&size_of_header, 4, image_buffer_c + 0x54, 4);

	//find all section headers
	vector<IMAGE_SECTION_HEADER> section_headers;
	image_buffer_c += (0x18 + size_of_optional_header);
	for (int i = 0; i < number_of_sections; i++)
	{
		IMAGE_SECTION_HEADER section_header;
		memset(&section_header, 0, sizeof(section_header));
		memcpy_s(&section_header, sizeof(section_header), image_buffer_c, sizeof(section_header));

		section_headers.push_back(section_header);
		image_buffer_c += sizeof(section_header);
	}

	//allocate memory space for image buffer

	ULONG new_buffer_size = section_headers[section_headers.size() - 1].PointerToRawData + section_headers[section_headers.size() - 1].SizeOfRawData;
	new_buffer = new char[new_buffer_size];
	memset(new_buffer, '\0', new_buffer_size);

	//copy header
	memcpy_s(new_buffer, size_of_header, image_buffer, size_of_header);

	//copy each section
	for (int i = 0; i < number_of_sections; i++)
	{
		memcpy_s(
			static_cast<char*>(new_buffer) + section_headers[i].PointerToRawData,
			section_headers[i].SizeOfRawData,
			static_cast<const char*>(image_buffer) + section_headers[i].VirtualAddress,
			section_headers[i].SizeOfRawData);
	}

	return new_buffer_size;
}



bool PEUtilities::MemoryToFile(const char* file_path, const void* buffer, size_t buffer_size)
{
	std::fstream file;
	file.open(file_path, std::ios::binary | std::ios::out | std::ios::trunc);
	if (!file)
	{
		std::cerr << "Error while opening file\n";
		return false;
	}

	file.write(static_cast<const char*>(buffer), buffer_size);

	file.close();
	return true;
}


DWORD PEUtilities::RvaToFileOffset(void* file_buffer, DWORD dwRva)
{
	auto file_buffer_content = static_cast<char*>(file_buffer);
	auto dos_header = reinterpret_cast<PIMAGE_DOS_HEADER>(file_buffer_content);
	auto file_header = reinterpret_cast<PIMAGE_FILE_HEADER>(file_buffer_content + dos_header->e_lfanew + 0x4);
	auto optional_header = reinterpret_cast<PIMAGE_OPTIONAL_HEADER>(
		file_buffer_content +
		dos_header->e_lfanew +
		0x4 +
		0x14
		);

	auto section_header = reinterpret_cast<PIMAGE_SECTION_HEADER>(
		file_buffer_content +
		dos_header->e_lfanew +
		0x4 +
		0x14 +
		file_header->SizeOfOptionalHeader +
		0x28 * file_header->NumberOfSections
		);

	for (int i = 0; i < file_header->NumberOfSections; i++)
	{
		section_header--;
		if (section_header->VirtualAddress <= dwRva)
			break;
	}

	return (dwRva - section_header->VirtualAddress + section_header->PointerToRawData);
}


void* PEUtilities::GetFunctionAddrByName(void* file_buffer, const char* function_name)
{
	auto file_buffer_content = static_cast<char*>(file_buffer);
	auto dos_header = reinterpret_cast<PIMAGE_DOS_HEADER>(file_buffer_content);
	auto file_header = reinterpret_cast<PIMAGE_FILE_HEADER>(file_buffer_content + dos_header->e_lfanew + 0x4);
	auto optional_header = reinterpret_cast<PIMAGE_OPTIONAL_HEADER>(
		file_buffer_content +
		dos_header->e_lfanew +
		0x4 +
		0x14
		);

	auto export_table_rva = optional_header->DataDirectory[0].VirtualAddress;
	auto export_data_foa = PEUtilities::RvaToFileOffset(file_buffer, export_table_rva);

	auto export_table = reinterpret_cast<PIMAGE_EXPORT_DIRECTORY>(file_buffer_content + export_data_foa);
	auto name_table = reinterpret_cast<DWORD*>(file_buffer_content + RvaToFileOffset(file_buffer, export_table->AddressOfNames));
	auto name_ordinals_table = reinterpret_cast<SHORT*>(file_buffer_content + RvaToFileOffset(file_buffer, export_table->AddressOfNameOrdinals));
	auto function_table = reinterpret_cast<DWORD*>(file_buffer_content + RvaToFileOffset(file_buffer, export_table->AddressOfFunctions));

	for (int i = 0; i < export_table->NumberOfNames; i++)
	{
		auto cur_function_name = file_buffer_content + RvaToFileOffset(file_buffer, name_table[i]);
		if (strcmp(function_name, cur_function_name) != 0)
			continue;

		//if function names match
		return file_buffer_content + PEUtilities::RvaToFileOffset(file_buffer, function_table[name_ordinals_table[i]]);
	}

	return nullptr;
}


void* PEUtilities::GetFunctionAddrByOrdinals(void* file_buffer, short name_ordinal)
{
	auto file_buffer_content = static_cast<char*>(file_buffer);
	auto dos_header = reinterpret_cast<PIMAGE_DOS_HEADER>(file_buffer_content);
	auto file_header = reinterpret_cast<PIMAGE_FILE_HEADER>(file_buffer_content + dos_header->e_lfanew + 0x4);
	auto optional_header = reinterpret_cast<PIMAGE_OPTIONAL_HEADER>(
		file_buffer_content +
		dos_header->e_lfanew +
		0x4 +
		0x14
		);

	auto export_table_rva = optional_header->DataDirectory[0].VirtualAddress;
	auto export_data_foa = PEUtilities::RvaToFileOffset(file_buffer, export_table_rva);

	auto export_table = reinterpret_cast<PIMAGE_EXPORT_DIRECTORY>(file_buffer_content + export_data_foa);
	auto name_table = reinterpret_cast<DWORD*>(file_buffer_content + RvaToFileOffset(file_buffer, export_table->AddressOfNames));
	auto name_ordinals_table = reinterpret_cast<SHORT*>(file_buffer_content + RvaToFileOffset(file_buffer, export_table->AddressOfNameOrdinals));
	auto function_table = reinterpret_cast<DWORD*>(file_buffer_content + RvaToFileOffset(file_buffer, export_table->AddressOfFunctions));

	return file_buffer_content + PEUtilities::RvaToFileOffset(file_buffer, function_table[name_ordinal - export_table->Base]);
}


std::wstring PEUtilities::GetSectionName(const IMAGE_SECTION_HEADER& section_header)
{
	// Copy the Name field into a std::array that has room for a null terminator.
	std::array<char, 9> name = {};
	std::memcpy(name.data(), section_header.Name, 8);

	// Calculate the length of the wide string
	int wideStringLength = MultiByteToWideChar(CP_ACP, 0, name.data(), -1, NULL, 0);

	// Create a buffer for the wide string
	std::wstring wideName(wideStringLength, 0);

	// Convert the ASCII string to a wide string
	MultiByteToWideChar(CP_ACP, 0, name.data(), -1, &wideName[0], wideStringLength);

	return wideName;
}
