#pragma once

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

    uint32_t raw_aligment;
    uint32_t virtual_aligment;

    section_io_code last_code;
    section_io_mode mode;
    section_io_addressing_type addressing_type;


    void pe_section_io::update_section_boundaries();

    section_io_code pe_section_io::view_data(
        uint32_t raw_offset, uint32_t required_size,
        uint32_t& real_offset,uint32_t& available_size, uint32_t& start_displacement,
        uint32_t present_offset, uint32_t present_size);

    section_io_code pe_section_io::view_by_config(
        uint32_t raw_offset, uint32_t required_size,
        uint32_t& real_offset, uint32_t& available_size, uint32_t& start_displacement);
public:
    pe_section_io::pe_section_io(
        pe_section & _section,
        section_io_mode mode = section_io_mode_default,
        section_io_addressing_type type = section_address_rva,
        uint32_t raw_aligment = 0x200,
        uint32_t virtual_aligment = 0x1000
    );

    pe_section_io::pe_section_io(const pe_section_io& io_section);
    pe_section_io::~pe_section_io();

    pe_section_io& pe_section_io::operator=(const pe_section_io& io_section);
public:
    template <class T> friend pe_section_io& operator>>(pe_section_io& section_io, T& data); //read from section
   // template <class T> section_io_code operator<<(T& data); //write to section

    section_io_code pe_section_io::read(std::vector<uint8_t>& buffer, uint32_t size);
    section_io_code pe_section_io::write(std::vector<uint8_t>& buffer, uint32_t size);
public:
    pe_section_io& pe_section_io::align_up(uint32_t factor, bool offset_to_end = true);
    pe_section_io& pe_section_io::add_size(uint32_t size, bool offset_to_end = true);

    pe_section_io& pe_section_io::set_mode(section_io_mode mode);
    pe_section_io& pe_section_io::set_addressing_type(section_io_addressing_type type);
    pe_section_io& pe_section_io::set_section_offset(uint32_t offset);
    pe_section_io& pe_section_io::set_raw_aligment(uint32_t aligment);
    pe_section_io& pe_section_io::set_virtual_aligment(uint32_t aligment);
public:
    section_io_mode            pe_section_io::get_mode() const;
    section_io_code            pe_section_io::get_last_code() const;
    section_io_addressing_type pe_section_io::get_addressing_type() const;
    uint32_t                   pe_section_io::get_section_offset() const;
    uint32_t                   pe_section_io::get_raw_aligment() const;
    uint32_t                   pe_section_io::get_virtual_aligment() const;


    pe_section* pe_section_io::get_section();
};