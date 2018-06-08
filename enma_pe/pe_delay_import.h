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
    delay_imported_library();
    delay_imported_library(const delay_imported_library& lib);
    ~delay_imported_library();

    delay_imported_library& operator=(const delay_imported_library& lib);
public:
    void set_library_name(const std::string& library_name);

    void set_attributes(uint32_t  attributes);
    void set_dll_name_rva(uint32_t  dll_name_rva);
    void set_module_handle_rva(uint32_t  rva);
    void set_iat_rva(uint32_t  rva);
    void set_names_table_rva(uint32_t  rva);
    void set_bound_table_iat_rva(uint32_t  rva);
    void set_unload_info_table_rva(uint32_t  rva);
    void set_timestamp(uint32_t  timestamp);

    void add_item(const imported_func& item);
public:
    std::string get_library_name() const;

    uint32_t get_attributes() const;
    uint32_t get_dll_name_rva() const;
    uint32_t get_module_handle_rva() const;
    uint32_t get_iat_rva() const;
    uint32_t get_names_table_rva() const;
    uint32_t get_bound_table_iat_rva() const;
    uint32_t get_unload_info_table_rva() const;
    uint32_t get_timestamp() const;
    const std::vector<imported_func>& get_items() const;

    std::vector<imported_func>& get_items();

    imported_library convert_to_imported_library() const;
};

class delay_import_table {
    std::vector<delay_imported_library> libraries;
public:
    delay_import_table();
    delay_import_table(const delay_import_table& imports);
    ~delay_import_table();

    delay_import_table& operator=(const delay_import_table& imports);
public:
    void add_library(const delay_imported_library& lib);
    void clear();
public:
    size_t size() const;
    import_table convert_to_import_table() const;
    const std::vector<delay_imported_library>& get_libraries() const;

    std::vector<delay_imported_library>& get_libraries();
};



directory_code get_delay_import_table(_In_ const pe_image &image, _Out_ delay_import_table& imports, 
    _In_ const bound_import_table& bound_imports);
directory_code get_placement_delay_import_table(_In_ const pe_image &image, _Inout_ std::vector<directory_placement>& placement,
    _In_ const bound_import_table& bound_imports);