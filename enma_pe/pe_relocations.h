#pragma once

struct relocation_item {
    uint32_t relative_virtual_address;
    uint32_t relocation_id;
    uint64_t data;
};


class relocation_table {
    std::vector<relocation_item> items;
public:
    relocation_table::relocation_table();
    relocation_table::relocation_table(const relocation_table& relocations);
    relocation_table::~relocation_table();

    relocation_table& relocation_table::operator=(const relocation_table& relocations);
public:
    void relocation_table::add_item(uint32_t rva, uint32_t relocation_id);
    bool relocation_table::erase_item(uint32_t rva);
    bool relocation_table::erase_first_item_by_id(uint32_t relocation_id);
    bool relocation_table::erase_all_items_by_id(uint32_t relocation_id);
    bool relocation_table::erase_all_items_in_zone(uint32_t rva, uint32_t size);

    void relocation_table::get_items_by_relocation_id(std::vector<relocation_item*>& found_relocs, uint32_t relocation_id);

    //using it only items was sorted
    void relocation_table::get_items_by_segment(std::vector<relocation_item>& relocs, uint32_t segment_rva, uint32_t segment_size);

    void relocation_table::clear();
    void relocation_table::sort();  
public:
    size_t relocation_table::size() const;
    bool relocation_table::has_item(uint32_t rva) const;
    bool relocation_table::has_item_id(uint32_t relocation_id) const;
    bool relocation_table::get_item_id(uint32_t rva, uint32_t& relocation_id) const;

    const std::vector<relocation_item>& relocation_table::get_items() const;
    std::vector<relocation_item>& relocation_table::get_items();
};




directory_code get_relocation_table(_In_ const pe_image &image,
	_Out_ relocation_table& relocs);
bool build_relocation_table(_Inout_ pe_image &image, _Inout_ pe_section& section,
	_Inout_ relocation_table& relocs);			
directory_code get_placement_relocation_table(_In_ const pe_image &image, _Inout_ std::vector<directory_placement>& placement);
