#pragma once

namespace enma {

    struct pe_debug_entry {

    private:
        uint32_t _characteristics;
        uint32_t _timestamp;
        uint16_t _major_version;
        uint16_t _minor_version;
        uint32_t _type;
        uint32_t _size_of_data;
        uint32_t _address_of_raw_data;
        uint32_t _pointer_to_raw_data;

        std::vector<uint8_t> _item_data;

    public:
        pe_debug_entry()
            : _characteristics(0)
            , _timestamp(0)
            , _major_version(0)
            , _minor_version(0)
            , _type(0)
            , _size_of_data(0)
            , _address_of_raw_data(0)
            , _pointer_to_raw_data(0) {};

        pe_debug_entry(uint32_t characteristics, uint32_t timestamp, uint16_t major_version, uint16_t minor_version,
            uint32_t type, uint32_t size_of_data, uint32_t address_of_raw_data, uint32_t pointer_to_raw_data,
            void* data);

        ~pe_debug_entry() = default;
        pe_debug_entry(const pe_debug_entry&) = default;
        pe_debug_entry& operator=(const pe_debug_entry&) = default;
        pe_debug_entry(pe_debug_entry&&) = default;
        pe_debug_entry& operator=(pe_debug_entry&&) = default;

    public:
        constexpr void set_characteristics(uint32_t characteristics) {
            _characteristics = characteristics;
        }
        constexpr void set_timestamp(uint32_t timestamp) {
            _timestamp = timestamp;
        }
        constexpr void set_major_version(uint16_t major_version) {
            _major_version = major_version;
        }
        constexpr void set_minor_version(uint16_t minor_version) {
            _minor_version = minor_version;
        }
        constexpr void set_type(uint32_t type) {
            _type = type;
        }
        constexpr void set_size_of_data(uint32_t size_of_data) {
            _size_of_data = size_of_data;
        }
        constexpr void set_address_of_raw_data(uint32_t address_of_raw_data) {
            _address_of_raw_data = address_of_raw_data;
        }
        constexpr void set_pointer_to_raw_data(uint32_t pointer_to_raw_data) {
            _pointer_to_raw_data = pointer_to_raw_data;
        }

    public:
        constexpr uint32_t get_characteristics() const {
            return _characteristics;
        }
        constexpr uint32_t get_timestamp() const {
            return _timestamp;
        }
        constexpr uint16_t get_major_version() const {
            return _major_version;
        }
        constexpr uint16_t get_minor_version() const {
            return _minor_version;
        }
        constexpr uint32_t get_type() const {
            return _type;
        }
        constexpr uint32_t get_size_of_data() const {
            return _size_of_data;
        }
        constexpr uint32_t get_address_of_raw_data() const {
            return _address_of_raw_data;
        }
        constexpr uint32_t get_pointer_to_raw_data() const {
            return _pointer_to_raw_data;
        }
        constexpr const std::vector<uint8_t>& get_item_data() const {
            return _item_data;
        }
        constexpr std::vector<uint8_t>& get_item_data() {
            return _item_data;
        }
    };

    struct pe_debug_directory {

    private:
        std::vector<pe_debug_entry> _entries;

    public:
        pe_debug_directory() {};
        pe_debug_directory(const pe_debug_directory& debug) = default;
        ~pe_debug_directory() {};

        pe_debug_directory& operator=(const pe_debug_directory& debug) = default;

    public:
        inline void add_entry(const pe_debug_entry& item) {
            _entries.push_back(item);
        }
        inline void clear() {
            _entries.clear();
        }

    public:
        inline size_t size() const {
            return _entries.size();
        }
        constexpr const std::vector<pe_debug_entry>& get_entries() const {
            return _entries;
        }
        constexpr std::vector<pe_debug_entry>& get_entries() {
            return _entries;
        }
    };

    pe_directory_code get_debug_directory(_In_ const pe_image& image, _Out_ pe_debug_directory& debug);
    pe_directory_code get_placement_debug_directory(_In_ const pe_image& image, _Inout_ pe_placement& placement);

}