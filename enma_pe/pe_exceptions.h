#pragma once

class exceptions_item {
    uint32_t address_begin;
    uint32_t address_end;
    uint32_t address_unwind_data;
public:
    exceptions_item::exceptions_item();
    exceptions_item::exceptions_item(const exceptions_item& item);
    exceptions_item::exceptions_item(uint32_t address_begin, uint32_t address_end, uint32_t address_unwind_data);
    exceptions_item::~exceptions_item();

    exceptions_item& exceptions_item::operator=(const exceptions_item& item);
public:
    void exceptions_item::set_begin_address(uint32_t rva_address);
    void exceptions_item::set_end_address(uint32_t rva_address);
    void exceptions_item::set_unwind_data_address(uint32_t rva_address);


public:
    uint32_t exceptions_item::get_begin_address() const;
    uint32_t exceptions_item::get_end_address() const;
    uint32_t exceptions_item::get_unwind_data_address() const;

};

class exceptions_table {

    std::vector<exceptions_item> items;
public:
    exceptions_table::exceptions_table();
    exceptions_table::exceptions_table(const exceptions_table& exceptions);
    exceptions_table::~exceptions_table();

    exceptions_table& exceptions_table::operator=(const exceptions_table& exceptions);
public:
    void exceptions_table::add_item(uint32_t address_begin, uint32_t address_end, uint32_t address_unwind_data);
    void exceptions_table::add_item(const exceptions_item& item);
    void exceptions_table::add_item(const image_ia64_runtime_function_entry& exc_entry);
    
public:
    size_t exceptions_table::size() const;

    std::vector<exceptions_item>& exceptions_table::get_items();
};



directory_code get_exception_table(_In_ const pe_image &image, _Out_ exceptions_table& exceptions);
bool build_exceptions_table(_Inout_ pe_image &image, _Inout_ pe_section& section,
    _In_ exceptions_table& exceptions);
directory_code get_placement_exceptions_table(_In_ const pe_image &image, _Inout_ std::vector<directory_placement>& placement);