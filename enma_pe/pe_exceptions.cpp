
#include "stdafx.h"
#include "pe_exceptions.h"


exception_unwind_info::exception_unwind_info() {

}

exception_unwind_info::exception_unwind_info(uint32_t unwind_info_rva, uint8_t version,
    uint8_t flags, uint8_t size_of_prolog, uint8_t count_of_codes, uint8_t frame_register, uint8_t frame_offset) 
    :unwind_info_rva(unwind_info_rva), version(version), flags(flags), size_of_prolog(size_of_prolog), 
    count_of_codes(count_of_codes), frame_register(frame_register), frame_offset(frame_offset), 
    handler_rva(0), chained_entry(0), custom_parameter(0), custom_id(0) {}

exception_unwind_info::exception_unwind_info(const exception_unwind_info& item) {
    this->operator=(item);
}

exception_unwind_info::~exception_unwind_info() {
    if (chained_entry) {
        delete chained_entry;
        chained_entry = 0;
    }
}

exception_unwind_info& exception_unwind_info::operator=(const exception_unwind_info& item) {
    this->unwind_info_rva = item.unwind_info_rva;
    this->version = item.version;
    this->flags = item.flags;
    this->size_of_prolog = item.size_of_prolog;
    this->count_of_codes = item.count_of_codes;
    this->frame_register = item.frame_register;
    this->frame_offset = item.frame_offset;
    this->handler_rva = item.handler_rva;
    this->chained_entry = item.chained_entry;
    this->codes = item.codes;
    this->params = item.params;

    return *this;
}


void exception_unwind_info::add_unwind_code(const unwind_code& code) {
    codes.push_back(code);
}

void exception_unwind_info::set_version(uint8_t version) {
    this->version = version;
}

void exception_unwind_info::set_flags(uint8_t flags) {
    this->flags = flags;
}

void exception_unwind_info::set_size_of_prolog(uint8_t size_of_prolog) {
    this->size_of_prolog = size_of_prolog;
}

void exception_unwind_info::set_count_of_codes(uint8_t count_of_codes) {
    this->count_of_codes = count_of_codes;
}

void exception_unwind_info::set_frame_register(uint8_t frame_register) {
    this->frame_register = frame_register;
}

void exception_unwind_info::set_frame_offset(uint8_t frame_offset) {
    this->frame_offset = frame_offset;
}

void exception_unwind_info::set_chained_entry(exception_entry * chained_entry) {
    this->chained_entry = chained_entry;
}

void exception_unwind_info::set_unwind_info_rva(uint32_t rva) {
    this->unwind_info_rva = rva;
}

void exception_unwind_info::set_codes(std::vector<unwind_code> &codes) {
    this->codes = codes;
}

void exception_unwind_info::set_params(std::vector<unwind_parameter> &params) {
    this->params = params;
}

void exception_unwind_info::set_custom_parameter(void * custom_parameter) {
    this->custom_parameter = custom_parameter;
}

void exception_unwind_info::set_custom_id(uint32_t custom_id) {
    this->custom_id = custom_id;
}

void exception_unwind_info::set_handler_rva(uint32_t rva) {
    this->handler_rva = rva;
}

uint8_t exception_unwind_info::get_version() const {
    return this->version;
}

uint8_t exception_unwind_info::get_flags() const {
    return this->flags;
}

uint8_t exception_unwind_info::get_size_of_prolog() const {
    return this->size_of_prolog;
}

uint8_t exception_unwind_info::get_count_of_codes() const {
    return this->count_of_codes;
}

uint8_t exception_unwind_info::get_frame_register() const {
    return this->frame_register;
}
uint8_t exception_unwind_info::get_frame_offset() const {
    return this->frame_offset;
}

std::vector<unwind_code>& exception_unwind_info::get_codes() {
    return this->codes;
}

const std::vector<unwind_code>& exception_unwind_info::get_codes() const {
    return this->codes;
}

std::vector<unwind_parameter>& exception_unwind_info::get_params() {
    return this->params;
}

const std::vector<unwind_parameter>& exception_unwind_info::get_params() const {
    return this->params;
}

void * exception_unwind_info::get_custom_parameter() {
    return this->custom_parameter;
}

const void * exception_unwind_info::get_custom_parameter() const {
    return this->custom_parameter;
}

uint32_t exception_unwind_info::get_custom_id() const {
    return this->custom_id;
}

const exception_entry * exception_unwind_info::get_chained_entry() const {
    return this->chained_entry;
}

exception_entry * exception_unwind_info::get_chained_entry() {
    return this->chained_entry;
}

uint32_t exception_unwind_info::get_handler_rva() const {
    return this->handler_rva;
}

uint32_t exception_unwind_info::get_unwind_info_rva() const {
    return this->unwind_info_rva;
}

exception_entry::exception_entry() :
    address_begin(0), address_end(0), address_unwind_data(0) {}

exception_entry::exception_entry(const exception_entry& entry) {
    this->operator=(entry);
}
exception_entry::exception_entry(uint32_t address_begin, uint32_t address_end, uint32_t address_unwind_data):
    address_begin(address_begin), address_end(address_end), address_unwind_data(address_unwind_data){}

exception_entry::~exception_entry() {
}

exception_entry& exception_entry::operator=(const exception_entry& entry) {

    this->address_begin = entry.address_begin;
    this->address_end = entry.address_end;
    this->address_unwind_data = entry.address_unwind_data;

    return *this;
}

void exception_entry::set_begin_address(uint32_t rva_address) {
    this->address_begin = rva_address;
}
void exception_entry::set_end_address(uint32_t rva_address) {
    this->address_end = rva_address;
}
void exception_entry::set_unwind_data_address(uint32_t rva_address) {
    this->address_unwind_data = rva_address;
}

uint32_t exception_entry::get_begin_address() const {
    return address_begin;
}
uint32_t exception_entry::get_end_address() const {
    return address_end;
}
uint32_t exception_entry::get_unwind_data_address() const {
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
    this->exception_entries = exceptions.exception_entries;

    return *this;
}

void exceptions_table::add_exception_entry(uint32_t address_begin, uint32_t address_end, uint32_t address_unwind_data) {
    exception_entries.push_back(exception_entry(address_begin, address_end, address_unwind_data));
}
void exceptions_table::add_exception_entry(const exception_entry& entry) {
    exception_entries.push_back(entry);
}

void exceptions_table::add_unwind_entry(const exception_unwind_info& entry) {
    this->unwind_entries.push_back(entry);
}

void exceptions_table::add_item(const runtime_function_entry& exc_entry) {
    exception_entries.push_back(exception_entry(exc_entry.begin_address, exc_entry.end_address, exc_entry.unwind_info_address));
}
void exceptions_table::clear() {
    this->exception_entries.clear();
}

size_t exceptions_table::size() const {
    return exception_entries.size();
}

const std::vector<exception_unwind_info>& exceptions_table::get_unwind_entries() const {
    return this->unwind_entries;
}

std::vector<exception_unwind_info>& exceptions_table::get_unwind_entries() {
    return this->unwind_entries;
}

const std::vector<exception_entry>& exceptions_table::get_exception_entries() const {
    return exception_entries;
}

std::vector<exception_entry>& exceptions_table::get_exception_entries() {
    return exception_entries;
}



directory_code handle_unwind_info(const pe_image &image, uint32_t rva, exceptions_table& exceptions) {
   
    for (auto& entry : exceptions.get_unwind_entries()) {
        if (entry.get_unwind_info_rva() == rva) { 
            return directory_code::directory_code_success; 
        }
    }

    pe_image_io exception_data_io(image);
    exception_data_io.set_image_offset(rva);

    _unwind_info info;

    if (exception_data_io.read(&info, sizeof(info)) != enma_io_code::enma_io_success) {
        return directory_code::directory_code_currupted;
    }

    {
        exception_unwind_info _unwind_entry(rva, info.version, info.flags, info.size_of_prolog,
            info.count_of_codes, info.frame_register, info.frame_offset);

        for (size_t idx = 0; idx < info.count_of_codes; idx++) {
            _unwind_code info_code;

            if (exception_data_io.read(&info_code, sizeof(info_code)) != enma_io_code::enma_io_success) {
                return directory_code::directory_code_currupted;
            }

            _unwind_entry.add_unwind_code(info_code);
        }

        exceptions.add_unwind_entry(_unwind_entry);
    }

    auto& unwind_entry = exceptions.get_unwind_entries()[exceptions.get_unwind_entries().size() - 1];


    exception_data_io.set_image_offset(
            ALIGN_UP(exception_data_io.get_image_offset(), 4)
        );

    if (info.flags) {

        if (info.flags & UNW_FLAG_CHAININFO) {
            runtime_function_entry exc_entry;

            if (exception_data_io.read(&exc_entry, sizeof(exc_entry)) != enma_io_code::enma_io_success) {
                return directory_code::directory_code_currupted;
            }

            exception_entry * exception_chained_entry = new exception_entry(exc_entry.begin_address, exc_entry.end_address, exc_entry.unwind_data);

            if (exc_entry.unwind_data) {

                if (handle_unwind_info(image, exc_entry.unwind_data, exceptions) == directory_code_currupted) {
                    delete exception_chained_entry;

                    return directory_code_currupted;
                }
            }

            unwind_entry.set_chained_entry(exception_chained_entry);
        }
        else {

            if ( (info.flags & UNW_FLAG_EHANDLER) || (info.flags & UNW_FLAG_UHANDLER)) {
                uint32_t handler_rva = 0;

                if (exception_data_io.read(&handler_rva, sizeof(handler_rva)) != enma_io_code::enma_io_success) {
                    return directory_code::directory_code_currupted;
                }

                unwind_entry.set_handler_rva(handler_rva);
            }
        }
    }


    return directory_code::directory_code_success;
}


directory_code get_exception_table(const pe_image &image, exceptions_table& exceptions) {
    exceptions.get_exception_entries().clear();

    uint32_t virtual_address = image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_EXCEPTION);
    uint32_t virtual_size    = image.get_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_EXCEPTION);

    if (virtual_address && virtual_size /*req size*/) {
        pe_image_io exception_io(image);
        exception_io.set_image_offset(virtual_address);

        while (exception_io.get_image_offset() < virtual_address + virtual_size) {
            runtime_function_entry exc_entry;

            if (exception_io.read(&exc_entry, sizeof(exc_entry)) != enma_io_code::enma_io_success) {

                return directory_code::directory_code_currupted;
            }

            if (exc_entry.unwind_info_address) {
                if (handle_unwind_info(image, exc_entry.unwind_info_address, exceptions) == directory_code_currupted) {
                    return directory_code::directory_code_currupted;
                }
            }

            exceptions.add_item(exc_entry);
        }

        return directory_code::directory_code_success;
    }

	return directory_code::directory_code_not_present;
}

bool build_exceptions_table(pe_image &image, pe_section& section, exceptions_table& exceptions, 
        relocation_table& relocs, bool build_unwindinfo) {


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

                        if (param.type == unwind_parameter_va) {
                            relocs.add_item(exc_section.get_section_offset(), 0);
                        }

                        if (exc_section.write(param.param_data) != enma_io_success) {
                            return false;
                        }
                    }
                }
            }

            for (auto & item : exceptions.get_unwind_entries()) { //fill unwind chain entries
                
                if (item.get_flags() & 4) { //chain info
                    exception_entry * entry = item.get_chained_entry();
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

directory_code get_placement_exceptions_table(const pe_image &image, pe_directory_placement& placement) {

    exceptions_table _exceptions;
    directory_code code = get_exception_table(image, _exceptions);

    if (code != directory_code::directory_code_success) {
        return code;
    }

    uint32_t virtual_address = image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_EXCEPTION);
    uint32_t virtual_size = image.get_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_EXCEPTION);

    placement[virtual_address] =
        directory_placement(virtual_size, id_pe_exception_descriptors, "");

    for (auto& unwind_entry : _exceptions.get_unwind_entries()) {
        placement[unwind_entry.get_unwind_info_rva()] =
            directory_placement( ALIGN_UP(
                sizeof(unwind_info) + 
                (unwind_entry.get_count_of_codes() * 2) + 
                unwind_entry.get_handler_rva() ? sizeof(uint32_t) : 0                
                , 4 ) , 
                id_pe_exception_unwindinfo, "");
    }
    

    return directory_code::directory_code_success;
}