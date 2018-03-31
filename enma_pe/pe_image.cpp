#include "stdafx.h"
#include "pe_image.h"


pe_image::pe_image() {
	clear_image();
}
pe_image::pe_image(const pe_image& image) {
    this->operator=(image);
}

pe_image::pe_image(bool _pe32) {
	clear_image();

	if (_pe32) {
		magic = IMAGE_NT_OPTIONAL_HDR32_MAGIC;
	}
	else {
		magic = IMAGE_NT_OPTIONAL_HDR64_MAGIC;
	}

	image_status = pe_image_status_ok;
}

pe_image::pe_image(void* pe_image, unsigned int size) {
	clear_image();
	init_from_file(pe_image, size);
}

pe_image::pe_image(std::string& file_path) {
	clear_image();
    OFSTRUCT of_struct = { 0 };
	HANDLE hfile = (HANDLE)OpenFile(file_path.c_str(), &of_struct, OF_READ);

	if (hfile && hfile != INVALID_HANDLE_VALUE) {
		unsigned int file_size = GetFileSize(hfile, 0);
		unsigned int read_size = 0;
		void * file_buffer = new BYTE[file_size];

        if (file_buffer) {
            if (ReadFile(hfile, file_buffer, file_size, (DWORD*)&read_size, 0) &&
                file_size == read_size) {
                init_from_file(file_buffer, file_size);
            }
            else {
                image_status = pe_image_status_bad_format;
            }

            delete[] file_buffer;
        }
		CloseHandle(hfile);
	}
	else {
		image_status = pe_image_status_unknown;
	}
}

pe_image::~pe_image() {
	clear_image();
}

pe_image& pe_image::operator=(const pe_image& image) {
	clear_image();

    dos_stub.set_dos_stub(image.dos_stub.get_dos_stub());
    rich_data = image.rich_data;

	image_status		= image.image_status;
	machine				= image.machine;
	timestamp			= image.timestamp;
	characteristics		= image.characteristics;
	magic				= image.magic;
	major_linker		= image.major_linker;
	minor_linker		= image.minor_linker;
	size_of_code		= image.size_of_code;
	size_of_init_data	= image.size_of_init_data;
	size_of_uninit_data = image.size_of_uninit_data;
	entry_point			= image.entry_point;
	base_of_code		= image.base_of_code;
	base_of_data		= image.base_of_data;
	image_base			= image.image_base;
	section_align		= image.section_align;
	file_align			= image.file_align;
	os_ver_major		= image.os_ver_major;
	os_ver_minor		= image.os_ver_minor;
	image_ver_major		= image.image_ver_major;
	image_ver_minor		= image.image_ver_minor;
	subsystem_ver_major = image.subsystem_ver_major;
	subsystem_ver_minor = image.subsystem_ver_minor;
	image_size			= image.image_size;
	headers_size		= image.headers_size;
	checksum			= image.checksum;
	sub_system			= image.sub_system;
	characteristics_dll = image.characteristics_dll;
	stack_reserve_size	= image.stack_reserve_size;
	stack_commit_size	= image.stack_commit_size;
	heap_reserve_size	= image.heap_reserve_size;
	heap_commit_size	= image.heap_commit_size;

	directories[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress	= image.directories[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
	directories[IMAGE_DIRECTORY_ENTRY_EXPORT].Size				= image.directories[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;
	directories[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress	= image.directories[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
	directories[IMAGE_DIRECTORY_ENTRY_IMPORT].Size				= image.directories[IMAGE_DIRECTORY_ENTRY_IMPORT].Size;
	directories[IMAGE_DIRECTORY_ENTRY_RESOURCE].VirtualAddress	= image.directories[IMAGE_DIRECTORY_ENTRY_RESOURCE].VirtualAddress;
	directories[IMAGE_DIRECTORY_ENTRY_RESOURCE].Size			= image.directories[IMAGE_DIRECTORY_ENTRY_RESOURCE].Size;
	directories[IMAGE_DIRECTORY_ENTRY_EXCEPTION].VirtualAddress = image.directories[IMAGE_DIRECTORY_ENTRY_EXCEPTION].VirtualAddress;
	directories[IMAGE_DIRECTORY_ENTRY_EXCEPTION].Size			= image.directories[IMAGE_DIRECTORY_ENTRY_EXCEPTION].Size;
	directories[IMAGE_DIRECTORY_ENTRY_SECURITY].VirtualAddress	= image.directories[IMAGE_DIRECTORY_ENTRY_SECURITY].VirtualAddress;
	directories[IMAGE_DIRECTORY_ENTRY_SECURITY].Size			= image.directories[IMAGE_DIRECTORY_ENTRY_SECURITY].Size;
	directories[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress = image.directories[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress;
	directories[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size			= image.directories[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size;
	directories[IMAGE_DIRECTORY_ENTRY_DEBUG].VirtualAddress		= image.directories[IMAGE_DIRECTORY_ENTRY_DEBUG].VirtualAddress;
	directories[IMAGE_DIRECTORY_ENTRY_DEBUG].Size				= image.directories[IMAGE_DIRECTORY_ENTRY_DEBUG].Size;
	directories[IMAGE_DIRECTORY_ENTRY_ARCHITECTURE].VirtualAddress = image.directories[IMAGE_DIRECTORY_ENTRY_ARCHITECTURE].VirtualAddress;
	directories[IMAGE_DIRECTORY_ENTRY_ARCHITECTURE].Size		= image.directories[IMAGE_DIRECTORY_ENTRY_ARCHITECTURE].Size;
	directories[IMAGE_DIRECTORY_ENTRY_GLOBALPTR].VirtualAddress = image.directories[IMAGE_DIRECTORY_ENTRY_GLOBALPTR].VirtualAddress;
	directories[IMAGE_DIRECTORY_ENTRY_GLOBALPTR].Size			= image.directories[IMAGE_DIRECTORY_ENTRY_GLOBALPTR].Size;
	directories[IMAGE_DIRECTORY_ENTRY_TLS].VirtualAddress		= image.directories[IMAGE_DIRECTORY_ENTRY_TLS].VirtualAddress;
	directories[IMAGE_DIRECTORY_ENTRY_TLS].Size					= image.directories[IMAGE_DIRECTORY_ENTRY_TLS].Size;
	directories[IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG].VirtualAddress = image.directories[IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG].VirtualAddress;
	directories[IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG].Size			= image.directories[IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG].Size;
	directories[IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT].VirtualAddress = image.directories[IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT].VirtualAddress;
	directories[IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT].Size		= image.directories[IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT].Size;
	directories[IMAGE_DIRECTORY_ENTRY_IAT].VirtualAddress		= image.directories[IMAGE_DIRECTORY_ENTRY_IAT].VirtualAddress;
	directories[IMAGE_DIRECTORY_ENTRY_IAT].Size					= image.directories[IMAGE_DIRECTORY_ENTRY_IAT].Size;
	directories[IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT].VirtualAddress = image.directories[IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT].VirtualAddress;
	directories[IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT].Size		= image.directories[IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT].Size;
	directories[IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR].VirtualAddress = image.directories[IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR].VirtualAddress;
	directories[IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR].Size		= image.directories[IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR].Size;

	for (unsigned int i = 0; i < image.get_sections_number(); i++) {
		this->add_section(*image.sections[i]);
	}

	return *this;
}


void pe_image::init_from_file(void * image, unsigned int size) {
	PIMAGE_DOS_HEADER dos_header = PIMAGE_DOS_HEADER(image);
	if (size < sizeof(IMAGE_DOS_HEADER)) {this->image_status = pe_image_status_bad_format;return;};


	if (dos_header->e_magic == IMAGE_DOS_SIGNATURE) { //check MZ sign

        get_image_dos_stub(image,dos_stub);

        if (has_image_rich_data(image) && checksum_rich(image)) {
            get_image_rich_data(image, rich_data);
        }

		if (*(DWORD*)(&((BYTE*)image)[dos_header->e_lfanew]) == IMAGE_NT_SIGNATURE) { //check PE00 sign
			unsigned int section_offset = dos_header->e_lfanew;
			unsigned int number_of_sections = 0;
			
			if (*(WORD*)(&((BYTE*)image)[(DWORD)dos_header->e_lfanew + (DWORD)offsetof(IMAGE_NT_HEADERS32, OptionalHeader.Magic)]) == IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
				if (size < section_offset + sizeof(IMAGE_NT_HEADERS32)) { this->image_status = pe_image_status_bad_format; return; };

				PIMAGE_NT_HEADERS32 pe_header = (PIMAGE_NT_HEADERS32)(&((BYTE*)image)[dos_header->e_lfanew]);

				set_machine(pe_header->FileHeader.Machine);
				set_timestamp(pe_header->FileHeader.TimeDateStamp);
				set_characteristics(pe_header->FileHeader.Characteristics);
				set_magic(pe_header->OptionalHeader.Magic);
				set_major_linker(pe_header->OptionalHeader.MajorLinkerVersion);
				set_minor_linker(pe_header->OptionalHeader.MinorLinkerVersion);
				set_size_of_code(pe_header->OptionalHeader.SizeOfCode);
				set_size_of_init_data(pe_header->OptionalHeader.SizeOfInitializedData);
				set_size_of_uninit_data(pe_header->OptionalHeader.SizeOfUninitializedData);
				set_entry_point(pe_header->OptionalHeader.AddressOfEntryPoint);
				set_base_of_code(pe_header->OptionalHeader.BaseOfCode);
				set_base_of_data(pe_header->OptionalHeader.BaseOfData);
				set_image_base(pe_header->OptionalHeader.ImageBase);
				set_section_align(pe_header->OptionalHeader.SectionAlignment);
				set_file_align(pe_header->OptionalHeader.FileAlignment);
				set_os_ver_major(pe_header->OptionalHeader.MajorOperatingSystemVersion);
				set_os_ver_minor(pe_header->OptionalHeader.MinorOperatingSystemVersion);
				set_image_ver_major(pe_header->OptionalHeader.MajorImageVersion);
				set_image_ver_minor(pe_header->OptionalHeader.MinorImageVersion);
				set_subsystem_ver_major(pe_header->OptionalHeader.MajorSubsystemVersion);
				set_subsystem_ver_minor(pe_header->OptionalHeader.MinorSubsystemVersion);
				set_image_size(pe_header->OptionalHeader.SizeOfImage);
				set_headers_size(pe_header->OptionalHeader.SizeOfHeaders);
				set_checksum(pe_header->OptionalHeader.CheckSum);
				set_sub_system(pe_header->OptionalHeader.Subsystem);
				set_characteristics_dll(pe_header->OptionalHeader.DllCharacteristics);
				set_stack_reserve_size(pe_header->OptionalHeader.SizeOfStackReserve);
				set_stack_commit_size(pe_header->OptionalHeader.SizeOfStackCommit);
				set_heap_reserve_size(pe_header->OptionalHeader.SizeOfHeapReserve);
				set_heap_commit_size(pe_header->OptionalHeader.SizeOfHeapCommit);

				for (unsigned int i = 0; i < 16; i++) {
					set_directory_virtual_address(i, pe_header->OptionalHeader.DataDirectory[i].VirtualAddress);
					set_directory_virtual_size(i, pe_header->OptionalHeader.DataDirectory[i].Size);
				}


				number_of_sections = pe_header->FileHeader.NumberOfSections;
				section_offset += sizeof(IMAGE_NT_HEADERS32);
			}
			else if (*(WORD*)(&((BYTE*)image)[(DWORD)dos_header->e_lfanew + (DWORD)offsetof(IMAGE_NT_HEADERS32, OptionalHeader.Magic)]) == IMAGE_NT_OPTIONAL_HDR64_MAGIC) {
				if (size < section_offset + sizeof(IMAGE_NT_HEADERS64)) { this->image_status = pe_image_status_bad_format; return; };

				PIMAGE_NT_HEADERS64 pe_header = (PIMAGE_NT_HEADERS64)(&((BYTE*)image)[dos_header->e_lfanew]);

				set_machine(pe_header->FileHeader.Machine);
				set_timestamp(pe_header->FileHeader.TimeDateStamp);
				set_characteristics(pe_header->FileHeader.Characteristics);
				set_magic(pe_header->OptionalHeader.Magic);
				set_major_linker(pe_header->OptionalHeader.MajorLinkerVersion);
				set_minor_linker(pe_header->OptionalHeader.MinorLinkerVersion);
				set_size_of_code(pe_header->OptionalHeader.SizeOfCode);
				set_size_of_init_data(pe_header->OptionalHeader.SizeOfInitializedData);
				set_size_of_uninit_data(pe_header->OptionalHeader.SizeOfUninitializedData);
				set_entry_point(pe_header->OptionalHeader.AddressOfEntryPoint);
				set_base_of_code(pe_header->OptionalHeader.BaseOfCode);
				set_base_of_data(0);
				set_image_base(pe_header->OptionalHeader.ImageBase);
				set_section_align(pe_header->OptionalHeader.SectionAlignment);
				set_file_align(pe_header->OptionalHeader.FileAlignment);
				set_os_ver_major(pe_header->OptionalHeader.MajorOperatingSystemVersion);
				set_os_ver_minor(pe_header->OptionalHeader.MinorOperatingSystemVersion);
				set_image_ver_major(pe_header->OptionalHeader.MajorImageVersion);
				set_image_ver_minor(pe_header->OptionalHeader.MinorImageVersion);
				set_subsystem_ver_major(pe_header->OptionalHeader.MajorSubsystemVersion);
				set_subsystem_ver_minor(pe_header->OptionalHeader.MinorSubsystemVersion);
				set_image_size(pe_header->OptionalHeader.SizeOfImage);
				set_headers_size(pe_header->OptionalHeader.SizeOfHeaders);
				set_checksum(pe_header->OptionalHeader.CheckSum);
				set_sub_system(pe_header->OptionalHeader.Subsystem);
				set_characteristics_dll(pe_header->OptionalHeader.DllCharacteristics);
				set_stack_reserve_size(pe_header->OptionalHeader.SizeOfStackReserve);
				set_stack_commit_size(pe_header->OptionalHeader.SizeOfStackCommit);
				set_heap_reserve_size(pe_header->OptionalHeader.SizeOfHeapReserve);
				set_heap_commit_size(pe_header->OptionalHeader.SizeOfHeapCommit);

				for (unsigned int i = 0; i < 16; i++) {
					set_directory_virtual_address(i, pe_header->OptionalHeader.DataDirectory[i].VirtualAddress);
					set_directory_virtual_size(i, pe_header->OptionalHeader.DataDirectory[i].Size);
				}

				number_of_sections = pe_header->FileHeader.NumberOfSections;
				section_offset += sizeof(IMAGE_NT_HEADERS64);
			}
			else {
				this->image_status = pe_image_status_bad_format;
				return;
			}



			for (unsigned int i = 0; i < number_of_sections; i++) {
				PIMAGE_SECTION_HEADER section_image = (PIMAGE_SECTION_HEADER)(&((BYTE*)image)[section_offset]);
				
				if (size < section_offset + sizeof(IMAGE_SECTION_HEADER) ||
					size < section_image->PointerToRawData + section_image->SizeOfRawData
					) {

					this->image_status = pe_image_status_bad_format; return; 
				};

				std::vector<BYTE> section_data;
				section_data.resize(section_image->SizeOfRawData);
				memcpy((void*)section_data.data(), (&((BYTE*)image)[section_image->PointerToRawData]), section_image->SizeOfRawData);

				add_section(pe_section(*section_image, section_data));
				section_offset += sizeof(IMAGE_SECTION_HEADER);
			}


			this->image_status = pe_image_status_ok;
			return;
		}

	}

	this->image_status = pe_image_status_bad_format;
}


bool	pe_image::is_x32_image() const{
	if (this->magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
		return true;
	}
	return false;
}
bool	pe_image::is_dll_image()  const {
	if (this->characteristics&IMAGE_FILE_DLL) {
		return true;
	}
	return false;
}
bool	pe_image::is_dotnet_image() const {
	return get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR) != 0;
}

void pe_image::clear_image() {
	image_status = pe_image_status_unknown;
	machine				= 0;
	timestamp			= 0;
	characteristics		= 0;
	magic				= 0;
	major_linker		= 0;
	minor_linker		= 0;
	size_of_code		= 0;
	size_of_init_data	= 0;
	size_of_uninit_data = 0;
	entry_point			= 0;
	base_of_code		= 0;
	base_of_data		= 0;
	image_base			= 0;
	section_align		= 0;
	file_align			= 0;
	os_ver_major		= 0;
	os_ver_minor		= 0;
	image_ver_major		= 0;
	image_ver_minor		= 0;
	subsystem_ver_major = 0;
	subsystem_ver_minor = 0;
	image_size			= 0;
	headers_size		= 0;
	checksum			= 0;
	sub_system			= 0;
	characteristics_dll = 0;
	stack_reserve_size	= 0;
	stack_commit_size	= 0;
	heap_reserve_size	= 0;
	heap_commit_size	= 0;

	directories[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress		= 0;
	directories[IMAGE_DIRECTORY_ENTRY_EXPORT].Size					= 0;
	directories[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress		= 0;
	directories[IMAGE_DIRECTORY_ENTRY_IMPORT].Size					= 0;
	directories[IMAGE_DIRECTORY_ENTRY_RESOURCE].VirtualAddress		= 0;
	directories[IMAGE_DIRECTORY_ENTRY_RESOURCE].Size				= 0;
	directories[IMAGE_DIRECTORY_ENTRY_EXCEPTION].VirtualAddress		= 0;
	directories[IMAGE_DIRECTORY_ENTRY_EXCEPTION].Size				= 0;
	directories[IMAGE_DIRECTORY_ENTRY_SECURITY].VirtualAddress		= 0;
	directories[IMAGE_DIRECTORY_ENTRY_SECURITY].Size				= 0;
	directories[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress		= 0;
	directories[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size				= 0;
	directories[IMAGE_DIRECTORY_ENTRY_DEBUG].VirtualAddress			= 0;
	directories[IMAGE_DIRECTORY_ENTRY_DEBUG].Size					= 0;
	directories[IMAGE_DIRECTORY_ENTRY_ARCHITECTURE].VirtualAddress	= 0;
	directories[IMAGE_DIRECTORY_ENTRY_ARCHITECTURE].Size			= 0;
	directories[IMAGE_DIRECTORY_ENTRY_GLOBALPTR].VirtualAddress		= 0;
	directories[IMAGE_DIRECTORY_ENTRY_GLOBALPTR].Size				= 0;
	directories[IMAGE_DIRECTORY_ENTRY_TLS].VirtualAddress			= 0;
	directories[IMAGE_DIRECTORY_ENTRY_TLS].Size						= 0;
	directories[IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG].VirtualAddress	= 0;
	directories[IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG].Size				= 0;
	directories[IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT].VirtualAddress	= 0;
	directories[IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT].Size			= 0;
	directories[IMAGE_DIRECTORY_ENTRY_IAT].VirtualAddress			= 0;
	directories[IMAGE_DIRECTORY_ENTRY_IAT].Size						= 0;
	directories[IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT].VirtualAddress	= 0;
	directories[IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT].Size			= 0;
	directories[IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR].VirtualAddress= 0;
	directories[IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR].Size			= 0;

	while (sections.size()) { delete sections[sections.size() - 1]; sections.pop_back(); }
}




pe_section& pe_image::add_section() {

	DWORD virtual_address = section_align;
	DWORD raw_pointer = file_align;

	if (sections.size()) {
		virtual_address = ALIGN_UP(sections[sections.size() - 1]->get_virtual_address() +
			sections[sections.size() - 1]->get_virtual_size(), section_align);

		raw_pointer = ALIGN_UP(sections[sections.size() - 1]->get_pointer_to_raw() +
			sections[sections.size() - 1]->get_size_of_raw_data(), file_align);

		if (!sections[sections.size() - 1]->get_virtual_size())		{ virtual_address += section_align; }
	}

	sections.push_back(new pe_section());
	pe_section& section =  *sections[sections.size() - 1];

	section.set_virtual_address(virtual_address);
	section.set_pointer_to_raw(raw_pointer);

	return section;
}
pe_section& pe_image::add_section(const pe_section& section) {
	return (add_section() = section);
}
pe_section& pe_image::add_section(const IMAGE_SECTION_HEADER& header) {
	sections.push_back(new pe_section(header));
	return *sections[sections.size() - 1];
}
pe_section& pe_image::add_section(const IMAGE_SECTION_HEADER& header, const std::vector<BYTE> data) {
	sections.push_back(new pe_section(header, data));
	return *sections[sections.size() - 1];
}

std::vector<pe_section*>& pe_image::get_sections() {
	return this->sections;
}

unsigned int pe_image::get_sections_number() const {
	return this->sections.size();
}

pe_section*	 pe_image::get_section_by_rva(DWORD rva) const {
	for (unsigned int section_idx = 0; section_idx < sections.size(); section_idx++) {
		if (
			sections[section_idx]->get_virtual_address() <= rva &&
			sections[section_idx]->get_virtual_address() + sections[section_idx]->get_virtual_size() > rva
			) {
			return sections[section_idx];
		}
	}
	return 0;
}
pe_section*	 pe_image::get_section_by_va(DWORD64 va) const {
	return get_section_by_rva(va_to_rva(va));
}
pe_section*	 pe_image::get_section_by_raw(DWORD raw) const {
	for (unsigned int section_idx = 0; section_idx < sections.size(); section_idx++) {
		if (
			sections[section_idx]->get_pointer_to_raw() <= raw &&
			sections[section_idx]->get_pointer_to_raw() + sections[section_idx]->get_size_of_raw_data() > raw
			) {
			return sections[section_idx];
		}
	}
	return 0;
}
pe_section*	 pe_image::get_section_by_idx(unsigned int idx) const {
	if (this->sections.size() > idx) {
		return this->sections[idx];
	}

	return 0;
}
DWORD		pe_image::va_to_rva(DWORD64 va) const {
    if (va) {
        return DWORD(va - this->image_base);
    }
    else {
        return 0;
    }
}
DWORD		pe_image::va_to_raw(DWORD64 va) const {

	pe_section * section = get_section_by_va(va);
	if (section)
		return (va_to_rva(va) - section->get_virtual_address()) + section->get_pointer_to_raw();
	else
		return 0;
}
DWORD64		pe_image::rva_to_va(DWORD rva) const {
	return (rva + this->image_base);
}
DWORD		pe_image::rva_to_raw(DWORD rva) const {

	pe_section * section = get_section_by_rva(rva);
	if (section)
		return (rva - section->get_virtual_address()) + section->get_pointer_to_raw();
	else
		return 0;

}
DWORD64		pe_image::raw_to_va(DWORD raw) const {
	return rva_to_va(raw_to_rva(raw));
}
DWORD		pe_image::raw_to_rva(DWORD raw) const {
	pe_section * section = get_section_by_raw(raw);
	if (section)
		return  (raw - section->get_pointer_to_raw()) + section->get_virtual_address();
	else
		return 0;
}

bool		pe_image::set_data_by_rva(DWORD rva, void* data, unsigned int data_size) {
	return set_data_by_rva(get_section_by_rva(rva), rva, data, data_size);
}
bool		pe_image::set_data_by_raw(DWORD raw, void* data, unsigned int data_size) {
	return set_data_by_raw(get_section_by_raw(raw), raw, data, data_size);
}
bool		pe_image::set_data_by_rva(pe_section * section, DWORD rva, void* data, unsigned int data_size) {

	if (section) {
		DWORD data_offset = rva - section->get_virtual_address();

		if (section->get_size_of_raw_data() < data_offset + data_size) {
			section->set_size_of_raw_data(data_offset + data_size);
		}

		memcpy(&section->get_section_data().data()[data_offset],
			data, data_size);

		return true;
	}

	return false;
}
bool		pe_image::set_data_by_raw(pe_section * section, DWORD raw, void* data, unsigned int data_size) {

	if (section) {
		DWORD data_offset = raw - section->get_pointer_to_raw();

		if (section->get_size_of_raw_data() < data_offset + data_size) {
			section->set_size_of_raw_data(data_offset + data_size);
		}

		memcpy(&section->get_section_data().data()[data_offset],
			data, data_size);

		return true;
	}

	return false;
}


bool		pe_image::get_data_by_rva(DWORD rva, void* data, unsigned int data_size) const {
	return get_data_by_rva(get_section_by_rva(rva), rva, data, data_size);

}
bool		pe_image::get_data_by_raw(DWORD raw, void* data, unsigned int data_size) const {
	return get_data_by_raw(get_section_by_raw(raw), raw, data, data_size);
}
bool		pe_image::get_data_by_rva(pe_section * section, DWORD rva, void* data, unsigned int data_size) const {

	if (section) {
		if (section->get_virtual_address() <= rva &&
			section->get_virtual_address() + section->get_virtual_size() > rva + data_size) {

			memcpy(data,
				&section->get_section_data().data()[rva - section->get_virtual_address()], data_size);

			return true;
		}
	}

	return false;
}
bool		pe_image::get_data_by_raw(pe_section * section, DWORD raw, void* data, unsigned int data_size) const {

	if (section) {
		if (section->get_pointer_to_raw() <= raw &&
			section->get_pointer_to_raw() + section->get_size_of_raw_data() > raw + data_size) {

			memcpy(data,
				&section->get_section_data().data()[raw - section->get_pointer_to_raw()], data_size);

			return true;
		}
	}

	return false;
}


void		pe_image::set_image_status(pe_image_status status) {
	this->image_status = status;
}
void        pe_image::set_dos_stub(pe_dos_stub& dos_stub) {
    this->dos_stub.set_dos_stub(dos_stub.get_dos_stub());
}
void        pe_image::set_rich_data(std::vector<pe_rich_data>& rich_data) {
    this->rich_data = rich_data;
}
void		pe_image::set_machine(WORD machine) {
	this->machine = machine;
}
void		pe_image::set_timestamp(DWORD timestamp) {
	this->timestamp = timestamp;
}
void		pe_image::set_characteristics(WORD characteristics) {
	this->characteristics = characteristics;
}
void		pe_image::set_magic(WORD magic) {
	this->magic = magic;
}
void		pe_image::set_major_linker(BYTE major_linker) {
	this->major_linker = major_linker;
}
void		pe_image::set_minor_linker(BYTE minor_linker){
	this->minor_linker = minor_linker;
}
void		pe_image::set_size_of_code(DWORD size_of_code) {
	this->size_of_code = size_of_code;
}
void		pe_image::set_size_of_init_data(DWORD size_of_init_data) {
	this->size_of_init_data = size_of_init_data;
}
void		pe_image::set_size_of_uninit_data(DWORD size_of_uninit_data) {
	this->size_of_uninit_data = size_of_uninit_data;
}
void		pe_image::set_entry_point(DWORD	entry_point) {
	this->entry_point = entry_point;
}
void		pe_image::set_base_of_code(DWORD base_of_code) {
	this->base_of_code = base_of_code;
}
void		pe_image::set_base_of_data(DWORD base_of_data) {
	this->base_of_data = base_of_data;
}
void		pe_image::set_image_base(ULONGLONG image_base) {
	this->image_base = image_base;
}
void		pe_image::set_section_align(DWORD section_align) {
	this->section_align = section_align;
}
void		pe_image::set_file_align(DWORD file_align) {
	this->file_align = file_align;
}
void		pe_image::set_os_ver_major(WORD os_ver_major) {
	this->os_ver_major = os_ver_major;
}
void		pe_image::set_os_ver_minor(WORD os_ver_minor) {
	this->os_ver_minor = os_ver_minor;
}
void		pe_image::set_image_ver_major(WORD image_ver_major) {
	this->image_ver_major = image_ver_major;
}
void		pe_image::set_image_ver_minor(WORD image_ver_minor) {
	this->image_ver_minor = image_ver_minor;
}
void		pe_image::set_subsystem_ver_major(WORD subsystem_ver_major) {
	this->subsystem_ver_major = subsystem_ver_major;
}
void		pe_image::set_subsystem_ver_minor(WORD subsystem_ver_minor) {
	this->subsystem_ver_minor = subsystem_ver_minor;
}
void		pe_image::set_image_size(DWORD image_size) {
	this->image_size = image_size;
}
void		pe_image::set_headers_size(DWORD headers_size) {
	this->headers_size = headers_size;
}
void		pe_image::set_checksum(DWORD checksum) {
	this->checksum = checksum;
}
void		pe_image::set_sub_system(WORD sub_system) {
	this->sub_system = sub_system;
}
void		pe_image::set_characteristics_dll(WORD characteristics_dll) {
	this->characteristics_dll = characteristics_dll;
}
void		pe_image::set_stack_reserve_size(ULONGLONG stack_reserve_size) {
	this->stack_reserve_size = stack_reserve_size;
}
void		pe_image::set_stack_commit_size(ULONGLONG stack_commit_size) {
	this->stack_commit_size = stack_commit_size;
}
void		pe_image::set_heap_reserve_size(ULONGLONG heap_reserve_size) {
	this->heap_reserve_size = heap_reserve_size;
}
void		pe_image::set_heap_commit_size(ULONGLONG heap_commit_size) {
	this->heap_commit_size = heap_commit_size;
}
void		pe_image::set_directory_virtual_address(unsigned int directory_idx, DWORD virtual_address) {
	if (directory_idx < IMAGE_NUMBEROF_DIRECTORY_ENTRIES) {
		this->directories[directory_idx].VirtualAddress = virtual_address;
	}
}
void		pe_image::set_directory_virtual_size(unsigned int directory_idx, DWORD virtual_size) {
	if (directory_idx < IMAGE_NUMBEROF_DIRECTORY_ENTRIES) {
		this->directories[directory_idx].Size = virtual_size;
	}
}


pe_image_status		pe_image::get_image_status() const {
	return image_status;
}
bool        pe_image::has_dos_stub() const {
    return dos_stub.get_dos_stub().size() != 0;
}
bool        pe_image::has_rich_data() const {
    return rich_data.size() != 0;
}
WORD		pe_image::get_machine() const {
	return machine;
}
DWORD		pe_image::get_timestamp() const {
	return timestamp;
}
WORD		pe_image::get_characteristics() const {
	return characteristics;
}
WORD		pe_image::get_magic() const {
	return magic;
}
BYTE		pe_image::get_major_linker() const {
	return major_linker;
}
BYTE		pe_image::get_minor_linker() const {
	return minor_linker;
}
DWORD		pe_image::get_size_of_code() const {
	return size_of_code;
}
DWORD		pe_image::get_size_of_init_data() const {
	return size_of_init_data;
}
DWORD		pe_image::get_size_of_uninit_data() const {
	return size_of_uninit_data;
}
DWORD		pe_image::get_entry_point() const {
	return entry_point;
}
DWORD		pe_image::get_base_of_code() const {
	return base_of_code;
}
DWORD		pe_image::get_base_of_data() const {
	return base_of_data;
}
ULONGLONG   pe_image::get_image_base() const {
	return image_base;
}
DWORD		pe_image::get_section_align() const {
	return section_align;
}
DWORD		pe_image::get_file_align() const {
	return file_align;
}
WORD		pe_image::get_os_ver_major() const {
	return os_ver_major;
}
WORD		pe_image::get_os_ver_minor() const {
	return os_ver_minor;
}
WORD		pe_image::get_image_ver_major() const {
	return image_ver_major;
}
WORD		pe_image::get_image_ver_minor() const {
	return image_ver_minor;
}
WORD		pe_image::get_subsystem_ver_major() const {
	return subsystem_ver_major;
}
WORD		pe_image::get_subsystem_ver_minor() const {
	return subsystem_ver_minor;
}
DWORD		pe_image::get_image_size() const {
	return image_size;
}
DWORD		pe_image::get_headers_size() const {
	return headers_size;
}
DWORD		pe_image::get_checksum() const {
	return checksum;
}
WORD		pe_image::get_sub_system() const {
	return sub_system;
}
WORD		pe_image::get_characteristics_dll() const {
	return characteristics_dll;
}
ULONGLONG   pe_image::get_stack_reserve_size() const {
	return stack_reserve_size;
}
ULONGLONG   pe_image::get_stack_commit_size() const {
	return stack_commit_size;
}
ULONGLONG   pe_image::get_heap_reserve_size() const {
	return heap_reserve_size;
}
ULONGLONG   pe_image::get_heap_commit_size() const {
	return heap_commit_size;
}

DWORD		pe_image::get_directory_virtual_address(unsigned int directory_idx) const {
	if (directory_idx < IMAGE_NUMBEROF_DIRECTORY_ENTRIES) {
		return this->directories[directory_idx].VirtualAddress;
	}

	return 0;
}
DWORD		pe_image::get_directory_virtual_size(unsigned int directory_idx) const {
	if (directory_idx < IMAGE_NUMBEROF_DIRECTORY_ENTRIES) {
		return this->directories[directory_idx].Size;
	}

	return 0;
}

bool		pe_image::has_directory(unsigned int directory_idx) const {
	if (directory_idx < IMAGE_NUMBEROF_DIRECTORY_ENTRIES) {
		return (this->directories[directory_idx].VirtualAddress != 0 || this->directories[directory_idx].Size != 0);
	}
	return false;
}

pe_dos_stub& pe_image::get_dos_stub() {
    return dos_stub;
}
std::vector<pe_rich_data>& pe_image::get_rich_data() {
    return rich_data;
}

DWORD calculate_checksum(const std::vector<BYTE> &file) {
    PIMAGE_DOS_HEADER p_dos_header = (PIMAGE_DOS_HEADER)file.data();
    if (p_dos_header->e_magic != IMAGE_DOS_SIGNATURE) { return 0; }

    unsigned int checksum_field_offset = p_dos_header->e_lfanew +
        offsetof(IMAGE_NT_HEADERS32, OptionalHeader.CheckSum);

    DWORD64 checksum = 0;

    for (unsigned int i = 0; i < file.size(); i += sizeof(DWORD)) {
        if (i == checksum_field_offset) { continue; }

        checksum = (checksum & 0xffffffff) + *(DWORD*)&file.data()[i] + (checksum >> 32);

        if (checksum > 0xffffffff) {
            checksum = (checksum & 0xffffffff) + (checksum >> 32);
        }
    }

    checksum = (checksum & 0xffff) + (checksum >> 16);
    checksum = (checksum)+(checksum >> 16);
    checksum = checksum & 0xffff;
    checksum += file.size();

    return DWORD(checksum & 0xffffffff);
}

void do_expanded_pe_image(pe_image_expanded& expanded_image,const pe_image &image) {
    expanded_image.image = image;
	get_export_table(expanded_image.image, expanded_image.exports);
	get_import_table(expanded_image.image, expanded_image.imports);
	get_resources_table(expanded_image.image, expanded_image.resources);
	get_exception_table(expanded_image.image, expanded_image.exceptions);
	get_relocation_table(expanded_image.image, expanded_image.relocations);
	get_debug_table(expanded_image.image, expanded_image.debug);
	get_tls_table(expanded_image.image, expanded_image.tls);
    get_load_config_table(expanded_image.image, expanded_image.load_config);
    get_bound_import_table(expanded_image.image, expanded_image.bound_imports);
	get_delay_import_table(expanded_image.image, expanded_image.delay_imports);
}

void erase_directories_pe_image(pe_image &image, std::vector<erased_zone>& zones, relocation_table* relocs, bool delete_empty_sections) {
	zones.clear();

	erase_export_table(image, &zones);
	erase_import_table(image, &zones);
	erase_resources_table(image, &zones);
	erase_exceptions_table(image, &zones);
	erase_relocation_table(image, &zones);
	erase_debug_table(image, &zones);
	erase_tls_table(image, &zones, relocs);
	erase_load_config_table(image, &zones, relocs);
    erase_bound_import_table(image, &zones);
	erase_delay_import_table(image, &zones);

	std::sort(zones.begin(), zones.end(), [](erased_zone& lhs, erased_zone& rhs) {
		return lhs.rva < rhs.rva;
	});

	for (unsigned int parent_zone_idx = 0; parent_zone_idx + 1 < zones.size(); parent_zone_idx++) { //link zones

		if (zones[parent_zone_idx].rva <= zones[parent_zone_idx + 1].rva &&
			(zones[parent_zone_idx].rva + zones[parent_zone_idx].size) >= zones[parent_zone_idx + 1].rva
			) {

			if ((zones[parent_zone_idx + 1].rva + zones[parent_zone_idx + 1].size) > (zones[parent_zone_idx].rva + zones[parent_zone_idx].size)) {
				zones[parent_zone_idx].size = ((zones[parent_zone_idx + 1].rva + zones[parent_zone_idx + 1].size) - zones[parent_zone_idx].rva);
			}

			zones.erase(zones.begin() + parent_zone_idx + 1);
			parent_zone_idx--;
		}
	}

	if (delete_empty_sections && image.get_sections().size()) {
		for (int section_idx = image.get_sections().size() - 1; section_idx >= 0; section_idx--) {
			auto _section = image.get_sections()[section_idx];

			for (auto& zone : zones) {
				if (_section->get_virtual_address() == zone.rva &&
					_section->get_virtual_size() <= zone.size) {

					image.get_sections().erase(image.get_sections().begin() + section_idx);
					goto go_next_;
				}
			}

			return;
		go_next_:;
		}
	}
}