#include "stdafx.h"
#include "pe_delay_import.h"

delay_imported_library::delay_imported_library() {
    attributes = 0;
    timestamp = 0;
    module_handle_rva = 0;
    relative_virtual_address_to_iat = 0;
    relative_virtual_address_to_bound_iat = 0;
    relative_virtual_address_to_unload_info_table = 0;
}
delay_imported_library::~delay_imported_library() {

}
delay_imported_library& delay_imported_library::operator=(const delay_imported_library& lib) {
    
    this->name              = lib.name;
    this->attributes        = lib.attributes;
    this->timestamp         = lib.timestamp;
    this->module_handle_rva = lib.module_handle_rva;
    this->relative_virtual_address_to_iat               = lib.relative_virtual_address_to_iat;
    this->relative_virtual_address_to_bound_iat         = lib.relative_virtual_address_to_bound_iat;
    this->relative_virtual_address_to_unload_info_table = lib.relative_virtual_address_to_unload_info_table;

    this->imported_items = lib.imported_items;

    return *this;
}

void delay_imported_library::set_name(std::string name) {
    this->name = name;
}
void delay_imported_library::set_attributes(DWORD attributes) {
    this->attributes = attributes;
}
void delay_imported_library::set_timestamp(DWORD timestamp) {
    this->timestamp = timestamp;
}
void delay_imported_library::set_rva_module_handle(DWORD relative_virtual_address) {
    this->module_handle_rva = relative_virtual_address;
}
void delay_imported_library::set_rva_iat(DWORD relative_virtual_address) {
    this->relative_virtual_address_to_iat = relative_virtual_address;
}
void delay_imported_library::set_rva_bound_iat(DWORD relative_virtual_address) {
    this->relative_virtual_address_to_bound_iat = relative_virtual_address;
}
void delay_imported_library::set_rva_unload_info_table(DWORD relative_virtual_address) {
    this->relative_virtual_address_to_unload_info_table = relative_virtual_address;
}
void delay_imported_library::add_item(imported_func& item) {
    imported_items.push_back(item);
}
std::string delay_imported_library::get_name() const {
    return this->name;
}
DWORD delay_imported_library::set_attributes() const {
    return this->attributes;
}
DWORD delay_imported_library::get_timestamp() const {
    return this->timestamp;
}
DWORD delay_imported_library::get_rva_module_handle() const {
    return this->module_handle_rva;
}
DWORD delay_imported_library::get_rva_iat() const {
    return this->relative_virtual_address_to_iat;
}
DWORD delay_imported_library::get_rva_bound_iat() const {
    return this->relative_virtual_address_to_bound_iat;
}
DWORD delay_imported_library::get_rva_unload_info_table() const {
    return this->relative_virtual_address_to_unload_info_table;
}
imported_library delay_imported_library::convert_to_imported_library() const {
    imported_library lib;
    lib.set_name(this->name);
    lib.set_timestamp(this->timestamp);
    lib.set_rva_iat(this->relative_virtual_address_to_iat);

    for (auto& item : imported_items) {
        lib.add_item(item);
    }

    return lib;
}

delay_import_table::delay_import_table() {

}
delay_import_table::~delay_import_table(){

}

delay_import_table& delay_import_table::operator=(const delay_import_table& imports) {
    this->libs = imports.libs;

    return *this;
}
void delay_import_table::add_lib(delay_imported_library& lib) {
    this->libs.push_back(lib);
}
import_table delay_import_table::convert_to_import_table() const {
    import_table imports;

    for (auto& lib : libs) {
        imports.add_lib(lib.convert_to_imported_library());
    }
    return imports;
}
std::vector<delay_imported_library>& delay_import_table::get_libs() {
    return libs;
}

bool get_delay_import_table(const pe_image &image, delay_import_table& imports) {
	imports.get_libs().clear();

	DWORD virtual_address = image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT);
	DWORD virtual_size = image.get_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT);

	if (virtual_address && virtual_size) {
		pe_section * imp_section = image.get_section_by_rva(virtual_address);
		if (imp_section) {
			BYTE * raw_decs = &imp_section->get_section_data().data()[virtual_address - imp_section->get_virtual_address()];

			for (unsigned int imp_size = 0; imp_size < virtual_size; imp_size += sizeof(IMAGE_DELAYLOAD_DESCRIPTOR)) {
				PIMAGE_DELAYLOAD_DESCRIPTOR imp_description = (PIMAGE_DELAYLOAD_DESCRIPTOR)(&raw_decs[imp_size]);

				if (imp_description->DllNameRVA && imp_description->ImportAddressTableRVA) {
                    delay_imported_library lib;
					pe_section * imp_dll_name_section = image.get_section_by_rva(imp_description->DllNameRVA);
					pe_section * imp_name_section = image.get_section_by_rva(imp_description->ImportNameTableRVA);
					
					lib.set_name(std::string(((char*)&imp_dll_name_section->get_section_data().data()[imp_description->DllNameRVA - imp_dll_name_section->get_virtual_address()])));
					lib.set_rva_iat(imp_description->ImportAddressTableRVA);
                    lib.set_rva_bound_iat(imp_description->BoundImportAddressTableRVA);
                    lib.set_rva_unload_info_table(imp_description->UnloadInformationTableRVA);
                    lib.set_rva_module_handle(imp_description->ModuleHandleRVA);
                    lib.set_timestamp(imp_description->TimeDateStamp);
                    lib.set_attributes(imp_description->Attributes.AllAttributes);

					void* lib_iat = &imp_name_section->get_section_data().data()[(imp_description->ImportNameTableRVA - imp_name_section->get_virtual_address())];
					DWORD func_iat_rva = imp_description->ImportAddressTableRVA;


					if (image.is_x32_image()) {
						while (*(DWORD*)lib_iat) {
							if (*(DWORD*)lib_iat&IMAGE_ORDINAL_FLAG32) {
								lib.add_item(imported_func(func_iat_rva, *(DWORD*)lib_iat & 0xFFFF));
							}
							else {
								pe_section * imp_func = image.get_section_by_rva(*(DWORD*)lib_iat);
								if (imp_func) {
									lib.add_item(imported_func(
										func_iat_rva,
										(char*)&imp_func->get_section_data().data()[(*(DWORD*)lib_iat - imp_func->get_virtual_address()) + sizeof(WORD)],
										*(WORD*)&imp_func->get_section_data().data()[(*(DWORD*)lib_iat - imp_func->get_virtual_address())]
									));
								}
							}
							func_iat_rva += sizeof(DWORD);
							lib_iat = (void*)(&((DWORD*)lib_iat)[1]);
						}
					}
					else {
						while (*(DWORD64*)lib_iat) {
							if (*(DWORD64*)lib_iat&IMAGE_ORDINAL_FLAG64) {
								lib.add_item(imported_func(func_iat_rva, *(DWORD64*)lib_iat & 0xFFFF));
							}
							else {
								pe_section * imp_func = image.get_section_by_rva(*(DWORD*)lib_iat);
								if (imp_func) {
									lib.add_item(imported_func(
										func_iat_rva,
										(char*)&imp_func->get_section_data().data()[(*(DWORD*)lib_iat - imp_func->get_virtual_address()) + sizeof(WORD)],
										*(WORD*)&imp_func->get_section_data().data()[(*(DWORD*)lib_iat - imp_func->get_virtual_address())]
									));
								}
							}

							func_iat_rva += sizeof(DWORD64);
							lib_iat = (void*)(&((DWORD64*)lib_iat)[1]);
						}
					}


					imports.add_lib(lib);
				}
				else {
					return true;
				}
			}
			return true;
		}
	}

	return false;
}
bool erase_delay_import_table(pe_image &image, std::vector<erased_zone>* zones) {
	DWORD virtual_address = image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT);
	DWORD virtual_size = image.get_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT);

	if (virtual_address && virtual_size) {
		
		pe_section * imp_section = image.get_section_by_rva(virtual_address);
		if (imp_section) {
			BYTE * raw_import = &imp_section->get_section_data().data()[virtual_address - imp_section->get_virtual_address()];

			if (imp_section->get_size_of_raw_data() >= virtual_size) {
				ZeroMemory(raw_import, virtual_size);

				if (zones) {zones->push_back({virtual_address ,virtual_size});}
				image.set_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT, 0);
				image.set_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT, 0);
				return true;
			}		
		}
	}
	return false;
}