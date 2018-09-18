#include "stdafx.h"
#include "pe_delay_import.h"

delay_imported_library::delay_imported_library() {
    attributes = 0 ;
    dll_name_rva = 0;
    module_handle_rva = 0;
    iat_rva = 0;
    names_table_rva = 0;
    iat_bound_table_rva = 0;
    unload_info_table_rva = 0;
    timestamp = 0;
}
delay_imported_library::delay_imported_library(const delay_imported_library& lib) {
    this->operator=(lib);
}
delay_imported_library::~delay_imported_library() {

}
delay_imported_library& delay_imported_library::operator=(const delay_imported_library& lib) {
    
    this->library_name      = lib.library_name;
    this->attributes        = lib.attributes;
    this->dll_name_rva      = lib.dll_name_rva;
    this->module_handle_rva = lib.module_handle_rva;
    this->iat_rva           = lib.iat_rva;
    this->names_table_rva   = lib.names_table_rva;
    this->iat_bound_table_rva = lib.iat_bound_table_rva;
    this->unload_info_table_rva = lib.unload_info_table_rva;
    this->timestamp         = lib.timestamp;
    this->imported_items = lib.imported_items;

    return *this;
}


void delay_imported_library::set_library_name(const std::string& library_name) {
    this->library_name = library_name;
}
void delay_imported_library::set_attributes(uint32_t  attributes) {
    this->attributes = attributes;
}
void delay_imported_library::set_dll_name_rva(uint32_t  dll_name_rva) {
    this->dll_name_rva = dll_name_rva;
}
void delay_imported_library::set_module_handle_rva(uint32_t  rva) {
    this->module_handle_rva = rva;
}
void delay_imported_library::set_iat_rva(uint32_t  rva) {
    this->iat_rva = rva;
}
void delay_imported_library::set_names_table_rva(uint32_t  rva) {
    this->names_table_rva = rva;
}
void delay_imported_library::set_bound_table_iat_rva(uint32_t  rva) {
    this->iat_bound_table_rva = rva;
}
void delay_imported_library::set_unload_info_table_rva(uint32_t  rva) {
    this->unload_info_table_rva = rva;
}
void delay_imported_library::set_timestamp(uint32_t  timestamp) {
    this->timestamp = timestamp;
}

void delay_imported_library::add_item(const imported_func& item) {
    imported_items.push_back(item);
}


std::string delay_imported_library::get_library_name() const {
    return this->library_name;
}
uint32_t delay_imported_library::get_attributes() const {
    return this->attributes;
}
uint32_t delay_imported_library::get_dll_name_rva() const {
    return this->dll_name_rva;
}
uint32_t delay_imported_library::get_module_handle_rva() const {
    return this->module_handle_rva;
}
uint32_t delay_imported_library::get_iat_rva() const {
    return this->iat_rva;
}
uint32_t delay_imported_library::get_names_table_rva() const {
    return this->names_table_rva;
}
uint32_t delay_imported_library::get_bound_table_iat_rva() const {
    return this->iat_bound_table_rva;
}
uint32_t delay_imported_library::get_unload_info_table_rva() const {
    return this->unload_info_table_rva;
}
uint32_t delay_imported_library::get_timestamp() const {
    return this->timestamp;
}
const std::vector<imported_func>& delay_imported_library::get_items() const {
    return this->imported_items;
}
std::vector<imported_func>& delay_imported_library::get_items() {
    return this->imported_items;
}

imported_library delay_imported_library::convert_to_imported_library() const {
    imported_library lib;
    lib.set_library_name(this->library_name);
    lib.set_timestamp(this->timestamp  ? -1 : 0);
    lib.set_rva_iat(this->iat_rva);
    lib.set_rva_oft(this->unload_info_table_rva);

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
    this->libraries = imports.libraries;

    return *this;
}
void delay_import_table::add_library(const delay_imported_library& lib) {
    this->libraries.push_back(lib);
}

void delay_import_table::clear() {
    this->libraries.clear();
}
size_t delay_import_table::size() const {
    return this->libraries.size();
}

import_table delay_import_table::convert_to_import_table() const {
    import_table imports;

    for (auto& lib : libraries) {
        imports.add_library(lib.convert_to_imported_library());
    }
    return imports;
}
const std::vector<delay_imported_library>& delay_import_table::get_libraries() const {
    return libraries;
}
std::vector<delay_imported_library>& delay_import_table::get_libraries() {
    return libraries;
}

template<typename image_format>
directory_code _get_delay_import_table(const pe_image &image, delay_import_table& imports,
     const bound_import_table& bound_imports) {

    imports.clear();

    uint32_t  virtual_address = image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT);

    if (virtual_address) {
        pe_image_io delay_imp_desc_io(image);
        delay_imp_desc_io.set_image_offset(virtual_address);

        image_delayload_descriptor import_desc;

        if (delay_imp_desc_io.read(&import_desc,sizeof(import_desc)) != enma_io_success) {
            return directory_code::directory_code_currupted;
        }

        if (import_desc.dll_name_rva && import_desc.import_address_table_rva) {

            do {
                delay_imported_library lib;
                std::string library_name;

                //only for x32 if used VA we convert it to RVA
                if (image.is_x32_image() && !import_desc.attributes.rva_based) { 
                    if (import_desc.dll_name_rva) { import_desc.dll_name_rva = image.va_to_rva(import_desc.dll_name_rva); }
                    if (import_desc.module_handle_rva) { import_desc.module_handle_rva = image.va_to_rva(import_desc.module_handle_rva); }
                    if (import_desc.import_address_table_rva) { import_desc.import_address_table_rva = image.va_to_rva(import_desc.import_address_table_rva); }
                    if (import_desc.import_name_table_rva) { import_desc.import_name_table_rva = image.va_to_rva(import_desc.import_name_table_rva); }
                    if (import_desc.bound_import_address_table_rva) { import_desc.bound_import_address_table_rva = image.va_to_rva(import_desc.bound_import_address_table_rva); }
                    if (import_desc.unload_information_table_rva) { import_desc.unload_information_table_rva = image.va_to_rva(import_desc.unload_information_table_rva); }
                }

                if (pe_image_io(image).set_image_offset(import_desc.dll_name_rva).read_string(library_name) != enma_io_success) {
                    return directory_code::directory_code_currupted;
                }

                lib.set_library_name(library_name);
                lib.set_attributes(import_desc.attributes.all_attributes);
                lib.set_dll_name_rva(import_desc.dll_name_rva);
                lib.set_module_handle_rva(import_desc.module_handle_rva);
                lib.set_iat_rva(import_desc.import_address_table_rva);
                lib.set_names_table_rva(import_desc.import_name_table_rva);
                lib.set_bound_table_iat_rva(import_desc.bound_import_address_table_rva);
                lib.set_unload_info_table_rva(import_desc.unload_information_table_rva);
                lib.set_timestamp(import_desc.time_date_stamp);

                pe_image_io delay_import_names_io(image);
                pe_image_io delay_import_bound_iat_io(image);

                delay_import_names_io.set_image_offset(import_desc.import_name_table_rva);
                delay_import_bound_iat_io.set_image_offset(import_desc.bound_import_address_table_rva);

                bool is_used_bound_table = (import_desc.time_date_stamp &&
                    bound_imports.has_library(library_name, import_desc.time_date_stamp));

                for (uint32_t iat_func_address = import_desc.import_address_table_rva;; //get funcs
                    iat_func_address += (uint32_t)sizeof(typename image_format::ptr_size)) {


                    typename image_format::ptr_size name_item = 0;
                    typename image_format::ptr_size bound_item = 0;

                    if (is_used_bound_table) {
                        if (delay_import_bound_iat_io.read(&bound_item, sizeof(bound_item)) != enma_io_success) {
                            return directory_code::directory_code_currupted;
                        }
                    }

                    if (delay_import_names_io.read(&name_item, sizeof(name_item)) != enma_io_success) {
                        return directory_code::directory_code_currupted;
                    }

                    if (name_item) {
                        if (name_item&image_format::ordinal_flag) {
                            lib.add_item(imported_func(iat_func_address, uint16_t(name_item^image_format::ordinal_flag), bound_item));
                        }
                        else {
                            uint16_t hint;
                            std::string func_name;

                            pe_image_io delay_import_func_name_io(image);
                            delay_import_func_name_io.set_image_offset(uint32_t(name_item));

                            if (delay_import_func_name_io.read(&hint, sizeof(hint)) != enma_io_success) {
                                return directory_code::directory_code_currupted;
                            }
                            if (delay_import_func_name_io.read_string(func_name) != enma_io_success) {
                                return directory_code::directory_code_currupted;
                            }

                            lib.add_item(imported_func(iat_func_address, func_name, hint , bound_item));
                        }
                    }
                    else {
                        break;
                    }

                }

                imports.add_library(lib);

                if (delay_imp_desc_io.read(&import_desc, sizeof(import_desc)) != enma_io_success) {
                    return directory_code::directory_code_currupted;
                }

            } while (import_desc.dll_name_rva && import_desc.import_address_table_rva);

        }

        return directory_code::directory_code_success;
    }

    return directory_code::directory_code_not_present;
}


template<typename image_format>
directory_code _get_placement_delay_import_table(const pe_image &image, pe_directory_placement& placement,
    const bound_import_table& bound_imports) {

    uint32_t  virtual_address = image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT);

    if (virtual_address) {
        pe_image_io delay_import_desc_io(image);
        delay_import_desc_io.set_image_offset(virtual_address);

        image_delayload_descriptor import_desc;

        if (delay_import_desc_io.read(&import_desc, sizeof(import_desc)) != enma_io_success) {
            return directory_code::directory_code_currupted;
        }

        if (import_desc.dll_name_rva && import_desc.import_address_table_rva) {

            do {
                std::string lib_name;

                //only for x32 if used VA we convert it to RVA
                if (image.is_x32_image() && !import_desc.attributes.rva_based) {
                    if (import_desc.dll_name_rva) { import_desc.dll_name_rva = image.va_to_rva(import_desc.dll_name_rva); }
                    if (import_desc.module_handle_rva) { import_desc.module_handle_rva = image.va_to_rva(import_desc.module_handle_rva); }
                    if (import_desc.import_address_table_rva) { import_desc.import_address_table_rva = image.va_to_rva(import_desc.import_address_table_rva); }
                    if (import_desc.import_name_table_rva) { import_desc.import_name_table_rva = image.va_to_rva(import_desc.import_name_table_rva); }
                    if (import_desc.bound_import_address_table_rva) { import_desc.bound_import_address_table_rva = image.va_to_rva(import_desc.bound_import_address_table_rva); }
                    if (import_desc.unload_information_table_rva) { import_desc.unload_information_table_rva = image.va_to_rva(import_desc.unload_information_table_rva); }
                }

                if (pe_image_io(image).set_image_offset(import_desc.dll_name_rva).read_string(lib_name) != enma_io_success) {
                    return directory_code::directory_code_currupted;
                }


                placement[import_desc.dll_name_rva] =
                    directory_placement((uint32_t)ALIGN_UP((lib_name.length() + 1), 0x2), id_pe_delay_import_library_name, lib_name);

                pe_image_io delay_import_names_io(image);
                delay_import_names_io.set_image_offset(import_desc.import_name_table_rva);

                uint32_t iat_func_address;


                for (iat_func_address = import_desc.import_address_table_rva;; //get funcs
                    iat_func_address += (uint32_t)sizeof(typename image_format::ptr_size)) {

                    typename image_format::ptr_size name_item = 0;

                    if (delay_import_names_io.read(&name_item, sizeof(name_item)) != enma_io_success) {
                        return directory_code::directory_code_currupted;
                    }

                    if (name_item) {
                        if ( !(name_item&image_format::ordinal_flag)) {
                            uint16_t hint;
                            std::string func_name;

                            pe_image_io delay_import_func_name_io(image);
                            delay_import_func_name_io.set_image_offset(uint32_t(name_item));

                            if (delay_import_func_name_io.read(&hint, sizeof(hint)) != enma_io_success) {
                                return directory_code::directory_code_currupted;
                            }
                            if (delay_import_func_name_io.read_string(func_name) != enma_io_success) {
                                return directory_code::directory_code_currupted;
                            }

                            placement[(uint32_t)name_item] =
                                directory_placement(sizeof(uint16_t) + (uint32_t)ALIGN_UP((func_name.length() + 1), 0x2), id_pe_delay_import_function_name, func_name);
                        }
                    }
                    else {
                        break;
                    }

                }

                placement[delay_import_desc_io.get_image_offset() - (uint32_t)sizeof(image_delayload_descriptor)] =
                    directory_placement(sizeof(image_delayload_descriptor), id_pe_delay_import_descriptor, "");


                uint32_t table_size = (delay_import_names_io.get_image_offset() - iat_func_address) + (uint32_t)sizeof(typename image_format::ptr_size);

                if (import_desc.import_name_table_rva) {
                    placement[import_desc.import_name_table_rva] = directory_placement(table_size, id_pe_delay_import_name_table, "");
                }
                if (import_desc.bound_import_address_table_rva) {
                    placement[import_desc.import_name_table_rva] = directory_placement(table_size, id_pe_delay_import_bound_table, "");
                }
                if (import_desc.unload_information_table_rva) {
                    placement[import_desc.import_name_table_rva] = directory_placement(table_size, id_pe_delay_import_unload_table, "");
                }

                
                if (delay_import_desc_io.read(&import_desc, sizeof(import_desc)) != enma_io_success) {
                    return directory_code::directory_code_currupted;
                }

            } while (import_desc.dll_name_rva && import_desc.import_address_table_rva);
        }

        placement[delay_import_desc_io.get_image_offset() - (uint32_t)sizeof(image_delayload_descriptor)] =
            directory_placement(sizeof(image_delayload_descriptor), id_pe_delay_import_descriptor, "");

        return directory_code::directory_code_success;
    }

    return directory_code::directory_code_not_present;
}

directory_code get_delay_import_table(const pe_image &image, delay_import_table& imports, const bound_import_table& bound_imports) {
	
    if (image.is_x32_image()) {
        return _get_delay_import_table<image_32>(image, imports, bound_imports);
    }
    else {
        return _get_delay_import_table<image_64>(image, imports, bound_imports);
    }
}

directory_code get_placement_delay_import_table(const pe_image &image, pe_directory_placement& placement,
    const bound_import_table& bound_imports) {

    if (image.is_x32_image()) {
        return _get_placement_delay_import_table<image_32>(image, placement, bound_imports);
    }
    else {
        return _get_placement_delay_import_table<image_64>(image, placement, bound_imports);
    }
}