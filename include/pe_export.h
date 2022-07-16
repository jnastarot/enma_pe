#pragma once

namespace enma {

    struct pe_export_entry {

    private:
        uint32_t _rva;
        uint16_t _ordinal;
        uint16_t _name_ordinal;
        bool _b_name;
        bool _b_forward;
        std::string _forward_name;
        std::string _func_name;
    public:
        pe_export_entry()
            : _ordinal(0)
            , _rva(0)
            , _b_name(0)
            , _b_forward(0)
            , _name_ordinal(0) {};

        ~pe_export_entry() {};

        pe_export_entry(const pe_export_entry&) = default;
        pe_export_entry& operator=(const pe_export_entry&) = default;
        pe_export_entry(pe_export_entry&&) = default;
        pe_export_entry& operator=(pe_export_entry&&) = default;

        bool operator==(const pe_export_entry& item);

    public:
        constexpr pe_export_entry& set_rva(uint32_t rva) {
            _rva = rva;

            return *this;
        }
        constexpr pe_export_entry& set_ordinal(uint16_t ordinal) {
            _ordinal = ordinal;

            return *this;
        }
        constexpr pe_export_entry& set_name_ordinal(uint16_t ordinal) {
            _name_ordinal = ordinal;

            return *this;
        }
        constexpr pe_export_entry& set_has_name(bool b) {
            _b_name = b;

            return *this;
        }
        constexpr pe_export_entry& set_forward(bool b) {
            _b_forward = b;

            return *this;
        }
        inline pe_export_entry& set_forward_name(const std::string& forward_name) {
            _b_forward = true;
            _forward_name = forward_name;

            return *this;
        }
        inline pe_export_entry& set_func_name(const std::string& func_name) {
            _b_name = true;
            _func_name = func_name;

            return *this;
        }

    public:
        constexpr uint32_t get_rva() const {
            return _rva;
        }
        constexpr uint16_t get_ordinal() const {
            return _ordinal;
        }
        constexpr uint16_t get_name_ordinal() const {
            return _name_ordinal;
        }
        constexpr bool has_name() const {
            return _b_name;
        }
        constexpr bool is_forward() const {
            return _b_forward;
        }
        constexpr const std::string& get_forward_name() const {
            return _forward_name;
        }
        constexpr const std::string& get_func_name() const {
            return _func_name;
        }
    };

    struct pe_export_directory {

    private:
        uint32_t _characteristics;
        uint32_t _time_stamp;
        uint16_t _major_version;
        uint16_t _minor_version;
        uint32_t _ordinal_base;
        uint32_t _number_of_functions;
        uint32_t _number_of_names;

        std::string _library_name;
        std::vector<pe_export_entry> _functions;

    public:
        pe_export_directory()
            : _characteristics(0)
            , _time_stamp(0)
            , _major_version(0)
            , _minor_version(0)
            , _ordinal_base(0)
            , _number_of_functions(0)
            , _number_of_names(0) {};

        ~pe_export_directory() = default;

        pe_export_directory(const pe_export_directory&) = default;
        pe_export_directory& operator=(const pe_export_directory&) = default;
        pe_export_directory(pe_export_directory&&) = default;
        pe_export_directory& operator=(pe_export_directory&&) = default;

    public:
        constexpr pe_export_directory& set_characteristics(uint32_t characteristics) {
            _characteristics = characteristics;

            return *this;
        }
        constexpr pe_export_directory& set_time_stamp(uint32_t time_stamp) {
            _time_stamp = time_stamp;

            return *this;
        }
        constexpr pe_export_directory& set_major_version(uint16_t major_version) {
            _major_version = major_version;

            return *this;
        }
        constexpr pe_export_directory& set_minor_version(uint16_t minor_version) {
            _minor_version = minor_version;

            return *this;
        }
        constexpr pe_export_directory& set_ordinal_base(uint32_t ordinal_base) {
            _ordinal_base = ordinal_base;

            return *this;
        }
        constexpr pe_export_directory& set_number_of_functions(uint32_t number_of_functions) {
            _number_of_functions = number_of_functions;

            return *this;
        }
        constexpr pe_export_directory& set_number_of_names(uint32_t number_of_names) {
            _number_of_names = number_of_names;

            return *this;
        }
        inline pe_export_directory& set_library_name(const std::string& library_name) {
            _library_name = library_name;

            return *this;
        }
        inline pe_export_directory& add_function(const pe_export_entry& function) {
            _functions.push_back(function);

            return *this;
        }

    public:
        inline size_t size() const {
            return _functions.size();
        }
        constexpr uint32_t get_characteristics() const {
            return _characteristics;
        }
        constexpr uint32_t get_time_stamp() const {
            return _time_stamp;
        }
        constexpr uint16_t get_major_version() const {
            return _major_version;
        }
        constexpr uint16_t get_minor_version() const {
            return _minor_version;
        }
        constexpr uint32_t get_ordinal_base() const {
            return _ordinal_base;
        }
        constexpr uint32_t get_number_of_functions() const {
            return _number_of_functions;
        }
        constexpr uint32_t get_number_of_names() const {
            return _number_of_names;
        }
        constexpr const std::string& get_library_name() const {
            return _library_name;
        }
        constexpr const std::vector<pe_export_entry>& get_functions() const {
            return _functions;
        }
        constexpr std::vector<pe_export_entry>& get_functions() {
            return _functions;
        }

        bool get_exported_function(const std::string& func_name, pe_export_entry*& function);
        bool get_exported_function(uint16_t ordinal, pe_export_entry*& function);
    };


    pe_directory_code get_export_directory(_In_ const pe_image& image,
        _Out_ pe_export_directory& exports);
    bool build_export_directory(_Inout_ pe_image& image,
        _Inout_ pe_section& section, _In_ const pe_export_directory& exports);
    pe_directory_code get_placement_export_directory(_In_ const pe_image& image, _Inout_ pe_placement& placement);

}