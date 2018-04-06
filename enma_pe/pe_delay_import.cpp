#include "stdafx.h"
#include "pe_delay_import.h"

delay_imported_library::delay_imported_library() {
    attributes = 0;
    timestamp = 0;
    module_handle_rva = 0;
    rva_to_iat = 0;
    rva_to_bound_iat = 0;
    rva_to_unload_info_table = 0;
}
delay_imported_library::delay_imported_library(const delay_imported_library& lib) {
    this->operator=(lib);
}
delay_imported_library::~delay_imported_library() {

}
delay_imported_library& delay_imported_library::operator=(const delay_imported_library& lib) {
    
    this->library_name      = lib.library_name;
    this->attributes        = lib.attributes;
    this->timestamp         = lib.timestamp;
    this->module_handle_rva = lib.module_handle_rva;
    this->rva_to_iat               = lib.rva_to_iat;
    this->rva_to_bound_iat         = lib.rva_to_bound_iat;
    this->rva_to_unload_info_table = lib.rva_to_unload_info_table;

    this->imported_items = lib.imported_items;

    return *this;
}

void delay_imported_library::set_library_name(const std::string& library_name) {
    this->library_name = library_name;
}
void delay_imported_library::set_attributes(uint32_t  attributes) {
    this->attributes = attributes;
}
void delay_imported_library::set_timestamp(uint32_t  timestamp) {
    this->timestamp = timestamp;
}
void delay_imported_library::set_rva_module_handle(uint32_t  rva) {
    this->module_handle_rva = rva;
}
void delay_imported_library::set_rva_iat(uint32_t  rva) {
    this->rva_to_iat = rva;
}
void delay_imported_library::set_rva_bound_iat(uint32_t  rva) {
    this->rva_to_bound_iat = rva;
}
void delay_imported_library::set_rva_unload_info_table(uint32_t  rva) {
    this->rva_to_unload_info_table = rva;
}
void delay_imported_library::add_item(const imported_func& item) {
    imported_items.push_back(item);
}
std::string delay_imported_library::get_library_name() const {
    return this->library_name;
}
uint32_t  delay_imported_library::set_attributes() const {
    return this->attributes;
}
uint32_t  delay_imported_library::get_timestamp() const {
    return this->timestamp;
}
uint32_t  delay_imported_library::get_rva_module_handle() const {
    return this->module_handle_rva;
}
uint32_t  delay_imported_library::get_rva_iat() const {
    return this->rva_to_iat;
}
uint32_t  delay_imported_library::get_rva_bound_iat() const {
    return this->rva_to_bound_iat;
}
uint32_t  delay_imported_library::get_rva_unload_info_table() const {
    return this->rva_to_unload_info_table;
}
imported_library delay_imported_library::convert_to_imported_library() const {
    imported_library lib;
    lib.set_library_name(this->library_name);
    lib.set_timestamp(this->timestamp);
    lib.set_rva_iat(this->rva_to_iat);

    for (auto& item : imported_items) {
        lib.add_item(item);
    }

    return lib;
}

delay_import_table::delay_import_table() {

}
delay_import_table::delay_import_table(const delay_import_table& imports) {
    this->operator=(imports);
}
delay_import_table::~delay_import_table(){

}

delay_import_table& delay_import_table::operator=(const delay_import_table& imports) {
    this->libs = imports.libs;

    return *this;
}
void delay_import_table::add_lib(const delay_imported_library& lib) {
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

	uint32_t  virtual_address = image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT);
	uint32_t  virtual_size    = image.get_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT);

	if (virtual_address && virtual_size) {
		pe_section * imp_section = image.get_section_by_rva(virtual_address);
		if (imp_section) {
			uint8_t  * raw_decs = &imp_section->get_section_data().data()[virtual_address - imp_section->get_virtual_address()];
            
			for (size_t imp_size = 0; imp_size < virtual_size; imp_size += sizeof(image_delayload_descriptor)) {
                image_delayload_descriptor* imp_description = (image_delayload_descriptor*)(&raw_decs[imp_size]);

				if (imp_description->dll_name_rva && imp_description->import_address_table_rva) {
                    delay_imported_library lib;
					pe_section * imp_dll_name_section = image.get_section_by_rva(imp_description->dll_name_rva);
					pe_section * imp_name_section = image.get_section_by_rva(imp_description->import_name_table_rva);
					
					lib.set_library_name(std::string(((char*)&imp_dll_name_section->get_section_data().data()[imp_description->dll_name_rva - imp_dll_name_section->get_virtual_address()])));
					lib.set_rva_iat(imp_description->import_address_table_rva);
                    lib.set_rva_bound_iat(imp_description->bound_import_address_table_rva);
                    lib.set_rva_unload_info_table(imp_description->unload_information_table_rva);
                    lib.set_rva_module_handle(imp_description->module_handle_rva);
                    lib.set_timestamp(imp_description->time_date_stamp);
                    lib.set_attributes(imp_description->attributes.all_attributes);

					uint8_t  * lib_iat = &imp_name_section->get_section_data().data()[(imp_description->import_name_table_rva - imp_name_section->get_virtual_address())];
					uint32_t  func_iat_rva = imp_description->import_address_table_rva;


					if (image.is_x32_image()) {
						while (*(uint32_t *)lib_iat) {
							if (*(uint32_t *)lib_iat&IMAGE_ORDINAL_FLAG32) {
								lib.add_item(imported_func(func_iat_rva, *(uint32_t *)lib_iat & 0xFFFF));
							}
							else {
								pe_section * imp_func = image.get_section_by_rva(*(uint32_t *)lib_iat);
								if (imp_func) {
									lib.add_item(imported_func(
										func_iat_rva,
										(char*)&imp_func->get_section_data().data()[(*(uint32_t *)lib_iat - imp_func->get_virtual_address()) + sizeof(uint16_t )],
										*(uint16_t *)&imp_func->get_section_data().data()[(*(uint32_t *)lib_iat - imp_func->get_virtual_address())]
									));
								}
							}
							func_iat_rva += sizeof(uint32_t );
							lib_iat = (uint8_t *)(&((uint32_t *)lib_iat)[1]);
						}
					}
					else {
						while (*(uint64_t *)lib_iat) {
							if (*(uint64_t *)lib_iat&IMAGE_ORDINAL_FLAG64) {
								lib.add_item(imported_func(func_iat_rva, *(uint64_t *)lib_iat & 0xFFFF));
							}
							else {
								pe_section * imp_func = image.get_section_by_rva(*(uint32_t *)lib_iat);
								if (imp_func) {
									lib.add_item(imported_func(
										func_iat_rva,
										(char*)&imp_func->get_section_data().data()[(*(uint32_t *)lib_iat - imp_func->get_virtual_address()) + sizeof(uint16_t )],
										*(uint16_t *)&imp_func->get_section_data().data()[(*(uint32_t *)lib_iat - imp_func->get_virtual_address())]
									));
								}
							}

							func_iat_rva += sizeof(uint64_t );
							lib_iat = (uint8_t *)(&((uint64_t *)lib_iat)[1]);
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

bool get_placement_delay_import_table(pe_image &image, std::vector<directory_placement>& placement) {
	uint32_t  virtual_address = image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT);
	uint32_t  virtual_size    = image.get_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT);

	if (virtual_address && virtual_size) {
		
		pe_section * imp_section = image.get_section_by_rva(virtual_address);
		if (imp_section) {
            if (ALIGN_UP(imp_section->get_virtual_size(), image.get_section_align()) >= virtual_size) {

                placement.push_back({virtual_address ,virtual_size, dp_id_delay_import });
				return true;
			}		
		}
	}
	return false;
}