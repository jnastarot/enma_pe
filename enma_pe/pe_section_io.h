#pragma once





/*
        ................. section1 data
 |    | |     |     |    ...................section2 data
 v    v |     |     |       |                ...............section3 data
 ...... io data (data not present)                        |      |     
        |     |     |       |                             |      |
        v     v     |       |                             |      |
        ....... io data (code success)                    |      |
                    |       |                             |      |
                    v       v                             |      |
                    ......... io data (code incomplete)   v      v
                                                          ........ io data (
                                                                      section_io_success = section_io_mode_default
                                                                      section_io_data_not_present = section_io_mode_allow_expand 
                                                                     )    
                                     
                                     */  

/*


                ............................................ section data
   ........................................................................... i\o data
   |            |                                          |                 |
   |            |                                          |                 |
   |<---------->|<---------------------------------------->|<--------------->|
   down_oversize              available_size                    up_oversize
                    


*/


class pe_section_io {
    pe_section*  section;
    uint32_t section_offset;

    uint32_t raw_aligment;
    uint32_t virtual_aligment;

    enma_io_code last_code;
    enma_io_mode mode;
    enma_io_addressing_type addressing_type;
    

    uint32_t pe_section_io::get_present_size(uint32_t required_offset);
public:
    pe_section_io::pe_section_io(
        pe_section & _section,
        enma_io_mode mode = enma_io_mode_default,
        enma_io_addressing_type type = enma_io_address_rva,
        uint32_t raw_aligment = 0x200,
        uint32_t virtual_aligment = 0x1000
    );

    pe_section_io::pe_section_io(
        pe_section & _section,
        pe_image& image,
        enma_io_mode mode = enma_io_mode_default,
        enma_io_addressing_type type = enma_io_address_rva
    );

    pe_section_io::pe_section_io(const pe_section_io& io_section);
    pe_section_io::~pe_section_io();

    pe_section_io& pe_section_io::operator=(const pe_section_io& io_section);
public:

    template<typename T>
    pe_section_io& pe_section_io::operator>> (const T& data){//read from section

        read((void*)&data, sizeof(T));

        return *this;
    }
    template<typename T>
    pe_section_io& pe_section_io::operator << (const T& data) {//write to section

        write((void*)&data, sizeof(T));

        return *this;
    }

    enma_io_code pe_section_io::read(void * data, uint32_t size);
    enma_io_code pe_section_io::write(void * data, uint32_t size);

    enma_io_code pe_section_io::read(std::vector<uint8_t>& buffer, uint32_t size);
    enma_io_code pe_section_io::write(std::vector<uint8_t>& buffer);

    enma_io_code pe_section_io::read_string(std::string& _string);
    enma_io_code pe_section_io::read_wstring(std::wstring& _wstring);

    enma_io_code pe_section_io::internal_read(uint32_t data_offset,
        void * buffer, uint32_t size,
        uint32_t& readed_size, uint32_t& down_oversize, uint32_t& up_oversize
    );
    enma_io_code pe_section_io::internal_write(uint32_t data_offset,
        void * buffer, uint32_t size,
        uint32_t& wrote_size, uint32_t& down_oversize, uint32_t& up_oversize
    );

    bool pe_section_io::view_section( //-> return like in view_data
        uint32_t required_offset, uint32_t required_size, uint32_t& real_offset,
        uint32_t& available_size, uint32_t& down_oversize, uint32_t& up_oversize);
public:
    pe_section_io& pe_section_io::align_up(uint32_t factor, bool offset_to_end = true);
    pe_section_io& pe_section_io::add_size(uint32_t size, bool offset_to_end = true);

    pe_section_io& pe_section_io::set_mode(enma_io_mode mode);
    pe_section_io& pe_section_io::set_addressing_type(enma_io_addressing_type type);
    pe_section_io& pe_section_io::set_section_offset(uint32_t offset);
    pe_section_io& pe_section_io::set_raw_aligment(uint32_t aligment);
    pe_section_io& pe_section_io::set_virtual_aligment(uint32_t aligment);

    pe_section_io& pe_section_io::seek_to_start();
    pe_section_io& pe_section_io::seek_to_end();
public:

    enma_io_mode            pe_section_io::get_mode() const;
    enma_io_code            pe_section_io::get_last_code() const;
    enma_io_addressing_type pe_section_io::get_addressing_type() const;
    uint32_t                   pe_section_io::get_section_offset() const;
    uint32_t                   pe_section_io::get_raw_aligment() const;
    uint32_t                   pe_section_io::get_virtual_aligment() const;


    pe_section* pe_section_io::get_section();
};
