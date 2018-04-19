#pragma once

class debug_item {
    uint32_t   characteristics;
    uint32_t   timestamp;
    uint16_t   major_version;
    uint16_t   minor_version;
    uint32_t   type;
    uint32_t   size_of_data;
    uint32_t   address_of_raw_data;
    uint32_t   pointer_to_raw_data;

    std::vector<uint8_t> item_data;
public:
    debug_item::debug_item();
    debug_item::debug_item(const debug_item& item);
    debug_item::debug_item(uint32_t   characteristics, uint32_t   timestamp, uint16_t    major_version, uint16_t    minor_version,
        uint32_t   type, uint32_t   size_of_data, uint32_t   address_of_raw_data, uint32_t   pointer_to_raw_data,
     void * data);

    debug_item::~debug_item();

    debug_item& debug_item::operator=(const debug_item& item);
public:
    void  debug_item::set_characteristics(uint32_t characteristics);
    void  debug_item::set_timestamp(uint32_t timestamp);
    void  debug_item::set_major_version(uint16_t major_version);
    void  debug_item::set_minor_version(uint16_t minor_version);
    void  debug_item::set_type(uint32_t type);
    void  debug_item::set_size_of_data(uint32_t size_of_data);
    void  debug_item::set_address_of_raw_data(uint32_t address_of_raw_data);
    void  debug_item::set_pointer_to_raw_data(uint32_t pointer_to_raw_data);
public:
    uint32_t  debug_item::get_characteristics() const;
    uint32_t  debug_item::get_timestamp() const;
    uint16_t  debug_item::get_major_version() const;
    uint16_t  debug_item::get_minor_version() const;
    uint32_t  debug_item::get_type() const;
    uint32_t  debug_item::get_size_of_data() const;
    uint32_t  debug_item::get_address_of_raw_data() const;
    uint32_t  debug_item::get_pointer_to_raw_data() const;

    std::vector<uint8_t>& debug_item::get_item_data();
};

class debug_table {

    std::vector<debug_item> items;
public:
    debug_table::debug_table();
    debug_table::debug_table(const debug_table& debug);
    debug_table::~debug_table();

    debug_table& debug_table::operator=(const debug_table& debug);
public:
    void debug_table::add_item(const debug_item& item);
public:

    std::vector<debug_item>& debug_table::get_items();
};

directory_code get_debug_table(_In_ const pe_image &image, _Out_ debug_table& debug);
directory_code get_placement_debug_table(_In_ const pe_image &image, _Inout_ std::vector<directory_placement>& placement);