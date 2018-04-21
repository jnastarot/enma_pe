#pragma once


class delay_imported_library {
    std::string library_name;

    uint32_t  attributes;   
    uint32_t  dll_name_rva;
    uint32_t  module_handle_rva;
    uint32_t  iat_rva;
    uint32_t  names_table_rva;
    uint32_t  iat_bound_table_rva;
    uint32_t  unload_info_table_rva;
    uint32_t  timestamp;

    std::vector<imported_func> imported_items;
public:
    delay_imported_library::delay_imported_library();
    delay_imported_library::delay_imported_library(const delay_imported_library& lib);
    delay_imported_library::~delay_imported_library();

    delay_imported_library& delay_imported_library::operator=(const delay_imported_library& lib);
public:
    void delay_imported_library::set_library_name(const std::string& library_name);

    void delay_imported_library::set_attributes(uint32_t  attributes);
    void delay_imported_library::set_dll_name_rva(uint32_t  dll_name_rva);
    void delay_imported_library::set_module_handle_rva(uint32_t  rva);
    void delay_imported_library::set_iat_rva(uint32_t  rva);
    void delay_imported_library::set_names_table_rva(uint32_t  rva);
    void delay_imported_library::set_bound_table_iat_rva(uint32_t  rva);
    void delay_imported_library::set_unload_info_table_rva(uint32_t  rva);
    void delay_imported_library::set_timestamp(uint32_t  timestamp);

    void delay_imported_library::add_item(const imported_func& item);
public:
    std::string delay_imported_library::get_library_name() const;

    uint32_t delay_imported_library::get_attributes() const;
    uint32_t delay_imported_library::get_dll_name_rva() const;
    uint32_t delay_imported_library::get_module_handle_rva() const;
    uint32_t delay_imported_library::get_iat_rva() const;
    uint32_t delay_imported_library::get_names_table_rva() const;
    uint32_t delay_imported_library::get_bound_table_iat_rva() const;
    uint32_t delay_imported_library::get_unload_info_table_rva() const;
    uint32_t delay_imported_library::get_timestamp() const;

    imported_library delay_imported_library::convert_to_imported_library() const;
};

class delay_import_table {
    std::vector<delay_imported_library> libraries;
public:
    delay_import_table::delay_import_table();
    delay_import_table::delay_import_table(const delay_import_table& imports);
    delay_import_table::~delay_import_table();

    delay_import_table& delay_import_table::operator=(const delay_import_table& imports);
public:
    void delay_import_table::add_library(const delay_imported_library& lib);
    void delay_import_table::clear();
public:
    size_t delay_import_table::size() const;
    import_table delay_import_table::convert_to_import_table() const;
    std::vector<delay_imported_library>& delay_import_table::get_libs();
};



directory_code get_delay_import_table(_In_ const pe_image &image, _Out_ delay_import_table& imports, 
    _In_opt_ const bound_import_table* bound_imports = 0);
directory_code get_placement_delay_import_table(_In_ const pe_image &image, _Inout_ std::vector<directory_placement>& placement);