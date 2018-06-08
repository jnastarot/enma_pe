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
    debug_item();
    debug_item(const debug_item& item);
    debug_item(uint32_t   characteristics, uint32_t   timestamp, uint16_t    major_version, uint16_t    minor_version,
        uint32_t   type, uint32_t   size_of_data, uint32_t   address_of_raw_data, uint32_t   pointer_to_raw_data,
     void * data);

    ~debug_item();

    debug_item& operator=(const debug_item& item);
public:
    void  set_characteristics(uint32_t characteristics);
    void  set_timestamp(uint32_t timestamp);
    void  set_major_version(uint16_t major_version);
    void  set_minor_version(uint16_t minor_version);
    void  set_type(uint32_t type);
    void  set_size_of_data(uint32_t size_of_data);
    void  set_address_of_raw_data(uint32_t address_of_raw_data);
    void  set_pointer_to_raw_data(uint32_t pointer_to_raw_data);
public:
    uint32_t  get_characteristics() const;
    uint32_t  get_timestamp() const;
    uint16_t  get_major_version() const;
    uint16_t  get_minor_version() const;
    uint32_t  get_type() const;
    uint32_t  get_size_of_data() const;
    uint32_t  get_address_of_raw_data() const;
    uint32_t  get_pointer_to_raw_data() const;

    const std::vector<uint8_t>& get_item_data() const;
    std::vector<uint8_t>& get_item_data();
};

class debug_table {

    std::vector<debug_item> items;
public:
    debug_table();
    debug_table(const debug_table& debug);
    ~debug_table();

    debug_table& operator=(const debug_table& debug);
public:
    void add_item(const debug_item& item);
    void clear();
public:
    size_t size() const;

    const std::vector<debug_item>& get_items() const;
    std::vector<debug_item>& get_items();
};

directory_code get_debug_table(_In_ const pe_image &image, _Out_ debug_table& debug);
directory_code get_placement_debug_table(_In_ const pe_image &image, _Inout_ std::vector<directory_placement>& placement);