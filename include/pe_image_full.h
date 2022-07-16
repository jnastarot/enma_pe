#pragma once

namespace enma {

    struct pe_image_full {

    private:
        uint32_t _code;

        pe_export_directory _exports;
        pe_import_directory _imports;
        pe_resource_directory _resources;
        pe_exceptions_directory _exceptions;
        pe_security_directory _security;
        pe_relocations_directory _relocations;
        pe_debug_directory _debug;
        pe_tls_directory _tls;
        pe_load_config_directory _load_config;
        pe_delay_import_directory _delay_imports;
        pe_bound_import_directory _bound_imports;

        pe_image _image;

    public:

        pe_image_full()
            : _code(pe_directory_code_currupted) {}
        ~pe_image_full() {}

        pe_image_full(const pe_image& image);
        pe_image_full(const pe_image_full& image);
        explicit pe_image_full(bool _pe32, bool init_dos_thunk);
        pe_image_full(const uint8_t* raw_image, uint32_t size);
        pe_image_full(const std::string& file_path);
        pe_image_full(const std::wstring& file_path);

        pe_image_full& operator=(const pe_image_full& image);
        pe_image_full& operator=(const pe_image& image);
    public:

        constexpr void set_directory_code(pe_directory_code code) {
            _code = code;
        }

        inline void set_exports(pe_export_directory& export_directory) {
            _exports = export_directory;
        }
        inline void set_imports(pe_import_directory& import_directory) {
            _imports = import_directory;
        }
        inline void set_resources(pe_resource_directory& resources_directory) {
            _resources = resources_directory;
        }
        inline void set_exceptions(pe_exceptions_directory& exceptions_directory) {
            _exceptions = exceptions_directory;
        }
        inline void set_security(pe_security_directory& security_directory) {
            _security = security_directory;
        }
        inline void set_relocations(pe_relocations_directory& relocations_directory) {
            _relocations = relocations_directory;
        }
        inline void set_debug(pe_debug_directory& debug_directory) {
            _debug = debug_directory;
        }
        inline void set_tls(pe_tls_directory& tls_directory) {
            _tls = tls_directory;
        }
        inline void set_load_config(pe_load_config_directory& load_config_directory) {
            _load_config = load_config_directory;
        }
        inline void set_delay_imports(pe_delay_import_directory& delay_import_directory) {
            _delay_imports = delay_import_directory;
        }
        inline void set_bound_imports(pe_bound_import_directory& bound_import_directory) {
            _bound_imports = bound_import_directory;
        }
        inline void set_image(pe_image& image) {
            _image = image;
        }

    public:
        constexpr pe_directory_code get_directory_code() const {
            return (pe_directory_code)_code;
        }
        constexpr pe_export_directory& get_exports() {
            return _exports;
        }
        constexpr const pe_export_directory& get_exports() const {
            return _exports;
        }
        constexpr pe_import_directory& get_imports() {
            return _imports;
        }
        constexpr const pe_import_directory& get_imports() const {
            return _imports;
        }
        constexpr pe_resource_directory& get_resources() {
            return _resources;
        }
        constexpr const pe_resource_directory& get_resources() const {
            return _resources;
        }
        constexpr pe_exceptions_directory& get_exceptions() {
            return _exceptions;
        }
        constexpr const pe_exceptions_directory& get_exceptions() const {
            return _exceptions;
        }
        constexpr pe_security_directory& get_security() {
            return _security;
        }
        constexpr const pe_security_directory& get_security() const {
            return _security;
        }
        constexpr pe_relocations_directory& get_relocations() {
            return _relocations;
        }
        constexpr const pe_relocations_directory& get_relocations() const {
            return _relocations;
        }
        constexpr pe_debug_directory& get_debug() {
            return _debug;
        }
        constexpr const pe_debug_directory& get_debug() const {
            return _debug;
        }
        constexpr pe_tls_directory& get_tls() {
            return _tls;
        }
        constexpr const pe_tls_directory& get_tls() const {
            return _tls;
        }
        constexpr pe_load_config_directory& get_load_config() {
            return _load_config;
        }
        constexpr const pe_load_config_directory& get_load_config() const {
            return _load_config;
        }
        constexpr pe_delay_import_directory& get_delay_imports() {
            return _delay_imports;
        }
        constexpr const pe_delay_import_directory& get_delay_imports() const {
            return _delay_imports;
        }
        constexpr pe_bound_import_directory& get_bound_imports() {
            return _bound_imports;
        }
        constexpr const pe_bound_import_directory& get_bound_imports() const {
            return _bound_imports;
        }
        constexpr pe_image& get_image() {
            return _image;
        }
        constexpr const pe_image& get_image() const {
            return _image;
        }
    };


    pe_directory_code get_directories_placement(_Inout_ pe_image& image, _Out_ pe_placement& placement,
        _In_opt_ const pe_bound_import_directory* bound_imports = 0);

}