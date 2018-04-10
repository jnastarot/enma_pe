#include "stdafx.h"
#include "pe_section_io.h"


pe_section_io::pe_section_io(pe_section & _section,
    section_io_mode mode,
    section_io_addressing_type type,
    uint32_t raw_aligment,
    uint32_t virtual_aligment) :
    section(&_section), section_offset(0),
    last_code(section_io_success), mode(mode), addressing_type(type), 
    raw_aligment(raw_aligment), virtual_aligment(virtual_aligment){}

pe_section_io::pe_section_io(const pe_section_io& io_section) {
    this->operator=(io_section);
}

pe_section_io::~pe_section_io() {

}

pe_section_io& pe_section_io::operator=(const pe_section_io& io_section) {
    this->section           = io_section.section;
    this->section_offset    = io_section.section_offset;
    this->last_code         = io_section.last_code;
    this->mode              = io_section.mode;
    this->addressing_type   = io_section.addressing_type;

    return *this;
}

void pe_section_io::update_section_boundaries() {

}

section_io_code pe_section_io::view_data(
    uint32_t  raw_offset, uint32_t required_size,
    uint32_t& real_offset, uint32_t& available_size,uint32_t& start_displacement,
    uint32_t present_offset, uint32_t present_size) {


    //         ...............
    //  .............................
    //  |    | |             |
    //  v    v |             |
    // (start_displacement)  |
    //         |             |
    //         v             v
    //         (available_size)



    if (raw_offset < present_offset) {
        real_offset = 0;
        start_displacement = (present_offset - raw_offset);
        available_size = (required_size - start_displacement);

        if (available_size > present_size) {
            available_size = present_size;

            return section_io_code::section_io_incomplete;
        }

        return section_io_code::section_io_incomplete;
    }
    else if(raw_offset >= present_offset){
        start_displacement = 0;

        if (raw_offset < (present_offset + present_size)) {
            real_offset = (raw_offset - present_offset);

            if (required_size + real_offset > (present_offset + present_size)) {
                available_size = ((present_offset + present_size) - real_offset);

                return section_io_code::section_io_incomplete;
            }
            else {
                available_size = required_size;

                return section_io_code::section_io_success;
            }
        }
        else {
            real_offset = (raw_offset - present_offset + present_size);
            available_size = 0;

            return section_io_code::section_io_incomplete;
        }
    }

    return section_io_code::section_io_success;
}

section_io_code pe_section_io::view_by_config(
    uint32_t raw_offset, uint32_t required_size,
    uint32_t& real_offset, uint32_t& available_size, uint32_t& start_displacement) {

 
    switch (addressing_type) {
        case section_io_addressing_type::section_address_raw: {
            return view_data(
                raw_offset, required_size,
                real_offset, available_size, start_displacement,
                this->section->get_pointer_to_raw(), ALIGN_UP(this->section->get_section_data().size(), this->raw_aligment)
            );
        }

        case section_io_addressing_type::section_address_rva: {
            return view_data(
                raw_offset, required_size,
                real_offset, available_size, start_displacement,
                this->section->get_virtual_address(), ALIGN_UP(this->section->get_virtual_size(), this->virtual_aligment)
            );
        }
    }
    
    return section_io_code::section_io_success;
}


template <class T>  pe_section_io& operator>>(pe_section_io& section_io, T& data) {//read

    std::vector<uint8_t> buffer;
    section_io_code code = section_io.read(buffer,sizeof(T));

    switch (code) {
        case section_io_code::section_io_success:
        case section_io_code::section_io_incomplete: {
            memcpy(data, buffer.data(), sizeof(T));
            break;
        }
    }

    return *this;
}
/*
template <class T> section_io_code pe_section_io::operator<<(T& data) {//write

    return section_io_code::section_io_success;
}
*/

section_io_code pe_section_io::read(std::vector<uint8_t>& buffer, uint32_t size) {

    uint32_t real_offset = 0;
    uint32_t available_size = 0;
    uint32_t start_displacement = 0;

    section_io_code code = view_by_config(this->section_offset, size,
        real_offset, available_size, start_displacement);

    buffer.clear();
    buffer.resize(available_size);

    switch (code) {
        case section_io_code::section_io_success:
        case section_io_code::section_io_incomplete: {
            memset(&buffer[0], 0, start_displacement);
            if (size <= available_size) {
                memcpy(&buffer[start_displacement], &section->get_section_data().data()[real_offset], size);
                this->section_offset += size;
            }
            else {
                memcpy(&buffer[start_displacement], &section->get_section_data().data()[real_offset], available_size);
                this->section_offset += available_size;
            }
            
            break;
        }

        case section_io_code::section_io_data_not_present: {
            break;
        }
    }

    
    return code;
}

section_io_code pe_section_io::write(std::vector<uint8_t>& buffer, uint32_t size) {

    return section_io_code::section_io_success;
}

pe_section_io& pe_section_io::align_up(uint32_t factor, bool offset_to_end) {
    this->section->get_section_data().resize(ALIGN_UP(this->section->get_size_of_raw_data(), factor));
    if (offset_to_end) {
        this->section_offset = this->section->get_size_of_raw_data();
    }
    add_size(ALIGN_UP(this->section->get_size_of_raw_data(), factor));

    return *this;
}

pe_section_io& pe_section_io::add_size(uint32_t size, bool offset_to_end) {
    if (size) {
        this->section->get_section_data().resize(this->section->get_section_data().size() + size);
    }
    if (offset_to_end) {
        this->section_offset = this->section->get_size_of_raw_data();
    }

    return *this;
}

pe_section_io& pe_section_io::set_mode(section_io_mode mode) {
    this->mode = mode;

    return *this;
}
pe_section_io& pe_section_io::set_addressing_type(section_io_addressing_type type) {
    this->addressing_type = type;

    return *this;
}
pe_section_io& pe_section_io::set_section_offset(uint32_t offset) {
    this->section_offset = offset;

    return *this;
}
pe_section_io& pe_section_io::set_raw_aligment(uint32_t aligment) {
    this->raw_aligment = aligment;

    return *this;
}
pe_section_io& pe_section_io::set_virtual_aligment(uint32_t aligment) {
    this->virtual_aligment = aligment;

    return *this;
}


section_io_mode pe_section_io::get_mode() const {
    return this->mode;
}
section_io_code pe_section_io::get_last_code() const {
    return this->last_code;
}
section_io_addressing_type pe_section_io::get_addressing_type() const {
    return this->addressing_type;
}
uint32_t pe_section_io::get_section_offset() const {
    return this->section_offset;
}
uint32_t pe_section_io::get_raw_aligment() const {
    return this->raw_aligment;
}
uint32_t pe_section_io::get_virtual_aligment() const {
    return this->virtual_aligment;
}
pe_section* pe_section_io::get_section() {
    return this->section;
}