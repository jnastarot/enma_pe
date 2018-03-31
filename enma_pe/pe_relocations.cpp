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

void relocation_table::add_item(DWORD rva, DWORD relocation_id) {
   items.push_back({ rva , relocation_id,0});
}

bool relocation_table::erase_item(DWORD rva) {

    for (unsigned int item_idx = 0; item_idx < items.size(); item_idx++) {
        
        if (items[item_idx].relative_virtual_address == rva) {
            items.erase(items.begin() + item_idx);
            return true;
        }
    }

    return false;
}

bool relocation_table::erase_first_item_by_id(DWORD relocation_id) {

    for (unsigned int item_idx = 0; item_idx < items.size(); item_idx++) {

        if (items[item_idx].relocation_id == relocation_id) {
            items.erase(items.begin() + item_idx);
            return true;
        }
    }

    return false;
}

bool relocation_table::erase_all_items_by_id(DWORD relocation_id) {

    bool ret = false;

    for (unsigned int item_idx = 0; item_idx < items.size(); item_idx++) {

        if (items[item_idx].relocation_id == relocation_id) {
            items.erase(items.begin() + item_idx);
            item_idx--;
            ret = true;
        }
    }

    return ret;
}

void relocation_table::get_items_by_relocation_id(std::vector<relocation_item*>& found_relocs, DWORD relocation_id) {

    found_relocs.clear();

    for (unsigned int item_idx = 0; item_idx < items.size(); item_idx++) {
        if (items[item_idx].relocation_id == relocation_id) {
            found_relocs.push_back(&items[item_idx]);
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
unsigned int relocation_table::size() const {
    return items.size();
}

bool relocation_table::has_item(DWORD rva) const {

    for (unsigned int item_idx = 0; item_idx < items.size(); item_idx++) {

        if (items[item_idx].relative_virtual_address == rva) {
            return true;
        }
    }

    return false;
}

bool relocation_table::has_item_id(DWORD relocation_id) const {

    for (unsigned int item_idx = 0; item_idx < items.size(); item_idx++) {

        if (items[item_idx].relocation_id == relocation_id) {
            return true;
        }
    }

    return false;
}

bool relocation_table::get_item_id(DWORD rva, DWORD& relocation_id) const {

    for (unsigned int item_idx = 0; item_idx < items.size(); item_idx++) {

        if (items[item_idx].relative_virtual_address == rva) {
            relocation_id = items[item_idx].relocation_id;
            return true;
        }
    }
    return false;
}

std::vector<relocation_item>& relocation_table::get_items() {
    return items;
}


bool get_relocation_table(const pe_image &image, relocation_table& relocs) {
	relocs.clear();
	
	DWORD virtual_address	= image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_BASERELOC);
	DWORD virtual_size		= image.get_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_BASERELOC);

	if (virtual_address && virtual_size) {
		pe_section * reloc_section = image.get_section_by_rva(virtual_address);

		if (reloc_section && reloc_section->get_size_of_raw_data() >= virtual_size) {
			BYTE * reloc_raw = &reloc_section->get_section_data().data()[(DWORD)(virtual_address - reloc_section->get_virtual_address())];

			for (DWORD s_offset = 0; s_offset < virtual_size;) {
				PIMAGE_BASE_RELOCATION reloc_base = PIMAGE_BASE_RELOCATION(&reloc_raw[s_offset]);
				s_offset += sizeof(IMAGE_BASE_RELOCATION);

                relocs.get_items().reserve(reloc_base->SizeOfBlock/sizeof(WORD));

				for (DWORD block_offset = sizeof(IMAGE_BASE_RELOCATION); s_offset < virtual_size && block_offset < reloc_base->SizeOfBlock; block_offset += sizeof(WORD), s_offset += sizeof(WORD)) {
					WORD rel = *(WORD*)&reloc_raw[s_offset];
					if (rel >> 12 == IMAGE_REL_BASED_ABSOLUTE)continue;

                    relocs.get_items().push_back({ (rel & 0x0FFF) | reloc_base->VirtualAddress,0 ,0});
				}

			}

			return true;
		}
	}

	return false;
}

void build_relocation_table(pe_image &image, pe_section& section, relocation_table& relocs) {

    if (section.get_size_of_raw_data() & 0xF) {
        section.get_section_data().resize(
            section.get_section_data().size() + (0x10 - (section.get_section_data().size() & 0xF))
        );
    }


	IMAGE_BASE_RELOCATION reloc_decs;
	std::vector<BYTE> data_buffer;
	std::vector<WORD> reloc_block;
	DWORD virtual_address = section.get_virtual_address() + section.get_size_of_raw_data();
	DWORD reloc_type;
	unsigned int raw_idxer = 0;

	if (image.is_x32_image()){
		reloc_type = IMAGE_REL_BASED_HIGHLOW << 12;
	}
	else{
		reloc_type = IMAGE_REL_BASED_DIR64 << 12;
	}


    relocs.sort();

	for (unsigned int r_i = 0; r_i < relocs.size(); r_i++) {

		if (!r_i) { reloc_decs.VirtualAddress = (relocs.get_items()[r_i].relative_virtual_address & 0xFFFFF000); }

	loop_n:
		if (reloc_decs.VirtualAddress == (relocs.get_items()[r_i].relative_virtual_address & 0xFFFFF000)) {
			reloc_block.push_back( WORD((relocs.get_items()[r_i].relative_virtual_address & 0xFFF) | reloc_type));
		}
		else {
			if (reloc_block.size() & 1) { reloc_block.push_back(0); }//align of parity
			data_buffer.resize(data_buffer.size() + sizeof(IMAGE_BASE_RELOCATION) + reloc_block.size() * sizeof(WORD));
			reloc_decs.SizeOfBlock = sizeof(IMAGE_BASE_RELOCATION) + reloc_block.size() * sizeof(WORD);

			memcpy(&data_buffer.data()[raw_idxer], (BYTE*)&reloc_decs, sizeof(IMAGE_BASE_RELOCATION)); raw_idxer += sizeof(IMAGE_BASE_RELOCATION);
			memcpy(&data_buffer.data()[raw_idxer], reloc_block.data(), reloc_block.size() * sizeof(WORD)); raw_idxer += reloc_block.size() * sizeof(WORD);

			reloc_block.clear();
			reloc_decs.VirtualAddress = (relocs.get_items()[r_i].relative_virtual_address & 0xFFFFF000);
			if (r_i == relocs.size())break;
			goto loop_n;
		}
	}

	if (reloc_block.size()) {
		if (reloc_block.size() & 1) { reloc_block.push_back(0); }//align of parity
		data_buffer.resize(data_buffer.size() + sizeof(IMAGE_BASE_RELOCATION) + reloc_block.size() * sizeof(WORD));
		reloc_decs.SizeOfBlock = sizeof(IMAGE_BASE_RELOCATION) + reloc_block.size() * sizeof(WORD);

		memcpy(&data_buffer.data()[raw_idxer], (BYTE*)&reloc_decs, sizeof(IMAGE_BASE_RELOCATION)); raw_idxer += sizeof(IMAGE_BASE_RELOCATION);
		memcpy(&data_buffer.data()[raw_idxer], reloc_block.data(), reloc_block.size() * sizeof(WORD)); raw_idxer += reloc_block.size() * sizeof(WORD);

		reloc_block.clear();
	}


	if (image.set_data_by_rva(&section, virtual_address, data_buffer.data(), data_buffer.size())) {
		image.set_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_BASERELOC, virtual_address);
		image.set_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_BASERELOC, data_buffer.size());
	}
}


bool erase_relocation_table(pe_image &image, std::vector<erased_zone>* zones) {

	DWORD virtual_address = image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_BASERELOC);
	DWORD virtual_size	  = image.get_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_BASERELOC);

	if (virtual_address && virtual_size) {
		

		pe_section * reloc_section = image.get_section_by_rva(virtual_address);

		if (reloc_section) {

			BYTE * reloc_raw = &reloc_section->get_section_data().data()[(DWORD)(virtual_address - reloc_section->get_virtual_address())];
			if (reloc_section->get_size_of_raw_data() >= virtual_size) {

				ZeroMemory(reloc_raw, virtual_size);

				if (zones) {
					zones->push_back({
						virtual_address ,
						virtual_size
					});
				}

				image.set_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_BASERELOC, 0);
				image.set_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_BASERELOC, 0);
				return true;
			}
		}
	}

	return false;
}
