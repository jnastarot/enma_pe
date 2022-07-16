#pragma once

#include "pe_image_exceptions_hl.h"

namespace enma {

    struct pe_exception_entry;

    enum unwind_parameter_type {
        unwind_parameter_raw,
        unwind_parameter_rva,
        unwind_parameter_va
    };

    struct unwind_parameter {
        unwind_parameter_type type;
        std::vector<uint8_t> param_data;
    };

    struct pe_exception_unwind_info {

    private:
        uint32_t _unwind_info_rva;

        uint8_t _version;
        uint8_t _flags;
        uint8_t _size_of_prolog;
        uint8_t _count_of_codes;
        uint8_t _frame_register;
        uint8_t _frame_offset;

        uint32_t _handler_rva;
        pe_exception_entry* _chained_entry;

        std::vector<unwind_code> _codes;

        std::vector<unwind_parameter> _params;

        hl::exceptions_handler_specific_data _custom_parameter;

    public:

        pe_exception_unwind_info()
            : _unwind_info_rva(0)
            , _version(0)
            , _flags(0)
            , _size_of_prolog(0)
            , _count_of_codes(0)
            , _frame_register(0)
            , _frame_offset(0)
            , _handler_rva(0)
            , _chained_entry(0) {}

        pe_exception_unwind_info(uint32_t unwind_info_rva, uint8_t version,
            uint8_t flags, uint8_t size_of_prolog, uint8_t count_of_codes, uint8_t frame_register, uint8_t frame_offset)
            : _unwind_info_rva(unwind_info_rva)
            , _version(version)
            , _flags(flags)
            , _size_of_prolog(size_of_prolog)
            , _count_of_codes(count_of_codes)
            , _frame_register(frame_register)
            , _frame_offset(frame_offset)
            , _handler_rva(0)
            , _chained_entry(0) {}

        pe_exception_unwind_info(const pe_exception_unwind_info& item);
        ~pe_exception_unwind_info();

        pe_exception_unwind_info& operator=(const pe_exception_unwind_info& item);

    public:
        inline void add_unwind_code(const unwind_code& code) {
            _codes.push_back(code);
        }
        constexpr void set_version(uint8_t version) {
            _version = version;
        }
        constexpr void set_flags(uint8_t flags) {
            _flags = flags;
        }
        constexpr void set_size_of_prolog(uint8_t size_of_prolog) {
            _size_of_prolog = size_of_prolog;
        }
        constexpr void set_count_of_codes(uint8_t count_of_codes) {
            _count_of_codes = count_of_codes;
        }
        constexpr void set_frame_register(uint8_t frame_register) {
            _frame_register = frame_register;
        }
        constexpr void set_frame_offset(uint8_t frame_offset) {
            _frame_offset = frame_offset;
        }
        constexpr void set_chained_entry(pe_exception_entry* chained_entry) {
            _chained_entry = chained_entry;
        }
        constexpr void set_handler_rva(uint32_t rva) {
            _handler_rva = rva;
        }
        constexpr void set_unwind_info_rva(uint32_t rva) {
            _unwind_info_rva = rva;
        }
        inline void set_codes(std::vector<unwind_code>& codes) {
            _codes = codes;
        }
        inline void set_params(std::vector<unwind_parameter>& params) {
            _params = params;
        }
        inline void set_custom_parameter(const hl::exceptions_handler_specific_data& custom_parameter) {
            _custom_parameter = custom_parameter;
        }

    public:
        constexpr uint8_t get_version() const {
            return _version;
        }
        constexpr uint8_t get_flags() const {
            return _flags;
        }
        constexpr uint8_t get_size_of_prolog() const {
            return _size_of_prolog;
        }
        constexpr uint8_t get_count_of_codes() const {
            return _count_of_codes;
        }
        constexpr uint8_t get_frame_register() const {
            return _frame_register;
        }
        constexpr uint8_t get_frame_offset() const {
            return _frame_offset;
        }
        constexpr std::vector<unwind_code>& get_codes() {
            return _codes;
        }
        constexpr const std::vector<unwind_code>& get_codes() const {
            return _codes;
        }
        constexpr std::vector<unwind_parameter>& get_params() {
            return _params;
        }
        constexpr const std::vector<unwind_parameter>& get_params() const {
            return _params;
        }
        constexpr hl::exceptions_handler_specific_data& get_custom_parameter() {
            return _custom_parameter;
        }
        constexpr const hl::exceptions_handler_specific_data& get_custom_parameter() const {
            return _custom_parameter;
        }
        constexpr const pe_exception_entry* get_chained_entry() const {
            return _chained_entry;
        }
        constexpr pe_exception_entry* get_chained_entry() {
            return _chained_entry;
        }
        constexpr uint32_t get_handler_rva() const {
            return _handler_rva;
        }
        constexpr uint32_t get_unwind_info_rva() const {
            return _unwind_info_rva;
        }
    };

    struct pe_exception_entry {

    private:
        uint32_t _address_begin;
        uint32_t _address_end;
        uint32_t _address_unwind_data;

    public:

        pe_exception_entry()
            : _address_begin(0)
            , _address_end(0)
            , _address_unwind_data(0) {};

        pe_exception_entry(uint32_t address_begin, uint32_t address_end, uint32_t address_unwind_data)
            : _address_begin(address_begin)
            , _address_end(address_end)
            , _address_unwind_data(address_unwind_data) {};

        pe_exception_entry(const pe_exception_entry& entry) = default;

        ~pe_exception_entry() {}

        pe_exception_entry& operator=(const pe_exception_entry& entry) = default;

    public:
        constexpr void set_begin_address(uint32_t rva_address) {
            _address_begin = rva_address;
        }
        constexpr void set_end_address(uint32_t rva_address) {
            _address_end = rva_address;
        }
        constexpr void set_unwind_data_address(uint32_t rva_address) {
            _address_unwind_data = rva_address;
        }

    public:
        constexpr uint32_t get_begin_address() const {
            return _address_begin;
        }
        constexpr uint32_t get_end_address() const {
            return _address_end;
        }
        constexpr uint32_t get_unwind_data_address() const {
            return _address_unwind_data;
        }
    };

    struct pe_exceptions_directory {

    private:
        std::vector<pe_exception_entry> _exception_entries;
        std::vector<pe_exception_unwind_info> _unwind_entries;

    public:
        pe_exceptions_directory() {}
        pe_exceptions_directory(const pe_exceptions_directory& exceptions) = default;
        ~pe_exceptions_directory() {}

        pe_exceptions_directory& operator=(const pe_exceptions_directory& exceptions) = default;

    public:
        inline void add_exception_entry(uint32_t address_begin, uint32_t address_end, uint32_t address_unwind_data) {
            _exception_entries.push_back(pe_exception_entry(address_begin, address_end, address_unwind_data));
        }
        inline void add_exception_entry(const pe_exception_entry& entry) {
            _exception_entries.push_back(entry);
        }
        inline void add_unwind_entry(const pe_exception_unwind_info& entry) {
            _unwind_entries.push_back(entry);
        }
        inline void add_item(const runtime_function_entry& exc_entry) {
            _exception_entries.push_back(pe_exception_entry(exc_entry.begin_address, exc_entry.end_address, exc_entry.unwind_info_address));
        }
        inline void clear() {
            _exception_entries.clear();
        }

    public:
        inline size_t size() const {
            return _exception_entries.size();
        }
        constexpr const std::vector<pe_exception_unwind_info>& get_unwind_entries() const {
            return _unwind_entries;
        }
        constexpr std::vector<pe_exception_unwind_info>& get_unwind_entries() {
            return _unwind_entries;
        }
        constexpr const std::vector<pe_exception_entry>& get_exception_entries() const {
            return _exception_entries;
        }
        constexpr std::vector<pe_exception_entry>& get_exception_entries() {
            return _exception_entries;
        }
    };


    pe_directory_code get_exception_directory(_In_ const pe_image& image, _Out_ pe_exceptions_directory& exceptions);
    bool build_exceptions_directory(_Inout_ pe_image& image, _Inout_ pe_section& section,
        _Inout_ pe_exceptions_directory& exceptions, _Inout_ pe_relocations_directory& relocs, _In_ bool build_unwindinfo = false);
    pe_directory_code get_placement_exceptions_directory(_In_ const pe_image& image, _Inout_ pe_placement& placement);

}