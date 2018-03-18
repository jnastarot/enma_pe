
#include "stdafx.h"
#include "pe_exceptions.h"

exceptions_item::exceptions_item(DWORD address_begin, DWORD address_end, DWORD address_unwind_data):
    address_begin(address_begin), address_end(address_end), address_unwind_data(address_unwind_data){}

exceptions_item::~exceptions_item() {
}

void exceptions_item::set_begin_address(DWORD rva_address) {
    this->address_begin = address_begin;
}
void exceptions_item::set_end_address(DWORD rva_address) {
    this->address_end = address_end;
}
void exceptions_item::set_unwind_data_address(DWORD rva_address) {
    this->address_unwind_data = rva_address;
}

DWORD exceptions_item::get_begin_address() const {
    return address_begin;
}
DWORD exceptions_item::get_end_address() const {
    return address_end;
}
DWORD exceptions_item::get_unwind_data_address() const {
    return address_unwind_data;
}



exceptions_table::exceptions_table() {

}
exceptions_table::~exceptions_table() {

}
void exceptions_table::add_item(DWORD address_begin, DWORD address_end, DWORD address_unwind_data) {
    items.push_back(exceptions_item(address_begin, address_end, address_unwind_data));
}
void exceptions_table::add_item(exceptions_item& item) {
    items.push_back(item);
}
std::vector<exceptions_item>& exceptions_table::get_items() {
    return items;
}


bool get_exception_table(const pe_image &image, exceptions_table& exceptions) {
    exceptions.get_items().clear();

    DWORD virtual_address = image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_EXCEPTION);
    DWORD virtual_size = image.get_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_EXCEPTION);

    if (virtual_address && virtual_size) {
        pe_section * except_section = image.get_section_by_rva(virtual_address);

        for (DWORD used_size = 0; used_size < virtual_size; used_size += sizeof(IMAGE_IA64_RUNTIME_FUNCTION_ENTRY)) {

            PIMAGE_IA64_RUNTIME_FUNCTION_ENTRY entry = (PIMAGE_IA64_RUNTIME_FUNCTION_ENTRY)&except_section->get_section_data().data()[
                (virtual_address + used_size) - except_section->get_virtual_address()
            ];

            if (!entry->BeginAddress && !entry->EndAddress && !entry->UnwindInfoAddress) { continue; }

            exceptions.add_item(entry->BeginAddress, entry->EndAddress, entry->UnwindInfoAddress);
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

    DWORD virtual_address = section.get_virtual_address() + section.get_size_of_raw_data();
    DWORD p_exceptions_offset = section.get_size_of_raw_data();


    for (auto & item : exceptions.get_items()) {     
        IMAGE_IA64_RUNTIME_FUNCTION_ENTRY entry = { item.get_begin_address(), item.get_end_address(), item.get_unwind_data_address()};

        section.get_section_data().resize(section.get_section_data().size() + sizeof(IMAGE_IA64_RUNTIME_FUNCTION_ENTRY));
        memcpy(section.get_section_data().data() + section.get_section_data().size() - sizeof(IMAGE_IA64_RUNTIME_FUNCTION_ENTRY),
            &entry,
            sizeof(IMAGE_IA64_RUNTIME_FUNCTION_ENTRY));
    }


    image.set_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_EXCEPTION, virtual_address);
    image.set_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_EXCEPTION, section.get_size_of_raw_data() - p_exceptions_offset);
}

bool erase_exceptions_table(pe_image &image, std::vector<erased_zone>* zones) {

	DWORD virtual_address = image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_EXCEPTION);
	DWORD virtual_size = image.get_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_EXCEPTION);

	if (virtual_address && virtual_size) {
		pe_section * except_section = image.get_section_by_rva(virtual_address);

		if (except_section) {
		
			BYTE * except_raw = &except_section->get_section_data().data()[(DWORD)(virtual_address - except_section->get_virtual_address())];
			if (except_section->get_size_of_raw_data() >= virtual_size) {
                if (zones) { zones->push_back({ virtual_address ,virtual_size }); }
				ZeroMemory(except_raw, virtual_size);
                image.set_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_EXCEPTION, 0);
                image.set_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_EXCEPTION, 0);
				return true;
			}
		}
	}

	return false;

}