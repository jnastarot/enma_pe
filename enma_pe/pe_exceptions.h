#pragma once

class exceptions_item {
    DWORD address_begin;
    DWORD address_end;
    DWORD address_unwind_data;
public:
    exceptions_item::exceptions_item(DWORD address_begin, DWORD address_end, DWORD address_unwind_data);
    exceptions_item::~exceptions_item();


public:
    void exceptions_item::set_begin_address(DWORD rva_address);
    void exceptions_item::set_end_address(DWORD rva_address);
    void exceptions_item::set_unwind_data_address(DWORD rva_address);


public:
    DWORD exceptions_item::get_begin_address() const;
    DWORD exceptions_item::get_end_address() const;
    DWORD exceptions_item::get_unwind_data_address() const;

};

class exceptions_table {

    std::vector<exceptions_item> items;
public:
    exceptions_table::exceptions_table();
    exceptions_table::~exceptions_table();


public:
    void exceptions_table::add_item(DWORD address_begin, DWORD address_end, DWORD address_unwind_data);
    void exceptions_table::add_item(exceptions_item& item);
public:

    std::vector<exceptions_item>& exceptions_table::get_items();
};



bool get_exception_table(_In_ const pe_image &image, _Out_ exceptions_table& exceptions);
void build_exceptions_table(_Inout_ pe_image &image, _Inout_ pe_section& section,
    _In_ exceptions_table& exceptions);
bool erase_exceptions_table(_Inout_ pe_image &image,
	_Inout_opt_ std::vector<erased_zone>* zones = 0);