#include "stdafx.h"

using namespace enma;
using namespace hl;

pe_exception_unwind_info::pe_exception_unwind_info(const pe_exception_unwind_info& item) {
    operator=(item);
}

pe_exception_unwind_info::~pe_exception_unwind_info() {
    
    if (_chained_entry) {
        delete _chained_entry;
        _chained_entry = 0;
    }
}

pe_exception_unwind_info& pe_exception_unwind_info::operator=(const pe_exception_unwind_info& item) {
    
    _unwind_info_rva = item._unwind_info_rva;
    _version = item._version;
    _flags = item._flags;
    _size_of_prolog = item._size_of_prolog;
    _count_of_codes = item._count_of_codes;
    _frame_register = item._frame_register;
    _frame_offset = item._frame_offset;
    _handler_rva = item._handler_rva;

    if (item._chained_entry) {
        _chained_entry = new pe_exception_entry(*item._chained_entry);
    }
    else {
        _chained_entry = 0;
    }
    
    _codes = item._codes;
    _params = item._params;

    _custom_parameter = item._custom_parameter;

    return *this;
}

static pe_directory_code handle_unwind_info(const pe_image &image, uint32_t rva, pe_exceptions_directory& exceptions) {
   
    for (auto& entry : exceptions.get_unwind_entries()) {
        if (entry.get_unwind_info_rva() == rva) { 
            return pe_directory_code::pe_directory_code_success; 
        }
    }

    pe_image_io exception_data_io(image);
    exception_data_io.set_image_offset(rva);

    _unwind_info info;

    if (exception_data_io.read(&info, sizeof(info)) != enma_io_code::enma_io_success) {
        return pe_directory_code::pe_directory_code_currupted;
    }

    {
        pe_exception_unwind_info _unwind_entry(rva, info.version, info.flags, info.size_of_prolog,
            info.count_of_codes, info.frame_register, info.frame_offset);

        for (size_t idx = 0; idx < info.count_of_codes; idx++) {
            _unwind_code info_code;

            if (exception_data_io.read(&info_code, sizeof(info_code)) != enma_io_code::enma_io_success) {
                return pe_directory_code::pe_directory_code_currupted;
            }

            _unwind_entry.add_unwind_code(info_code);
        }

        exceptions.add_unwind_entry(_unwind_entry);
    }

    size_t unwind_entry_idx = exceptions.get_unwind_entries().size() - 1;

    exception_data_io.set_image_offset(
            ALIGN_UP(exception_data_io.get_image_offset(), 4)
        );

    if (info.flags) {

        if (info.flags & UNW_FLAG_CHAININFO) {
            runtime_function_entry exc_entry;

            if (exception_data_io.read(&exc_entry, sizeof(exc_entry)) != enma_io_code::enma_io_success) {
                return pe_directory_code::pe_directory_code_currupted;
            }

            pe_exception_entry * exception_chained_entry = new pe_exception_entry(exc_entry.begin_address, exc_entry.end_address, exc_entry.unwind_data);

            if (exc_entry.unwind_data) {

                if (handle_unwind_info(image, exc_entry.unwind_data, exceptions) == pe_directory_code_currupted) {
                    delete exception_chained_entry;

                    return pe_directory_code_currupted;
                }
            }

            exceptions.get_unwind_entries()[unwind_entry_idx].set_chained_entry(exception_chained_entry);
        }
        else {

            if ( (info.flags & UNW_FLAG_EHANDLER) || (info.flags & UNW_FLAG_UHANDLER)) {
                uint32_t handler_rva = 0;

                if (exception_data_io.read(&handler_rva, sizeof(handler_rva)) != enma_io_code::enma_io_success) {
                    return pe_directory_code::pe_directory_code_currupted;
                }

                exceptions.get_unwind_entries()[unwind_entry_idx].set_handler_rva(handler_rva);
            }
        }
    }


    return pe_directory_code::pe_directory_code_success;
}


pe_directory_code enma::get_exception_directory(const pe_image &image, pe_exceptions_directory& exceptions) {
    exceptions.get_exception_entries().clear();

    uint32_t virtual_address = image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_EXCEPTION);
    uint32_t virtual_size    = image.get_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_EXCEPTION);

    if (virtual_address && virtual_size /*req size*/) {
        pe_image_io exception_io(image);

        if (!exception_io.is_present_rva(virtual_address)) {
            return pe_directory_code::pe_directory_code_not_present;
        }

        exception_io.set_image_offset(virtual_address);

        while (exception_io.get_image_offset() < virtual_address + virtual_size) {
            runtime_function_entry exc_entry;

            if (exception_io.read(&exc_entry, sizeof(exc_entry)) != enma_io_code::enma_io_success) {

                return pe_directory_code::pe_directory_code_currupted;
            }

            if (exc_entry.unwind_info_address) {
                if (handle_unwind_info(image, exc_entry.unwind_info_address, exceptions) == pe_directory_code_currupted) {
                    return pe_directory_code::pe_directory_code_currupted;
                }
            }

            exceptions.add_item(exc_entry);
        }

        return pe_directory_code::pe_directory_code_success;
    }

    return pe_directory_code::pe_directory_code_not_present;
}

bool enma::build_exceptions_directory(pe_image &image, pe_section& section, pe_exceptions_directory& exceptions,
        pe_relocations_directory& relocs, bool build_unwindinfo) {


    if (exceptions.get_exception_entries().size()) {
        pe_section_io exc_section(section, image, enma_io_mode_allow_expand);

        
        if (build_unwindinfo) {
            std::map<size_t, size_t> unwind_info_map;

            exc_section.seek_to_end().align_up(0x10);

            for (auto & item : exceptions.get_unwind_entries()) { //fill unwind info with codes and params
                exc_section.seek_to_end().align_up(0x4);

                unwind_info info;
                info.version = item.get_version();
                info.flags = item.get_flags();
                info.size_of_prolog = item.get_size_of_prolog();
                info.count_of_codes = item.get_count_of_codes();
                info.frame_register = item.get_frame_register();
                info.frame_offset = item.get_frame_offset();
                
                unwind_info_map[item.get_unwind_info_rva()] = exc_section.get_section_offset();
                item.set_unwind_info_rva((uint32_t)exc_section.get_section_offset());

                if (exc_section.write(&info, sizeof(unwind_info)) != enma_io_success) {
                    return false;
                }

                for (auto & unwind_code_entry : item.get_codes()) {
                    unwind_code code;
                    code.frame_offset = unwind_code_entry.frame_offset;

                    if (exc_section.write(&code, sizeof(unwind_code)) != enma_io_success) {
                        return false;
                    }
                }

                exc_section.seek_to_end().align_up(0x4);

                if (item.get_flags() & 4) { //chain info
                    runtime_function_entry exc_entry = { 0 }; //be filled after 

                    if (exc_section.write(&exc_entry, sizeof(runtime_function_entry)) != enma_io_success) {
                        return false;
                    }
                }
                else if (item.get_flags() & 3) {//has handlers
                    uint32_t handler_rva = item.get_handler_rva();

                    if (exc_section.write(&handler_rva, sizeof(uint32_t)) != enma_io_success) {
                        return false;
                    }

                    for (auto& param : item.get_params()) { //fill handler params

                       
                        if (param.type == unwind_parameter_type::unwind_parameter_va) {
                            relocs.add_relocation(exc_section.get_section_offset(), 0, (image.is_x32_image() ? IMAGE_REL_BASED_HIGHLOW : IMAGE_REL_BASED_DIR64));
                        }

                        if (exc_section.write(param.param_data) != enma_io_success) {
                            return false;
                        }
                    }
                }
            }

            for (auto & item : exceptions.get_unwind_entries()) { //fill unwind chain entries
                
                if (item.get_flags() & 4) { //chain info
                    pe_exception_entry * entry = item.get_chained_entry();
                    runtime_function_entry exc_entry_;  
                    exc_entry_.begin_address = entry->get_begin_address();
                    exc_entry_.end_address = entry->get_end_address();
                    exc_entry_.unwind_data = (uint32_t)unwind_info_map[entry->get_unwind_data_address()];

                    entry->set_unwind_data_address(exc_entry_.unwind_data);

                    exc_section.set_section_offset( uint32_t(
                        item.get_unwind_info_rva() + 
                        sizeof(unwind_info) +
                        ALIGN_UP(sizeof(unwind_code) * item.get_codes().size(), 4)
                    ) );

                    if (exc_section.write(&exc_entry_, sizeof(runtime_function_entry)) != enma_io_success) {
                        return false;
                    }
                }
            }

            for (auto & item : exceptions.get_unwind_entries()) { //reset unwind info rva
                item.set_unwind_info_rva((uint32_t)unwind_info_map[item.get_unwind_info_rva()]);
            }

            for (auto & item : exceptions.get_exception_entries()) { //reset unwind info rva in runtime function entries
                item.set_unwind_data_address((uint32_t)unwind_info_map[item.get_unwind_data_address()]);
            }
        }

        exc_section.seek_to_end().align_up(0x10);

        size_t exc_virtual_address = exc_section.get_section_offset();

        for (auto & item : exceptions.get_exception_entries()) {
            runtime_function_entry entry = { item.get_begin_address(), item.get_end_address(), item.get_unwind_data_address() };

            if (exc_section.write(&entry, sizeof(entry)) != enma_io_success) {
                return false;
            }
        }

        image.set_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_EXCEPTION, (uint32_t)exc_virtual_address);
        image.set_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_EXCEPTION, 
            uint32_t(exceptions.get_exception_entries().size() * sizeof(runtime_function_entry)));
    }
    else {

        image.set_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_EXCEPTION, 0);
        image.set_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_EXCEPTION, 0);
    }  

    return true;
}


pe_directory_code enma::get_placement_exceptions_directory(const pe_image &image, pe_placement& placement) {

    pe_exceptions_directory _exceptions;
    pe_directory_code code = get_exception_directory(image, _exceptions);

    if (code != pe_directory_code::pe_directory_code_success) {
        return code;
    }

    uint32_t virtual_address = image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_EXCEPTION);
    uint32_t virtual_size = image.get_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_EXCEPTION);

    pe_image_io exception_io(image);
    if (!exception_io.is_present_rva(virtual_address)) {
        return pe_directory_code::pe_directory_code_not_present;
    }

    placement[virtual_address] =
        pe_placement_entry(virtual_size, id_pe_placement::id_pe_exception_descriptors, "");

    for (auto& unwind_entry : _exceptions.get_unwind_entries()) {

        pe_placement_entry dsp = pe_placement_entry(ALIGN_UP(
            sizeof(unwind_info) +
            (unwind_entry.get_count_of_codes() * sizeof(unwind_code)) +
            ((unwind_entry.get_flags() & (UNW_FLAG_EHANDLER | UNW_FLAG_UHANDLER)) ? sizeof(uint32_t) : 0) +
            ((unwind_entry.get_flags() & UNW_FLAG_CHAININFO) ? sizeof(runtime_function_entry) : 0)
            , 4),
            id_pe_placement::id_pe_exception_unwindinfo, "");

            placement[unwind_entry.get_unwind_info_rva()] = dsp;
            
    }
    

    return pe_directory_code::pe_directory_code_success;
}