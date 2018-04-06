
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
std::vector<exceptions_item>& exceptions_table::get_items() {
    return items;
}


bool get_exception_table(const pe_image &image, exceptions_table& exceptions) {
    exceptions.get_items().clear();

    uint32_t virtual_address = image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_EXCEPTION);
    uint32_t virtual_size = image.get_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_EXCEPTION);

    if (virtual_address && virtual_size) {
        pe_section * except_section = image.get_section_by_rva(virtual_address);

        for (size_t used_size = 0; used_size < virtual_size; used_size += sizeof(image_ia64_runtime_function_entry)) {

            image_ia64_runtime_function_entry* entry = (image_ia64_runtime_function_entry*)&except_section->get_section_data().data()[
                (virtual_address + used_size) - except_section->get_virtual_address()
            ];

            if (!entry->begin_address && !entry->end_address && !entry->unwind_info_address) { continue; }

            exceptions.add_item(entry->begin_address, entry->end_address, entry->unwind_info_address);
        }

        return true;
    }

	return false;
}

void build_exceptions_table(pe_image &image, pe_section& section, exceptions_table& exceptions) {

    if (!exceptions.get_items().size()) { return; }

    if (section.get_size_of_raw_data() & 0xF) {
        section.get_section_data().resize(
            section.get_section_data().size() + (0x10 - (section.get_section_data().size() & 0xF))
        );
    }

    uint32_t virtual_address = section.get_virtual_address() + section.get_size_of_raw_data();
    uint32_t p_exceptions_offset = section.get_size_of_raw_data();


    for (auto & item : exceptions.get_items()) {     
        image_ia64_runtime_function_entry entry = { item.get_begin_address(), item.get_end_address(), item.get_unwind_data_address()};

        section.get_section_data().resize(section.get_section_data().size() + sizeof(image_ia64_runtime_function_entry));
        memcpy(section.get_section_data().data() + section.get_section_data().size() - sizeof(image_ia64_runtime_function_entry),
            &entry,
            sizeof(image_ia64_runtime_function_entry));
    }


    image.set_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_EXCEPTION, virtual_address);
    image.set_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_EXCEPTION, section.get_size_of_raw_data() - p_exceptions_offset);
}

bool get_placement_exceptions_table(pe_image &image, std::vector<directory_placement>& placement) {

    uint32_t virtual_address = image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_EXCEPTION);
    uint32_t virtual_size    = image.get_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_EXCEPTION);

	if (virtual_address && virtual_size) {
		pe_section * exception_section = image.get_section_by_rva(virtual_address);

		if (exception_section) {
            if (ALIGN_UP(exception_section->get_virtual_size(), image.get_section_align()) >= virtual_size) {

                placement.push_back({ virtual_address ,virtual_size, dp_id_exceptions_desc });
				return true;
			}
		}
	}

	return false;
}