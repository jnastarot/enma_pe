#pragma once

#include "pe_rich_dos.h"
#include "pe_section.h"

enum pe_image_status {
	pe_image_status_ok,
	pe_image_status_bad_format,
	pe_image_status_unknown,
};

class pe_image{
	pe_image_status image_status;
	WORD		machine;
	DWORD		timestamp;
	WORD		characteristics;
	WORD		magic;
	BYTE		major_linker;
	BYTE		minor_linker;
	DWORD		size_of_code;
	DWORD		size_of_init_data;
	DWORD		size_of_uninit_data;
	DWORD		entry_point;
	DWORD		base_of_code;
	DWORD		base_of_data;
	ULONGLONG   image_base;
	DWORD		section_align;
	DWORD		file_align;
	WORD		os_ver_major;
	WORD		os_ver_minor;
	WORD		image_ver_major;
	WORD		image_ver_minor;
	WORD		subsystem_ver_major;
	WORD		subsystem_ver_minor;
	DWORD		image_size;
	DWORD		headers_size;
	DWORD		checksum;
	WORD		sub_system;
	WORD		characteristics_dll;
	ULONGLONG   stack_reserve_size;
	ULONGLONG   stack_commit_size;
	ULONGLONG   heap_reserve_size;
	ULONGLONG   heap_commit_size;
	IMAGE_DATA_DIRECTORY directories[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];

    pe_dos_stub dos_stub;
    std::vector<pe_rich_data> rich_data;

	std::vector<pe_section*> sections;

	void pe_image::init_from_file(void * image,unsigned int size);
public:
	pe_image::pe_image();
	pe_image::pe_image(bool _pe32);
	pe_image::pe_image(void* pe_image,unsigned int size);
	pe_image::pe_image(std::string file_path);

	pe_image::~pe_image();

	pe_image& pe_image::operator=(const pe_image& image);
	

	bool	pe_image::is_x32_image() const;
	bool	pe_image::is_dll_image() const;
	bool	pe_image::is_dotnet_image() const;
public:// data/sections helpers
	pe_section& pe_image::add_section();
	pe_section& pe_image::add_section(const pe_section& section);
	pe_section& pe_image::add_section(const IMAGE_SECTION_HEADER& header);
	pe_section& pe_image::add_section(const IMAGE_SECTION_HEADER& header, const std::vector<BYTE> data);

	std::vector<pe_section*>& pe_image::get_sections();

	unsigned int pe_image::get_sections_number() const;;
	pe_section*	 pe_image::get_section_by_rva(DWORD rva) const;/*0 - failed \ not 0 - success*/
	pe_section*	 pe_image::get_section_by_va(DWORD64 va) const;
	pe_section*	 pe_image::get_section_by_raw(DWORD raw) const;
	pe_section*	 pe_image::get_section_by_idx(unsigned int idx) const;

	
	DWORD		pe_image::va_to_rva(DWORD64 va) const;
	DWORD		pe_image::va_to_raw(DWORD64 va) const;
	DWORD64		pe_image::rva_to_va(DWORD rva) const;
	DWORD		pe_image::rva_to_raw(DWORD rva) const;
	DWORD64		pe_image::raw_to_va(DWORD raw) const;
	DWORD		pe_image::raw_to_rva(DWORD raw) const;
public:
	bool		pe_image::set_data_by_rva(DWORD rva, void* data, unsigned int data_size);
	bool		pe_image::set_data_by_raw(DWORD raw, void* data, unsigned int data_size);
	bool		pe_image::set_data_by_rva(pe_section * section, DWORD rva, void* data, unsigned int data_size);
	bool		pe_image::set_data_by_raw(pe_section * section, DWORD raw, void* data, unsigned int data_size);

	bool		pe_image::get_data_by_rva(DWORD rva, void* data, unsigned int data_size) const;
	bool		pe_image::get_data_by_raw(DWORD raw, void* data, unsigned int data_size) const;
	bool		pe_image::get_data_by_rva(pe_section * section, DWORD rva, void* data, unsigned int data_size) const;
	bool		pe_image::get_data_by_raw(pe_section * section, DWORD raw, void* data, unsigned int data_size) const;
public:
	void		pe_image::set_image_status(pe_image_status status);
    void        pe_image::set_dos_stub(pe_dos_stub& dos_stub);
    void        pe_image::set_rich_data(std::vector<pe_rich_data>& rich_data);
	void		pe_image::set_machine(WORD machine);
	void		pe_image::set_timestamp(DWORD timestamp);
	void		pe_image::set_characteristics(WORD characteristics);
	void		pe_image::set_magic(WORD magic);
	void		pe_image::set_major_linker(BYTE major_linker);
	void		pe_image::set_minor_linker(BYTE minor_linker);
	void		pe_image::set_size_of_code(DWORD size_of_code);
	void		pe_image::set_size_of_init_data(DWORD size_of_init_data);
	void		pe_image::set_size_of_uninit_data(DWORD size_of_uninit_data);
	void		pe_image::set_entry_point(DWORD	entry_point);
	void		pe_image::set_base_of_code(DWORD base_of_code);
	void		pe_image::set_base_of_data(DWORD base_of_data);
	void		pe_image::set_image_base(ULONGLONG image_base);
	void		pe_image::set_section_align(DWORD section_align);
	void		pe_image::set_file_align(DWORD file_align);
	void		pe_image::set_os_ver_major(WORD os_ver_major);
	void		pe_image::set_os_ver_minor(WORD os_ver_minor);
	void		pe_image::set_image_ver_major(WORD image_ver_major);
	void		pe_image::set_image_ver_minor(WORD image_ver_minor);
	void		pe_image::set_subsystem_ver_major(WORD subsystem_ver_major);
	void		pe_image::set_subsystem_ver_minor(WORD subsystem_ver_minor);
	void		pe_image::set_image_size(DWORD image_size);
	void		pe_image::set_headers_size(DWORD headers_size);
	void		pe_image::set_checksum(DWORD checksum);
	void		pe_image::set_sub_system(WORD sub_system);
	void		pe_image::set_characteristics_dll(WORD characteristics_dll);
	void		pe_image::set_stack_reserve_size(ULONGLONG stack_reserve_size);
	void		pe_image::set_stack_commit_size(ULONGLONG stack_commit_size);
	void		pe_image::set_heap_reserve_size(ULONGLONG heap_reserve_size);
	void		pe_image::set_heap_commit_size(ULONGLONG heap_commit_size);
	//directories
	void		pe_image::set_directory_virtual_address(unsigned int directory_idx,DWORD virtual_address);
	void		pe_image::set_directory_virtual_size(unsigned int directory_idx, DWORD virtual_size);
public://getter
	pe_image_status		       pe_image::get_image_status() const;
    bool        pe_image::has_dos_stub() const;
    bool        pe_image::has_rich_data() const;
	WORD		pe_image::get_machine() const;
	DWORD		pe_image::get_timestamp() const;
	WORD		pe_image::get_characteristics() const;
	WORD		pe_image::get_magic() const;
	BYTE		pe_image::get_major_linker() const;
	BYTE		pe_image::get_minor_linker() const;
	DWORD		pe_image::get_size_of_code() const;
	DWORD		pe_image::get_size_of_init_data() const;
	DWORD		pe_image::get_size_of_uninit_data() const;
	DWORD		pe_image::get_entry_point() const;
	DWORD		pe_image::get_base_of_code() const;
	DWORD		pe_image::get_base_of_data() const;
	ULONGLONG   pe_image::get_image_base() const;
	DWORD		pe_image::get_section_align() const;
	DWORD		pe_image::get_file_align() const;
	WORD		pe_image::get_os_ver_major() const;
	WORD		pe_image::get_os_ver_minor() const;
	WORD		pe_image::get_image_ver_major() const;
	WORD		pe_image::get_image_ver_minor() const;
	WORD		pe_image::get_subsystem_ver_major() const;
	WORD		pe_image::get_subsystem_ver_minor() const;
	DWORD		pe_image::get_image_size() const;
	DWORD		pe_image::get_headers_size() const;
	DWORD		pe_image::get_checksum() const;
	WORD		pe_image::get_sub_system() const;
	WORD		pe_image::get_characteristics_dll() const;
	ULONGLONG   pe_image::get_stack_reserve_size() const;
	ULONGLONG   pe_image::get_stack_commit_size() const;
	ULONGLONG   pe_image::get_heap_reserve_size() const;
	ULONGLONG   pe_image::get_heap_commit_size() const;
	//directories
	DWORD		pe_image::get_directory_virtual_address(unsigned int directory_idx) const;
	DWORD		pe_image::get_directory_virtual_size(unsigned int directory_idx) const;
	bool		pe_image::has_directory(unsigned int directory_idx) const;

    pe_dos_stub&               pe_image::get_dos_stub();
    std::vector<pe_rich_data>& pe_image::get_rich_data();
public://util
	void				pe_image::clear_image();
};

//void save_pe_image(_In_ const pe_image &image, _Out_ std::vector<BYTE>& image_out);
//void load_pe_image(_In_ std::vector<BYTE>& image_out , _Out_ pe_image &image);

