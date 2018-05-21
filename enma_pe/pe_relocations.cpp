#include "stdafx.h"
#include "pe_relocations.h"


relocation_table::relocation_table() {

}
relocation_table::relocation_table(const relocation_table& relocations) {
    this->operator=(relocations);
}
relocation_table::~relocation_table() {

}
relocation_table& relocation_table::operator=(const relocation_table& relocations) {
    this->items = relocations.items;
    return *this;
}

void relocation_table::add_item(uint32_t rva, uint32_t relocation_id) {
   items.push_back({ rva , relocation_id,0});
}

bool relocation_table::erase_item(uint32_t rva) {

    for (size_t item_idx = 0; item_idx < items.size(); item_idx++) {
        
        if (items[item_idx].relative_virtual_address == rva) {
            items.erase(items.begin() + item_idx);
            return true;
        }
    }

    return false;
}

bool relocation_table::erase_first_item_by_id(uint32_t relocation_id) {

    for (size_t item_idx = 0; item_idx < items.size(); item_idx++) {

        if (items[item_idx].relocation_id == relocation_id) {
            items.erase(items.begin() + item_idx);
            return true;
        }
    }

    return false;
}

bool relocation_table::erase_all_items_by_id(uint32_t relocation_id) {

    bool ret = false;

    for (size_t item_idx = 0; item_idx < items.size(); item_idx++) {

        if (items[item_idx].relocation_id == relocation_id) {
            items.erase(items.begin() + item_idx);
            item_idx--;
            ret = true;
        }
    }

    return ret;
}

bool relocation_table::erase_all_items_in_zone(uint32_t rva, uint32_t size) {

    bool ret = false;

    for (size_t item_idx = 0; item_idx < items.size(); item_idx++) {
        if (items[item_idx].relative_virtual_address >= rva &&
            items[item_idx].relative_virtual_address < rva + size) {
            items.erase(items.begin() + item_idx);
            item_idx--;
            ret = true;
        }
    }

    return ret;
}


void relocation_table::get_items_by_relocation_id(std::vector<relocation_item*>& found_relocs, uint32_t relocation_id) {

    found_relocs.clear();

    for (size_t item_idx = 0; item_idx < items.size(); item_idx++) {
        if (items[item_idx].relocation_id == relocation_id) {
            found_relocs.push_back(&items[item_idx]);
        }
    }
}

void relocation_table::get_items_by_segment(std::vector<relocation_item>& relocs, uint32_t segment_rva, uint32_t segment_size) {

    relocs.clear();

    for (auto& item : items) {
        if (item.relative_virtual_address >= segment_rva + segment_size) { return; }

        if (item.relative_virtual_address >= segment_rva &&
            item.relative_virtual_address < segment_rva + segment_size) {
            relocs.push_back(item);
        }
    }
}

void relocation_table::clear() {
    items.clear();
}
void relocation_table::sort() {
    std::sort(items.begin(), items.end(), [](relocation_item& lhs, relocation_item& rhs) {
        return lhs.relative_virtual_address < rhs.relative_virtual_address;
    });
}
size_t relocation_table::size() const {
    return items.size();
}

bool relocation_table::has_item(uint32_t rva) const {

    for (size_t item_idx = 0; item_idx < items.size(); item_idx++) {

        if (items[item_idx].relative_virtual_address == rva) {
            return true;
        }
    }

    return false;
}

bool relocation_table::has_item_id(uint32_t relocation_id) const {

    for (size_t item_idx = 0; item_idx < items.size(); item_idx++) {

        if (items[item_idx].relocation_id == relocation_id) {
            return true;
        }
    }

    return false;
}

bool relocation_table::get_item_id(uint32_t rva, uint32_t& relocation_id) const {

    for (size_t item_idx = 0; item_idx < items.size(); item_idx++) {

        if (items[item_idx].relative_virtual_address == rva) {
            relocation_id = items[item_idx].relocation_id;
            return true;
        }
    }
    return false;
}

const std::vector<relocation_item>& relocation_table::get_items() const {
    return items;
}

std::vector<relocation_item>& relocation_table::get_items() {
    return items;
}


directory_code get_relocation_table(const pe_image &image, relocation_table& relocs) {
	relocs.clear();
	
    uint32_t virtual_address	= image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_BASERELOC);
    uint32_t virtual_size		= image.get_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_BASERELOC);

	if (virtual_address && virtual_size) {
        pe_image_io reloc_io(image);
        reloc_io.set_image_offset(virtual_address);


        while (reloc_io.get_image_offset() < virtual_address + virtual_size) {
            image_base_relocation reloc_base;

            if (reloc_io.read(&reloc_base,sizeof(reloc_base)) != enma_io_success) {
                return directory_code::directory_code_currupted;
            }

            uint32_t current_block_size = sizeof(image_base_relocation);

            relocs.get_items().reserve(reloc_base.size_of_block / sizeof(uint16_t)); 
            while ( (reloc_io.get_image_offset() < virtual_address + virtual_size) &&
                (current_block_size < reloc_base.size_of_block)) {
                uint16_t rel;

                if (reloc_io.read(&rel, sizeof(rel)) != enma_io_success) {
                    return directory_code::directory_code_currupted;
                }

                if (rel >> 12 == IMAGE_REL_BASED_ABSOLUTE) { current_block_size += sizeof(uint16_t);  continue; }

                relocs.add_item((rel & 0x0FFF) | reloc_base.virtual_address, 0);
                current_block_size += sizeof(uint16_t);
            }
        }
        return directory_code::directory_code_success;
	}

	return directory_code::directory_code_not_present;
}

bool build_relocation_table(pe_image &image, pe_section& section, relocation_table& relocs) {


    if (relocs.get_items().size()) {
        pe_section_io reloc_section = pe_section_io(section, image, enma_io_mode::enma_io_mode_allow_expand);
        reloc_section.align_up(0x10).seek_to_end();

        uint32_t virtual_address = reloc_section.get_section_offset();
        uint16_t reloc_type;
        
        if (image.is_x32_image()) {
            reloc_type = IMAGE_REL_BASED_HIGHLOW << 12;
        }
        else {
            reloc_type = IMAGE_REL_BASED_DIR64 << 12;
        }

        relocs.sort();

        std::vector<std::vector<uint32_t>> reloc_tables;

        uint32_t reloc_last_hi = 0;
        for (auto& reloc : relocs.get_items()) {
            if (!reloc_tables.size() ||
                reloc_last_hi != (reloc.relative_virtual_address & 0xFFFFF000)) {

                reloc_tables.push_back(std::vector<uint32_t>());
                reloc_last_hi = reloc.relative_virtual_address & 0xFFFFF000;
            }

            reloc_tables[reloc_tables.size() - 1].push_back(reloc.relative_virtual_address);
        }

        for (auto& reloc_table : reloc_tables) {
            image_base_relocation reloc_base = { 
                reloc_table[0] & 0xFFFFF000 ,
                uint32_t(sizeof(image_base_relocation) + reloc_table.size()*sizeof(uint16_t) + (reloc_table.size() & 1 ? sizeof(uint16_t) : 0))
            };

            if (reloc_section.write(&reloc_base, sizeof(reloc_base)) != enma_io_success) { return false; }

            for (auto& reloc : reloc_table) {
                uint16_t reloc_ = reloc & 0xFFF | reloc_type;
                if (reloc_section.write(&reloc_, sizeof(reloc_)) != enma_io_success) { return false; }
            }

            if (reloc_table.size()&1) { //parity align
                uint16_t reloc_ = 0;
                if (reloc_section.write(&reloc_, sizeof(reloc_)) != enma_io_success) { return false; }
            }

        }

        image.set_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_BASERELOC, virtual_address);
        image.set_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_BASERELOC, reloc_section.get_section_offset() - virtual_address);
    }
    else {
        image.set_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_BASERELOC, 0);
        image.set_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_BASERELOC, 0);
    }

    return true;
}


directory_code get_placement_relocation_table(const pe_image &image, std::vector<directory_placement>& placement) {

    uint32_t virtual_address  = image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_BASERELOC);
    uint32_t virtual_size	  = image.get_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_BASERELOC);

	if (virtual_address && virtual_size) {
	
        pe_image_io reloc_io(image);

        uint32_t _offset_real = 0;
        uint32_t available_size = 0;
        uint32_t down_oversize = 0;
        uint32_t up_oversize = 0;

        reloc_io.view_image(
            virtual_address, ALIGN_UP(virtual_size,0x10),
            _offset_real,
            available_size, down_oversize, up_oversize
        );

        placement.push_back({ virtual_address ,available_size ,dp_id_relocations_desc });

        if (!down_oversize && !up_oversize) {
            return directory_code::directory_code_success;
        }

        return directory_code::directory_code_currupted;
	}

	return directory_code::directory_code_not_present;
}
