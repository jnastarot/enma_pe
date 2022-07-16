#include "stdafx.h"

using namespace enma;

pe_section::pe_section(const image_section_header& header) 
    : _virtual_size(header.virtual_size)
    , _virtual_address(header.virtual_address)
    , _pointer_to_raw(header.pointer_to_raw_data)
    , _characteristics(header.characteristics) {

    _section_name.reserve(8);
    _section_name.resize(strnlen((char*)header.name, 8));
    memcpy((void*)_section_name.data(), header.name,8);

    _section_data.clear();
}
pe_section::pe_section(const image_section_header& header, const std::vector<uint8_t>& data)
    :_virtual_size(header.virtual_size)
    , _virtual_address(header.virtual_address)
    , _pointer_to_raw(header.pointer_to_raw_data)
    , _characteristics(header.characteristics)
    , _section_data(data){

    _section_name.reserve(8);
    _section_name.resize(strnlen((char*)header.name, 8));
    memcpy((void*)_section_name.data(), header.name, 8);
}

void pe_section::add_data(const uint8_t * data, size_t data_size) {

    _section_data.resize(_section_data.size() + data_size);
    memcpy(_section_data.data() + _section_data.size() - data_size, data, data_size);
}




