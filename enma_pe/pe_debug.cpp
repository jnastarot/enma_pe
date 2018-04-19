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

debug_item::debug_item(const debug_item& item) {
    this->operator=(item);
}

debug_item::debug_item(uint32_t   characteristics, uint32_t   timestamp, uint16_t    major_version, uint16_t    minor_version,
    uint32_t   type, uint32_t   size_of_data, uint32_t   address_of_raw_data, uint32_t   pointer_to_raw_data,
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

void  debug_item::set_characteristics(uint32_t characteristics) {
    this->characteristics = characteristics;
}
void  debug_item::set_timestamp(uint32_t timestamp) {
    this->timestamp = timestamp;
}
void  debug_item::set_major_version(uint16_t major_version){
    this->major_version = major_version;
}
void  debug_item::set_minor_version(uint16_t minor_version) {
    this->minor_version = minor_version;
}
void  debug_item::set_type(uint32_t type) {
    this->type = type;
}
void  debug_item::set_size_of_data(uint32_t size_of_data) {
    this->size_of_data = size_of_data;
}
void  debug_item::set_address_of_raw_data(uint32_t address_of_raw_data) {
    this->address_of_raw_data = address_of_raw_data;
}
void  debug_item::set_pointer_to_raw_data(uint32_t pointer_to_raw_data) {
    this->pointer_to_raw_data = pointer_to_raw_data;
}

uint32_t  debug_item::get_characteristics() const {
    return this->characteristics;
}
uint32_t  debug_item::get_timestamp() const {
    return this->timestamp;
}
uint16_t   debug_item::get_major_version() const{
    return this->major_version;
}
uint16_t   debug_item::get_minor_version() const {
    return this->minor_version;
}
uint32_t  debug_item::get_type() const {
    return this->type;
}
uint32_t  debug_item::get_size_of_data() const {
    return this->size_of_data;
}
uint32_t  debug_item::get_address_of_raw_data() const {
    return this->address_of_raw_data;
}
uint32_t  debug_item::get_pointer_to_raw_data() const {
    return this->pointer_to_raw_data;
}
std::vector<uint8_t>& debug_item::get_item_data() {
    return this->item_data;
}

debug_table::debug_table() {

}
debug_table::debug_table(const debug_table& debug) {
    this->operator=(debug);
}
debug_table::~debug_table() {

}

debug_table& debug_table::operator=(const debug_table& debug) {
    this->items = debug.items;
    return *this;
}

void debug_table::add_item(const debug_item& item) {
    items.push_back(item);
}
std::vector<debug_item>& debug_table::get_items() {
    return items;
}


directory_code get_debug_table(const pe_image &image, debug_table& debug) {
    debug.get_items().clear();

    uint32_t virtual_address = image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_DEBUG);
    uint32_t virtual_size = image.get_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_DEBUG);

    if (virtual_address && virtual_size) {
        pe_image_io debug_io(image);

        debug_io.set_image_offset(virtual_address);
        while (debug_io.get_image_offset() < virtual_address + virtual_size) {
            image_debug_directory debug_desc;

            if (debug_io.read(&debug_desc,sizeof(debug_desc)) != enma_io_success) {
                return directory_code::directory_code_currupted;
            }

            debug_item item;
            item.set_characteristics(debug_desc.characteristics);
            item.set_timestamp(debug_desc.time_date_stamp);
            item.set_major_version(debug_desc.major_version);
            item.set_minor_version(debug_desc.minor_version);
            item.set_type(debug_desc.type);
            item.set_size_of_data(debug_desc.size_of_data);
            item.set_address_of_raw_data(debug_desc.address_of_raw_data);
            item.set_pointer_to_raw_data(debug_desc.pointer_to_raw_data);

           
            if (debug_desc.size_of_data && debug_desc.address_of_raw_data) {
                uint32_t _offset_real = 0;
                uint32_t available_size = 0;
                uint32_t down_oversize = 0;
                uint32_t up_oversize = 0;

                debug_io.view_image(
                    debug_desc.address_of_raw_data, debug_desc.size_of_data,
                    _offset_real,
                    available_size, down_oversize, up_oversize
                );

                if (down_oversize || up_oversize) {
                    return directory_code::directory_code_currupted;
                }

                if (pe_image_io(image).set_image_offset(debug_desc.address_of_raw_data).read(
                    item.get_item_data(), debug_desc.size_of_data) != enma_io_success) {
                    return directory_code::directory_code_currupted;
                }
            }

            debug.add_item(item);
        }

        return directory_code::directory_code_success;
    }

    return directory_code::directory_code_not_present;
}

directory_code get_placement_debug_table(pe_image &image, std::vector<directory_placement>& placement) {

    uint32_t virtual_address = image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_DEBUG);
    uint32_t virtual_size = image.get_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_DEBUG);

    if (virtual_address && virtual_size) {
        pe_image_io debug_io(image);

        debug_io.set_image_offset(virtual_address);
        while (debug_io.get_image_offset() < virtual_address + virtual_size) {
            placement.push_back({ debug_io.get_image_offset(),
                ALIGN_UP(sizeof(image_debug_directory),0x10),dp_id_debug_desc });

            image_debug_directory debug_desc;

            if (debug_io.read(&debug_desc, sizeof(debug_desc)) != enma_io_success) {
                return directory_code::directory_code_currupted;
            }

            if (debug_desc.size_of_data && debug_desc.address_of_raw_data) {
                uint32_t _offset_real = 0;
                uint32_t available_size = 0;
                uint32_t down_oversize = 0;
                uint32_t up_oversize = 0;

                debug_io.view_image(
                    debug_desc.address_of_raw_data, debug_desc.size_of_data,
                    _offset_real,
                    available_size, down_oversize, up_oversize
                );

                if (down_oversize || up_oversize) {
                    return directory_code::directory_code_currupted;
                }
                placement.push_back({ debug_desc.address_of_raw_data ,ALIGN_UP(debug_desc.size_of_data,0x10) , dp_id_debug_item_data });
            }
        }

        return directory_code::directory_code_success;
    }

	return directory_code::directory_code_not_present;
}