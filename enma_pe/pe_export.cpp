#include "stdafx.h"

using namespace enma;

bool pe_export_entry::operator==(const pe_export_entry& item) {

    if (item._b_name == _b_name) {
        
        if (item._b_name) {

            if (item._func_name == _func_name) { 
                return true; 
            }
        }
        else {
            
            if (item._name_ordinal == _name_ordinal) { 
                return true; 
            }
        }
    }

    return false;
}

bool pe_export_directory::get_exported_function(const std::string& func_name, pe_export_entry * &_function) {

    for (auto &function : _functions) {
        if (function.has_name() && function.get_func_name() == func_name) {
            _function = &function;
            return true;
        }
    }
    return false;
}

bool pe_export_directory::get_exported_function(uint16_t ordinal, pe_export_entry * &_function) {

    for (auto & function : _functions) {
        if (!function.has_name() && function.get_ordinal() == ordinal) {
            _function = &function;
            return true;
        }
    }
    return false;
}

pe_directory_code enma::get_export_directory(const pe_image &image, pe_export_directory& exports) {
    exports.set_characteristics(0);
    exports.set_time_stamp(0);
    exports.set_major_version(0);
    exports.set_minor_version(0);
    exports.set_ordinal_base(0);
    exports.set_number_of_functions(0);
    exports.set_number_of_names(0);
    exports.set_library_name("");
    exports.get_functions().clear();


    uint32_t virtual_address = image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_EXPORT);
    uint32_t virtual_size = image.get_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_EXPORT);


    if (virtual_address) {

        pe_image_io export_io(image);

        if (!export_io.is_present_rva(virtual_address)) {
            return pe_directory_code::pe_directory_code_not_present;
        }

        image_export_directory export_desc;

        if (export_io.set_image_offset(virtual_address).read(&export_desc, sizeof(export_desc)) != enma_io_success) {
            return pe_directory_code::pe_directory_code_currupted;
        }

        exports.set_characteristics(export_desc.characteristics);
        exports.set_time_stamp(export_desc.time_date_stamp);
        exports.set_major_version(export_desc.major_version);
        exports.set_minor_version(export_desc.minor_version);
        exports.set_ordinal_base(export_desc.base);
        exports.set_number_of_functions(export_desc.number_of_functions);
        exports.set_number_of_names(export_desc.number_of_names);

        if (!exports.get_number_of_functions()) { return pe_directory_code::pe_directory_code_success; }

        if (export_desc.name) {

            std::string lib_name;

            if (export_io.set_image_offset(export_desc.name).read_string(lib_name) != enma_io_success) {
                exports.set_library_name(lib_name);
            }          
        }

        std::unordered_map<uint32_t, std::string> names_of_functions;

        for (uint32_t idx = 0; idx < export_desc.number_of_names; idx++) {

            uint16_t function_ordinal;

            if (export_io.set_image_offset(export_desc.address_of_name_ordinals + idx * INT16_SIZE).read(
                &function_ordinal, sizeof(function_ordinal)) != enma_io_success) {

                break;
            }

            uint32_t function_name_rva;

            if (export_io.set_image_offset(export_desc.address_of_names + idx * INT32_SIZE).read(
                &function_name_rva, sizeof(function_name_rva)) != enma_io_success) {
                break;
            }

            std::string func_name;

            if (export_io.set_image_offset(function_name_rva).read_string(func_name) != enma_io_success) {
                break;
            }

            names_of_functions[function_ordinal] = func_name;
        }

        for (uint32_t ordinal = 0; ordinal < export_desc.number_of_functions; ordinal++) {

            uint32_t func_rva;

            pe_export_entry func;

            if (export_io.set_image_offset(export_desc.address_of_functions + ordinal * INT32_SIZE).read(
                &func_rva, sizeof(func_rva)) != enma_io_success) {
                return pe_directory_code::pe_directory_code_currupted;
            }

            //if (!func_rva) { continue; }

            func.set_rva(func_rva)
                .set_ordinal(uint16_t(export_desc.base + ordinal));

            auto name_it = names_of_functions.find(ordinal);

            if (name_it != names_of_functions.end()) {

                func.set_func_name(name_it->second)
                    .set_has_name(true)
                    .set_name_ordinal(name_it->first);

                if (func_rva >= virtual_address + sizeof(image_export_directory) &&
                    func_rva < virtual_address + virtual_size) {

                    std::string forwarded_func_name;

                    if (export_io.set_image_offset(func_rva).read_string(forwarded_func_name) != enma_io_success) {
                        return pe_directory_code::pe_directory_code_currupted;
                    }

                    func.set_forward_name(std::string(forwarded_func_name))
                        .set_forward(true);
                }
            }

            exports.add_function(func);
        }

        return pe_directory_code::pe_directory_code_success;
    }

    return pe_directory_code::pe_directory_code_not_present;
}

bool enma::build_export_directory(pe_image &image, pe_section& section, const pe_export_directory& exports) {

    if (!exports.get_functions().size()) { return true; }

    pe_section_io export_io(section, image, enma_io_mode_allow_expand);
    export_io.align_up(0x10).seek_to_end();


    uint32_t needed_size_for_strings = uint32_t(exports.get_library_name().length() + 1);
    uint32_t number_of_names            = 0;
    uint32_t max_ordinal                = 0;
    uint32_t ordinal_base                = -1;


    if (exports.get_functions().size()) {
        ordinal_base = exports.get_ordinal_base();
    }

    uint32_t needed_size_for_function_names = 0;
    uint32_t needed_size_for_function_forwards = 0;

    for (auto& func : exports.get_functions()) {
        max_ordinal = std::max<uint32_t>(max_ordinal, func.get_ordinal());
        ordinal_base = std::min<uint32_t>(ordinal_base, func.get_ordinal());

        if (func.has_name()) {
            number_of_names++;
            needed_size_for_function_names += uint32_t(func.get_func_name().length() + 1);
        }
        if (func.is_forward()) {
            needed_size_for_function_forwards += uint32_t(func.get_forward_name().length() + 1);
        }
    }

    needed_size_for_strings += needed_size_for_function_names;
    needed_size_for_strings += needed_size_for_function_forwards;
    uint32_t needed_size_for_function_name_ordinals = number_of_names * INT16_SIZE;
    uint32_t needed_size_for_function_name_rvas     = number_of_names * INT32_SIZE;
    uint32_t needed_size_for_function_addresses     = (max_ordinal - ordinal_base + 1) * INT32_SIZE;
    
    uint32_t directory_pos = export_io.get_section_offset();
    uint32_t current_pos_of_function_names = uint32_t(ALIGN_UP(exports.get_library_name().length() + 1,0x2) + directory_pos + sizeof(image_export_directory));
    uint32_t current_pos_of_function_name_ordinals = current_pos_of_function_names + needed_size_for_function_names;
    uint32_t current_pos_of_function_forwards   = current_pos_of_function_name_ordinals + needed_size_for_function_name_ordinals;
    uint32_t current_pos_of_function_addresses  = current_pos_of_function_forwards + needed_size_for_function_forwards;
    uint32_t current_pos_of_function_names_rvas = current_pos_of_function_addresses + needed_size_for_function_addresses;


    image_export_directory export_desc = { 0 };
    export_desc.characteristics     = exports.get_characteristics();
    export_desc.major_version       = exports.get_major_version();
    export_desc.minor_version       = exports.get_minor_version();
    export_desc.time_date_stamp        = exports.get_time_stamp();
    export_desc.number_of_functions    = max_ordinal - ordinal_base + 1;
    export_desc.number_of_names        = number_of_names;
    export_desc.base                = ordinal_base;
    export_desc.address_of_functions    = current_pos_of_function_addresses;
    export_desc.address_of_name_ordinals= current_pos_of_function_name_ordinals;
    export_desc.address_of_names        = current_pos_of_function_names_rvas;
    export_desc.name                    = directory_pos + (uint32_t)sizeof(image_export_directory);

    if (export_io.write(&export_desc, sizeof(export_desc)) != enma_io_success) {
        return false;
    }

    if (export_io.write((void*)exports.get_library_name().c_str(), uint32_t(exports.get_library_name().length() + 1)) != enma_io_success) {
        return false;
    }
    export_io.align_up(0x2);


    std::map<std::string, uint16_t> funcs;

    uint32_t last_ordinal = ordinal_base;
    for (auto& func : exports.get_functions()) {
        if (func.get_ordinal() > last_ordinal){

            uint32_t len = INT32_SIZE * (func.get_ordinal() - last_ordinal - 1);
            if (len){
                if (export_io.set_section_offset(current_pos_of_function_addresses).memory_set(len,0) != enma_io_success) {
                    return false;
                }

                current_pos_of_function_addresses += len;
            }

            last_ordinal = func.get_ordinal();
        }


        if (func.has_name()) {
            funcs.insert(std::make_pair(func.get_func_name(), static_cast<uint16_t>(func.get_ordinal() - ordinal_base)));
        }

        if (func.is_forward()) {
            
            if (export_io.set_section_offset(current_pos_of_function_addresses).write(
                &current_pos_of_function_forwards, sizeof(current_pos_of_function_forwards)) != enma_io_success) {

                return false;
            }
            
            current_pos_of_function_addresses += INT32_SIZE;

            if (export_io.set_section_offset(current_pos_of_function_forwards).write(
                (void*)func.get_forward_name().c_str(), uint32_t(func.get_forward_name().length() + 1)) != enma_io_success) {

                return false;
            }

            current_pos_of_function_forwards += static_cast<uint32_t>(func.get_forward_name().length() + 1);
        }
        else{
            uint32_t function_rva = func.get_rva();

            if (export_io.set_section_offset(current_pos_of_function_addresses).write(
                &function_rva, sizeof(function_rva)) != enma_io_success) {

                return false;
            }

            current_pos_of_function_addresses += INT32_SIZE;
        }
    }

    for (auto& func : funcs){


        if (export_io.set_section_offset(current_pos_of_function_names_rvas).write(
            &current_pos_of_function_names, sizeof(current_pos_of_function_names)) != enma_io_success) {

            return false;
        }

        current_pos_of_function_names_rvas += INT32_SIZE;

        if (export_io.set_section_offset(current_pos_of_function_names).write(
           (void*) func.first.c_str(), uint32_t(func.first.length() + 1)) != enma_io_success) {

            return false;
        }

        current_pos_of_function_names += static_cast<uint32_t>(func.first.length() + 1);


        if (export_io.set_section_offset(current_pos_of_function_name_ordinals).write(
            (void*)&(func.second), sizeof(func.second)) != enma_io_success) {

            return false;
        }

        current_pos_of_function_name_ordinals += INT16_SIZE;
    }


    image.set_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_EXPORT, directory_pos);
    image.set_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_EXPORT, 
        (uint32_t)sizeof(image_export_directory) +
        needed_size_for_function_name_ordinals + 
        needed_size_for_function_addresses +
        needed_size_for_strings +
        needed_size_for_function_name_rvas
    );

    return true;
}

pe_directory_code enma::get_placement_export_directory(const pe_image &image, pe_placement& placement) {
   
    uint32_t virtual_address = image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_EXPORT);
    uint32_t virtual_size    = image.get_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_EXPORT);

    if (virtual_address) {
        pe_image_io export_io(image);

        if (!export_io.is_present_rva(virtual_address)) {
            return pe_directory_code::pe_directory_code_not_present;
        }

        image_export_directory export_desc;

        if (export_io.set_image_offset(virtual_address).read(&export_desc, sizeof(export_desc)) != enma_io_success) {
            return pe_directory_code::pe_directory_code_currupted;
        }

        if (!export_desc.number_of_functions) { return pe_directory_code::pe_directory_code_success; }

        if (export_desc.name) {
            std::string lib_name;

            if (export_io.set_image_offset(export_desc.name).read_string(lib_name) != enma_io_success) {
                return pe_directory_code::pe_directory_code_currupted;
            }


            placement[export_desc.name] = pe_placement_entry(ALIGN_UP(lib_name.length() + 1, 0x2), id_pe_placement::id_pe_export_name, lib_name);
        }


        for (uint32_t ordinal = 0; ordinal < export_desc.number_of_functions; ordinal++) {

            uint32_t func_rva;

            if (export_io.set_image_offset(export_desc.address_of_functions + ordinal * INT32_SIZE).read(
                &func_rva, sizeof(func_rva)) != enma_io_success) {
                return pe_directory_code::pe_directory_code_currupted;
            }

            if (!func_rva) { continue; }


            for (uint32_t i = 0; i < export_desc.number_of_names; i++) {

                uint16_t ordinal2;

                if (export_io.set_image_offset(export_desc.address_of_name_ordinals + i * INT16_SIZE).read(
                    &ordinal2, sizeof(ordinal2)) != enma_io_success) {
                    return pe_directory_code::pe_directory_code_currupted;
                }

                if (ordinal == ordinal2) {

                    uint32_t function_name_rva;

                    if (export_io.set_image_offset(export_desc.address_of_names + i * INT32_SIZE).read(
                        &function_name_rva, sizeof(function_name_rva)) != enma_io_success) {
                        return pe_directory_code::pe_directory_code_currupted;
                    }

                    std::string func_name;

                    if (export_io.set_image_offset(function_name_rva).read_string(func_name) != enma_io_success) {
                        return pe_directory_code::pe_directory_code_currupted;
                    }

                    placement[function_name_rva] =
                        pe_placement_entry(ALIGN_UP(func_name.length() + 1, 0x2), id_pe_placement::id_pe_export_function_name, func_name);

                    if (func_rva >= virtual_address + sizeof(image_export_directory) &&
                        func_rva < virtual_address + virtual_size) {

                        std::string forwarded_func_name;

                        if (export_io.set_image_offset(func_rva).read_string(forwarded_func_name) != enma_io_success) {
                            return pe_directory_code::pe_directory_code_currupted;
                        }


                        placement[func_rva] =
                            pe_placement_entry(ALIGN_UP(forwarded_func_name.length() + 1, 0x2), id_pe_placement::id_pe_export_function_forwarded_name, forwarded_func_name);
                    }

                    break;
                }
            }
        }

        if (export_desc.address_of_functions) {
            placement[export_desc.address_of_functions] =
                pe_placement_entry(export_desc.number_of_functions * sizeof(uint32_t), id_pe_placement::id_pe_export_functions_table, "");
        }

        if (export_desc.address_of_names) {
            placement[export_desc.address_of_names] =
                pe_placement_entry(export_desc.number_of_names * sizeof(uint32_t), id_pe_placement::id_pe_export_names_table, "");
        }

        if (export_desc.address_of_name_ordinals) {
            placement[export_desc.address_of_name_ordinals] = 
                pe_placement_entry(export_desc.number_of_functions * sizeof(uint16_t), id_pe_placement::id_pe_export_ordinals_table, "");
        }

        placement[virtual_address] = pe_placement_entry(ALIGN_UP(sizeof(image_export_directory), 0x10), id_pe_placement::id_pe_export_descriptor, "");
        return pe_directory_code::pe_directory_code_success;
    }

    return pe_directory_code::pe_directory_code_not_present;
}


