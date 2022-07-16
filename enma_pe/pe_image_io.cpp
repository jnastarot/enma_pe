#include "stdafx.h"

using namespace enma;

bool pe_image_io::view_image( //-> return like in view_data
    size_t required_offset, size_t required_size, size_t& real_offset,
    size_t& available_size, size_t& down_oversize, size_t& up_oversize) const {

    if (_image->get_sections_number()) {
        
        switch (_addressing_type) {
        
        case enma_io_addressing_type::enma_io_address_raw: {

            size_t top_raw = _image->get_headers_data().size();
            
            for (auto& section : _image->get_sections()) {
                size_t current_section_raw = section->get_pointer_to_raw() + section->get_size_of_raw_data();
                
                if (current_section_raw > top_raw) {
                    top_raw = current_section_raw;
                }
            }

            return view_data(
                required_offset, required_size,
                real_offset, available_size, down_oversize, up_oversize,
                0, ALIGN_UP(
                    top_raw,
                    _image->get_file_align())
                + uint32_t(_image->get_overlay_data().size()));
        }

        case enma_io_addressing_type::enma_io_address_rva: {

            size_t top_rva = _image->get_headers_data().size();

            for (auto& section : _image->get_sections()) {
                size_t current_section_rva = section->get_virtual_address() + section->get_virtual_size();

                if (current_section_rva > top_rva) {
                    top_rva = current_section_rva;
                }
            }

            return view_data(
                required_offset, required_size,
                real_offset, available_size, down_oversize, up_oversize,
                0, ALIGN_UP(
                    top_rva,
                    _image->get_section_align())
            );
        }

        default: {return false; }
        }
    }

    return false;
}

enma_io_code pe_image_io::internal_read(size_t data_offset,
    void * buffer, size_t size,
    size_t& readed_size, size_t& down_oversize, size_t& up_oversize) {

    if (_addressing_type == enma_io_addressing_type::enma_io_address_va) {

        return enma_io_code::enma_io_type_not_supported;
    }

    size_t real_offset = 0;

    bool b_view = view_image(data_offset, size,
        real_offset,
        readed_size, down_oversize, up_oversize);

    if (b_view && readed_size) {
        size_t total_readed_size    = 0;
        size_t total_up_oversize    = 0;

        size_t available_headers_size = _image->get_headers_data().size();
        size_t view_headers_size = _addressing_type == enma_io_addressing_type::enma_io_address_raw ?
            ALIGN_UP(available_headers_size, _image->get_file_align()) : ALIGN_UP(available_headers_size, _image->get_section_align());


        if (data_offset < view_headers_size) {
            
            size_t header_readed_size = 0;
            size_t header_down_oversize = 0;
            size_t header_up_oversize = 0;

            
            b_view = view_data(
                data_offset, size,
                real_offset, header_readed_size, header_down_oversize, header_up_oversize,
                0, view_headers_size);

            if (b_view) {
                
                if (available_headers_size) {

                    if (available_headers_size >= (header_readed_size + real_offset) ) {
                        memcpy(&((uint8_t*)buffer)[header_down_oversize], &_image->get_headers_data().data()[real_offset], header_readed_size);
                    }
                    else {
                        if (available_headers_size > real_offset) {

                            // available_headers_size - real_offset = is available data in vector for read
                            // header_readed_size = must be readed, includes virtual aligment

                            memcpy(&((uint8_t*)buffer)[header_down_oversize], &_image->get_headers_data().data()[real_offset], 
                                available_headers_size - real_offset
                            );

                            memset(&((uint8_t*)buffer)[header_down_oversize + (available_headers_size - real_offset)], 0,
                                header_readed_size - ((available_headers_size - real_offset)));
                        }
                        else {
                            memset(&((uint8_t*)buffer)[header_down_oversize], 0, header_readed_size);
                        }                  
                    }
                }
                else {
                    memset(&((uint8_t*)buffer)[header_down_oversize], 0, header_readed_size);
                }

                total_readed_size += header_readed_size;
                total_up_oversize = header_up_oversize;
            }
        }

        for (auto &section : _image->get_sections()) {

            if (total_readed_size == readed_size) { break; }

            size_t sec_readed_size   = 0;
            size_t sec_down_oversize = 0;
            size_t sec_up_oversize   = 0;

            pe_section_io(*section, *_image, _mode, _addressing_type).internal_read(
                data_offset, buffer, size, sec_readed_size, sec_down_oversize, sec_up_oversize
            );

            total_readed_size += sec_readed_size;
            total_up_oversize = sec_up_oversize;

            if (total_readed_size == readed_size) { break; }
        }

        if (_addressing_type == enma_io_addressing_type::enma_io_address_raw &&
            total_up_oversize && _image->get_overlay_data().size()) { //take up size from overlay

            size_t top_section_raw = 0;

            size_t overlay_readed_size = 0;
            size_t overlay_down_oversize = 0;
            size_t overlay_up_oversize = 0;

            if (_image->get_sections_number()) {
                top_section_raw = 
                    ALIGN_UP(_image->get_last_section()->get_pointer_to_raw() + _image->get_last_section()->get_size_of_raw_data(), _image->get_file_align());
            }

            b_view = view_data(
                data_offset, size,
                real_offset, overlay_readed_size, overlay_down_oversize, overlay_up_oversize,
                top_section_raw, _image->get_overlay_data().size());
            
            if (b_view) {
                memcpy(&((uint8_t*)buffer)[down_oversize], &_image->get_overlay_data().data()[real_offset],overlay_readed_size);


                total_readed_size += overlay_readed_size;
                total_up_oversize = overlay_up_oversize;
            }
        }

        readed_size = total_readed_size;
        up_oversize = total_up_oversize;

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

enma_io_code pe_image_io::internal_write(size_t data_offset,
    const void * buffer, size_t size,
    size_t& wrote_size, size_t& down_oversize, size_t& up_oversize) {

    if (_addressing_type == enma_io_addressing_type::enma_io_address_va) {

        return enma_io_code::enma_io_type_not_supported;
    }

    size_t real_offset = 0;

    bool b_view = view_image(data_offset, size,
        real_offset,
        wrote_size, down_oversize, up_oversize);

    if (b_view &&
        (wrote_size || (up_oversize && _mode == enma_io_mode::enma_io_mode_allow_expand))) {

        size_t total_wroted_size   = 0;
        size_t total_up_oversize   = 0;

        for (size_t section_idx = 0; section_idx < _image->get_sections().size(); section_idx++) {

            size_t sec_wroted_size   = 0;
            size_t sec_down_oversize = 0;
            size_t sec_up_oversize   = 0;

            pe_section_io section_io(*_image->get_sections()[section_idx], *_image,
                (section_idx == (_image->get_sections().size()-1) &&
                    _mode == enma_io_mode::enma_io_mode_allow_expand) ? enma_io_mode_allow_expand : enma_io_mode_default, _addressing_type);


            section_io.internal_write(data_offset, buffer, size,
                sec_wroted_size, sec_down_oversize, sec_up_oversize);


            total_wroted_size += sec_wroted_size;
            total_up_oversize  = sec_up_oversize;

            if (!total_up_oversize) { break; }
        }
        up_oversize = total_up_oversize;
        wrote_size  = total_wroted_size;

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

pe_image_io& pe_image_io::seek_to_end() {

    if (_image->get_sections_number()) {

        switch (_addressing_type) {

        case enma_io_addressing_type::enma_io_address_raw: {

            size_t top_raw = _image->get_headers_data().size();

            for (auto& section : _image->get_sections()) {
                size_t current_section_raw = section->get_pointer_to_raw() + section->get_size_of_raw_data();

                if (current_section_raw > top_raw) {
                    top_raw = current_section_raw;
                }
            }
            _image_offset = (uint32_t)ALIGN_UP(
                top_raw,
                _image->get_file_align()
            );
            break;
        }

        case enma_io_addressing_type::enma_io_address_rva: {

            size_t top_rva = _image->get_headers_data().size();

            for (auto& section : _image->get_sections()) {
                size_t current_section_rva = section->get_virtual_address() + section->get_virtual_size();

                if (current_section_rva > top_rva) {
                    top_rva = current_section_rva;
                }
            }

            _image_offset = (uint32_t)ALIGN_UP(
                top_rva,
                _image->get_section_align()
            );

            break;
        }

        default: {_image_offset = 0; break; }
        }
    }
    else {
        _image_offset = 0;
    }

    return *this;
}

bool  pe_image_io::is_present_rva(uint32_t rva) {

    uint8_t test_byte;

    uint32_t current_offset = _image_offset;
    enma_io_addressing_type addressing_type = _addressing_type;

    _addressing_type = enma_io_address_rva;
    _image_offset = rva;
    
    enma_io_code code = read((void*)&test_byte, 1);

    _image_offset = current_offset;
    _addressing_type = addressing_type;

    return code == enma_io_success;
}

bool pe_image_io::is_present_raw(uint32_t raw) {

    uint8_t test_byte;

    uint32_t current_offset = _image_offset;
    enma_io_addressing_type addressing_type = _addressing_type;

    _addressing_type = enma_io_address_raw;
    _image_offset = raw;

    enma_io_code code = read((void*)&test_byte, 1);

    _image_offset = current_offset;
    _addressing_type = addressing_type;

    return code == enma_io_success;
}