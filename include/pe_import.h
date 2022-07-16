#pragma once

namespace enma {

    struct pe_import_function {

    private:
        uint32_t _iat_rva;
        uint32_t _oft_rva;

        uint16_t _hint;
        std::string _func_name;

        uint16_t _ordinal;
        uint64_t _iat_item;
        uint64_t _oft_item;

        bool _b_import_by_name;

    public:
        pe_import_function()
            : _hint(0)
            , _ordinal(0)
            , _b_import_by_name(0)
            , _iat_rva(0)
            , _iat_item(0)
            , _oft_rva(0)
            , _oft_item(0) {}
        pe_import_function(uint32_t iat_rva, uint32_t oft_rva, const std::string& func_name, uint16_t hint, uint64_t iat_item = 0, uint64_t oft_item = 0)
            : _iat_rva(iat_rva)
            , _oft_rva(oft_rva)
            , _hint(hint)
            , _func_name(func_name)
            , _ordinal(0)
            , _iat_item(iat_item)
            , _oft_item(oft_item)
            , _b_import_by_name(true) {}
        pe_import_function(uint32_t iat_rva, uint32_t oft_rva, uint16_t ordinal, uint64_t iat_item = 0, uint64_t oft_item = 0)
            : _iat_rva(iat_rva)
            , _oft_rva(oft_rva)
            , _hint(0)
            , _func_name("")
            , _ordinal(ordinal)
            , _iat_item(iat_item)
            , _oft_item(oft_item)
            , _b_import_by_name(false) {}

        ~pe_import_function() = default;

        pe_import_function(const pe_import_function&) = default;
        pe_import_function& operator=(const pe_import_function&) = default;
        pe_import_function(pe_import_function&&) = default;
        pe_import_function& operator=(pe_import_function&&) = default;

    public:
        constexpr pe_import_function& set_hint(uint16_t hint) {
            _hint = hint;

            return *this;
        }
        constexpr pe_import_function& set_ordinal(uint16_t ordinal) {
            _b_import_by_name = false;
            _ordinal = ordinal;

            return *this;
        }
        inline pe_import_function& set_func_name(const std::string& func_name) {
            _b_import_by_name = true;
            _func_name = func_name;

            return *this;
        }
        constexpr pe_import_function& set_import_by_name(bool b) {
            _b_import_by_name = b;

            return *this;
        }
        constexpr pe_import_function& set_iat_rva(uint32_t rva) {
            _iat_rva = rva;

            return *this;
        }
        constexpr pe_import_function& set_iat_item(uint64_t iat_item) {
            _iat_item = iat_item;

            return *this;
        }
        constexpr pe_import_function& set_oft_rva(uint32_t rva) {
            _oft_rva = rva;

            return *this;
        }
        constexpr pe_import_function& set_oft_item(uint64_t oft_item) {
            _oft_item = oft_item;

            return *this;
        }

    public:
        constexpr uint16_t get_hint() const {
            return _hint;
        }
        constexpr uint16_t get_ordinal() const {
            return _ordinal;
        }
        constexpr const std::string& get_func_name() const {
            return _func_name;
        }
        constexpr bool is_import_by_name() const {
            return _b_import_by_name;
        }
        constexpr uint32_t get_iat_rva() const {
            return _iat_rva;
        }
        constexpr uint64_t get_iat_item()const {
            return _iat_item;
        }
        constexpr uint32_t get_oft_rva() const {
            return _oft_rva;
        }
        constexpr uint64_t get_oft_item()const {
            return _oft_item;
        }
    };

    struct pe_import_library {

    private:
        std::string _library_name;

        bool _is_bound;

        uint32_t _timestamp;
        uint32_t _iat_rva;
        uint32_t _oft_rva;
        uint32_t _library_name_rva;

        std::vector<pe_import_function> _functions;

    public:
        pe_import_library()
            : _timestamp(0)
            , _is_bound(0)
            , _iat_rva(-1)
            , _oft_rva(-1)
            , _library_name_rva(0) {}
        pe_import_library(const std::string& library_name)
            : _timestamp(0)
            , _is_bound(0)
            , _iat_rva(-1)
            , _oft_rva(-1)
            , _library_name_rva(0)
            , _library_name(library_name) {}

        ~pe_import_library() = default;

        pe_import_library(const pe_import_library&) = default;
        pe_import_library& operator=(const pe_import_library&) = default;
        pe_import_library(pe_import_library&&) = default;
        pe_import_library& operator=(pe_import_library&&) = default;

    public:
        inline pe_import_library& set_library_name(const std::string& library_name) {
            _library_name = library_name;

            return *this;
        }
        constexpr pe_import_library& set_timestamp(uint32_t timestamp) {
            _timestamp = timestamp;

            return *this;
        }
        constexpr pe_import_library& set_rva_iat(uint32_t rva) {
            _iat_rva = rva;

            return *this;
        }
        constexpr pe_import_library& set_rva_oft(uint32_t rva) {
            _oft_rva = rva;

            return *this;
        }
        constexpr pe_import_library& set_rva_library_name(uint32_t rva) {
            _library_name_rva = rva;

            return *this;
        }
        constexpr pe_import_library& set_bound_library(bool is_bound) {
            _is_bound = is_bound;

            return *this;
        }
        inline pe_import_library& add_function(const pe_import_function& function) {
            _functions.push_back(function);

            return *this;
        }
        inline void clear() {
            _functions.clear();
        }

    public:
        inline size_t size() {
            return _functions.size();
        }
        constexpr const std::string& get_library_name() const {
            return _library_name;
        }
        constexpr uint32_t get_timestamp() const {
            return _timestamp;
        }
        constexpr uint32_t get_rva_iat() const {
            return _iat_rva;
        }
        constexpr uint32_t get_rva_oft() const {
            return _oft_rva;
        }
        constexpr uint32_t get_rva_library_name() const {
            return _library_name_rva;
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

    };

    struct pe_import_directory {

    private:
        std::vector<pe_import_library> _libraries;

    public:
        pe_import_directory() = default;
        ~pe_import_directory() = default;

        pe_import_directory(const pe_import_directory&) = default;
        pe_import_directory& operator=(const pe_import_directory&) = default;
        pe_import_directory(pe_import_directory&&) = default;
        pe_import_directory& operator=(pe_import_directory&&) = default;

    public:
        inline void add_library(const pe_import_library& lib) {
            _libraries.push_back(lib);
        }
        inline void clear() {
            _libraries.clear();
        }

    public:
        inline size_t size() {
            return _libraries.size();
        }
        constexpr std::vector<pe_import_library>& get_libraries() {
            return _libraries;
        }
        constexpr const std::vector<pe_import_library>& get_libraries() const {
            return _libraries;
        }

        bool get_imported_lib(const std::string& lib_name, pe_import_library*& lib);
        bool get_imported_func(const std::string& lib_name, const std::string& func_name, pe_import_library*& lib, pe_import_function*& func);
        bool get_imported_func(const std::string& lib_name, uint16_t ordinal, pe_import_library*& lib, pe_import_function*& func);
    };


    enum import_table_build_id {
        import_table_build_iat = 1 << 1,
        import_table_build_oft = 1 << 2,
        import_table_build_library_name = 1 << 3,
    };


    pe_directory_code get_import_directory(_In_ const pe_image& image,
        _Out_ pe_import_directory& imports,
        _In_ const pe_bound_import_directory& bound_imports = pe_bound_import_directory());

    bool build_internal_import_directory_data(_Inout_ pe_image& image,
        _Inout_ pe_section& section, _Inout_ pe_import_directory& imports,
        _In_ uint32_t build_items_ids/*import_table_build_id*/,
        uint32_t iat_rva = -1, uint32_t oft_rva = -1, uint32_t names_rva = -1,
        _In_opt_ const pe_bound_import_directory* bound_imports = 0);
    bool build_import_directory_only(_Inout_ pe_image& image,
        _Inout_ pe_section& section, _Inout_ pe_import_directory& imports,
        uint32_t desc_rva = -1);

    bool build_import_directory_full(_Inout_ pe_image& image,
        _Inout_ pe_section& section, _Inout_ pe_import_directory& imports,
        uint32_t iat_rva = -1, uint32_t oft_rva = -1, uint32_t names_rva = -1, uint32_t desc_rva = -1,
        _In_opt_ const pe_bound_import_directory* bound_imports = 0);

    pe_directory_code get_placement_import_directory(_In_ const pe_image& image, _Inout_ pe_placement& placement,
        _In_ const pe_bound_import_directory& bound_imports);
}