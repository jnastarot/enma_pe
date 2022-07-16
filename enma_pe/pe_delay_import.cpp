#include "stdafx.h"

using namespace enma;

pe_import_library pe_delay_library::convert_to_pe_import_library() const {

    pe_import_library lib;
    lib.set_library_name(_library_name);
    lib.set_timestamp(_timestamp  ? -1 : 0);
    lib.set_rva_iat(_iat_rva);
    lib.set_rva_oft(0);
    lib.set_bound_library(_is_bound);

    for (auto& function : _functions) {
        lib.add_function(function);
    }
    
    return lib;
}

pe_import_directory pe_delay_import_directory::convert_to_import_table() const {
    
    pe_import_directory imports;

    for (auto& lib : _libraries) {
        imports.add_library(lib.convert_to_pe_import_library());
    }

    return imports;
}

template<typename image_format>
pe_directory_code _get_delay_import_directory(const pe_image &image, pe_delay_import_directory& imports,
     const pe_bound_import_directory& bound_imports) {

    imports.clear();

    uint32_t  virtual_address = image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT);

    if (virtual_address) {
        pe_image_io delay_imp_desc_io(image);
        delay_imp_desc_io.set_image_offset(virtual_address);

        if (!delay_imp_desc_io.is_present_rva(virtual_address)) {
            return pe_directory_code::pe_directory_code_not_present;
        }

        image_delayload_descriptor import_desc;

        if (delay_imp_desc_io.read(&import_desc,sizeof(import_desc)) != enma_io_success) {
            return pe_directory_code::pe_directory_code_currupted;
        }

        if (import_desc.dll_name_rva && import_desc.import_address_table_rva) {

            do {
                pe_delay_library library;
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
                    return pe_directory_code::pe_directory_code_currupted;
                }


                bool is_used_bound_table = (import_desc.time_date_stamp &&
                    bound_imports.has_library(library_name, import_desc.time_date_stamp));

                library.set_library_name(library_name)
                    .set_attributes(import_desc.attributes.all_attributes)
                    .set_dll_name_rva(import_desc.dll_name_rva)
                    .set_module_handle_rva(import_desc.module_handle_rva)
                    .set_iat_rva(import_desc.import_address_table_rva)
                    .set_names_table_rva(import_desc.import_name_table_rva)
                    .set_bound_table_iat_rva(import_desc.bound_import_address_table_rva)
                    .set_unload_info_table_rva(import_desc.unload_information_table_rva)
                    .set_timestamp(import_desc.time_date_stamp)
                    .set_bound_library(is_used_bound_table);

                pe_image_io delay_import_names_io(image);
                pe_image_io delay_import_bound_iat_io(image);

                delay_import_names_io.set_image_offset(import_desc.import_name_table_rva);
                delay_import_bound_iat_io.set_image_offset(import_desc.bound_import_address_table_rva);


                for (uint32_t iat_func_address = import_desc.import_address_table_rva;; //get funcs
                    iat_func_address += (uint32_t)sizeof(typename image_format::ptr_size)) {

                    typename image_format::ptr_size name_item = 0;
                    typename image_format::ptr_size bound_item = 0;

                    if (is_used_bound_table) {
                        if (delay_import_bound_iat_io.read(&bound_item, sizeof(bound_item)) != enma_io_success) {
                            return pe_directory_code::pe_directory_code_currupted;
                        }
                    }

                    if (delay_import_names_io.read(&name_item, sizeof(name_item)) != enma_io_success) {
                        return pe_directory_code::pe_directory_code_currupted;
                    }

                    if (name_item) {

                        pe_import_function func;

                        func.set_iat_rva(iat_func_address)
                            .set_iat_item(bound_item)
                            .set_oft_rva(0)
                            .set_oft_item(0);

                        if (name_item&image_format::ordinal_flag) {

                            func.set_import_by_name(false)
                                .set_ordinal(uint16_t(name_item ^ image_format::ordinal_flag));

                        }
                        else {

                            uint16_t hint;
                            std::string func_name;

                            pe_image_io delay_import_func_name_io(image);
                            delay_import_func_name_io.set_image_offset(uint32_t(name_item));

                            if (delay_import_func_name_io.read(&hint, sizeof(hint)) != enma_io_success) {
                                return pe_directory_code::pe_directory_code_currupted;
                            }
                            if (delay_import_func_name_io.read_string(func_name) != enma_io_success) {
                                return pe_directory_code::pe_directory_code_currupted;
                            }

                            func.set_import_by_name(true)
                                .set_hint(hint)
                                .set_func_name(func_name);

                        }

                        library.add_function(func);
                    }
                    else {
                        break;
                    }

                }

                imports.add_library(library);

                if (delay_imp_desc_io.read(&import_desc, sizeof(import_desc)) != enma_io_success) {
                    return pe_directory_code::pe_directory_code_currupted;
                }

            } while (import_desc.dll_name_rva && import_desc.import_address_table_rva);

        }

        return pe_directory_code::pe_directory_code_success;
    }

    return pe_directory_code::pe_directory_code_not_present;
}


template<typename image_format>
pe_directory_code _get_placement_delay_import_directory(const pe_image &image, pe_placement& placement,
    const pe_bound_import_directory& bound_imports) {

    uint32_t  virtual_address = image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT);

    if (virtual_address) {
        pe_image_io delay_import_desc_io(image);

        if (!delay_import_desc_io.is_present_rva(virtual_address)) {
            return pe_directory_code::pe_directory_code_not_present;
        }

        delay_import_desc_io.set_image_offset(virtual_address);

        image_delayload_descriptor import_desc;

        if (delay_import_desc_io.read(&import_desc, sizeof(import_desc)) != enma_io_success) {
            return pe_directory_code::pe_directory_code_currupted;
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
                    return pe_directory_code::pe_directory_code_currupted;
                }


                placement[import_desc.dll_name_rva] =
                    pe_placement_entry(ALIGN_UP((lib_name.length() + 1), 0x2), id_pe_placement::id_pe_delay_import_library_name, lib_name);

                pe_image_io delay_import_names_io(image);
                delay_import_names_io.set_image_offset(import_desc.import_name_table_rva);

                uint32_t iat_func_address;


                for (iat_func_address = import_desc.import_address_table_rva;; //get funcs
                    iat_func_address += (uint32_t)sizeof(typename image_format::ptr_size)) {

                    typename image_format::ptr_size name_item = 0;

                    if (delay_import_names_io.read(&name_item, sizeof(name_item)) != enma_io_success) {
                        return pe_directory_code::pe_directory_code_currupted;
                    }

                    if (name_item) {
                        if ( !(name_item&image_format::ordinal_flag)) {
                            uint16_t hint;
                            std::string func_name;

                            pe_image_io delay_import_func_name_io(image);
                            delay_import_func_name_io.set_image_offset(uint32_t(name_item));

                            if (delay_import_func_name_io.read(&hint, sizeof(hint)) != enma_io_success) {
                                return pe_directory_code::pe_directory_code_currupted;
                            }
                            if (delay_import_func_name_io.read_string(func_name) != enma_io_success) {
                                return pe_directory_code::pe_directory_code_currupted;
                            }

                            placement[(uint32_t)name_item] =
                                pe_placement_entry(sizeof(uint16_t) + ALIGN_UP((func_name.length() + 1), 0x2), id_pe_placement::id_pe_delay_import_function_name, func_name);
                        }
                    }
                    else {
                        break;
                    }

                }

                placement[delay_import_desc_io.get_image_offset() - (uint32_t)sizeof(image_delayload_descriptor)] =
                    pe_placement_entry(sizeof(image_delayload_descriptor), id_pe_placement::id_pe_delay_import_descriptor, "");


                uint32_t table_size = (delay_import_names_io.get_image_offset() - iat_func_address) + (uint32_t)sizeof(typename image_format::ptr_size);

                if (import_desc.import_name_table_rva) {
                    placement[import_desc.import_name_table_rva] = pe_placement_entry(table_size, id_pe_placement::id_pe_delay_import_name_table, "");
                }
                if (import_desc.bound_import_address_table_rva) {
                    placement[import_desc.import_name_table_rva] = pe_placement_entry(table_size, id_pe_placement::id_pe_delay_import_bound_table, "");
                }
                if (import_desc.unload_information_table_rva) {
                    placement[import_desc.import_name_table_rva] = pe_placement_entry(table_size, id_pe_placement::id_pe_delay_import_unload_table, "");
                }

                
                if (delay_import_desc_io.read(&import_desc, sizeof(import_desc)) != enma_io_success) {
                    return pe_directory_code::pe_directory_code_currupted;
                }

            } while (import_desc.dll_name_rva && import_desc.import_address_table_rva);
        }

        placement[delay_import_desc_io.get_image_offset() - (uint32_t)sizeof(image_delayload_descriptor)] =
            pe_placement_entry(sizeof(image_delayload_descriptor), id_pe_placement::id_pe_delay_import_descriptor, "");

        return pe_directory_code::pe_directory_code_success;
    }

    return pe_directory_code::pe_directory_code_not_present;
}

pe_directory_code enma::get_delay_import_directory(const pe_image &image, pe_delay_import_directory& imports, const pe_bound_import_directory& bound_imports) {
    
    if (image.is_x32_image()) {
        return _get_delay_import_directory<pe_image_32>(image, imports, bound_imports);
    }
    else {
        return _get_delay_import_directory<pe_image_64>(image, imports, bound_imports);
    }
}

pe_directory_code enma::get_placement_delay_import_directory(const pe_image &image, pe_placement& placement,
    const pe_bound_import_directory& bound_imports) {

    if (image.is_x32_image()) {
        return _get_placement_delay_import_directory<pe_image_32>(image, placement, bound_imports);
    }
    else {
        return _get_placement_delay_import_directory<pe_image_64>(image, placement, bound_imports);
    }
}