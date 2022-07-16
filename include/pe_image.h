#pragma once

#include "pe_section.h"

namespace enma {

    struct pe_image {

    private:
        pe_image_status _image_status;

        std::vector<uint8_t> _headers_data;
        pe_rich_header _rich_header;

        uint16_t _machine;
        uint32_t _timestamp;
        uint32_t _pointer_to_symbol_table;
        uint32_t _number_of_symbols;
        uint32_t _size_of_optional_header;
        uint16_t _characteristics;

        uint16_t _magic;
        uint8_t  _major_linker;
        uint8_t  _minor_linker;
        uint32_t _size_of_code;
        uint32_t _size_of_init_data;
        uint32_t _size_of_uninit_data;
        uint32_t _entry_point;
        uint32_t _base_of_code;
        uint32_t _base_of_data;
        uint64_t _image_base;
        uint32_t _section_align;
        uint32_t _file_align;
        uint16_t _os_ver_major;
        uint16_t _os_ver_minor;
        uint16_t _image_ver_major;
        uint16_t _image_ver_minor;
        uint16_t _subsystem_ver_major;
        uint16_t _subsystem_ver_minor;
        uint32_t _win32_version_value;
        uint32_t _image_size;
        uint32_t _headers_size;
        uint32_t _checksum;
        uint16_t _sub_system;
        uint16_t _characteristics_dll;
        uint64_t _stack_reserve_size;
        uint64_t _stack_commit_size;
        uint64_t _heap_reserve_size;
        uint64_t _heap_commit_size;
        uint32_t _loader_flags;

        image_data_directory _directories[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];

        std::vector<pe_section*> _sections;

        std::vector<uint8_t> _overlay_data;

        void init_from_file(const uint8_t* image, size_t size);
    public:

        pe_image()
            : _image_status(pe_image_status_unknown)
            , _machine(0)
            , _timestamp(0)
            , _pointer_to_symbol_table(0)
            , _number_of_symbols(0)
            , _size_of_optional_header(0)
            , _characteristics(0)
            , _magic(0)
            , _major_linker(0)
            , _minor_linker(0)
            , _size_of_code(0)
            , _size_of_init_data(0)
            , _size_of_uninit_data(0)
            , _entry_point(0)
            , _base_of_code(0)
            , _base_of_data(0)
            , _image_base(0)
            , _section_align(0)
            , _file_align(0)
            , _os_ver_major(0)
            , _os_ver_minor(0)
            , _image_ver_major(0)
            , _image_ver_minor(0)
            , _subsystem_ver_major(0)
            , _subsystem_ver_minor(0)
            , _win32_version_value(0)
            , _image_size(0)
            , _headers_size(0)
            , _checksum(0)
            , _sub_system(0)
            , _characteristics_dll(0)
            , _stack_reserve_size(0)
            , _stack_commit_size(0)
            , _heap_reserve_size(0)
            , _heap_commit_size(0)
            , _loader_flags(0)
            , _directories{} {}

        pe_image(const pe_image& image);
        explicit pe_image(bool _pe32, bool init_dos_thunk);
        pe_image(const uint8_t* raw_image, uint32_t size);
        pe_image(const std::string& file_path);
        pe_image(const std::wstring& file_path);
        ~pe_image();

        pe_image& operator=(const pe_image& image);

    public:

        constexpr bool is_x32_image() const {
            if (this->_magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
                return true;
            }
            return false;
        }
        constexpr bool is_dll_image()  const {
            if (this->_characteristics & IMAGE_FILE_DLL) {
                return true;
            }
            return false;
        }
        constexpr bool is_dotnet_image() const {
            return get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR) != 0;
        }

    public:// data/sections helpers

        inline pe_section& add_section() {

            uint32_t virtual_address = _section_align;
            uint32_t raw_pointer = _file_align;

            if (_sections.size()) {

                virtual_address = ALIGN_UP(_sections[_sections.size() - 1]->get_virtual_address() +
                    _sections[_sections.size() - 1]->get_virtual_size(), _section_align);

                raw_pointer = ALIGN_UP(_sections[_sections.size() - 1]->get_pointer_to_raw() +
                    _sections[_sections.size() - 1]->get_size_of_raw_data(), _file_align);

                if (!_sections[_sections.size() - 1]->get_virtual_size()) {
                    virtual_address += _section_align;
                }
            }

            _sections.push_back(new pe_section());
            pe_section& section = *_sections[_sections.size() - 1];

            section.set_virtual_address(virtual_address);
            section.set_pointer_to_raw(raw_pointer);

            return section;
        }
        inline pe_section& add_section(const pe_section& section) {
            return (add_section() = section);
        }
        inline pe_section& add_section(const image_section_header& header) {
            _sections.push_back(new pe_section(header));
            return *_sections[_sections.size() - 1];
        }
        inline pe_section& add_section(const image_section_header& header, const std::vector<uint8_t>& data) {
            _sections.push_back(new pe_section(header, data));
            return *_sections[_sections.size() - 1];
        }

        constexpr std::vector<pe_section*>& get_sections() {
            return _sections;
        }
        constexpr const std::vector<pe_section*>& get_sections() const {
            return _sections;
        }
        inline size_t get_sections_number() const {
            return _sections.size();
        }
        inline pe_section* get_section_by_rva(uint32_t rva) const {
            for (size_t section_idx = 0; section_idx < _sections.size(); section_idx++) {
                if (
                    _sections[section_idx]->get_virtual_address() <= rva &&
                    _sections[section_idx]->get_virtual_address() + _sections[section_idx]->get_virtual_size() > rva
                    ) {

                    return _sections[section_idx];
                }
            }
            return 0;
        }
        inline pe_section* get_section_by_va(uint64_t va) const {
            return get_section_by_rva(va_to_rva(va));
        }
        inline pe_section* get_section_by_raw(uint32_t raw) const {
            for (size_t section_idx = 0; section_idx < _sections.size(); section_idx++) {
                if (
                    _sections[section_idx]->get_pointer_to_raw() <= raw &&
                    _sections[section_idx]->get_pointer_to_raw() + _sections[section_idx]->get_size_of_raw_data() > raw
                    ) {
                    return _sections[section_idx];
                }
            }
            return 0;
        }
        inline pe_section* get_section_by_idx(uint32_t idx) const {
            if (_sections.size() > idx) {
                return _sections[idx];
            }

            return 0;
        }
        inline pe_section* get_last_section() const {
            if (_sections.size()) {
                return _sections[_sections.size() - 1];
            }

            return 0;
        }

        inline pe_section* get_section_top_raw() const {

            pe_section* current_top = 0;

            for (auto& section : _sections) {

                if (!current_top ||
                    current_top->get_pointer_to_raw() < section->get_pointer_to_raw()) {

                    current_top = section;
                }
            }

            return current_top;
        }

        inline pe_section* get_section_top_rva() const {

            pe_section* current_top = 0;

            for (auto& section : _sections) {

                if (!current_top ||
                    current_top->get_virtual_address() < section->get_virtual_address()) {

                    current_top = section;
                }
            }

            return current_top;
        }

        constexpr uint32_t va_to_rva(uint64_t va) const {

            if (va) {
                return uint32_t(va - _image_base);
            }
            else {
                return 0;
            }
        }

        inline uint32_t va_to_raw(uint64_t va) const {

            pe_section* section = get_section_by_va(va);

            if (section) {
                return (va_to_rva(va) - section->get_virtual_address()) + section->get_pointer_to_raw();
            }
            else {
                return 0;
            }
        }

        constexpr uint64_t rva_to_va(uint32_t rva) const {
            return (rva + _image_base);
        }

        inline uint32_t rva_to_raw(uint32_t rva) const {

            pe_section* section = get_section_by_rva(rva);

            if (section) {
                return (rva - section->get_virtual_address()) + section->get_pointer_to_raw();
            }
            else {
                return 0;
            }
        }

        inline uint64_t raw_to_va(uint32_t raw) const {
            return rva_to_va(raw_to_rva(raw));
        }

        inline uint32_t raw_to_rva(uint32_t raw) const {

            pe_section* section = get_section_by_raw(raw);

            if (section) {
                return  (raw - section->get_pointer_to_raw()) + section->get_virtual_address();
            }
            else {
                return 0;
            }
        }

    public:

        constexpr pe_image& set_image_status(pe_image_status status) {
            _image_status = status;

            return *this;
        }
        inline pe_image& set_rich_header(const pe_rich_header& header) {
            _rich_header = header;

            return *this;
        }
        inline  pe_image& set_headers_data(const std::vector<uint8_t>& headers_data) {
            _headers_data = headers_data;

            return *this;
        }
        inline pe_image& set_dos_header(const image_dos_header& header) {

            if (_headers_data.size() < sizeof(image_dos_header)) {
                _headers_data.resize(sizeof(image_dos_header));
            }

            memcpy(_headers_data.data(), &header, sizeof(image_dos_header));

            return *this;
        }

        constexpr pe_image& set_machine(uint16_t machine) {
            _machine = machine;

            return *this;
        }
        constexpr pe_image& set_timestamp(uint32_t timestamp) {
            _timestamp = timestamp;

            return *this;
        }
        constexpr pe_image& set_pointer_to_symbol_table(uint32_t pointer_to_symbol_table) {
            _pointer_to_symbol_table = pointer_to_symbol_table;

            return *this;
        }
        constexpr pe_image& set_number_of_symbols(uint32_t number_of_symbols) {
            _number_of_symbols = number_of_symbols;

            return *this;
        }
        constexpr pe_image& set_size_of_optional_header(uint32_t size_of_optional_header) {
            _size_of_optional_header = size_of_optional_header;

            return *this;
        }
        constexpr pe_image& set_characteristics(uint16_t characteristics) {
            _characteristics = characteristics;

            return *this;
        }
        constexpr pe_image& set_magic(uint16_t magic) {
            _magic = magic;

            return *this;
        }
        constexpr pe_image& set_major_linker(uint8_t major_linker) {
            _major_linker = major_linker;

            return *this;
        }
        constexpr pe_image& set_minor_linker(uint8_t minor_linker) {
            _minor_linker = minor_linker;

            return *this;
        }
        constexpr pe_image& set_size_of_code(uint32_t size_of_code) {
            _size_of_code = size_of_code;

            return *this;
        }
        constexpr pe_image& set_size_of_init_data(uint32_t size_of_init_data) {
            _size_of_init_data = size_of_init_data;

            return *this;
        }
        constexpr pe_image& set_size_of_uninit_data(uint32_t size_of_uninit_data) {
            _size_of_uninit_data = size_of_uninit_data;

            return *this;
        }
        constexpr pe_image& set_entry_point(uint32_t entry_point) {
            _entry_point = entry_point;

            return *this;
        }
        constexpr pe_image& set_base_of_code(uint32_t base_of_code) {
            _base_of_code = base_of_code;

            return *this;
        }
        constexpr pe_image& set_base_of_data(uint32_t base_of_data) {
            _base_of_data = base_of_data;

            return *this;
        }
        constexpr pe_image& set_image_base(uint64_t image_base) {
            _image_base = image_base;

            return *this;
        }
        constexpr pe_image& set_section_align(uint32_t section_align) {
            _section_align = section_align;

            return *this;
        }
        constexpr pe_image& set_file_align(uint32_t file_align) {
            _file_align = file_align;

            return *this;
        }
        constexpr pe_image& set_os_ver_major(uint16_t os_ver_major) {
            _os_ver_major = os_ver_major;

            return *this;
        }
        constexpr pe_image& set_os_ver_minor(uint16_t os_ver_minor) {
            _os_ver_minor = os_ver_minor;

            return *this;
        }
        constexpr pe_image& set_image_ver_major(uint16_t image_ver_major) {
            _image_ver_major = image_ver_major;

            return *this;
        }
        constexpr pe_image& set_image_ver_minor(uint16_t image_ver_minor) {
            _image_ver_minor = image_ver_minor;

            return *this;
        }
        constexpr pe_image& set_subsystem_ver_major(uint16_t subsystem_ver_major) {
            _subsystem_ver_major = subsystem_ver_major;

            return *this;
        }
        constexpr pe_image& set_subsystem_ver_minor(uint16_t subsystem_ver_minor) {
            _subsystem_ver_minor = subsystem_ver_minor;

            return *this;
        }
        constexpr pe_image& set_win32_version_value(uint32_t win32_version_value) {
            _win32_version_value = win32_version_value;

            return *this;
        }
        constexpr pe_image& set_image_size(uint32_t image_size) {
            _image_size = image_size;

            return *this;
        }
        constexpr pe_image& set_headers_size(uint32_t headers_size) {
            _headers_size = headers_size;

            return *this;
        }
        constexpr pe_image& set_checksum(uint32_t checksum) {
            _checksum = checksum;

            return *this;
        }
        constexpr pe_image& set_sub_system(uint16_t sub_system) {
            _sub_system = sub_system;

            return *this;
        }
        constexpr pe_image& set_characteristics_dll(uint16_t characteristics_dll) {
            _characteristics_dll = characteristics_dll;

            return *this;
        }
        constexpr pe_image& set_stack_reserve_size(uint64_t stack_reserve_size) {
            _stack_reserve_size = stack_reserve_size;

            return *this;
        }
        constexpr pe_image& set_stack_commit_size(uint64_t stack_commit_size) {
            _stack_commit_size = stack_commit_size;

            return *this;
        }
        constexpr pe_image& set_heap_reserve_size(uint64_t heap_reserve_size) {
            _heap_reserve_size = heap_reserve_size;

            return *this;
        }
        constexpr pe_image& set_heap_commit_size(uint64_t heap_commit_size) {
            _heap_commit_size = heap_commit_size;

            return *this;
        }
        constexpr pe_image& set_loader_flags(uint32_t loader_flags) {
            _loader_flags = loader_flags;

            return *this;
        }

        inline pe_image& set_overlay_data(std::vector<uint8_t>& data) {
            _overlay_data = data;

            return *this;
        }

        //directories
        constexpr pe_image& set_directory_virtual_address(uint32_t directory_idx, uint32_t virtual_address) {

            if (directory_idx < IMAGE_NUMBEROF_DIRECTORY_ENTRIES) {
                _directories[directory_idx].virtual_address = virtual_address;
            }

            return *this;
        }
        constexpr pe_image& set_directory_virtual_size(uint32_t directory_idx, uint32_t virtual_size) {

            if (directory_idx < IMAGE_NUMBEROF_DIRECTORY_ENTRIES) {
                _directories[directory_idx].size = virtual_size;
            }

            return *this;
        }

    public://getter
        constexpr pe_image_status get_image_status() const {
            return _image_status;
        }

        constexpr pe_rich_header& get_rich_header() {
            return _rich_header;
        }
        constexpr const pe_rich_header& get_rich_header() const {
            return _rich_header;
        }
        constexpr std::vector<uint8_t>& get_headers_data() {
            return _headers_data;
        }
        constexpr const std::vector<uint8_t>& get_headers_data() const {
            return _headers_data;
        }

        inline void get_dos_header(image_dos_header& header) const {

            memset(&header, 0, sizeof(image_dos_header));

            if (_headers_data.size() < sizeof(image_dos_header)) {
                memcpy(&header, _headers_data.data(), _headers_data.size());
            }
            else {
                memcpy(&header, _headers_data.data(), sizeof(image_dos_header));
            }
        }

        constexpr uint16_t get_machine() const {
            return _machine;
        }
        constexpr uint32_t get_timestamp() const {
            return _timestamp;
        }
        constexpr uint32_t get_pointer_to_symbol_table() const {
            return _pointer_to_symbol_table;
        }
        constexpr uint32_t get_number_of_symbols() const {
            return _number_of_symbols;
        }
        constexpr uint32_t get_size_of_optional_header() const {
            return _size_of_optional_header;
        }
        constexpr uint16_t get_characteristics() const {
            return _characteristics;
        }
        constexpr uint16_t get_magic() const {
            return _magic;
        }
        constexpr uint8_t get_major_linker() const {
            return _major_linker;
        }
        constexpr uint8_t get_minor_linker() const {
            return _minor_linker;
        }
        constexpr uint32_t get_size_of_code() const {
            return _size_of_code;
        }
        constexpr uint32_t get_size_of_init_data() const {
            return _size_of_init_data;
        }
        constexpr uint32_t get_size_of_uninit_data() const {
            return _size_of_uninit_data;
        }
        constexpr uint32_t get_entry_point() const {
            return _entry_point;
        }
        constexpr uint32_t get_base_of_code() const {
            return _base_of_code;
        }
        constexpr uint32_t get_base_of_data() const {
            return _base_of_data;
        }
        constexpr uint64_t get_image_base() const {
            return _image_base;
        }
        constexpr uint32_t get_section_align() const {
            return _section_align;
        }
        constexpr uint32_t get_file_align() const {
            return _file_align;
        }
        constexpr uint16_t get_os_ver_major() const {
            return _os_ver_major;
        }
        constexpr uint16_t get_os_ver_minor() const {
            return _os_ver_minor;
        }
        constexpr uint16_t get_image_ver_major() const {
            return _image_ver_major;
        }
        constexpr uint16_t get_image_ver_minor() const {
            return _image_ver_minor;
        }
        constexpr uint16_t get_subsystem_ver_major() const {
            return _subsystem_ver_major;
        }
        constexpr uint16_t get_subsystem_ver_minor() const {
            return _subsystem_ver_minor;
        }
        constexpr uint32_t get_win32_version_value() const {
            return _win32_version_value;
        }
        constexpr uint32_t get_image_size() const {
            return _image_size;
        }
        constexpr uint32_t get_headers_size() const {
            return _headers_size;
        }
        constexpr uint32_t get_checksum() const {
            return _checksum;
        }
        constexpr uint16_t get_sub_system() const {
            return _sub_system;
        }
        constexpr uint16_t get_characteristics_dll() const {
            return _characteristics_dll;
        }
        constexpr uint64_t get_stack_reserve_size() const {
            return _stack_reserve_size;
        }
        constexpr uint64_t get_stack_commit_size() const {
            return _stack_commit_size;
        }
        constexpr uint64_t get_heap_reserve_size() const {
            return _heap_reserve_size;
        }
        constexpr uint64_t get_heap_commit_size() const {
            return _heap_commit_size;
        }
        constexpr uint32_t get_loader_flags() const {
            return _loader_flags;
        }

        //directories
        constexpr uint32_t get_directory_virtual_address(uint32_t directory_idx) const {
            if (directory_idx < IMAGE_NUMBEROF_DIRECTORY_ENTRIES) {
                return _directories[directory_idx].virtual_address;
            }

            return 0;
        }

        constexpr uint32_t get_directory_virtual_size(uint32_t directory_idx) const {
            if (directory_idx < IMAGE_NUMBEROF_DIRECTORY_ENTRIES) {
                return _directories[directory_idx].size;
            }

            return 0;
        }

        constexpr bool has_directory(uint32_t directory_idx) const {
            if (directory_idx < IMAGE_NUMBEROF_DIRECTORY_ENTRIES) {
                return (_directories[directory_idx].virtual_address != 0);
            }
            return false;
        }

        //overlay
        constexpr std::vector<uint8_t>& get_overlay_data() {
            return _overlay_data;
        }
        constexpr const std::vector<uint8_t>& get_overlay_data() const {
            return _overlay_data;
        }

    public://util
        void clear_image();

    };

    pe_image_status load_virtual_pe_image(_In_ const uint8_t* hmodule, _Out_ pe_image& image, _In_ bool check_signs = true,
        _In_ bool (*readmem)(void* dst, const uint8_t* src, size_t size) = ([](void* dst, const uint8_t* src, size_t size) -> bool {

            try {
                memcpy(dst, src, size);
            }
            catch (std::exception&) {
                return false;
            }

            return true;
            }));

}