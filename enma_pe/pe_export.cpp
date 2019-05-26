#include "stdafx.h"
#include "pe_export.h"


pe_export_entry::pe_export_entry() {
	ordinal = 0;
	rva = 0;
	b_name = 0;
	b_forward = 0;
	name_ordinal = 0;
}
pe_export_entry::pe_export_entry(const pe_export_entry& item) {
    this->operator=(item);
}
pe_export_entry::~pe_export_entry() {};

pe_export_entry& pe_export_entry::operator=(const pe_export_entry& item) {

	this->rva	        = item.rva;
	this->ordinal       = item.ordinal;
	this->name_ordinal  = item.name_ordinal;
	this->b_name        = item.b_name;
	this->b_forward     = item.b_forward;
	this->forward_name  = item.forward_name;
    this->func_name     = item.func_name;

    return *this;
}

bool pe_export_entry::operator==(const pe_export_entry& item) {
	if (item.b_name == this->b_name) {
		if (item.b_name) {
			if (item.func_name == this->func_name) { return true; }
		}
		else {
			if (item.name_ordinal == this->name_ordinal) { return true; }
		}
	}

	return false;
}

void  pe_export_entry::set_rva(uint32_t _rva) {
	this->rva = _rva;
}
void  pe_export_entry::set_ordinal(uint16_t _ordinal) {
	this->ordinal = _ordinal;
}
void  pe_export_entry::set_name_ordinal(uint16_t _ordinal) {
	this->name_ordinal = _ordinal;
}
void  pe_export_entry::set_has_name(bool b) {
	this->b_name = b;
}
void  pe_export_entry::set_forward(bool b) {
	this->b_forward = b;
}
void  pe_export_entry::set_forward_name(const std::string& forward_name) {
	this->b_forward = true;
	this->forward_name = forward_name;
}
void  pe_export_entry::set_func_name(const std::string& func_name) {
	this->b_name = true;
	this->func_name = func_name;
}
uint32_t pe_export_entry::get_rva() const {
	return rva;
}
uint16_t  pe_export_entry::get_ordinal() const {
	return ordinal;
}
uint16_t  pe_export_entry::get_name_ordinal() const {
	return name_ordinal;
}
bool  pe_export_entry::has_name() const {
	return b_name;
}
bool  pe_export_entry::is_forward() const {
	return b_forward;
}
std::string  pe_export_entry::get_forward_name() const {
	return forward_name;
}
std::string  pe_export_entry::get_func_name() const {
	return func_name;
}

pe_export_directory::pe_export_directory() {
	characteristics = 0;
	time_stamp = 0;
	major_version = 0;
	minor_version = 0;
	ordinal_base = 0;
	number_of_functions = 0;
	number_of_names = 0;
}
pe_export_directory::pe_export_directory(const pe_export_directory& exports) {
    this->operator=(exports);
}
pe_export_directory::~pe_export_directory() {};

pe_export_directory& pe_export_directory::operator=(const pe_export_directory& exports) {

    this->characteristics       = exports.characteristics;
    this->time_stamp            = exports.time_stamp;
    this->major_version         = exports.major_version;
    this->minor_version         = exports.minor_version;
    this->ordinal_base          = exports.ordinal_base;
    this->number_of_functions   = exports.number_of_functions;
    this->number_of_names       = exports.number_of_names;
    this->library_name          = exports.library_name;
    this->entries          = exports.entries;

    return *this;
}

void pe_export_directory::set_characteristics(uint32_t characteristics) {
	this->characteristics = characteristics;
}
void pe_export_directory::set_time_stamp(uint32_t time_stamp) {
	this->time_stamp = time_stamp;
}
void pe_export_directory::set_major_version(uint16_t major_version) {
	this->major_version = major_version;
}
void pe_export_directory::set_minor_version(uint16_t minor_version) {
	this->minor_version = minor_version;
}
void pe_export_directory::set_ordinal_base(uint32_t  ordinal_base) {
	this->ordinal_base = ordinal_base;
}
void pe_export_directory::set_number_of_functions(uint32_t  number_of_functions){
	this->number_of_functions = number_of_functions;
}
void pe_export_directory::set_number_of_names(uint32_t  number_of_names) {
	this->number_of_names = number_of_names;
}
void pe_export_directory::set_library_name(const std::string& library_name) {
	this->library_name = library_name;
}
void pe_export_directory::add_entry(const pe_export_entry& entry) {
    entries.push_back(entry);
}

uint32_t			pe_export_directory::get_characteristics() const {
	return this->characteristics;
}
uint32_t			pe_export_directory::get_time_stamp() const {
	return this->time_stamp;
}
uint16_t			pe_export_directory::get_major_version() const {
	return this->major_version;
}
uint16_t			pe_export_directory::get_minor_version() const {
	return this->minor_version;
}
uint32_t	pe_export_directory::get_ordinal_base() const {
	return this->ordinal_base;
}
uint32_t	pe_export_directory::get_number_of_functions() const {
	return this->number_of_functions;
}
uint32_t	pe_export_directory::get_number_of_names() const {
	return this->number_of_names;
}
std::string		pe_export_directory::get_library_name() const {
	return this->library_name;
}
const std::vector<pe_export_entry>& pe_export_directory::get_entries() const {
    return this->entries;
}
std::vector<pe_export_entry>& pe_export_directory::get_entries() {
	return this->entries;
}

bool pe_export_directory::get_exported_function(const std::string& func_name, pe_export_entry * &_entry) {

	for (auto &entry : entries) {
		if (entry.has_name() && entry.get_func_name() == func_name) {
            _entry = &entry;
			return true;
		}
	}
	return false;
}
bool pe_export_directory::get_exported_function(uint16_t ordinal, pe_export_entry * &_entry) {

	for (auto &entry : entries) {
		if (!entry.has_name() && entry.get_ordinal() == ordinal) {
            _entry = &entry;
			return true;
		}
	}
	return false;
}

pe_directory_code get_export_directory(const pe_image &image, pe_export_directory& exports) {
	exports.set_characteristics(0);
	exports.set_time_stamp(0);
	exports.set_major_version(0);
	exports.set_minor_version(0);
	exports.set_ordinal_base(0);
	exports.set_number_of_functions(0);
	exports.set_number_of_names(0);
	exports.set_library_name("");
	exports.get_entries().clear();


	uint32_t virtual_address = image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_EXPORT);
	uint32_t virtual_size = image.get_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_EXPORT);


    if (virtual_address) {
        pe_image_io export_io(image);

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
                return pe_directory_code::pe_directory_code_currupted;
            }

            exports.set_library_name(lib_name);
        }

        for (uint32_t ordinal = 0; ordinal < export_desc.number_of_functions; ordinal++) {

            uint32_t func_rva;

            pe_export_entry func;

            if (export_io.set_image_offset(export_desc.address_of_functions + ordinal * INT32_SIZE).read(
                &func_rva, sizeof(func_rva)) != enma_io_success) {
                return pe_directory_code::pe_directory_code_currupted;
            }

            func.set_rva(func_rva);

            if (!func_rva) { continue; }

            func.set_ordinal(uint16_t(export_desc.base + ordinal));

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

                    func.set_func_name(func_name);
                    func.set_has_name(true);
                    func.set_name_ordinal(ordinal2);

                    if (func_rva >= virtual_address + sizeof(image_export_directory) &&
                        func_rva < virtual_address + virtual_size) {

                        std::string forwarded_func_name;

                        if (export_io.set_image_offset(func_rva).read_string(forwarded_func_name) != enma_io_success) {
                            return pe_directory_code::pe_directory_code_currupted;
                        }

                        func.set_forward_name(std::string(forwarded_func_name));
                        func.set_forward(true);
                    }

                    break;
                }
            }

            exports.add_entry(func);
        }

        return pe_directory_code::pe_directory_code_success;
    }

	return pe_directory_code::pe_directory_code_not_present;
}

bool build_export_directory(pe_image &image, pe_section& section, const pe_export_directory& exports) {

	if (!exports.get_entries().size()) { return true; }

    pe_section_io export_io(section, image, enma_io_mode_allow_expand);
    export_io.align_up(0x10).seek_to_end();


	uint32_t needed_size_for_strings = uint32_t(exports.get_library_name().length() + 1);
	uint32_t number_of_names			= 0;
	uint32_t max_ordinal				= 0;
	uint32_t ordinal_base				= -1;


	if (exports.get_entries().size()) {
		ordinal_base = exports.get_ordinal_base();
	}

	uint32_t needed_size_for_function_names = 0;
	uint32_t needed_size_for_function_forwards = 0;

	for (auto& func : exports.get_entries()) {
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
    export_desc.time_date_stamp	    = exports.get_time_stamp();
    export_desc.number_of_functions	= max_ordinal - ordinal_base + 1;
    export_desc.number_of_names	    = number_of_names;
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
	for (auto& func : exports.get_entries()) {
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

pe_directory_code get_placement_export_directory(const pe_image &image, pe_placement& placement) {
   
    uint32_t virtual_address = image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_EXPORT);
    uint32_t virtual_size    = image.get_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_EXPORT);


    if (virtual_address) {
        pe_image_io export_io(image);

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


            placement[export_desc.name] = pe_placement_entry(ALIGN_UP(lib_name.length() + 1, 0x2), id_pe_export_name, lib_name);
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
                        pe_placement_entry(ALIGN_UP(func_name.length() + 1, 0x2), id_pe_export_function_name, func_name);

                    if (func_rva >= virtual_address + sizeof(image_export_directory) &&
                        func_rva < virtual_address + virtual_size) {

                        std::string forwarded_func_name;

                        if (export_io.set_image_offset(func_rva).read_string(forwarded_func_name) != enma_io_success) {
                            return pe_directory_code::pe_directory_code_currupted;
                        }


                        placement[func_rva] =
                            pe_placement_entry(ALIGN_UP(forwarded_func_name.length() + 1, 0x2), id_pe_export_function_forwarded_name, forwarded_func_name);
                    }

                    break;
                }
            }
        }

        if (export_desc.address_of_functions) {
            placement[export_desc.address_of_functions] =
                pe_placement_entry(export_desc.number_of_functions * sizeof(uint32_t), id_pe_export_functions_table, "");
        }

        if (export_desc.address_of_names) {
            placement[export_desc.address_of_names] =
                pe_placement_entry(export_desc.number_of_names * sizeof(uint32_t), id_pe_export_names_table, "");
        }

        if (export_desc.address_of_name_ordinals) {
            placement[export_desc.address_of_name_ordinals] = 
                pe_placement_entry(export_desc.number_of_functions * sizeof(uint16_t), id_pe_export_ordinals_table, "");
        }

        placement[virtual_address] = pe_placement_entry(ALIGN_UP(sizeof(image_export_directory), 0x10), id_pe_export_descriptor, "");
        return pe_directory_code::pe_directory_code_success;
    }

    return pe_directory_code::pe_directory_code_not_present;
}


