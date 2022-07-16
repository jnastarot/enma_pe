#include "stdafx.h"

using namespace enma;

bool pe_section_io::view_section(
    size_t required_offset, size_t required_size, size_t& real_offset,
    size_t& available_size, size_t& down_oversize, size_t& up_oversize) const {

 
    switch (_addressing_type) {
        case enma_io_addressing_type::enma_io_address_raw: {
            return view_data(
                required_offset, required_size,
                real_offset, available_size, down_oversize, up_oversize,
                _section->get_pointer_to_raw(), ALIGN_UP(_section->get_size_of_raw_data(), _raw_aligment)
            );
        }

        case enma_io_addressing_type::enma_io_address_rva: {
            return view_data(
                required_offset, required_size,
                real_offset, available_size, down_oversize, up_oversize,
                _section->get_virtual_address(), ALIGN_UP(_section->get_virtual_size(), _virtual_aligment)
            );
        }

        default: {return false; }
    }
    
    return false;
}

uint32_t pe_section_io::get_present_size(uint32_t required_offset) {

    if (required_offset < _section->get_size_of_raw_data()) {
        return _section->get_size_of_raw_data() - required_offset;
    }

    return 0;
}

enma_io_code pe_section_io::internal_read(
    size_t data_offset,
    void * buffer, size_t size,
    size_t& readed_size, size_t& down_oversize, size_t& up_oversize) {

    size_t real_offset    = 0;

    bool b_view = view_section(data_offset, size,
        real_offset,
        readed_size, down_oversize, up_oversize);

    if (b_view && readed_size) {
        size_t present_size = get_present_size((uint32_t)real_offset);

        if (present_size) {

            if (present_size >= readed_size) {
                memcpy(&((uint8_t*)buffer)[down_oversize], &_section->get_section_data().data()[real_offset], readed_size);
            }
            else {               
                memcpy(&((uint8_t*)buffer)[down_oversize], &_section->get_section_data().data()[real_offset], present_size);
                memset(&((uint8_t*)buffer)[down_oversize + present_size], 0, readed_size - present_size);
            }
        }
        else {
            memset(&((uint8_t*)buffer)[down_oversize], 0, readed_size);
        }

        if (down_oversize || up_oversize) {
            _last_code = enma_io_code::enma_io_incomplete;
        }
        else {
            _last_code = enma_io_code::enma_io_success;
        }
    }
    else {
        _last_code = enma_io_code::enma_io_data_not_present;
    }

    return enma_io_code::enma_io_data_not_present;
}

enma_io_code pe_section_io::internal_write(
    size_t data_offset,
    const void * buffer, size_t size,
    size_t& wrote_size, size_t& down_oversize, size_t& up_oversize) {

    size_t real_offset = 0;

    bool b_view = view_section(data_offset, size,
        real_offset,
        wrote_size, down_oversize, up_oversize);

    if (b_view &&
        (wrote_size || (up_oversize && _mode == enma_io_mode::enma_io_mode_allow_expand) )) {

        if ((up_oversize && _mode == enma_io_mode::enma_io_mode_allow_expand)) { 
            
            if (_addressing_type == enma_io_addressing_type::enma_io_address_raw) {

                add_size(
                    (ALIGN_UP(_section->get_size_of_raw_data(), _raw_aligment) - _section->get_size_of_raw_data()) + up_oversize            
                    , false);

                wrote_size += min(up_oversize, size);
                up_oversize = 0;
            }
            else if (_addressing_type == enma_io_addressing_type::enma_io_address_rva) {

                add_size(
                    (ALIGN_UP(_section->get_virtual_size(), _virtual_aligment) - _section->get_virtual_size()) + up_oversize
                    , false);

                wrote_size += min(up_oversize, size);
                up_oversize = 0;
            }
            else {
                return enma_io_code::enma_io_data_not_present;//unk addressing_type
            }        
        }
    
        if (real_offset + wrote_size > _section->get_size_of_raw_data()) { //aligned by raw align

            add_size((real_offset + wrote_size) - _section->get_size_of_raw_data(), false);
        }

        memcpy(&_section->get_section_data().data()[real_offset], &((uint8_t*)buffer)[down_oversize], wrote_size);

        if (down_oversize || up_oversize) {
            _last_code = enma_io_code::enma_io_incomplete;
        }
        else {
            _last_code = enma_io_code::enma_io_success;
        }
    }
    else {
        _last_code = enma_io_code::enma_io_data_not_present;
    }

    return _last_code;
}

pe_section_io& pe_section_io::seek_to_end() {

    switch (_addressing_type) {
    case enma_io_addressing_type::enma_io_address_raw: {
        _section_offset = _section->get_pointer_to_raw() + _section->get_size_of_raw_data();
        break;
    }

    case enma_io_addressing_type::enma_io_address_rva: {
        _section_offset = _section->get_virtual_address() + _section->get_virtual_size();
        break;
    }

    default: {_section_offset = 0; break; }
    }

    return *this;
}
