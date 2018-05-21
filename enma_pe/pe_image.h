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

    pe_dos_header  dos_header;
    pe_dos_stub    dos_stub;
    pe_rich_data   rich_data;

	uint16_t   machine;
	uint32_t   timestamp;
	uint16_t   characteristics;
	uint16_t   magic;
	uint8_t	   major_linker;
	uint8_t    minor_linker;
	uint32_t   size_of_code;
	uint32_t   size_of_init_data;
	uint32_t   size_of_uninit_data;
	uint32_t   entry_point;
	uint32_t   base_of_code;
	uint32_t   base_of_data;
	uint64_t   image_base;
	uint32_t   section_align;
	uint32_t   file_align;
	uint16_t   os_ver_major;
	uint16_t   os_ver_minor;
	uint16_t   image_ver_major;
	uint16_t   image_ver_minor;
	uint16_t   subsystem_ver_major;
	uint16_t   subsystem_ver_minor;
	uint32_t   image_size;
	uint32_t   headers_size;
	uint32_t   checksum;
	uint16_t   sub_system;
	uint16_t   characteristics_dll;
	uint64_t   stack_reserve_size;
	uint64_t   stack_commit_size;
	uint64_t   heap_reserve_size;
	uint64_t   heap_commit_size;
	image_data_directory directories[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];

	std::vector<pe_section*> sections;

    std::vector<uint8_t> overlay_data;

	void pe_image::init_from_file(const uint8_t * image, size_t size);
public:
	pe_image::pe_image();
    pe_image::pe_image(const pe_image& image);
	pe_image::pe_image(bool _pe32);
	pe_image::pe_image(const uint8_t* pe_image,uint32_t size);
	pe_image::pe_image(const std::string& file_path);

	pe_image::~pe_image();

	pe_image& pe_image::operator=(const pe_image& image);
	

	bool	pe_image::is_x32_image() const;
	bool	pe_image::is_dll_image() const;
	bool	pe_image::is_dotnet_image() const;
public:// data/sections helpers
	pe_section& pe_image::add_section();
	pe_section& pe_image::add_section(const pe_section& section);
	pe_section& pe_image::add_section(const image_section_header& header);
	pe_section& pe_image::add_section(const image_section_header& header, const std::vector<uint8_t>& data);

	std::vector<pe_section*>& pe_image::get_sections();

    size_t pe_image::get_sections_number() const;
	pe_section*	 pe_image::get_section_by_rva(uint32_t rva) const;/*0 - failed \ not 0 - success*/
	pe_section*	 pe_image::get_section_by_va(uint64_t va) const;
	pe_section*	 pe_image::get_section_by_raw(uint32_t raw) const;
	pe_section*	 pe_image::get_section_by_idx(uint32_t idx) const;
    pe_section*	 pe_image::get_last_section() const;
	
	uint32_t    pe_image::va_to_rva(uint64_t va) const;
	uint32_t    pe_image::va_to_raw(uint64_t va) const;
    uint64_t    pe_image::rva_to_va(uint32_t rva) const;
	uint32_t    pe_image::rva_to_raw(uint32_t rva) const;
    uint64_t    pe_image::raw_to_va(uint32_t raw) const;
	uint32_t    pe_image::raw_to_rva(uint32_t raw) const;

public:
    void        pe_image::set_image_status(pe_image_status status);
    void        pe_image::set_dos_header(const pe_dos_header& header);
    void        pe_image::set_dos_stub(const pe_dos_stub& dos_stub);
    void        pe_image::set_rich_data(const pe_rich_data& rich_data);
	void        pe_image::set_machine(uint16_t machine);
	void        pe_image::set_timestamp(uint32_t timestamp);
	void        pe_image::set_characteristics(uint16_t characteristics);
	void        pe_image::set_magic(uint16_t magic);
	void        pe_image::set_major_linker(uint8_t major_linker);
	void        pe_image::set_minor_linker(uint8_t minor_linker);
	void        pe_image::set_size_of_code(uint32_t size_of_code);
	void        pe_image::set_size_of_init_data(uint32_t size_of_init_data);
	void        pe_image::set_size_of_uninit_data(uint32_t size_of_uninit_data);
	void        pe_image::set_entry_point(uint32_t	entry_point);
	void        pe_image::set_base_of_code(uint32_t base_of_code);
	void        pe_image::set_base_of_data(uint32_t base_of_data);
	void        pe_image::set_image_base(uint64_t image_base);
	void        pe_image::set_section_align(uint32_t section_align);
	void        pe_image::set_file_align(uint32_t file_align);
	void        pe_image::set_os_ver_major(uint16_t os_ver_major);
	void        pe_image::set_os_ver_minor(uint16_t os_ver_minor);
	void        pe_image::set_image_ver_major(uint16_t image_ver_major);
	void        pe_image::set_image_ver_minor(uint16_t image_ver_minor);
	void        pe_image::set_subsystem_ver_major(uint16_t subsystem_ver_major);
	void        pe_image::set_subsystem_ver_minor(uint16_t subsystem_ver_minor);
	void        pe_image::set_image_size(uint32_t image_size);
	void        pe_image::set_headers_size(uint32_t headers_size);
	void        pe_image::set_checksum(uint32_t checksum);
	void        pe_image::set_sub_system(uint16_t sub_system);
	void        pe_image::set_characteristics_dll(uint16_t characteristics_dll);
	void        pe_image::set_stack_reserve_size(uint64_t stack_reserve_size);
	void        pe_image::set_stack_commit_size(uint64_t stack_commit_size);
	void        pe_image::set_heap_reserve_size(uint64_t heap_reserve_size);
	void        pe_image::set_heap_commit_size(uint64_t heap_commit_size);
    void        pe_image::set_overlay_data(std::vector<uint8_t>& data);
	//directories
	void		pe_image::set_directory_virtual_address(uint32_t directory_idx,uint32_t virtual_address);
	void		pe_image::set_directory_virtual_size(uint32_t directory_idx, uint32_t virtual_size);

    
public://getter
	pe_image_status		       pe_image::get_image_status() const;

    pe_dos_header&  pe_image::get_dos_header();
    pe_dos_stub&    pe_image::get_dos_stub();
    pe_rich_data&   pe_image::get_rich_data();

    bool        pe_image::has_dos_stub() const;
    bool        pe_image::has_rich_data() const;
	uint16_t    pe_image::get_machine() const;
	uint32_t    pe_image::get_timestamp() const;
	uint16_t    pe_image::get_characteristics() const;
	uint16_t    pe_image::get_magic() const;
	uint8_t	    pe_image::get_major_linker() const;
	uint8_t	    pe_image::get_minor_linker() const;
	uint32_t    pe_image::get_size_of_code() const;
	uint32_t    pe_image::get_size_of_init_data() const;
	uint32_t    pe_image::get_size_of_uninit_data() const;
	uint32_t    pe_image::get_entry_point() const;
	uint32_t    pe_image::get_base_of_code() const;
	uint32_t    pe_image::get_base_of_data() const;
	uint64_t    pe_image::get_image_base() const;
	uint32_t    pe_image::get_section_align() const;
	uint32_t    pe_image::get_file_align() const;
	uint16_t    pe_image::get_os_ver_major() const;
	uint16_t    pe_image::get_os_ver_minor() const;
	uint16_t    pe_image::get_image_ver_major() const;
	uint16_t    pe_image::get_image_ver_minor() const;
	uint16_t    pe_image::get_subsystem_ver_major() const;
	uint16_t    pe_image::get_subsystem_ver_minor() const;
	uint32_t    pe_image::get_image_size() const;
	uint32_t    pe_image::get_headers_size() const;
	uint32_t    pe_image::get_checksum() const;
	uint16_t    pe_image::get_sub_system() const;
	uint16_t    pe_image::get_characteristics_dll() const;
	uint64_t    pe_image::get_stack_reserve_size() const;
	uint64_t    pe_image::get_stack_commit_size() const;
	uint64_t    pe_image::get_heap_reserve_size() const;
	uint64_t    pe_image::get_heap_commit_size() const;
    std::vector<uint8_t>&   pe_image::get_overlay_data();
	//directories
	uint32_t    pe_image::get_directory_virtual_address(uint32_t directory_idx) const;
	uint32_t    pe_image::get_directory_virtual_size(uint32_t directory_idx) const;
	bool        pe_image::has_directory(uint32_t directory_idx) const;

    
public://util
	void				pe_image::clear_image();
};

//void save_pe_image(_In_ const pe_image &image, _Out_ std::vector<uint8_t>& image_out);
//void load_pe_image(_In_ std::vector<uint8_t>& image_out , _Out_ pe_image &image);

