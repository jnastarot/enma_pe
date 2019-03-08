#pragma once

struct relocation_item {
    uint32_t relative_virtual_address;
    uint32_t relocation_id;
    uint64_t data;
};


class relocation_table {
    std::vector<relocation_item> items;
public:
    relocation_table();
    relocation_table(const relocation_table& relocations);
    ~relocation_table();

    relocation_table& operator=(const relocation_table& relocations);
public:
    void add_item(uint32_t rva, uint32_t relocation_id);
    bool erase_item(uint32_t rva);
    bool erase_first_item_by_id(uint32_t relocation_id);
    bool erase_all_items_by_id(uint32_t relocation_id);
    

    void get_items_by_relocation_id(std::vector<relocation_item*>& found_relocs, uint32_t relocation_id);

    void get_items_in_segment(std::vector<relocation_item>& relocs, uint32_t segment_rva, size_t segment_size);
    bool erase_items_in_segment(uint32_t segment_rva, size_t segment_size);


    void clear();
    void sort();  
public:
    size_t size() const;
    bool has_item(uint32_t rva) const;
    bool has_item_id(uint32_t relocation_id) const;
    bool get_item_id(uint32_t rva, uint32_t& relocation_id) const;

    const std::vector<relocation_item>& get_items() const;
    std::vector<relocation_item>& get_items();
};




directory_code get_relocation_table(_In_ const pe_image &image,
	_Out_ relocation_table& relocs);
bool build_relocation_table(_Inout_ pe_image &image, _Inout_ pe_section& section,
	_In_ const relocation_table& relocs);
directory_code get_placement_relocation_table(_In_ const pe_image &image, _Inout_ pe_directory_placement& placement);
