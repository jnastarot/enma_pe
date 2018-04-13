#pragma once

enum enma_io_mode {
    enma_io_mode_default,
    enma_io_mode_allow_expand,
};

enum enma_io_addressing_type {
    enma_io_address_raw,
    enma_io_address_rva,
};

enum enma_io_code {
    enma_io_success,
    enma_io_incomplete, //part of read\write
    enma_io_data_not_present,
};


#include "pe_section_io.h"

class pe_image_io{
    pe_image* image;
    uint32_t image_offset;

    enma_io_code last_code;
    enma_io_mode mode;
    enma_io_addressing_type addressing_type;

    bool pe_image_io::view_data( //-> true if data or path of data is available or can be used trought adding size 
        uint32_t required_offset, uint32_t required_size, uint32_t& real_offset,
        uint32_t& available_size, uint32_t& down_oversize, uint32_t& up_oversize,
        uint32_t present_offset, uint32_t present_size);

    uint32_t pe_image_io::get_present_size(uint32_t required_offset);
public:
    pe_image_io::pe_image_io(
        pe_image& image,
        enma_io_mode mode = enma_io_mode_default,
        enma_io_addressing_type type = enma_io_address_rva
    );
    pe_image_io::pe_image_io(const pe_image_io& image_io);
    
    pe_image_io::~pe_image_io();

    pe_image_io& pe_image_io::operator=(const pe_image_io& image_io);
public:
    template<typename T>
    pe_image_io& pe_image_io::operator >> (const T& data) {//read from section

        read((void*)&data, sizeof(T));

        return *this;
    }
    template<typename T>
    pe_image_io& pe_image_io::operator << (const T& data) {//write to section

        write((void*)&data, sizeof(T));

        return *this;
    }

    enma_io_code pe_image_io::read(void * data, uint32_t size);
    enma_io_code pe_image_io::write(void * data, uint32_t size);

    enma_io_code pe_image_io::read(std::vector<uint8_t>& buffer, uint32_t size);
    enma_io_code pe_image_io::write(std::vector<uint8_t>& buffer);


    enma_io_code pe_image_io::internal_read(uint32_t data_offset,
        void * buffer, uint32_t size,
        uint32_t& readed_size, uint32_t& down_oversize, uint32_t& up_oversize
    );
    enma_io_code pe_image_io::internal_write(uint32_t data_offset,
        void * buffer, uint32_t size,
        uint32_t& wrote_size, uint32_t& down_oversize, uint32_t& up_oversize
    );

    bool pe_image_io::view_image( //-> return like in view_data
        uint32_t required_offset, uint32_t required_size, uint32_t& real_offset,
        uint32_t& available_size, uint32_t& down_oversize, uint32_t& up_oversize);
public:
    pe_image_io&   pe_image_io::set_mode(enma_io_mode mode);
    pe_image_io&   pe_image_io::set_addressing_type(enma_io_addressing_type type);
    pe_image_io&   pe_image_io::set_image_offset(uint32_t offset);
public:

    enma_io_mode            pe_image_io::get_mode() const;
    enma_io_code            pe_image_io::get_last_code() const;
    enma_io_addressing_type pe_image_io::get_addressing_type() const;
    uint32_t                pe_image_io::get_image_offset() const;
    pe_image*               pe_image_io::get_image();
};

