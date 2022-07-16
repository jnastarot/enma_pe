#pragma once



namespace enma {


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


    constexpr inline bool view_data(
        size_t  required_offset, size_t required_size, size_t& real_offset,
        size_t& available_size, size_t& down_oversize, size_t& up_oversize,
        size_t present_offset, size_t present_size) {


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

                if (required_size + required_offset > (present_offset + present_size)) {
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
                real_offset = (required_offset - present_offset);
                up_oversize = (required_size + required_offset) - (present_offset + present_size);
                available_size = 0;

                return true; //trough full adding size 
            }
        }

        return true;
    }

    struct pe_section_io {

    private:
        pe_section* _section;
        uint32_t _section_offset;

        uint32_t _raw_aligment;
        uint32_t _virtual_aligment;

        enma_io_code _last_code;
        enma_io_mode _mode;
        enma_io_addressing_type _addressing_type;

        uint32_t get_present_size(uint32_t required_offset);
    public:
        constexpr pe_section_io(
            pe_section& _section,
            enma_io_mode mode = enma_io_mode_default,
            enma_io_addressing_type type = enma_io_address_rva,
            uint32_t raw_aligment = 0x200,
            uint32_t virtual_aligment = 0x1000)
            : _section(&_section)
            , _section_offset(0)
            , _raw_aligment(raw_aligment)
            , _virtual_aligment(virtual_aligment)
            , _last_code(enma_io_code::enma_io_success)
            , _mode(mode)
            , _addressing_type(type) {}

        constexpr pe_section_io(
            pe_section& _section,
            pe_image& image,
            enma_io_mode mode = enma_io_mode_default,
            enma_io_addressing_type type = enma_io_address_rva)
            : _section(&_section)
            , _section_offset(0)
            , _raw_aligment(image.get_file_align())
            , _virtual_aligment(image.get_section_align())
            , _last_code(enma_io_code::enma_io_success)
            , _mode(mode)
            , _addressing_type(type) {}

        pe_section_io(const pe_section_io& io_section) = default;
        ~pe_section_io() {};

        pe_section_io& operator=(const pe_section_io& io_section) = default;
    public:

        template<typename T>
        pe_section_io& operator>> (const T& data) {//read from section

            read((void*)&data, sizeof(T));

            return *this;
        }
        template<typename T>
        pe_section_io& operator << (const T& data) {//write to section

            write((void*)&data, sizeof(T));

            return *this;
        }

        inline enma_io_code read(void* data, size_t size) {

            size_t readed_size;
            size_t down_oversize;
            size_t up_oversize;


            enma_io_code code = internal_read(_section_offset, data, size,
                readed_size, down_oversize, up_oversize);

            _section_offset += (uint32_t)readed_size;

            return code;
        }

        inline enma_io_code write(const void* data, size_t size) {

            size_t wrote_size;
            size_t down_oversize;
            size_t up_oversize;

            enma_io_code code = internal_write(_section_offset, data, size,
                wrote_size, down_oversize, up_oversize);

            _section_offset += (uint32_t)wrote_size;

            return code;
        }

        inline enma_io_code read(std::vector<uint8_t>& buffer, size_t size) {

            if (buffer.size() < size) { buffer.resize(size); }

            return read(buffer.data(), uint32_t(buffer.size()));
        }

        inline enma_io_code write(const std::vector<uint8_t>& buffer) {

            return write(buffer.data(), uint32_t(buffer.size()));
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

        bool view_section( //-> return like in view_data
            size_t required_offset, size_t required_size, size_t& real_offset,
            size_t& available_size, size_t& down_oversize, size_t& up_oversize) const;

    public:
        inline pe_section_io& align_up(size_t factor, bool offset_to_end = true) {
            add_size(ALIGN_UP(_section->get_size_of_raw_data(), factor) - _section->get_size_of_raw_data(), offset_to_end);

            return *this;
        }
        constexpr pe_section_io& align_up_offset(size_t factor) {
            _section_offset = (uint32_t)ALIGN_UP(_section_offset, factor);

            return *this;
        }
        inline pe_section_io& add_size(size_t size, bool offset_to_end = true) {

            if (size) {
                _section->get_section_data().resize(_section->get_size_of_raw_data() + size);
            }

            if (_section->get_size_of_raw_data() > _section->get_virtual_size()) {
                _section->set_virtual_size(_section->get_size_of_raw_data());
            }

            if (offset_to_end) { seek_to_end(); }

            return *this;
        }

        constexpr pe_section_io& set_mode(enma_io_mode mode) {
            _mode = mode;

            return *this;
        }
        constexpr pe_section_io& set_addressing_type(enma_io_addressing_type type) {
            _addressing_type = type;

            return *this;
        }
        constexpr pe_section_io& set_section_offset(uint32_t offset) {
            _section_offset = offset;

            return *this;
        }
        constexpr pe_section_io& set_raw_aligment(uint32_t aligment) {
            _raw_aligment = aligment;

            return *this;
        }
        constexpr pe_section_io& set_virtual_aligment(uint32_t aligment) {
            _virtual_aligment = aligment;

            return *this;
        }
        inline pe_section_io& seek_to_start() {

            switch (_addressing_type) {

            case enma_io_addressing_type::enma_io_address_raw: {
                _section_offset = _section->get_pointer_to_raw();
                break;
            }

            case enma_io_addressing_type::enma_io_address_rva: {
                _section_offset = _section->get_virtual_address();
                break;
            }

            default: {_section_offset = 0; break; }
            }

            return *this;
        }

        pe_section_io& seek_to_end();

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
        constexpr uint32_t get_section_offset() const {
            return _section_offset;
        }
        constexpr uint32_t get_raw_aligment() const {
            return _raw_aligment;
        }
        constexpr uint32_t get_virtual_aligment() const {
            return _virtual_aligment;
        }
        constexpr pe_section* get_section() {
            return _section;
        }
    };

}