#pragma once

class delay_imported_library {
    std::string library_name;
    uint32_t  attributes;
    uint32_t  timestamp;
    uint32_t  module_handle_rva;
    uint32_t  rva_to_iat;
    uint32_t  rva_to_bound_iat;
    uint32_t  rva_to_unload_info_table;
    
    std::vector<imported_func> imported_items;
public:
    delay_imported_library::delay_imported_library();
    delay_imported_library::delay_imported_library(const delay_imported_library& lib);
    delay_imported_library::~delay_imported_library();

    delay_imported_library& delay_imported_library::operator=(const delay_imported_library& lib);
public:
    void delay_imported_library::set_library_name(const std::string& library_name);
    void delay_imported_library::set_attributes(uint32_t  attributes);
    void delay_imported_library::set_timestamp(uint32_t  timestamp);
    void delay_imported_library::set_rva_module_handle(uint32_t  rva);
    void delay_imported_library::set_rva_iat(uint32_t  rva);
    void delay_imported_library::set_rva_bound_iat(uint32_t  rva);
    void delay_imported_library::set_rva_unload_info_table(uint32_t  rva);
    void delay_imported_library::add_item(const imported_func& item);
public:
    std::string delay_imported_library::get_library_name() const;
    uint32_t  delay_imported_library::set_attributes() const;
    uint32_t  delay_imported_library::get_timestamp() const;
    uint32_t  delay_imported_library::get_rva_module_handle() const;
    uint32_t  delay_imported_library::get_rva_iat() const;
    uint32_t  delay_imported_library::get_rva_bound_iat() const;
    uint32_t  delay_imported_library::get_rva_unload_info_table() const;
    imported_library delay_imported_library::convert_to_imported_library() const;
};

class delay_import_table {
    std::vector<delay_imported_library> libs;
public:
    delay_import_table::delay_import_table();
    delay_import_table::delay_import_table(const delay_import_table& imports);
    delay_import_table::~delay_import_table();

    delay_import_table& delay_import_table::operator=(const delay_import_table& imports);
public:
    void delay_import_table::add_lib(const delay_imported_library& lib);
public:
    import_table delay_import_table::convert_to_import_table() const;
    std::vector<delay_imported_library>& delay_import_table::get_libs();
};



bool get_delay_import_table(_In_ const pe_image &image, _Out_ delay_import_table& imports);
bool get_placement_delay_import_table(_Inout_ pe_image &image, _Inout_ std::vector<directory_placement>& placement);