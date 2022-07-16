#pragma once

namespace enma {

    struct pe_delay_library {

    private:
        std::string _library_name;

        bool _is_bound;

        uint32_t _attributes;
        uint32_t _dll_name_rva;
        uint32_t _module_handle_rva;
        uint32_t _iat_rva;
        uint32_t _names_table_rva;
        uint32_t _iat_bound_table_rva;
        uint32_t _unload_info_table_rva;
        uint32_t _timestamp;

        std::vector<pe_import_function> _functions;

    public:
        pe_delay_library()
            : _attributes(0)
            , _is_bound(0)
            , _dll_name_rva(0)
            , _module_handle_rva(0)
            , _iat_rva(0)
            , _names_table_rva(0)
            , _iat_bound_table_rva(0)
            , _unload_info_table_rva(0)
            , _timestamp(0) {}

        ~pe_delay_library() = default;

        pe_delay_library(const pe_delay_library&) = default;
        pe_delay_library& operator=(const pe_delay_library&) = default;
        pe_delay_library(pe_delay_library&&) = default;
        pe_delay_library& operator=(pe_delay_library&&) = default;

    public:
        inline pe_delay_library& set_library_name(const std::string& library_name) {
            _library_name = library_name;

            return *this;
        }
        constexpr pe_delay_library& set_attributes(uint32_t attributes) {
            _attributes = attributes;

            return *this;
        }
        constexpr pe_delay_library& set_dll_name_rva(uint32_t dll_name_rva) {
            _dll_name_rva = dll_name_rva;

            return *this;
        }
        constexpr pe_delay_library& set_module_handle_rva(uint32_t rva) {
            _module_handle_rva = rva;

            return *this;
        }
        constexpr pe_delay_library& set_iat_rva(uint32_t rva) {
            _iat_rva = rva;

            return *this;
        }
        constexpr pe_delay_library& set_names_table_rva(uint32_t rva) {
            _names_table_rva = rva;

            return *this;
        }
        constexpr pe_delay_library& set_bound_table_iat_rva(uint32_t rva) {
            _iat_bound_table_rva = rva;

            return *this;
        }
        constexpr pe_delay_library& set_unload_info_table_rva(uint32_t rva) {
            _unload_info_table_rva = rva;

            return *this;
        }
        constexpr pe_delay_library& set_timestamp(uint32_t timestamp) {
            _timestamp = timestamp;

            return *this;
        }
        constexpr pe_delay_library& set_bound_library(bool is_bound) {
            _is_bound = is_bound;

            return *this;
        }

        inline pe_delay_library& add_function(const pe_import_function& function) {
            _functions.push_back(function);

            return *this;
        }

    public:
        inline size_t size() const {
            return _functions.size();
        }
        constexpr const std::string& get_library_name() const {
            return _library_name;
        }
        constexpr uint32_t get_attributes() const {
            return _attributes;
        }
        constexpr uint32_t get_dll_name_rva() const {
            return _dll_name_rva;
        }
        constexpr uint32_t get_module_handle_rva() const {
            return _module_handle_rva;
        }
        constexpr uint32_t get_iat_rva() const {
            return _iat_rva;
        }
        constexpr uint32_t get_names_table_rva() const {
            return _names_table_rva;
        }
        constexpr uint32_t get_bound_table_iat_rva() const {
            return _iat_bound_table_rva;
        }
        constexpr uint32_t get_unload_info_table_rva() const {
            return _unload_info_table_rva;
        }
        constexpr uint32_t get_timestamp() const {
            return _timestamp;
        }
        constexpr bool is_bound_library() const {
            return _is_bound;
        }
        constexpr const std::vector<pe_import_function>& get_functions() const {
            return _functions;
        }
        constexpr std::vector<pe_import_function>& get_functions() {
            return _functions;
        }

        pe_import_library convert_to_pe_import_library() const;
    };

    struct pe_delay_import_directory {

    private:
        std::vector<pe_delay_library> _libraries;

    public:
        pe_delay_import_directory() = default;
        ~pe_delay_import_directory() = default;

        pe_delay_import_directory(const pe_delay_import_directory&) = default;
        pe_delay_import_directory& operator=(const pe_delay_import_directory&) = default;
        pe_delay_import_directory(pe_delay_import_directory&&) = default;
        pe_delay_import_directory& operator=(pe_delay_import_directory&&) = default;

    public:
        inline void add_library(const pe_delay_library& lib) {
            _libraries.push_back(lib);
        }
        inline void clear() {
            _libraries.clear();
        }

    public:
        inline size_t size() const {
            return _libraries.size();
        }
        constexpr const std::vector<pe_delay_library>& get_libraries() const {
            return _libraries;
        }
        constexpr std::vector<pe_delay_library>& get_libraries() {
            return _libraries;
        }

        pe_import_directory convert_to_import_table() const;
    };


    pe_directory_code get_delay_import_directory(_In_ const pe_image& image, _Out_ pe_delay_import_directory& imports,
        _In_ const pe_bound_import_directory& bound_imports);
    pe_directory_code get_placement_delay_import_directory(_In_ const pe_image& image, _Inout_ pe_placement& placement,
        _In_ const pe_bound_import_directory& bound_imports);
}