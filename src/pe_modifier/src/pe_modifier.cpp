
#include "pe_modifier.h"
#include <iostream>

#include "log.h"
#include "pe_utilities.h"

PEModifier::PEModifier()
	: m_file_path()
	, m_ready(false)
	, m_file_buffer(nullptr)
	, m_file_buffer_size(0)
	, m_dos_header(nullptr)
	, m_file_header(nullptr)
	, m_optional_header(nullptr)
	, m_section_headers() { }

PEModifier::~PEModifier()
{
	m_ready = false;
	if (m_file_buffer != nullptr)
		delete static_cast<char*>(m_file_buffer);
	m_file_buffer = nullptr;
	m_file_buffer_size = 0;
	m_dos_header = nullptr;
	m_file_header = nullptr;
	m_optional_header = nullptr;
	m_section_headers = nullptr;
}


bool PEModifier::LoadPEFile(const std::wstring& file_path)
{
	if (m_ready && m_file_buffer != nullptr)
		Reset();

	m_file_path = file_path;
	ReadPEHeaders();

	if (!m_ready)
	{
		Log::DebugPrintf(L"Error while reading PE header\n");
		Reset();
		return false;
	}

	return true;
}


void PEModifier::ReadPEHeaders()
{
	m_file_buffer_size = PEUtilities::ReadPEFile(m_file_path, m_file_buffer);
	if (m_file_buffer_size <= 0)
		return;

	char* file_buffer_content = static_cast<char*>(m_file_buffer);
	m_dos_header = reinterpret_cast<PIMAGE_DOS_HEADER>(file_buffer_content);
	m_file_header = reinterpret_cast<PIMAGE_FILE_HEADER>(file_buffer_content + m_dos_header->e_lfanew + 0x4);
	m_optional_header = reinterpret_cast<PIMAGE_OPTIONAL_HEADER>(file_buffer_content + m_dos_header->e_lfanew + 0x4 + 0x14);
	m_section_headers = reinterpret_cast<PIMAGE_SECTION_HEADER>(
		file_buffer_content +
		m_dos_header->e_lfanew +
		0x4 +
		0x14 +
		m_file_header->SizeOfOptionalHeader
		);

	m_ready = true;
}


void PEModifier::Reset()
{
	m_ready = false;
	if (m_file_buffer != nullptr)
		delete static_cast<char*>(m_file_buffer);
	m_file_buffer = nullptr;
	m_file_buffer_size = 0;
	m_dos_header = nullptr;
	m_file_header = nullptr;
	m_optional_header = nullptr;
	m_section_headers = nullptr;
}



bool PEModifier::Save() const
{
	if (!m_ready || !m_file_buffer)
	{
		std::cerr << "PEHeaderAnalyzer is not ready\n";
		return false;
	}

	std::fstream file;
	file.open(m_file_path, std::ios::binary | std::ios::out | std::ios::trunc);
	file.seekg(0, std::ios::beg);
	file.write(static_cast<char*>(m_file_buffer), m_file_buffer_size);
	file.close();
	return true;
}


PIMAGE_SECTION_HEADER PEModifier::AddNewSection(const char* section_name, int memory_page_size, DWORD characteristics)
{
	if (!m_ready)
	{
		using namespace std;
		cout << "must read pe file into memory first!" << endl;
		return nullptr;
	}

	if (memory_page_size <= 0)
	{
		using namespace std;
		cout << "memory page size must be greater than 0!" << endl;
		return nullptr;
	}

	if (m_dos_header->e_lfanew < 0x28)
	{
		using namespace std;
		cout << "there is not enough space to add a new section header" << endl;
		return nullptr;
	}

	//move file buffer to a larger memory block
	DWORD new_section_size = memory_page_size * m_optional_header->SectionAlignment;
	int new_file_buffer_size = m_file_buffer_size + new_section_size;
	void* new_file_buffer = new char[m_file_buffer_size + new_section_size];
	memset(new_file_buffer, '\0', new_file_buffer_size);
	memcpy_s(new_file_buffer, new_file_buffer_size, m_file_buffer, m_file_buffer_size);

	char* file_buffer_content = static_cast<char*>(new_file_buffer);
	auto dos_header = reinterpret_cast<PIMAGE_DOS_HEADER>(file_buffer_content);
	auto file_header = reinterpret_cast<PIMAGE_FILE_HEADER>(file_buffer_content + dos_header->e_lfanew + 0x4);
	auto optional_header = reinterpret_cast<PIMAGE_OPTIONAL_HEADER>(file_buffer_content + dos_header->e_lfanew + 0x4 + 0x14);

	//copy file header + optional_header + section headers
	rsize_t size_to_move = 0x4 + 0x14 + file_header->SizeOfOptionalHeader + 0x28 * file_header->NumberOfSections;
	memcpy_s(
		file_buffer_content + dos_header->e_lfanew - 0x28,
		size_to_move,
		file_buffer_content + dos_header->e_lfanew,
		size_to_move
	);

	//re-calculate pointers
	dos_header->e_lfanew -= 0x28;
	file_header = reinterpret_cast<PIMAGE_FILE_HEADER>(file_buffer_content + dos_header->e_lfanew + 0x4);
	optional_header = reinterpret_cast<PIMAGE_OPTIONAL_HEADER>(file_buffer_content + dos_header->e_lfanew + 0x4 + 0x14);

	auto section_headers = reinterpret_cast<PIMAGE_SECTION_HEADER>(
		file_buffer_content +
		dos_header->e_lfanew +
		0x4 +
		0x14 +
		file_header->SizeOfOptionalHeader
		);

	auto new_section_header = &(section_headers[file_header->NumberOfSections]);
	memset(new_section_header, '\0', sizeof(new_section_header));
	memcpy_s(new_section_header->Name, sizeof(new_section_header->Name), section_name, strlen(section_name));
	new_section_header->Characteristics = characteristics;
	new_section_header->Misc.VirtualSize = new_section_size;
	new_section_header->VirtualAddress = optional_header->SizeOfImage;
	new_section_header->PointerToRawData = m_file_buffer_size;
	new_section_header->SizeOfRawData = new_section_size;

	//update header infos
	file_header->NumberOfSections++;
	optional_header->SizeOfImage += new_section_size;

	delete static_cast<char*>(m_file_buffer);
	m_file_buffer = new_file_buffer;
	m_file_buffer_size = new_file_buffer_size;
	m_dos_header = dos_header;
	m_file_header = file_header;
	m_optional_header = optional_header;
	m_section_headers = section_headers;

	return new_section_header;
}


void PEModifier::MoveExportTable(char* file_buffer_content, char*& current_pos) const
{
	if (m_optional_header->DataDirectory[0].VirtualAddress == 0 && m_optional_header->DataDirectory[0].Size == 0)
		return;

	auto export_table = reinterpret_cast<PIMAGE_EXPORT_DIRECTORY>(file_buffer_content + RvaToFileOffset(m_optional_header->DataDirectory[0].VirtualAddress));
	auto function_table = reinterpret_cast<DWORD*>(file_buffer_content + RvaToFileOffset(export_table->AddressOfFunctions));
	auto name_ordinals_table = reinterpret_cast<SHORT*>(file_buffer_content + RvaToFileOffset(export_table->AddressOfNameOrdinals));
	auto name_table = reinterpret_cast<DWORD*>(file_buffer_content + RvaToFileOffset(export_table->AddressOfNames));

	//copy function table
	auto function_table_size = sizeof(DWORD) * export_table->NumberOfFunctions;
	memcpy_s(current_pos, function_table_size, function_table, function_table_size);
	export_table->AddressOfFunctions = foaToVirtualAddress(current_pos - file_buffer_content);
	current_pos += function_table_size;

	//copy name ordinals table
	auto name_ordinals_table_size = sizeof(SHORT) * export_table->NumberOfNames;
	memcpy_s(current_pos, name_ordinals_table_size, name_ordinals_table, name_ordinals_table_size);
	export_table->AddressOfNameOrdinals = foaToVirtualAddress(current_pos - file_buffer_content);
	current_pos += name_ordinals_table_size;

	//copy name table
	auto name_table_size = sizeof(DWORD) * export_table->NumberOfNames;
	memcpy_s(current_pos, name_table_size, name_table, name_table_size);
	export_table->AddressOfNames = foaToVirtualAddress(current_pos - file_buffer_content);
	current_pos += name_table_size;

	//copy the actual function names
	auto new_name_table = reinterpret_cast<DWORD*>(current_pos);
	for (int i = 0; i < export_table->NumberOfNames; i++)
	{
		char* function_name = file_buffer_content + RvaToFileOffset(name_table[i]);
		using namespace std;
		cout << function_name << endl;

		auto function_name_size = strlen(function_name) + 1;
		memcpy_s(current_pos, function_name_size, function_name, function_name_size);
		new_name_table[i] = foaToVirtualAddress(current_pos - file_buffer_content);
		current_pos += function_name_size;
	}

	//copy export table
	memcpy_s(current_pos, sizeof(IMAGE_EXPORT_DIRECTORY), export_table, sizeof(IMAGE_EXPORT_DIRECTORY));
	m_optional_header->DataDirectory[0].VirtualAddress = foaToVirtualAddress(current_pos - file_buffer_content);
	current_pos += sizeof(IMAGE_EXPORT_DIRECTORY);
}

void PEModifier::MoveBaseRelocateTable(char* file_buffer_content, char*& current_pos) const
{
	auto base_relocation_table_start = reinterpret_cast<PIMAGE_BASE_RELOCATION>(file_buffer_content + RvaToFileOffset(m_optional_header->DataDirectory[5].VirtualAddress));
	auto base_relocation_table = base_relocation_table_start;
	int total_size = 0;
	while (base_relocation_table->VirtualAddress != 0 && base_relocation_table->SizeOfBlock != 0)
	{
		total_size += base_relocation_table->SizeOfBlock;
		base_relocation_table = reinterpret_cast<PIMAGE_BASE_RELOCATION>(reinterpret_cast<char*>(base_relocation_table) + base_relocation_table->SizeOfBlock);
	}

	memcpy_s(current_pos, total_size, base_relocation_table_start, total_size);
	m_optional_header->DataDirectory[5].VirtualAddress = foaToVirtualAddress(current_pos - file_buffer_content);
	current_pos += total_size;
}


void PEModifier::MoveImportTable(char* file_buffer_content, char*& current_pos) const
{
	//copy import descriptor table
	auto import_descriptors = reinterpret_cast<PIMAGE_IMPORT_DESCRIPTOR>(file_buffer_content + RvaToFileOffset(m_optional_header->DataDirectory[1].VirtualAddress));
	int count = 0;
	while (
		import_descriptors[count].Name != 0 ||
		import_descriptors[count].OriginalFirstThunk != 0 ||
		import_descriptors[count].ForwarderChain != 0 ||
		import_descriptors[count].TimeDateStamp != 0 ||
		import_descriptors[count].FirstThunk != 0
		)
	{
		count++;
	}
	const rsize_t total_size = count * sizeof(IMAGE_IMPORT_DESCRIPTOR);
	memcpy_s(current_pos, total_size, import_descriptors, total_size);
	import_descriptors = reinterpret_cast<PIMAGE_IMPORT_DESCRIPTOR>(current_pos);
	current_pos += total_size;

	//for now, ignore the sub-structures used by the import table (IAT, INT, ILT, DLL names...)

	//reset data directory info
	m_optional_header->DataDirectory[1].VirtualAddress = foaToVirtualAddress(reinterpret_cast<char*>(import_descriptors) - file_buffer_content);
}



bool PEModifier::MoveDataDirectories()
{
	if (!AddNewSection(".mysec", 1, 0xE0000020))
		return false;

	auto file_buffer_content = static_cast<char*>(m_file_buffer);
	auto current_pos = file_buffer_content + m_section_headers[m_file_header->NumberOfSections - 1].PointerToRawData;

	//MoveExportTable(file_buffer_content, current_pos);
	//MoveBaseRelocateTable(file_buffer_content, current_pos);
	MoveImportTable(file_buffer_content, current_pos);

	return true;
}


bool PEModifier::InjectDLL(const char* dll_name)
{
	if (!AddNewSection(".import", 1, 0xE0000020))
		return false;

	auto file_buffer = static_cast<char*>(m_file_buffer);
	auto current_pos = file_buffer + m_section_headers[m_file_header->NumberOfSections - 1].PointerToRawData;

	//copy import descriptor table
	auto import_descriptors = reinterpret_cast<PIMAGE_IMPORT_DESCRIPTOR>(file_buffer + RvaToFileOffset(m_optional_header->DataDirectory[1].VirtualAddress));
	int count = 0;
	while (
		import_descriptors[count].Name != 0 ||
		import_descriptors[count].OriginalFirstThunk != 0 ||
		import_descriptors[count].ForwarderChain != 0 ||
		import_descriptors[count].TimeDateStamp != 0 ||
		import_descriptors[count].FirstThunk != 0
		)
	{
		count++;
	}
	const rsize_t total_size = count * sizeof(IMAGE_IMPORT_DESCRIPTOR);
	memcpy_s(current_pos, total_size, import_descriptors, total_size);
	import_descriptors = reinterpret_cast<PIMAGE_IMPORT_DESCRIPTOR>(current_pos);
	current_pos += total_size;
	current_pos += (sizeof(IMAGE_IMPORT_DESCRIPTOR) * 2);      //reserving some space for the new DLL

	//add a new INT table for the injected DLL
	PIMAGE_THUNK_DATA new_INT = reinterpret_cast<PIMAGE_THUNK_DATA>(current_pos);
	new_INT[0].u1.Ordinal = 0x80000001;    //1 at the highest bit means the function is imported by ordinal
	new_INT[1].u1.Ordinal = 0;             //this empty entry marks the end of the INT table
	current_pos += sizeof(IMAGE_THUNK_DATA) * 2;

	//add a new IAT table for the injected DLL
	PIMAGE_THUNK_DATA new_IAT = reinterpret_cast<PIMAGE_THUNK_DATA>(current_pos);
	new_IAT[0].u1.Ordinal = 0x80000001;
	new_IAT[1].u1.Ordinal = 0;
	current_pos += sizeof(IMAGE_THUNK_DATA) * 2;

	//allocate some space to store DLL name
	rsize_t dll_name_size = strlen(dll_name) + 1;
	memcpy_s(current_pos, dll_name_size, dll_name, dll_name_size);

	//target and modify the new import descriptor
	auto& new_descriptor = import_descriptors[count];
	new_descriptor.OriginalFirstThunk = foaToVirtualAddress(reinterpret_cast<char*>(new_INT) - file_buffer);
	new_descriptor.FirstThunk = foaToVirtualAddress(reinterpret_cast<char*>(new_IAT) - file_buffer);
	new_descriptor.Name = foaToVirtualAddress(current_pos - file_buffer);
	current_pos += dll_name_size;

	//reset data directory info
	m_optional_header->DataDirectory[1].VirtualAddress = foaToVirtualAddress(reinterpret_cast<char*>(import_descriptors) - file_buffer);
}





void PEModifier::PrintDosHeader() const
{
	if (!m_ready)
	{
		std::cerr << "PEHeaderAnalyzer is not ready\n";
		return;
	}

	{
		using namespace std;
		cout << hex << "DOS HEADER" << endl;
		cout << "e_magic: " << hex << this->m_dos_header->e_magic << endl;
		cout << "e_lfanew: " << hex << this->m_dos_header->e_lfanew << endl << endl;
	}
}



void PEModifier::PrintPEHeader() const
{
	if (!m_ready)
	{
		std::cerr << "PEHeaderAnalyzer is not ready\n";
		return;
	}
	{
		using namespace std;
		cout << hex << "PE HEADER" << endl;
		cout << "machine: " << hex << m_file_header->Machine << endl;
		cout << "number_of_sections: " << hex << m_file_header->NumberOfSections << endl;
		cout << "size_of_optional_header: " << hex << m_file_header->SizeOfOptionalHeader << endl;
		cout << "characteristics: " << hex << m_file_header->Characteristics << endl << endl;
	}
}



void PEModifier::PrintOptionalPEHeader() const
{
	if (!m_ready)
	{
		std::cerr << "PEHeaderAnalyzer is not ready\n";
		return;
	}
	{
		using namespace std;
		cout << hex << "OPTIONAL PE HEADER" << endl;
		cout << "magic: " << hex << m_optional_header->Magic << endl;
		cout << "address_of_entry_point: " << hex << m_optional_header->AddressOfEntryPoint << endl;
		cout << "image_base: " << hex << m_optional_header->ImageBase << endl;
		cout << "section_alignment: " << hex << m_optional_header->SectionAlignment << endl;
		cout << "file_alignment: " << hex << m_optional_header->FileAlignment << endl;
		cout << "size_of_image: " << hex << m_optional_header->SizeOfImage << endl;
		cout << "size_of_headers: " << hex << m_optional_header->SizeOfHeaders << endl;
		cout << "dll_characteristics: " << hex << m_optional_header->DllCharacteristics << endl;
		cout << "size_of_stack_reserve: " << hex << m_optional_header->SizeOfStackReserve << endl;
		cout << "size_of_stack_commit: " << hex << m_optional_header->SizeOfStackCommit << endl;
		cout << "size_of_heap_reserve: " << hex << m_optional_header->SizeOfHeapReserve << endl;
		cout << "size_of_heap_commit: " << hex << m_optional_header->SizeOfHeapCommit << endl;
		cout << "number_of_rva_and_sizes: " << hex << m_optional_header->NumberOfRvaAndSizes << endl << endl;

		//print data dictionary
		cout
			<< "export table: " << endl
			<< "virtual address: " << hex << m_optional_header->DataDirectory[0].VirtualAddress << endl
			<< "size: " << m_optional_header->DataDirectory[0].Size << endl << endl;

		cout
			<< "import table: " << endl
			<< "virtual address: " << m_optional_header->DataDirectory[1].VirtualAddress << endl
			<< "size: " << m_optional_header->DataDirectory[1].Size << endl << endl;

		cout
			<< "resource table: " << endl
			<< "virtual address: " << m_optional_header->DataDirectory[2].VirtualAddress << endl
			<< "size: " << m_optional_header->DataDirectory[2].Size << endl << endl;

		cout
			<< "exception table: " << endl
			<< "virtual address: " << m_optional_header->DataDirectory[3].VirtualAddress << endl
			<< "size: " << m_optional_header->DataDirectory[3].Size << endl << endl;

		cout
			<< "security table: " << endl
			<< "virtual address: " << m_optional_header->DataDirectory[4].VirtualAddress << endl
			<< "size: " << m_optional_header->DataDirectory[4].Size << endl << endl;

		cout
			<< "relocation table: " << endl
			<< "virtual address: " << m_optional_header->DataDirectory[5].VirtualAddress << endl
			<< "size: " << m_optional_header->DataDirectory[5].Size << endl << endl;

		cout
			<< "debug table: " << endl
			<< "virtual address: " << m_optional_header->DataDirectory[6].VirtualAddress << endl
			<< "size: " << m_optional_header->DataDirectory[6].Size << endl << endl;

		cout
			<< "copyright table: " << endl
			<< "virtual address: " << m_optional_header->DataDirectory[7].VirtualAddress << endl
			<< "size: " << m_optional_header->DataDirectory[7].Size << endl << endl;

		cout
			<< "global pointer table: " << endl
			<< "virtual address: " << m_optional_header->DataDirectory[8].VirtualAddress << endl
			<< "size: " << m_optional_header->DataDirectory[8].Size << endl << endl;

		cout
			<< "TLS table: " << endl
			<< "virtual address: " << m_optional_header->DataDirectory[9].VirtualAddress << endl
			<< "size: " << m_optional_header->DataDirectory[9].Size << endl << endl;

		cout
			<< "load config table: " << endl
			<< "virtual address: " << m_optional_header->DataDirectory[10].VirtualAddress << endl
			<< "size: " << m_optional_header->DataDirectory[10].Size << endl << endl;

		cout
			<< "bound import table: " << endl
			<< "virtual address: " << m_optional_header->DataDirectory[11].VirtualAddress << endl
			<< "size: " << m_optional_header->DataDirectory[11].Size << endl << endl;

		cout
			<< "IAT table: " << endl
			<< "virtual address: " << m_optional_header->DataDirectory[12].VirtualAddress << endl
			<< "size: " << m_optional_header->DataDirectory[12].Size << endl << endl;


		cout
			<< "delay import table: " << endl
			<< "virtual address: " << m_optional_header->DataDirectory[13].VirtualAddress << endl
			<< "size: " << m_optional_header->DataDirectory[13].Size << endl << endl;

		cout
			<< "descriptor table: " << endl
			<< "virtual address: " << m_optional_header->DataDirectory[14].VirtualAddress << endl
			<< "size: " << m_optional_header->DataDirectory[14].Size << endl << endl;
	}
}


void PEModifier::PrintSectionHeaders() const
{
	if (!m_ready)
	{
		std::cerr << "PEHeaderAnalyzer is not ready\n";
		return;
	}
	{
		using namespace std;
		cout << hex << "SECTION HEADERS" << endl;
		char name[9];
		for (int i = 0; i < m_file_header->NumberOfSections; i++)
		{
			memset(name, '\0', 9);
			_memccpy(name, this->m_section_headers[i].Name, '\0', sizeof(this->m_section_headers[i].Name));

			cout << "name: " << name << endl;
			cout << "virtual_size: " << hex << this->m_section_headers[i].Misc.VirtualSize << endl;
			cout << "virtual_address: " << hex << this->m_section_headers[i].VirtualAddress << endl;
			cout << "size_of_raw_data: " << hex << this->m_section_headers[i].SizeOfRawData << endl;
			cout << "pointer_to_raw_data: " << hex << this->m_section_headers[i].PointerToRawData << endl;
			cout << "pointer_to_relocations: " << hex << this->m_section_headers[i].PointerToRelocations << endl;
			cout << "pointer_to_linenumbers: " << hex << this->m_section_headers[i].PointerToLinenumbers << endl;
			cout << "number_of_relocations: " << hex << this->m_section_headers[i].NumberOfRelocations << endl;
			cout << "number_of_linenumbers: " << hex << this->m_section_headers[i].NumberOfLinenumbers << endl;
			cout << "characteristics: " << hex << this->m_section_headers[i].Characteristics << endl << endl;
		}
	}
}


void PEModifier::PrintExportTable() const
{
	if (!m_ready)
	{
		std::cerr << "PEHeaderAnalyzer is not ready\n";
		return;
	}

	using namespace std;
	auto file_buffer_content = reinterpret_cast<char*>(m_file_buffer);
	auto export_table = reinterpret_cast<PIMAGE_EXPORT_DIRECTORY>(file_buffer_content + RvaToFileOffset(m_optional_header->DataDirectory[0].VirtualAddress));

}



void PEModifier::PrintBaseRelocationTable() const
{
	if (!m_ready)
	{
		std::cerr << "PEHeaderAnalyzer is not ready\n";
		return;
	}

	using namespace std;

	auto file_buffer_content = reinterpret_cast<char*>(m_file_buffer);
	auto base_relocation_table = reinterpret_cast<PIMAGE_BASE_RELOCATION>(file_buffer_content + RvaToFileOffset(m_optional_header->DataDirectory[5].VirtualAddress));

	cout << "base relocation table: " << endl;
	while (base_relocation_table->VirtualAddress != 0 && base_relocation_table->SizeOfBlock != 0)
	{
		cout << "virtual address: " << hex << base_relocation_table->VirtualAddress << endl;
		cout << "size: " << hex << base_relocation_table->SizeOfBlock << endl;

		auto section_header = m_section_headers + m_file_header->NumberOfSections;
		for (int i = 0; i < m_file_header->NumberOfSections; i++)
		{
			section_header--;
			if (section_header->VirtualAddress < base_relocation_table->VirtualAddress)
			{
				cout << "section: " << section_header->Name << endl << endl;
				break;
			}
		}

		auto number_of_offsets = (base_relocation_table->SizeOfBlock - 0x8) / 2;
		auto entry_start = reinterpret_cast<short*>(base_relocation_table + 1);
		for (unsigned long i = 0; i < number_of_offsets; i++)
		{
			auto entry = *(entry_start + i);
			cout
				<< i + 1 << "\'s entry"
				<< "; virtual address: " << hex << base_relocation_table->VirtualAddress + (entry & 0x0FFF)
				<< "; type of relocation: " << dec << ((entry & 0xF000) >> 12) << endl;
		}

		cout << endl << endl;

		base_relocation_table = reinterpret_cast<PIMAGE_BASE_RELOCATION>(reinterpret_cast<char*>(base_relocation_table) + base_relocation_table->SizeOfBlock);
	}
}



void PEModifier::PrintImportTable() const
{
	if (!m_ready)
	{
		std::cerr << "PEHeaderAnalyzer is not ready\n";
		return;
	}

	using namespace std;

	char* file_buffer = static_cast<char*>(m_file_buffer);
	auto import_descriptor = reinterpret_cast<PIMAGE_IMPORT_DESCRIPTOR>(file_buffer + RvaToFileOffset(m_optional_header->DataDirectory[1].VirtualAddress));
	while (
		import_descriptor->OriginalFirstThunk != 0 ||
		import_descriptor->TimeDateStamp != 0 ||
		import_descriptor->ForwarderChain != 0 ||
		import_descriptor->Name != 0 ||
		import_descriptor->FirstThunk != 0)
	{
		cout << "*****" << file_buffer + RvaToFileOffset(import_descriptor->Name) << "******" << endl;

		if (static_cast<signed long>(import_descriptor->TimeDateStamp) == -1)
		{
			cout << "bound import table is used!" << endl << endl;
			import_descriptor++;
			continue;
		}

		//cout << "timestamp:  " << import_descriptor->TimeDateStamp << endl << endl;

		auto thunk_data = reinterpret_cast<PIMAGE_THUNK_DATA>(file_buffer + RvaToFileOffset(import_descriptor->OriginalFirstThunk));
		while (thunk_data->u1.Function != 0)
		{
			if ((thunk_data->u1.Ordinal & 0x80000000) != 0)
			{
				auto ordinal = thunk_data->u1.Ordinal & 0x7FFFFFFF;
				cout << "import by ordinal: " << ordinal << endl;
			}
			else
			{
				auto function_name = reinterpret_cast<PIMAGE_IMPORT_BY_NAME>(file_buffer + RvaToFileOffset(thunk_data->u1.Function));
				cout << "import by name: " << function_name->Name << endl;
			}

			thunk_data++;
		}

		cout << endl << endl;
		import_descriptor++;
	}

}


void PEModifier::PrintResourceTabble() const
{
	if (!m_ready)
	{
		std::cerr << "PEHeaderAnalyzer is not ready\n";
		return;
	}

	using namespace std;

	//first level - resource types
	auto file_buffer = static_cast<char*>(m_file_buffer);
	auto resource_type_directory = reinterpret_cast<PIMAGE_RESOURCE_DIRECTORY>(file_buffer + RvaToFileOffset(m_optional_header->DataDirectory[2].VirtualAddress));
	auto number_of_resource_types = resource_type_directory->NumberOfIdEntries + resource_type_directory->NumberOfNamedEntries;
	cout << "there are " << number_of_resource_types << " types of resource in this program" << endl;

	auto resource_type_entry = reinterpret_cast<PIMAGE_RESOURCE_DIRECTORY_ENTRY>(resource_type_directory + 1);
	PrintResourceTypes(reinterpret_cast<char*>(resource_type_directory), resource_type_directory, resource_type_entry);
}

void PEModifier::PrintResourceTypes(char* resource_table_entry, PIMAGE_RESOURCE_DIRECTORY resource_type_directory, PIMAGE_RESOURCE_DIRECTORY_ENTRY entry) const
{
	using namespace std;
	auto number_of_resource_types = resource_type_directory->NumberOfIdEntries + resource_type_directory->NumberOfNamedEntries;
	for (int i = 0; i < number_of_resource_types; i++)
	{
		if (entry->NameIsString)
		{
			auto resource_type_name = reinterpret_cast<PIMAGE_RESOURCE_DIR_STRING_U>(resource_table_entry + entry->NameOffset);
			std::wstring name(resource_type_name->NameString, resource_type_name->Length);
			std::wcout << L"resource type : " << name << std::endl;
		}
		else
		{
			string resource_type_name = "unknown";
			switch (entry->Id)
			{
			case (int)RT_CURSOR:
				resource_type_name = "CURSOR";
				break;
			case (int)RT_BITMAP:
				resource_type_name = "BITMAP";
				break;
			case (int)RT_ICON:
				resource_type_name = "ICON";
				break;
			case (int)RT_MENU:
				resource_type_name = "MENU";
				break;
			case (int)RT_DIALOG:
				resource_type_name = "DIALOG";
				break;
			case (int)RT_STRING:
				resource_type_name = "STRING";
				break;
			case (int)RT_FONTDIR:
				resource_type_name = "FONTDIR";
				break;
			case (int)RT_FONT:
				resource_type_name = "FONT";
				break;
			case (int)RT_ACCELERATOR:
				resource_type_name = "ACCELERATOR";
				break;
			case (int)RT_RCDATA:
				resource_type_name = "RCDATA";
				break;
			case (int)RT_MESSAGETABLE:
				resource_type_name = "MESSAGETABLE";
				break;
			case (int)RT_GROUP_CURSOR:
				resource_type_name = "GROUP_CURSOR";
				break;
			case (int)RT_GROUP_ICON:
				resource_type_name = "GROUP_ICON";
				break;
			case (int)RT_VERSION:
				resource_type_name = "VERSION";
				break;
			case (int)RT_DLGINCLUDE:
				resource_type_name = "DLGINCLUDE";
				break;
			case (int)RT_PLUGPLAY:
				resource_type_name = "PLUGPLAY";
				break;
			case (int)RT_VXD:
				resource_type_name = "VXD";
				break;
			case (int)RT_ANICURSOR:
				resource_type_name = "ANICURSOR";
				break;
			case (int)RT_ANIICON:
				resource_type_name = "ANIICON";
				break;
			case (int)RT_HTML:
				resource_type_name = "HTML";
				break;
			case (int)RT_MANIFEST:
				resource_type_name = "MANIFEST";
				break;
			}
			std::cout << "resource type : " << resource_type_name << std::endl;
		}

		//second level - resource IDs
		PIMAGE_RESOURCE_DIRECTORY resource_id_directory = reinterpret_cast<PIMAGE_RESOURCE_DIRECTORY>(resource_table_entry + entry->OffsetToDirectory);
		PIMAGE_RESOURCE_DIRECTORY_ENTRY resource_id_entry = reinterpret_cast<PIMAGE_RESOURCE_DIRECTORY_ENTRY>(resource_id_directory + 1);
		PrintResourceIDs(resource_table_entry, resource_id_directory, resource_id_entry);
		std::cout << std::endl;
		entry++;
	}
}

void PEModifier::PrintResourceIDs(char* resource_table_entry, PIMAGE_RESOURCE_DIRECTORY resource_ID_directory, PIMAGE_RESOURCE_DIRECTORY_ENTRY entry) const
{
	using namespace std;
	auto number_of_resource_ids = resource_ID_directory->NumberOfIdEntries + resource_ID_directory->NumberOfNamedEntries;
	for (int i = 0; i < number_of_resource_ids; i++)
	{
		wcout << L"  resource ID : " << entry->Id << endl;
		auto resource_language_directory = reinterpret_cast<PIMAGE_RESOURCE_DIRECTORY>(resource_table_entry + entry->OffsetToDirectory);
		auto resource_language_entry = reinterpret_cast<PIMAGE_RESOURCE_DIRECTORY_ENTRY>(resource_language_directory + 1);
		PrintResourceLanguages(resource_table_entry, resource_language_directory, resource_language_entry);
		entry++;
	}
}

void PEModifier::PrintResourceLanguages(char* resource_table_entry, PIMAGE_RESOURCE_DIRECTORY resource_language_directory, PIMAGE_RESOURCE_DIRECTORY_ENTRY entry) const
{
	using namespace std;
	auto number_of_resource_languages = resource_language_directory->NumberOfIdEntries + resource_language_directory->NumberOfNamedEntries;
	for (int i = 0; i < number_of_resource_languages; i++)
	{
		WCHAR localeName[LOCALE_NAME_MAX_LENGTH];
		if (LCIDToLocaleName(entry->Id, localeName, LOCALE_NAME_MAX_LENGTH, 0) > 0) {
			wcout << L"    " << localeName;
		}
		else {
			wcout << L"    " << localeName;
			entry++;
		}

		wcout << L" -> " << DWORD(resource_table_entry + entry->OffsetToDirectory) << endl;
		entry++;
	}
}



IMAGE_DOS_HEADER& PEModifier::dos_header() const
{
	return *m_dos_header;
}

IMAGE_FILE_HEADER& PEModifier::file_header() const
{
	return *m_file_header;
}

IMAGE_OPTIONAL_HEADER& PEModifier::optional_header() const
{
	return *m_optional_header;
}


PIMAGE_SECTION_HEADER PEModifier::section_headers() const
{
	return m_section_headers;
}


DWORD PEModifier::RvaToFileOffset(DWORD dwRva) const
{
	if (dwRva < m_section_headers->VirtualAddress)
		return dwRva;

	auto section_header = m_section_headers + m_file_header->NumberOfSections;

	for (int i = 0; i < m_file_header->NumberOfSections; i++)
	{
		section_header--;
		if (section_header->VirtualAddress <= dwRva)
			break;
	}

	return (dwRva - section_header->VirtualAddress + section_header->PointerToRawData);
}

DWORD PEModifier::foaToVirtualAddress(DWORD dwFoa) const
{
	if (dwFoa < m_section_headers->PointerToRawData)
		return dwFoa;

	auto section_header = m_section_headers + m_file_header->NumberOfSections;

	for (int i = 0; i < m_file_header->NumberOfSections; i++)
	{
		section_header--;
		if (section_header->PointerToRawData <= dwFoa)
			break;
	}

	return (dwFoa - section_header->PointerToRawData + section_header->VirtualAddress);
}


int PEModifier::GetNumberOfSections() const
{
	return m_file_header->NumberOfSections;
}
