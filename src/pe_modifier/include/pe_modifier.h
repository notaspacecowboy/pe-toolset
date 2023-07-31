#pragma once
#pragma once
#include <string>
#include <fstream>
#include <vector>
#include <windows.h>




class PEModifier
{
private:
	std::wstring m_file_path;
	bool m_ready;

	//file buffer
	void* m_file_buffer;

	unsigned int m_file_buffer_size;

	//dos header
	PIMAGE_DOS_HEADER m_dos_header;

	//file header
	PIMAGE_FILE_HEADER m_file_header;

	//optional header
	PIMAGE_OPTIONAL_HEADER m_optional_header;

	//section headers
	PIMAGE_SECTION_HEADER m_section_headers;



private:
	void ReadPEHeaders();
	DWORD RvaToFileOffset(DWORD dwRva) const;
	DWORD foaToVirtualAddress(DWORD dwFoa) const;
	void Reset();

	void MoveExportTable(char* file_buffer_content, char*& current_pos) const;
	void MoveBaseRelocateTable(char* file_buffer_content, char*& current_pos) const;
	void MoveImportTable(char* file_buffer_content, char*& current_pos) const;

	void PrintResourceTypes(char* resource_table_entry, PIMAGE_RESOURCE_DIRECTORY resource_type_directory, PIMAGE_RESOURCE_DIRECTORY_ENTRY entry) const;
	void PrintResourceIDs(char* resource_table_entry, PIMAGE_RESOURCE_DIRECTORY resource_type_directory, PIMAGE_RESOURCE_DIRECTORY_ENTRY entry) const;
	void PrintResourceLanguages(char* resource_table_entry, PIMAGE_RESOURCE_DIRECTORY resource_language_directory, PIMAGE_RESOURCE_DIRECTORY_ENTRY entry) const;

public:
	PEModifier();
	~PEModifier();

	bool LoadPEFile(const std::wstring& file_path);
	bool Save() const;

	PIMAGE_SECTION_HEADER AddNewSection(const char* section_name, int memory_page_size, DWORD Characteristics);
	bool MoveDataDirectories();
	bool InjectDLL(const char* dll_name);

	void PrintDosHeader() const;
	void PrintPEHeader() const;
	void PrintOptionalPEHeader() const;
	void PrintSectionHeaders() const;
	void PrintExportTable() const;
	void PrintBaseRelocationTable() const;
	void PrintImportTable() const;
	void PrintResourceTabble() const;

	IMAGE_DOS_HEADER& dos_header() const;

	IMAGE_FILE_HEADER& file_header() const;

	IMAGE_OPTIONAL_HEADER& optional_header() const;

	PIMAGE_SECTION_HEADER section_headers() const;

	int GetNumberOfSections() const;


};
