#pragma once

namespace enma {

    struct pe_dotnet_directory {

    private:
        uint16_t _major_version;
        uint16_t _minor_version;
        uint32_t _flags;

        union {
            uint32_t _entry_point_token;
            uint32_t _entry_point_rva;
        };

        image_data_directory _meta_data;
        image_data_directory _resources;
        image_data_directory _strong_name_signature;
        image_data_directory _code_manager_table;
        image_data_directory _vtable_fixups;
        image_data_directory _export_address_table_jumps;
    public:
        pe_dotnet_directory() 
            : _major_version(0)
            , _minor_version(0)
            , _flags(0)
            , _entry_point_rva(0)
            , _meta_data({ 0,0 })
            , _resources({ 0,0 })
            , _strong_name_signature({ 0,0 })
            , _code_manager_table({ 0,0 })
            , _vtable_fixups({ 0,0 })
            , _export_address_table_jumps({ 0,0 }) {};
        ~pe_dotnet_directory() = default;

        pe_dotnet_directory(const pe_dotnet_directory&) = default;
        pe_dotnet_directory& operator=(const pe_dotnet_directory&) = default;
        pe_dotnet_directory(pe_dotnet_directory&&) = default;
        pe_dotnet_directory& operator=(pe_dotnet_directory&&) = default;
    
    public:
        constexpr void set_major_version(uint16_t major_version) {
            _major_version = major_version;
        }
        constexpr void set_minor_version(uint16_t minor_version) {
            _minor_version = minor_version;
        }
        constexpr void set_flags(uint32_t flags) {
            _flags = flags;
        }
        constexpr void set_entry_point(uint32_t entry_point) {
            _entry_point_rva = entry_point;
        }
        constexpr void set_meta_data(const image_data_directory& meta_data) {
            _meta_data = meta_data;
        }
        constexpr void set_resources(const image_data_directory& resources) {
            _resources = resources;
        }
        constexpr void set_strong_name_signature(const image_data_directory& strong_name_signature) {
            _strong_name_signature = strong_name_signature;
        }
        constexpr void set_code_manager_table(const image_data_directory& code_manager_table) {
            _code_manager_table = code_manager_table;
        }
        constexpr void set_vtable_fixups(const image_data_directory& vtable_fixups) {
            _vtable_fixups = vtable_fixups;
        }
        constexpr void set_export_address_table_jumps(const image_data_directory& export_address_table_jumps) {
            _export_address_table_jumps = export_address_table_jumps;
        }

    public:
        constexpr uint16_t get_major_version() const {
            return _major_version;
        }
        constexpr uint16_t get_minor_version() const {
            return _minor_version;
        }
        constexpr uint32_t get_flags()const {
            return _flags;
        }
        constexpr uint32_t get_entry_point()const {
            return _entry_point_rva;
        }
        constexpr const image_data_directory& get_meta_data() const {
            return _meta_data;
        }
        constexpr const image_data_directory& get_resources() const {
            return _resources;
        }
        constexpr const image_data_directory& get_strong_name_signature()const {
            return _strong_name_signature;
        }
        constexpr const image_data_directory& get_code_manager_table()const {
            return _code_manager_table;
        }
        constexpr const image_data_directory& get_vtable_fixups()const {
            return _vtable_fixups;
        }
        constexpr const image_data_directory& get_export_address_table_jumps() const {
            return _export_address_table_jumps;
        }
    };

    pe_directory_code get_dotnet_directory(_In_ const pe_image& image, _Out_ pe_dotnet_directory& dotnet);
}