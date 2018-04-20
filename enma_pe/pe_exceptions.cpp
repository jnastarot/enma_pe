
#include "stdafx.h"
#include "pe_exceptions.h"


exceptions_item::exceptions_item() :
    address_begin(0), address_end(0), address_unwind_data(0) {}

exceptions_item::exceptions_item(const exceptions_item& item) {
    this->operator=(item);
}
exceptions_item::exceptions_item(uint32_t address_begin, uint32_t address_end, uint32_t address_unwind_data):
    address_begin(address_begin), address_end(address_end), address_unwind_data(address_unwind_data){}

exceptions_item::~exceptions_item() {
}

exceptions_item& exceptions_item::operator=(const exceptions_item& item) {

    this->address_begin = item.address_begin;
    this->address_end = item.address_end;
    this->address_unwind_data = item.address_unwind_data;

    return *this;
}

void exceptions_item::set_begin_address(uint32_t rva_address) {
    this->address_begin = rva_address;
}
void exceptions_item::set_end_address(uint32_t rva_address) {
    this->address_end = rva_address;
}
void exceptions_item::set_unwind_data_address(uint32_t rva_address) {
    this->address_unwind_data = rva_address;
}

uint32_t exceptions_item::get_begin_address() const {
    return address_begin;
}
uint32_t exceptions_item::get_end_address() const {
    return address_end;
}
uint32_t exceptions_item::get_unwind_data_address() const {
    return address_unwind_data;
}



exceptions_table::exceptions_table() {

}
exceptions_table::exceptions_table(const exceptions_table& exceptions) {
    this->operator=(exceptions);
}
exceptions_table::~exceptions_table() {

}

exceptions_table& exceptions_table::operator=(const exceptions_table& exceptions) {
    this->items = exceptions.items;

    return *this;
}

void exceptions_table::add_item(uint32_t address_begin, uint32_t address_end, uint32_t address_unwind_data) {
    items.push_back(exceptions_item(address_begin, address_end, address_unwind_data));
}
void exceptions_table::add_item(const exceptions_item& item) {
    items.push_back(item);
}
void exceptions_table::add_item(const image_ia64_runtime_function_entry& exc_entry) {
    items.push_back(exceptions_item(exc_entry.begin_address, exc_entry.end_address, exc_entry.unwind_info_address));
}

size_t exceptions_table::size() const {
    return items.size();
}

std::vector<exceptions_item>& exceptions_table::get_items() {
    return items;
}


directory_code get_exception_table(const pe_image &image, exceptions_table& exceptions) {
    exceptions.get_items().clear();

    uint32_t virtual_address = image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_EXCEPTION);
    uint32_t virtual_size    = image.get_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_EXCEPTION);

    if (virtual_address && virtual_size /*req size*/) {
        pe_image_io exception_io(image);
        exception_io.set_image_offset(virtual_address);

        while (exception_io.get_image_offset() < virtual_address + virtual_size) {
            image_ia64_runtime_function_entry exc_entry;

            if (exception_io.read(&exc_entry, sizeof(exc_entry)) != enma_io_code::enma_io_success) {

                return directory_code::directory_code_currupted;
            }

            exceptions.add_item(exc_entry);
        }

        return directory_code::directory_code_success;
    }

	return directory_code::directory_code_not_present;
}

bool build_exceptions_table(pe_image &image, pe_section& section, exceptions_table& exceptions) {


    if (exceptions.get_items().size()) {
        pe_section_io exc_section(section, image, enma_io_mode_allow_expand);
        exc_section.seek_to_end().align_up(0x10);

        uint32_t exc_virtual_address = exc_section.get_section_offset();

        for (auto & item : exceptions.get_items()) {
            image_ia64_runtime_function_entry entry = { item.get_begin_address(), item.get_end_address(), item.get_unwind_data_address() };

            if (exc_section.write(&entry, sizeof(entry)) != enma_io_success) {
                return false;
            }
        }

        image.set_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_EXCEPTION, exc_virtual_address);
        image.set_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_EXCEPTION, 
            exceptions.get_items().size() * sizeof(image_ia64_runtime_function_entry));
    }
    else {

        image.set_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_EXCEPTION, 0);
        image.set_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_EXCEPTION, 0);
    }  

    return true;
}

directory_code get_placement_exceptions_table(const pe_image &image, std::vector<directory_placement>& placement) {

    uint32_t virtual_address = image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_EXCEPTION);
    uint32_t virtual_size    = image.get_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_EXCEPTION);

	if (virtual_address && virtual_size) {
        pe_image_io exception_io(image);

        uint32_t _offset_real   = 0;
        uint32_t available_size = 0;
        uint32_t down_oversize  = 0;
        uint32_t up_oversize    = 0;

        exception_io.view_image(
            virtual_address, ALIGN_UP(virtual_size,0x10),
            _offset_real,
            available_size, down_oversize, up_oversize
        );

        placement.push_back({ virtual_address ,available_size, dp_id_exceptions_desc });

        if (!down_oversize && !up_oversize) {          
            return directory_code::directory_code_success;
        }

        return directory_code::directory_code_currupted;
	}

	return directory_code::directory_code_not_present;
}