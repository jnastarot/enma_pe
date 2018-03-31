#pragma once

class delay_imported_library {
    std::string name;
    DWORD attributes;
    DWORD timestamp;
    DWORD module_handle_rva;
    DWORD rva_to_iat;
    DWORD rva_to_bound_iat;
    DWORD rva_to_unload_info_table;
    
    std::vector<imported_func> imported_items;
public:
    delay_imported_library::delay_imported_library();
    delay_imported_library::~delay_imported_library();

    delay_imported_library& delay_imported_library::operator=(const delay_imported_library& lib);
public:
    void delay_imported_library::set_name(std::string name);
    void delay_imported_library::set_attributes(DWORD attributes);
    void delay_imported_library::set_timestamp(DWORD timestamp);
    void delay_imported_library::set_rva_module_handle(DWORD rva);
    void delay_imported_library::set_rva_iat(DWORD rva);
    void delay_imported_library::set_rva_bound_iat(DWORD rva);
    void delay_imported_library::set_rva_unload_info_table(DWORD rva);
    void delay_imported_library::add_item(imported_func& item);
public:
    std::string delay_imported_library::get_name() const;
    DWORD delay_imported_library::set_attributes() const;
    DWORD delay_imported_library::get_timestamp() const;
    DWORD delay_imported_library::get_rva_module_handle() const;
    DWORD delay_imported_library::get_rva_iat() const;
    DWORD delay_imported_library::get_rva_bound_iat() const;
    DWORD delay_imported_library::get_rva_unload_info_table() const;
    imported_library delay_imported_library::convert_to_imported_library() const;
};

class delay_import_table {
    std::vector<delay_imported_library> libs;
public:
    delay_import_table::delay_import_table();
    delay_import_table::~delay_import_table();

    delay_import_table& delay_import_table::operator=(const delay_import_table& imports);
public:
    void delay_import_table::add_lib(delay_imported_library& lib);
public:
    import_table delay_import_table::convert_to_import_table() const;
    std::vector<delay_imported_library>& delay_import_table::get_libs();
};



bool get_delay_import_table(_In_ const pe_image &image, _Out_ delay_import_table& imports);
bool erase_delay_import_table(_Inout_ pe_image &image,
	_Inout_opt_ std::vector<erased_zone>* zones = 0);