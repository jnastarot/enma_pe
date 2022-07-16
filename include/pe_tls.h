#pragma once

namespace enma {

    struct pe_tls_directory {

    private:
        uint32_t _start_address_raw_data;
        uint32_t _end_address_raw_data;
        uint32_t _address_of_index;
        uint32_t _address_of_callbacks;
        uint32_t _size_of_zero_fill;
        uint32_t _characteristics;

        struct tls_callback {
            uint32_t rva_callback;
            bool use_relocation;
        };

        std::vector<uint8_t> _raw_data;
        std::vector<tls_callback> _callbacks;
    public:
        pe_tls_directory()
            : _start_address_raw_data(0)
            , _end_address_raw_data(0)
            , _address_of_index(0)
            , _address_of_callbacks(0)
            , _size_of_zero_fill(0)
            , _characteristics(0) {};

        ~pe_tls_directory() = default;

        pe_tls_directory(const pe_tls_directory&) = default;
        pe_tls_directory& operator=(const pe_tls_directory&) = default;
        pe_tls_directory(pe_tls_directory&&) = default;
        pe_tls_directory& operator=(pe_tls_directory&&) = default;

    public:
        constexpr void set_start_address_raw_data(uint32_t start_address_raw_data) {
            _start_address_raw_data = start_address_raw_data;
        }
        constexpr void set_end_address_raw_data(uint32_t end_address_raw_data) {
            _end_address_raw_data = end_address_raw_data;
        }
        constexpr void set_address_of_index(uint32_t address_of_index) {
            _address_of_index = address_of_index;
        }
        constexpr void set_address_of_callbacks(uint32_t address_of_callbacks) {
            _address_of_callbacks = address_of_callbacks;
        }
        constexpr void set_size_of_zero_fill(uint32_t size_of_zero_fill) {
            _size_of_zero_fill = size_of_zero_fill;
        }
        constexpr void set_characteristics(uint32_t characteristics) {
            _characteristics = characteristics;
        }

    public:
        constexpr uint32_t get_start_address_raw_data() const {
            return _start_address_raw_data;
        }
        constexpr uint32_t get_end_address_raw_data() const {
            return _end_address_raw_data;
        }
        constexpr uint32_t get_address_of_index() const {
            return _address_of_index;
        }
        constexpr uint32_t get_address_of_callbacks() const {
            return _address_of_callbacks;
        }
        constexpr uint32_t get_size_of_zero_fill() const {
            return _size_of_zero_fill;
        }
        constexpr uint32_t get_characteristics() const {
            return _characteristics;
        }
        constexpr const std::vector<uint8_t>& get_raw_data() const {
            return _raw_data;
        }
        constexpr std::vector<uint8_t>& get_raw_data() {
            return _raw_data;
        }
        constexpr const std::vector<tls_callback>& get_callbacks() const {
            return _callbacks;
        }
        constexpr std::vector<tls_callback>& get_callbacks() {
            return _callbacks;
        }
    };

    enum tls_table_build_id {
        tls_table_build_raw_data = 1 << 1,
        tls_table_build_address_of_index = 1 << 2,
        tls_table_build_callbacks = 1 << 3,
    };


    pe_directory_code get_tls_directory(_In_ const pe_image& image, _Out_ pe_tls_directory& tls);


    bool build_internal_tls_directory_data(_In_ const pe_image& image, _Inout_ pe_section& section,
        _Out_ pe_tls_directory& tls, _Out_ pe_relocations_directory& relocs,
        _In_ uint32_t build_items_ids/*tls_table_build_id*/);
    bool build_tls_directory_only(_Inout_ pe_image& image, _Inout_ pe_section& section,
        _Inout_ pe_tls_directory& tls, _Out_ pe_relocations_directory& relocs);
    bool build_tls_directory_full(_Inout_ pe_image& image, _Inout_ pe_section& section,
        _Inout_ pe_tls_directory& tls, _Out_ pe_relocations_directory& relocs);

    pe_directory_code get_placement_tls_directory(_In_ const pe_image& image, _Inout_ pe_placement& placement);

}