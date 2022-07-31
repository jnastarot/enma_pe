#pragma once

#include "pe_section_io.h"

namespace enma {

    struct pe_image_io {

    private:
        pe_image* _image;
        uint32_t _image_offset;

        enma_io_code _last_code;
        enma_io_mode _mode;
        enma_io_addressing_type _addressing_type;

    public:
        constexpr pe_image_io(
            pe_image& image,
            enma_io_mode mode = enma_io_mode_default,
            enma_io_addressing_type type = enma_io_address_rva)
            : _image(&image)
            , _image_offset(0)
            , _last_code(enma_io_success)
            , _mode(mode)
            , _addressing_type(type) {}

        constexpr pe_image_io(
            const pe_image& image,
            enma_io_addressing_type type = enma_io_address_rva)
            : _image((pe_image*)&image)
            , _image_offset(0)
            , _last_code(enma_io_success)
            , _mode(enma_io_mode_default)
            , _addressing_type(type) {}

        pe_image_io(const pe_image_io& image_io) = default;

        ~pe_image_io() {}

        pe_image_io& operator=(const pe_image_io& image_io) = default;

    public:
        template<typename T>
        pe_image_io& operator >> (const T& data) {//read from image

            read((void*)&data, sizeof(T));

            return *this;
        }
        template<typename T>
        pe_image_io& operator << (const T& data) {//write to image

            write((void*)&data, sizeof(T));

            return *this;
        }

        template<typename T>
        enma_io_code read(T& data) {
            return read(&data, sizeof(data));
        }

        inline enma_io_code read(void* data, size_t size) {

            size_t readed_size = 0;
            size_t down_oversize = 0;
            size_t up_oversize = 0;

            enma_io_code code = internal_read(_image_offset, data, size,
                readed_size, down_oversize, up_oversize);

            _image_offset += (uint32_t)readed_size;

            return code;
        }

        template<typename T>
        enma_io_code write(T& data) {
            return write(&data, sizeof(data));
        }

        inline enma_io_code write(const void* data, size_t size) {

            size_t wrote_size = 0;
            size_t down_oversize = 0;
            size_t up_oversize = 0;

            enma_io_code code = internal_write(_image_offset, data, size,
                wrote_size, down_oversize, up_oversize);

            _image_offset += (uint32_t)wrote_size;

            return code;
        }

        inline enma_io_code read(std::vector<uint8_t>& buffer, size_t size) {

            if (buffer.size() < size) { buffer.resize(size); }

            return read(buffer.data(), buffer.size());
        }

        inline enma_io_code write(std::vector<uint8_t>& buffer) {

            return write(buffer.data(), buffer.size());
        }

        inline enma_io_code memory_set(size_t size, uint8_t data) {

            std::vector<uint8_t> set_buffer;
            set_buffer.resize(size);
            memset(set_buffer.data(), data, size);

            return write(set_buffer);
        }

        inline enma_io_code read_string(std::string& _string) {

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

        inline enma_io_code read_wstring(std::wstring& _wstring) {

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

        enma_io_code internal_read(size_t data_offset,
            void* buffer, size_t size,
            size_t& readed_size, size_t& down_oversize, size_t& up_oversize
        );
        enma_io_code internal_write(size_t data_offset,
            const void* buffer, size_t size,
            size_t& wrote_size, size_t& down_oversize, size_t& up_oversize
        );

        bool view_image( //-> return like in view_data
            size_t required_offset, size_t required_size, size_t& real_offset,
            size_t& available_size, size_t& down_oversize, size_t& up_oversize) const;
    public:

        constexpr pe_image_io& set_mode(enma_io_mode mode) {

            _mode = mode;

            return *this;
        }
        constexpr pe_image_io& set_addressing_type(enma_io_addressing_type type) {

            _addressing_type = type;

            return *this;
        }
        constexpr pe_image_io& set_image_offset(uint32_t offset) {

            _image_offset = offset;

            return *this;
        }

        constexpr pe_image_io& seek_to_start() {

            _image_offset = 0;

            return *this;
        }

        pe_image_io& seek_to_end();

    public:
        constexpr enma_io_mode get_mode() const {
            return _mode;
        }
        constexpr enma_io_code get_last_code() const {
            return _last_code;
        }
        constexpr enma_io_addressing_type get_addressing_type() const {
            return _addressing_type;
        }
        constexpr uint32_t get_image_offset() const {
            return _image_offset;
        }

        inline bool is_executable_rva(uint32_t rva) const {

            pe_section* rva_section = _image->get_section_by_rva(rva);

            if (rva_section) {
                return rva_section->is_executable();
            }

            return false;
        }
        inline bool is_writeable_rva(uint32_t rva) const {

            pe_section* rva_section = _image->get_section_by_rva(rva);

            if (rva_section) {
                return rva_section->is_writeable();
            }

            return false;
        }
        inline bool is_readable_rva(uint32_t rva) const {

            pe_section* rva_section = _image->get_section_by_rva(rva);

            if (rva_section) {
                return rva_section->is_readable();
            }

            return false;
        }

        bool is_present_rva(uint32_t rva);
        bool is_present_raw(uint32_t raw);

        constexpr pe_image* get_image() {
            return _image;
        }
    };

}