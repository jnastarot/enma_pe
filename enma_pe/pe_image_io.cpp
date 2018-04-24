#include "stdafx.h"
#include "pe_image_io.h"


pe_image_io::pe_image_io(
    pe_image& image,
    enma_io_mode mode,
    enma_io_addressing_type type
):image(&image),mode(mode), addressing_type(type),image_offset(0),last_code(enma_io_success){}


pe_image_io::pe_image_io(
    const pe_image& image,
    enma_io_addressing_type type
): image((pe_image*)&image), mode(enma_io_mode_default), addressing_type(type), 
    image_offset(0), last_code(enma_io_success) {}

pe_image_io::pe_image_io(const pe_image_io& image_io) {
    operator=(image_io);
}

pe_image_io::~pe_image_io() {

}

pe_image_io& pe_image_io::operator=(const pe_image_io& image_io) {
    this->image         = image_io.image;
    this->image_offset  = image_io.image_offset;
    this->last_code     = image_io.last_code;
    this->mode          = image_io.mode;
    this->addressing_type = image_io.addressing_type;

    return *this;
}





bool pe_image_io::view_image( //-> return like in view_data
    uint32_t required_offset, uint32_t required_size, uint32_t& real_offset,
    uint32_t& available_size, uint32_t& down_oversize, uint32_t& up_oversize) {


    if (image->get_sections_number()) {
        switch (addressing_type) {
        case enma_io_addressing_type::enma_io_address_raw: {
            pe_section * first_ = image->get_sections()[0];
            pe_section * last_ = image->get_last_section();

            
            return view_data(
                required_offset, required_size,
                real_offset, available_size, down_oversize, up_oversize,
                first_->get_pointer_to_raw(), ALIGN_UP(
                    last_->get_pointer_to_raw() + last_->get_size_of_raw_data() - first_->get_pointer_to_raw(),
                    image->get_file_align())
                + image->get_overlay_data().size());
        }

        case enma_io_addressing_type::enma_io_address_rva: {
            pe_section * first_ = image->get_sections()[0];
            pe_section * last_ = image->get_last_section();

            return view_data(
                required_offset, required_size,
                real_offset, available_size, down_oversize, up_oversize,
                first_->get_virtual_address(), ALIGN_UP(
                    last_->get_virtual_address() + last_->get_virtual_size() - first_->get_virtual_address(),
                    image->get_section_align())
            );
        }

        default: {return false; }
        }
    }

    return false;
}


enma_io_code pe_image_io::internal_read(uint32_t data_offset,
    void * buffer, uint32_t size,
    uint32_t& readed_size, uint32_t& down_oversize, uint32_t& up_oversize
) {
    uint32_t real_offset = 0;

    bool b_view = view_image(data_offset, size,
        real_offset,
        readed_size, down_oversize, up_oversize);


    if (b_view && readed_size) {
        uint32_t total_readed_size    = 0;
        uint32_t total_down_oversize  = 0;
        uint32_t total_up_oversize    = 0;

        for (auto &section : image->get_sections()) {

            uint32_t sec_readed_size   = 0;
            uint32_t sec_down_oversize = 0;
            uint32_t sec_up_oversize   = 0;

            enma_io_code code = pe_section_io(*section, *image, mode, addressing_type).internal_read(
                data_offset, buffer, size, sec_readed_size, sec_down_oversize, sec_up_oversize
            );

            total_readed_size += sec_readed_size;
            total_up_oversize = sec_up_oversize;

            if (!total_up_oversize || total_readed_size == readed_size) {
                break;
            }
        }

        if (addressing_type == enma_io_addressing_type::enma_io_address_raw &&
            total_up_oversize && image->get_overlay_data().size()) { //take up size from overlay

            memcpy(&((uint8_t*)buffer)[down_oversize + total_readed_size], image->get_overlay_data().data(),
                min(image->get_overlay_data().size(), total_up_oversize));

            total_up_oversize -= min(image->get_overlay_data().size(), total_up_oversize);
        }

        readed_size = total_readed_size;
        up_oversize = total_up_oversize;

        if (down_oversize || up_oversize) {
            last_code = enma_io_code::enma_io_incomplete;
            return enma_io_code::enma_io_incomplete;
        }

        last_code = enma_io_code::enma_io_success;
        return enma_io_code::enma_io_success;
    }

    last_code = enma_io_code::enma_io_data_not_present;
    return enma_io_code::enma_io_data_not_present;
}

enma_io_code pe_image_io::internal_write(uint32_t data_offset,
    void * buffer, uint32_t size,
    uint32_t& wrote_size, uint32_t& down_oversize, uint32_t& up_oversize
) {

    uint32_t real_offset = 0;

    bool b_view = view_image(data_offset, size,
        real_offset,
        wrote_size, down_oversize, up_oversize);


    if (b_view &&
        (wrote_size || (up_oversize && mode == enma_io_mode::enma_io_mode_allow_expand))) {

        uint32_t total_wroted_size   = 0;
        uint32_t total_down_oversize = 0;
        uint32_t total_up_oversize   = 0;

        for (size_t section_idx = 0; section_idx < image->get_sections().size(); section_idx++) {

            uint32_t sec_wroted_size   = 0;
            uint32_t sec_down_oversize = 0;
            uint32_t sec_up_oversize   = 0;

            pe_section_io section_io(*image->get_sections()[section_idx], *image,
                (section_idx == (image->get_sections().size()-1) &&
                    mode == enma_io_mode::enma_io_mode_allow_expand) ? enma_io_mode_allow_expand : enma_io_mode_default,addressing_type);


            section_io.internal_write(data_offset, buffer, size,
                sec_wroted_size, sec_down_oversize, sec_up_oversize);


            total_wroted_size += sec_wroted_size;
            total_up_oversize  = sec_up_oversize;

            if (!total_up_oversize) { break; }
        }
        up_oversize = total_up_oversize;

        if (down_oversize || up_oversize) {

            last_code = enma_io_code::enma_io_incomplete;
            return enma_io_code::enma_io_incomplete;
        }

        last_code = enma_io_code::enma_io_success;
        return enma_io_code::enma_io_success;
    }

    last_code = enma_io_code::enma_io_data_not_present;
    return enma_io_code::enma_io_data_not_present;
}

enma_io_code pe_image_io::read(void * data, uint32_t size) {

    uint32_t readed_size;
    uint32_t down_oversize;
    uint32_t up_oversize;


    enma_io_code code = internal_read(image_offset, data, size,
        readed_size, down_oversize, up_oversize);

    image_offset += readed_size;

    return code;
}

enma_io_code pe_image_io::write(void * data, uint32_t size) {

    uint32_t wrote_size;
    uint32_t down_oversize;
    uint32_t up_oversize;

    enma_io_code code = internal_write(image_offset, data, size,
        wrote_size, down_oversize, up_oversize);

    image_offset += wrote_size;

    return code;
}

enma_io_code pe_image_io::read(std::vector<uint8_t>& buffer, uint32_t size) {

    if (buffer.size() < size) { buffer.resize(size); }

    return read(buffer.data(), buffer.size());
}

enma_io_code pe_image_io::write(std::vector<uint8_t>& buffer) {

    return write(buffer.data(), buffer.size());
}

enma_io_code pe_image_io::memory_set(uint32_t size, uint8_t data) {

    std::vector<uint8_t> set_buffer;
    set_buffer.resize(size);
    memset(set_buffer.data(), data, size);

    return write(set_buffer);
}

enma_io_code pe_image_io::read_string(std::string& _string) {

    _string.clear();
    char _char = 0;

    do {
        enma_io_code code = read(&_char, sizeof(_char));

        if (code != enma_io_success) {
            return code;
        }

        if (_char) {
            _string += _char;
        }

    } while (_char);

    return enma_io_code::enma_io_success;
}

enma_io_code pe_image_io::read_wstring(std::wstring& _wstring) {

    _wstring.clear();
    wchar_t _char = 0;

    do {
        enma_io_code code = read(&_char, sizeof(_char));

        if (code != enma_io_success) {
            return code;
        }

        if (_char) {
            _wstring += _char;
        }

    } while (_char);

    return enma_io_code::enma_io_success;
}


pe_image_io& pe_image_io::set_mode(enma_io_mode mode) {

    this->mode = mode;

    return *this;
}
pe_image_io& pe_image_io::set_addressing_type(enma_io_addressing_type type) {

    this->addressing_type = type;

    return *this;
}
pe_image_io& pe_image_io::set_image_offset(uint32_t offset) {

    this->image_offset = offset;

    return *this;
}

pe_image_io& pe_image_io::seek_to_start() {

    if (image->get_sections_number()) {
        switch (addressing_type) {
        case enma_io_addressing_type::enma_io_address_raw: {
            this->image_offset = image->get_section_by_idx(0)->get_pointer_to_raw();
            break;
        }

        case enma_io_addressing_type::enma_io_address_rva: {
            this->image_offset = image->get_section_by_idx(0)->get_virtual_address();
            break;
        }

        default: {this->image_offset = 0; break; }
        }
    }
    else {
        this->image_offset = 0;
    }

    return *this;
}

pe_image_io& pe_image_io::seek_to_end() {

    if (image->get_sections_number()) {
        switch (addressing_type) {
        case enma_io_addressing_type::enma_io_address_raw: {
            this->image_offset = image->get_last_section()->get_pointer_to_raw();
            break;
        }

        case enma_io_addressing_type::enma_io_address_rva: {
            this->image_offset = image->get_last_section()->get_virtual_address();
            break;
        }

        default: {this->image_offset = 0; break; }
        }
    }
    else {
        this->image_offset = 0;
    }

    return *this;
}

enma_io_mode  pe_image_io::get_mode() const {
    return this->mode;
}
enma_io_code  pe_image_io::get_last_code() const {
    return this->last_code;
}
enma_io_addressing_type pe_image_io::get_addressing_type() const {
    return this->addressing_type;
}
uint32_t  pe_image_io::get_image_offset() const {
    return this->image_offset;
}

bool  pe_image_io::is_executable_rva(uint32_t rva) const {

    pe_section * rva_section = image->get_section_by_rva(rva);

    if (rva_section) {
        return rva_section->is_executable();
    }

    return false;
}
bool  pe_image_io::is_writeable_rva(uint32_t rva) const {
    pe_section * rva_section = image->get_section_by_rva(rva);

    if (rva_section) {
        return rva_section->is_writeable();
    }

    return false;
}
bool  pe_image_io::is_readable_rva(uint32_t rva) const {
    pe_section * rva_section = image->get_section_by_rva(rva);

    if (rva_section) {
        return rva_section->is_readable();
    }

    return false;
}

pe_image*  pe_image_io::get_image() {
    return this->image;
}



bool view_data(
    uint32_t  required_offset, uint32_t required_size, uint32_t& real_offset,
    uint32_t& available_size, uint32_t& down_oversize, uint32_t& up_oversize,
    uint32_t present_offset, uint32_t present_size) {


    //         ...............
    //  .............................
    //  |    | |             |      |
    //  v    v |             |      |
    // (down_oversize)       |      |
    //         |             |      |
    //         v             v      |
    //         (available_size)     |
    //                       |      |
    //                       v      v
    //                       (up_oversize)

    real_offset = 0;
    available_size = 0;
    down_oversize = 0;
    up_oversize = 0;

    if (required_offset < present_offset) {
        down_oversize = (present_offset - required_offset);

        if (down_oversize >= required_size) {

            return false; //not in bounds
        }
        else {
            available_size = (required_size - down_oversize);

            if (available_size > present_size) {
                up_oversize = (available_size - present_size);
                available_size = present_size;

                return true;//partially in bounds
            }

            return true;//partially in bounds
        }
    }
    else {//if(required_offset >= present_offset)

        if (required_offset <= (present_offset + present_size)) {
            real_offset = (required_offset - present_offset);

            if (required_size + required_offset >(present_offset + present_size)) {
                up_oversize = (required_size + required_offset) - (present_offset + present_size);
                available_size = required_size - up_oversize;

                return true; //partially in bounds
            }
            else {
                available_size = required_size;

                return true; //full in bounds
            }
        }
        else {
            real_offset = (required_offset - present_offset + present_size);
            up_oversize = (required_size + required_offset) - (present_offset + present_size);
            available_size = 0;

            return true; //trough full adding size 
        }
    }

    return true;
}