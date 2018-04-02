#pragma once

enum section_io_mode {
    section_io_mode_default,
    section_io_mode_dont_expand_bound,
};

enum section_io_addressing_type {
    section_address_raw,
    section_address_rva,
};

enum section_io_code {
    section_io_success,
    section_io_Incomplete,
    section_io_bound_break,
};



class pe_section{
	std::string section_name;
	DWORD virtual_size;
	DWORD virtual_address;
	DWORD pointer_to_raw;
	DWORD characteristics;

	std::vector<BYTE> section_data;
public:
	pe_section::pe_section();
	pe_section::pe_section(const pe_section& section);
	pe_section::pe_section(const IMAGE_SECTION_HEADER& header);
	pe_section::pe_section(const IMAGE_SECTION_HEADER& header, const std::vector<BYTE>& data);
	pe_section::~pe_section();
    
	pe_section& pe_section::operator=(const pe_section& section);

public:
	pe_section& pe_section::set_section_name(const std::string& name);
	pe_section& pe_section::set_virtual_size(DWORD virtual_size);
	pe_section& pe_section::set_virtual_address(DWORD virtual_address);
	pe_section& pe_section::set_size_of_raw_data(DWORD size_of_raw_data);
	pe_section& pe_section::set_pointer_to_raw(DWORD pointer_to_raw);
	pe_section& pe_section::set_characteristics(DWORD characteristics);

	pe_section& pe_section::set_readable(bool flag);
	pe_section& pe_section::set_writeable(bool flag);
	pe_section& pe_section::set_executable(bool flag);

	void pe_section::add_data(void * data,unsigned int data_size);
public:
	std::string pe_section::get_section_name() const;
	DWORD pe_section::get_virtual_size() const;
	DWORD pe_section::get_virtual_address() const;
	DWORD pe_section::get_size_of_raw_data() const;
	DWORD pe_section::get_pointer_to_raw() const;
	DWORD pe_section::get_characteristics() const;

	bool pe_section::is_readable() const;
	bool pe_section::is_writeable() const;
	bool pe_section::is_executable() const;

    const std::vector<BYTE>& pe_section::get_section_data() const;
    std::vector<BYTE>& pe_section::get_section_data();
};

class pe_section_io {
    pe_section*  section;
    unsigned int section_offset;

    section_io_code last_code;
    section_io_mode mode;
    section_io_addressing_type addressing_type;


    void pe_section_io::update_section_boundaries();
    section_io_code pe_section_io::get_physical_data(unsigned int data_size,unsigned int &phys_offset);
public:
    pe_section_io::pe_section_io(
        pe_section & _section,
        section_io_mode mode = section_io_mode_default,
        section_io_addressing_type type = section_address_rva
    );

    pe_section_io::pe_section_io(const pe_section_io& io_section);
    pe_section_io::~pe_section_io();

    pe_section_io& pe_section_io::operator=(const pe_section_io& io_section);
public:
    template <class T> section_io_code operator>>(T& data); //read from section
    template <class T> section_io_code operator<<(T& data); //write to section

    section_io_code pe_section_io::read(std::vector<BYTE>& buffer, unsigned int size);
    section_io_code pe_section_io::write(std::vector<BYTE>& buffer, unsigned int size);
public:
    pe_section_io& pe_section_io::align_up(unsigned int factor, bool offset_to_end = true);
    pe_section_io& pe_section_io::add_size(unsigned int size, bool offset_to_end = true);

    pe_section_io& pe_section_io::set_mode(section_io_mode mode);
    pe_section_io& pe_section_io::set_addressing_type(section_io_addressing_type type);
    pe_section_io& pe_section_io::set_section_offset(unsigned int offset);
public:
    section_io_mode            pe_section_io::get_mode() const;
    section_io_code            pe_section_io::get_last_code() const;
    section_io_addressing_type pe_section_io::get_addressing_type() const;
    unsigned int               pe_section_io::get_section_offset() const;

    pe_section* pe_section_io::get_section();
};