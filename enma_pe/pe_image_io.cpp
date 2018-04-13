#include "stdafx.h"
#include "pe_image_io.h"


pe_image_io::pe_image_io(
    pe_image& image,
    enma_io_mode mode,
    enma_io_addressing_type type
):image(&image),mode(mode), addressing_type(type){}

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



bool pe_image_io::view_data( //-> true if data or path of data is available or can be used trought adding size 
    uint32_t required_offset, uint32_t required_size, uint32_t& real_offset,
    uint32_t& available_size, uint32_t& down_oversize, uint32_t& up_oversize,
    uint32_t present_offset, uint32_t present_size) {



    return false;
}

uint32_t pe_image_io::get_present_size(uint32_t required_offset) {


    return 0;
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

enma_io_code pe_image_io::internal_read(uint32_t data_offset,
    void * buffer, uint32_t size,
    uint32_t& readed_size, uint32_t& down_oversize, uint32_t& up_oversize
) {


    return enma_io_code::enma_io_data_not_present;
}

enma_io_code pe_image_io::internal_write(uint32_t data_offset,
    void * buffer, uint32_t size,
    uint32_t& wrote_size, uint32_t& down_oversize, uint32_t& up_oversize
) {


    return enma_io_code::enma_io_data_not_present;
}

bool pe_image_io::view_image( //-> return like in view_data
    uint32_t required_offset, uint32_t required_size, uint32_t& real_offset,
    uint32_t& available_size, uint32_t& down_oversize, uint32_t& up_oversize) {


    return false;
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

pe_image*  pe_image_io::get_image() {
    return this->image;
}