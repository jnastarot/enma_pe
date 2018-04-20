#pragma once

class bound_imported_ref{
    std::string ref_name;
    uint32_t  timestamp;
    
public:
    bound_imported_ref::bound_imported_ref();
    bound_imported_ref::bound_imported_ref(const bound_imported_ref& ref);
    bound_imported_ref::bound_imported_ref(const std::string& ref_name, uint32_t  timestamp);
    bound_imported_ref::~bound_imported_ref();

    bound_imported_ref& bound_imported_ref::operator=(const bound_imported_ref& ref);
public:
    void bound_imported_ref::set_ref_name(const std::string& ref_name);
    void bound_imported_ref::set_timestamp(uint32_t  timestamp);

public:
    std::string bound_imported_ref::get_ref_name() const;
    uint32_t  bound_imported_ref::get_timestamp() const;
};

class bound_imported_library {
    std::string library_name;
    uint32_t  timestamp;
    std::vector<bound_imported_ref> refs;
public:
    bound_imported_library::bound_imported_library();
    bound_imported_library::bound_imported_library(const bound_imported_library& lib);
    bound_imported_library::~bound_imported_library();

    bound_imported_library& bound_imported_library::operator=(const bound_imported_library& lib);
public:
    void bound_imported_library::set_library_name(const std::string& library_name);
    void bound_imported_library::set_timestamp(uint32_t  timestamp);

    void bound_imported_library::add_ref(const bound_imported_ref& ref);
public:
    std::string bound_imported_library::get_library_name() const;
    uint32_t  bound_imported_library::get_timestamp() const;
    size_t bound_imported_library::get_number_of_forwarder_refs() const;

    std::vector<bound_imported_ref>& bound_imported_library::get_refs();
};

class bound_import_table {
    std::vector<bound_imported_library> libs;
public:
    bound_import_table::bound_import_table();
    bound_import_table::bound_import_table(const bound_import_table& imports);
    bound_import_table::~bound_import_table();

    bound_import_table& bound_import_table::operator=(const bound_import_table& imports);
public:
    void bound_import_table::add_lib(const bound_imported_library& lib);
public:
    std::vector<bound_imported_library>& bound_import_table::get_libs();
};



directory_code get_bound_import_table(_In_ const pe_image &image, _Out_ bound_import_table& imports);
bool build_bound_import_table(_Inout_ pe_image &image, _Inout_ pe_section& section, 
    _In_ bound_import_table& imports);
directory_code get_placement_bound_import_table(_In_ const pe_image &image, _Inout_ std::vector<directory_placement>& placement);