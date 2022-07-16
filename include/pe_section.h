#pragma once

namespace enma {

    struct pe_section {

    private:
        std::string _section_name;
        uint32_t _virtual_size;
        uint32_t _virtual_address;
        uint32_t _pointer_to_raw;
        uint32_t _characteristics;

        std::vector<uint8_t> _section_data;

    public:

        pe_section()
            : _virtual_size(0)
            , _virtual_address(0)
            , _pointer_to_raw(0)
            , _characteristics(0) {}

        pe_section(const image_section_header& header);
        pe_section(const image_section_header& header, const std::vector<uint8_t>& data);
        ~pe_section() = default;

        pe_section(const pe_section&) = default;
        pe_section& operator=(const pe_section&) = default;
        pe_section(pe_section&&) = default;
        pe_section& operator=(pe_section&&) = default;

    public:
        inline pe_section& set_section_name(const std::string& name) {
            _section_name = name;
            return *this;
        }
        constexpr pe_section& set_virtual_size(uint32_t virtual_size) {
            _virtual_size = virtual_size;
            return *this;
        }
        constexpr pe_section& set_virtual_address(uint32_t virtual_address) {
            _virtual_address = virtual_address;
            return *this;
        }
        inline pe_section& set_size_of_raw_data(uint32_t size_of_raw_data) {
            _section_data.resize(size_of_raw_data);
            return *this;
        }
        constexpr pe_section& set_pointer_to_raw(uint32_t pointer_to_raw) {
            _pointer_to_raw = pointer_to_raw;
            return *this;
        }
        constexpr pe_section& set_characteristics(uint32_t characteristics) {
            _characteristics = characteristics;
            return *this;
        }
        constexpr pe_section& set_shared(bool flag) {

            if (flag) {
                _characteristics |= IMAGE_SCN_MEM_SHARED;
            }
            else {
                _characteristics &= ~IMAGE_SCN_MEM_SHARED;
            }

            return *this;
        }
        constexpr pe_section& set_readable(bool flag) {

            if (flag) {
                _characteristics |= IMAGE_SCN_MEM_READ;
            }
            else {
                _characteristics &= ~IMAGE_SCN_MEM_READ;
            }

            return *this;
        }
        constexpr pe_section& set_writeable(bool flag) {

            if (flag) {
                _characteristics |= IMAGE_SCN_MEM_WRITE;
            }
            else {
                _characteristics &= ~IMAGE_SCN_MEM_WRITE;
            }

            return *this;
        }
        constexpr pe_section& set_executable(bool flag) {

            if (flag) {
                _characteristics |= IMAGE_SCN_MEM_EXECUTE;
            }
            else {
                _characteristics &= ~IMAGE_SCN_MEM_EXECUTE;
            }

            return *this;
        }

        void add_data(const uint8_t* data, size_t data_size);

    public:
        constexpr const std::string& get_section_name() const {
            return _section_name;
        }
        constexpr uint32_t get_virtual_size() const {
            return _virtual_size;
        }
        constexpr uint32_t get_virtual_address() const {
            return _virtual_address;
        }
        inline uint32_t get_size_of_raw_data() const {
            return uint32_t(_section_data.size());
        }
        constexpr uint32_t get_pointer_to_raw() const {
            return _pointer_to_raw;
        }
        constexpr uint32_t get_characteristics() const {
            return _characteristics;
        }
        constexpr bool is_shared() const {
            return (_characteristics & IMAGE_SCN_MEM_SHARED) != 0;
        }
        constexpr bool is_readable() const {
            return (_characteristics & IMAGE_SCN_MEM_READ) != 0;
        }
        constexpr bool is_writeable() const {
            return (_characteristics & IMAGE_SCN_MEM_WRITE) != 0;
        }
        constexpr bool is_executable() const {
            return (_characteristics & IMAGE_SCN_MEM_EXECUTE) != 0;
        }
        constexpr const std::vector<uint8_t>& get_section_data() const {
            return _section_data;
        }
        constexpr std::vector<uint8_t>& get_section_data() {
            return _section_data;
        }
    };

}
