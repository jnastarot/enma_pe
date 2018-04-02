#pragma once

class debug_item {
    DWORD   characteristics;
    DWORD   timestamp;
    WORD    major_version;
    WORD    minor_version;
    DWORD   type;
    DWORD   size_of_data;
    DWORD   address_of_raw_data;
    DWORD   pointer_to_raw_data;

    std::vector<BYTE> item_data;
public:
    debug_item::debug_item();
    debug_item::debug_item(DWORD   characteristics,DWORD   timestamp,WORD    major_version, WORD    minor_version,
    DWORD   type, DWORD   size_of_data, DWORD   address_of_raw_data,DWORD   pointer_to_raw_data,
     void * data);

    debug_item::~debug_item();

    debug_item& debug_item::operator=(const debug_item& item);
public:
    void  debug_item::set_characteristics(DWORD characteristics);
    void  debug_item::set_timestamp(DWORD timestamp);
    void  debug_item::set_major_version(WORD major_version);
    void  debug_item::set_minor_version(WORD minor_version);
    void  debug_item::set_type(DWORD type);
    void  debug_item::set_size_of_data(DWORD size_of_data);
    void  debug_item::set_address_of_raw_data(DWORD address_of_raw_data);
    void  debug_item::set_pointer_to_raw_data(DWORD pointer_to_raw_data);
public:
    DWORD  debug_item::get_characteristics() const;
    DWORD  debug_item::get_timestamp() const;
    WORD   debug_item::get_major_version() const;
    WORD   debug_item::get_minor_version() const;
    DWORD  debug_item::get_type() const;
    DWORD  debug_item::get_size_of_data() const;
    DWORD  debug_item::get_address_of_raw_data() const;
    DWORD  debug_item::get_pointer_to_raw_data() const;

    std::vector<BYTE> debug_item::get_item_data();
};

class debug_table {

    std::vector<debug_item> items;
public:
    debug_table::debug_table();
    debug_table::~debug_table();

    debug_table& debug_table::operator=(const debug_table& debug);
public:
    void debug_table::add_item(const debug_item& item);
public:

    std::vector<debug_item>& debug_table::get_items();
};

bool get_debug_table(_In_ const pe_image &image, _Out_ debug_table& debug);
bool erase_debug_table(_Inout_ pe_image &image,
	_Inout_opt_ std::vector<erased_zone>* zones = 0);