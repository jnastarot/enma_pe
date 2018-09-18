#pragma once

class bound_imported_ref{
    std::string ref_name;
    uint32_t  timestamp;
    
public:
    bound_imported_ref();
    bound_imported_ref(const bound_imported_ref& ref);
    bound_imported_ref(const std::string& ref_name, uint32_t  timestamp);
    ~bound_imported_ref();

    bound_imported_ref& operator=(const bound_imported_ref& ref);
public:
    void set_ref_name(const std::string& ref_name);
    void set_timestamp(uint32_t  timestamp);

public:
    std::string get_ref_name() const;
    uint32_t  get_timestamp() const;
};

class bound_imported_library {
    std::string library_name;
    uint32_t  timestamp;
    std::vector<bound_imported_ref> refs;
public:
    bound_imported_library();
    bound_imported_library(const bound_imported_library& lib);
    ~bound_imported_library();

    bound_imported_library& operator=(const bound_imported_library& lib);
public:
    void set_library_name(const std::string& library_name);
    void set_timestamp(uint32_t  timestamp);

    void add_ref(const bound_imported_ref& ref);
public:
    std::string get_library_name() const;
    uint32_t  get_timestamp() const;
    size_t get_number_of_forwarder_refs() const;

    const std::vector<bound_imported_ref>& get_refs() const;
    std::vector<bound_imported_ref>& get_refs();
};

class bound_import_table {
    std::vector<bound_imported_library> libraries;
public:
    bound_import_table();
    bound_import_table(const bound_import_table& imports);
    ~bound_import_table();

    bound_import_table& operator=(const bound_import_table& imports);
public:
    void add_library(const bound_imported_library& lib);
    void clear();
public:
    bool has_library(const std::string& library_name, uint32_t timestamp) const;
    bool has_library(const std::string& library_name) const;

    size_t size() const;
    const std::vector<bound_imported_library>& get_libraries() const;

    std::vector<bound_imported_library>& get_libraries();
};



directory_code get_bound_import_table(_In_ const pe_image &image, _Out_ bound_import_table& imports);
bool build_bound_import_table(_Inout_ pe_image &image, _Inout_ pe_section& section, 
    _In_ bound_import_table& imports);
directory_code get_placement_bound_import_table(_In_ const pe_image &image, _Inout_ pe_directory_placement& placement);