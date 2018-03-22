#include "stdafx.h"
#include "pe_debug.h"

debug_item::debug_item() {
    this->characteristics   = 0;
    this->timestamp         = 0;
    this->major_version     = 0;
    this->minor_version     = 0;
    this->type              = 0;
    this->size_of_data      = 0;
    this->address_of_raw_data = 0;
    this->pointer_to_raw_data = 0;
}

debug_item::debug_item(DWORD   characteristics, DWORD   timestamp, WORD    major_version, WORD    minor_version,
    DWORD   type, DWORD   size_of_data, DWORD   address_of_raw_data, DWORD   pointer_to_raw_data,
    void * data) {

    this->characteristics = characteristics;
    this->timestamp = timestamp;
    this->major_version = major_version;
    this->minor_version = minor_version;
    this->type = type;
    this->size_of_data = size_of_data;
    this->address_of_raw_data = address_of_raw_data;
    this->pointer_to_raw_data = pointer_to_raw_data;

    item_data.resize(this->size_of_data);
    memcpy(item_data.data(), data, this->size_of_data);
}


debug_item::~debug_item() {

}
  
debug_item& debug_item::operator=(const debug_item& item) {
    this->characteristics = item.characteristics;
    this->timestamp = item.timestamp;
    this->major_version = item.major_version;
    this->minor_version = item.minor_version;
    this->type = item.type;
    this->size_of_data = item.size_of_data;
    this->address_of_raw_data = item.address_of_raw_data;
    this->pointer_to_raw_data = item.pointer_to_raw_data;
    this->item_data = item.item_data;
    return *this;
}

void  debug_item::set_characteristics(DWORD characteristics) {
    this->characteristics = characteristics;
}
void  debug_item::set_timestamp(DWORD timestamp) {
    this->timestamp = timestamp;
}
void  debug_item::set_major_version(WORD major_version){
    this->major_version = major_version;
}
void  debug_item::set_minor_version(WORD minor_version) {
    this->minor_version = minor_version;
}
void  debug_item::set_type(DWORD type) {
    this->type = type;
}
void  debug_item::set_size_of_data(DWORD size_of_data) {
    this->size_of_data = size_of_data;
}
void  debug_item::set_address_of_raw_data(DWORD address_of_raw_data) {
    this->address_of_raw_data = address_of_raw_data;
}
void  debug_item::set_pointer_to_raw_data(DWORD pointer_to_raw_data) {
    this->pointer_to_raw_data = pointer_to_raw_data;
}

DWORD  debug_item::get_characteristics() const {
    return this->characteristics;
}
DWORD  debug_item::get_timestamp() const {
    return this->timestamp;
}
WORD   debug_item::get_major_version() const{
    return this->major_version;
}
WORD   debug_item::get_minor_version() const {
    return this->minor_version;
}
DWORD  debug_item::get_type() const {
    return this->type;
}
DWORD  debug_item::get_size_of_data() const {
    return this->size_of_data;
}
DWORD  debug_item::get_address_of_raw_data() const {
    return this->address_of_raw_data;
}
DWORD  debug_item::get_pointer_to_raw_data() const {
    return this->pointer_to_raw_data;
}
std::vector<BYTE> debug_item::get_item_data() {
    return this->item_data;
}

debug_table::debug_table() {

}
debug_table::~debug_table() {

}
void debug_table::add_item(const debug_item& item) {
    items.push_back(item);
}
std::vector<debug_item>& debug_table::get_items() {
    return items;
}


bool get_debug_table(const pe_image &image, debug_table& debug) {
    debug.get_items().clear();

    DWORD virtual_address = image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_DEBUG);
    DWORD virtual_size = image.get_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_DEBUG);

    if (virtual_address) {
      	pe_section * debug_section = image.get_section_by_rva(virtual_address);

        if (debug_section) {
            for (unsigned int item_offset = 0; item_offset < virtual_size; item_offset += sizeof(IMAGE_DEBUG_DIRECTORY)) {
                PIMAGE_DEBUG_DIRECTORY p_debug_item = PIMAGE_DEBUG_DIRECTORY(&debug_section->get_section_data().data()[
                    virtual_address - debug_section->get_virtual_address() + item_offset
                ]);

                if (p_debug_item->SizeOfData) {
                    pe_section * debug_item_section = image.get_section_by_raw(p_debug_item->PointerToRawData);

                    if (debug_item_section) {
                        debug.add_item(debug_item(p_debug_item->Characteristics, p_debug_item->TimeDateStamp, p_debug_item->MajorVersion,
                            p_debug_item->MinorVersion, p_debug_item->Type, p_debug_item->SizeOfData, p_debug_item->AddressOfRawData,
                            p_debug_item->PointerToRawData, 
                            &debug_item_section->get_section_data().data()[
                                p_debug_item->PointerToRawData - debug_item_section->get_pointer_to_raw()
                            ]
                        ));
                    }
                }
            }
            return true;
        }
    }

    return false;
}

bool erase_debug_table(pe_image &image, std::vector<erased_zone>* zones) {

	DWORD virtual_address = image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_DEBUG);
	DWORD virtual_size = image.get_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_DEBUG);


	if (virtual_address && virtual_size) {
		pe_section * debug_section = image.get_section_by_rva(virtual_address);

		if (debug_section) {
			for (unsigned int item_offset = 0; item_offset < virtual_size; item_offset += sizeof(IMAGE_DEBUG_DIRECTORY)) {
				PIMAGE_DEBUG_DIRECTORY p_debug_item = PIMAGE_DEBUG_DIRECTORY(&debug_section->get_section_data().data()[
                    virtual_address - debug_section->get_virtual_address() + item_offset
                ]);

				if (p_debug_item->SizeOfData) {
					pe_section * debug_item_section = image.get_section_by_raw(p_debug_item->PointerToRawData);

					if (debug_item_section) {
						ZeroMemory(&debug_item_section->get_section_data().data()[
                            p_debug_item->PointerToRawData - debug_item_section->get_pointer_to_raw()
                        ], p_debug_item->SizeOfData);
					
						if (zones) {
							zones->push_back({ p_debug_item->AddressOfRawData ,p_debug_item->SizeOfData});
						}
					}
				}

                if (zones) {
                    zones->push_back({
                        debug_section->get_virtual_address() + item_offset ,
                        sizeof(IMAGE_DEBUG_DIRECTORY) + 4
                    });
                }

                ZeroMemory(p_debug_item,sizeof(IMAGE_DEBUG_DIRECTORY));
			}

			image.set_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_DEBUG, 0);
			image.set_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_DEBUG, 0);
			return true;
		}	
	}

	return false;
}