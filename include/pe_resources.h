#pragma once

namespace enma {

    struct pe_resource_directory;
    struct pe_resource_directory_entry;

    struct pe_resource_data_entry {

    private:
        uint32_t _codepage;
        std::vector<uint8_t> _data;

    public:

        pe_resource_data_entry()
            : _codepage(0) {}

        pe_resource_data_entry(const void* data, uint32_t data_size, uint32_t codepage);
        ~pe_resource_data_entry() = default;

        pe_resource_data_entry(const pe_resource_data_entry&) = default;
        pe_resource_data_entry& operator=(const pe_resource_data_entry&) = default;
        pe_resource_data_entry(pe_resource_data_entry&&) = default;
        pe_resource_data_entry& operator=(pe_resource_data_entry&&) = default;

    public:

        constexpr void set_codepage(uint32_t codepage) {
            _codepage = codepage;
        }

        inline void set_data(const void* data, uint32_t data_size) {
            _data.resize(data_size);
            memcpy(_data.data(), data, data_size);
        }

    public:

        constexpr uint32_t get_codepage() const {
            return _codepage;
        }
        constexpr const std::vector<uint8_t>& get_data() const {
            return _data;
        }
        constexpr std::vector<uint8_t>& get_data() {
            return _data;
        }
    };

    struct pe_resource_directory_entry {

    private:
        uint32_t _id;
        std::wstring _name;

        union includes {

            constexpr includes()
                : _data(0) {}

            pe_resource_data_entry* _data;
            pe_resource_directory* _dir;

        }_ptr;

        bool _includes_data;
        bool _named;
    public:
        pe_resource_directory_entry()
            : _id(0)
            , _includes_data(false)
            , _named(false) {}

        pe_resource_directory_entry(const pe_resource_directory_entry& other);
        ~pe_resource_directory_entry();

        pe_resource_directory_entry& operator=(const pe_resource_directory_entry& other);
        void release();

    public:
        inline void set_name(const std::wstring& name) {
            _name = name;
            _named = true;
            _id = 0;
        }
        inline void set_id(uint32_t id) {
            _id = id;
            _named = false;
            _name.clear();
        }

        void add_data_entry(const pe_resource_data_entry& entry);
        void add_resource_directory(const pe_resource_directory& dir);

    public:
        constexpr uint32_t get_id() const {
            return _id;
        }
        constexpr const std::wstring& get_name() const {
            return _name;
        }
        constexpr bool is_named() const {
            return _named;
        }
        constexpr bool is_includes_data() const {
            return _includes_data;
        }
        constexpr const pe_resource_directory& get_resource_directory() const {
            return (const pe_resource_directory&)(*_ptr._data);
        }
        constexpr const pe_resource_data_entry& get_data_entry() const {
            return (const pe_resource_data_entry&)(*_ptr._dir);
        }
        constexpr pe_resource_data_entry& get_data_entry() {
            return *_ptr._data;
        }
        constexpr pe_resource_directory& get_resource_directory() {
            return *_ptr._dir;
        }
    };

    struct pe_resource_directory {

    private:
        uint32_t _characteristics;
        uint32_t _timestamp;
        uint16_t _major_version;
        uint16_t _minor_version;
        uint32_t _number_of_named_entries;
        uint32_t _number_of_id_entries;
        std::vector<pe_resource_directory_entry> _entries;

    public:
        pe_resource_directory()
            : _characteristics(0)
            , _timestamp(0)
            , _major_version(0)
            , _minor_version(0)
            , _number_of_named_entries(0)
            , _number_of_id_entries(0) {}

        pe_resource_directory(const image_resource_directory& res_dir)
            : _characteristics(res_dir.characteristics)
            , _timestamp(res_dir.time_date_stamp)
            , _major_version(res_dir.major_version)
            , _minor_version(res_dir.minor_version)
            , _number_of_named_entries(res_dir.number_of_named_entries)
            , _number_of_id_entries(res_dir.number_of_id_entries) {}

        pe_resource_directory(const pe_resource_directory& resource_dir) = default;

        ~pe_resource_directory();

        pe_resource_directory& operator=(const pe_resource_directory& resource_dir) = default;

    public:
        bool entry_by_id(pe_resource_directory_entry*& entry, uint32_t id);
        bool entry_by_id(const pe_resource_directory_entry*& entry, uint32_t id);
        bool entry_by_name(pe_resource_directory_entry*& entry, const std::wstring& name);
        bool entry_by_name(const pe_resource_directory_entry*& entry, const std::wstring& name);
        
        constexpr void set_characteristics(uint32_t characteristics) {
            _characteristics = characteristics;
        }
        constexpr void set_timestamp(uint32_t timestamp) {
            _timestamp = timestamp;
        }
        constexpr void set_number_of_named_entries(uint32_t number) {
            _number_of_named_entries = number;
        }
        constexpr void set_number_of_id_entries(uint32_t number) {
            _number_of_id_entries = number;
        }
        constexpr void set_major_version(uint16_t major_version) {
            _major_version = major_version;
        }
        constexpr void set_minor_version(uint16_t minor_version) {
            _minor_version = minor_version;
        }

        void add_resource_directory_entry(const pe_resource_directory_entry& entry);
        void clear_resource_directory_entry_list();

    public:
        inline size_t size() const {
            return _entries.size();
        }
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
        constexpr uint32_t get_number_of_named_entries() const {
            return _number_of_named_entries;
        }
        constexpr uint32_t get_number_of_id_entries() const {
            return _number_of_id_entries;
        }
        constexpr const std::vector<pe_resource_directory_entry>& get_entry_list() const {
            return _entries;
        }
        constexpr std::vector<pe_resource_directory_entry>& get_entry_list() {
            return _entries;
        }
    };

    pe_directory_code get_resources_directory(_In_ const pe_image& image,
        _Out_ pe_resource_directory& resources);
    bool build_resources_directory(_Inout_ pe_image& image, _Inout_ pe_section& section,
        _Inout_ pe_resource_directory& resources);
    pe_directory_code get_placement_resources_directory(_In_ const pe_image& image, _Inout_ pe_placement& placement);
}