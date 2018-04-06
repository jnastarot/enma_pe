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

    void relocation_table::get_items_by_relocation_id(std::vector<relocation_item*>& found_relocs, uint32_t relocation_id);

    void relocation_table::clear();
    void relocation_table::sort();  
public:
    size_t relocation_table::size() const;
    bool relocation_table::has_item(uint32_t rva) const;
    bool relocation_table::has_item_id(uint32_t relocation_id) const;
    bool relocation_table::get_item_id(uint32_t rva, uint32_t& relocation_id) const;

    std::vector<relocation_item>& relocation_table::get_items();
};




bool get_relocation_table(_In_ const pe_image &image,
	_Out_ relocation_table& relocs);
void build_relocation_table(_Inout_ pe_image &image, _Inout_ pe_section& section,
	_Inout_ relocation_table& relocs);			
bool get_placement_relocation_table(_Inout_ pe_image &image, _Inout_ std::vector<directory_placement>& placement);
