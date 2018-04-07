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
    section_io_raw_not_present,
    section_io_incomplete, //part of read\write
    section_io_bound_break,
};



class pe_section{
	std::string section_name;
	uint32_t virtual_size;
	uint32_t virtual_address;
	uint32_t pointer_to_raw;
	uint32_t characteristics;

	std::vector<uint8_t> section_data;
public:
	pe_section::pe_section();
	pe_section::pe_section(const pe_section& section);
	pe_section::pe_section(const image_section_header& header);
	pe_section::pe_section(const image_section_header& header, const std::vector<uint8_t>& data);
	pe_section::~pe_section();
    
	pe_section& pe_section::operator=(const pe_section& section);

public:
	pe_section& pe_section::set_section_name(const std::string& name);
	pe_section& pe_section::set_virtual_size(uint32_t virtual_size);
	pe_section& pe_section::set_virtual_address(uint32_t virtual_address);
	pe_section& pe_section::set_size_of_raw_data(uint32_t size_of_raw_data);
	pe_section& pe_section::set_pointer_to_raw(uint32_t pointer_to_raw);
	pe_section& pe_section::set_characteristics(uint32_t characteristics);

	pe_section& pe_section::set_readable(bool flag);
	pe_section& pe_section::set_writeable(bool flag);
	pe_section& pe_section::set_executable(bool flag);

	void pe_section::add_data(const uint8_t * data,uint32_t data_size);
public:
	std::string pe_section::get_section_name() const;
	uint32_t pe_section::get_virtual_size() const;
	uint32_t pe_section::get_virtual_address() const;
	uint32_t pe_section::get_size_of_raw_data() const;
	uint32_t pe_section::get_pointer_to_raw() const;
	uint32_t pe_section::get_characteristics() const;

	bool pe_section::is_readable() const;
	bool pe_section::is_writeable() const;
	bool pe_section::is_executable() const;

    const std::vector<uint8_t>& pe_section::get_section_data() const;
    std::vector<uint8_t>& pe_section::get_section_data();
};



/*
        ................. section1 data
 |    | |     |     |    ...................section2 data
 v    v |     |     |       |                ...............section3 data
 ...... io data (raw not present)                         |      |     
        |     |     |       |                             |      |
        v     v     |       |                             |      |
        ....... io data (code success)                    |      |
                    |       |                             |      |
                    v       v                             |      |
                    ......... io data (code incomplete)   v      v
                                                          ........ write data (
                                                                        section_io_success = section_io_mode_default
                                                                        code bound break = mode_dont_expand_bound 
                                                                              )    
                                     
                                     */  

class pe_section_io {
    pe_section*  section;
    uint32_t section_offset;

    section_io_code last_code;
    section_io_mode mode;
    section_io_addressing_type addressing_type;


    void pe_section_io::update_section_boundaries();

    section_io_code pe_section_io::view_physical_data(uint32_t raw_offset, uint32_t data_size, 
        uint32_t& real_offset,uint32_t& available_size);
    section_io_code pe_section_io::view_virtual_data( uint32_t rva_offset, uint32_t data_size, 
        uint32_t& real_offset, uint32_t& available_size);
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

    section_io_code pe_section_io::read(std::vector<uint8_t>& buffer, uint32_t size);
    section_io_code pe_section_io::write(std::vector<uint8_t>& buffer, uint32_t size);
public:
    pe_section_io& pe_section_io::align_up(uint32_t factor, bool offset_to_end = true);
    pe_section_io& pe_section_io::add_size(uint32_t size, bool offset_to_end = true);

    pe_section_io& pe_section_io::set_mode(section_io_mode mode);
    pe_section_io& pe_section_io::set_addressing_type(section_io_addressing_type type);
    pe_section_io& pe_section_io::set_section_offset(uint32_t offset);
public:
    section_io_mode            pe_section_io::get_mode() const;
    section_io_code            pe_section_io::get_last_code() const;
    section_io_addressing_type pe_section_io::get_addressing_type() const;
    uint32_t                   pe_section_io::get_section_offset() const;

    pe_section* pe_section_io::get_section();
};