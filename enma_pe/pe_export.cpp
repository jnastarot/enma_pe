#include "stdafx.h"
#include "pe_export.h"


export_table_item::export_table_item() {
	ordinal = 0;
	rva = 0;
	b_name = 0;
	b_forward = 0;
	name_ordinal = 0;
}
export_table_item::export_table_item(const export_table_item& item) {
    this->operator=(item);
}
export_table_item::~export_table_item() {};

export_table_item& export_table_item::operator=(const export_table_item& item) {

	this->rva	        = item.rva;
	this->ordinal       = item.ordinal;
	this->name_ordinal  = item.name_ordinal;
	this->b_name        = item.b_name;
	this->b_forward     = item.b_forward;
	this->forward_name  = item.forward_name;
    this->func_name     = item.func_name;

    return *this;
}

bool export_table_item::operator==(const export_table_item& item) {
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

void  export_table_item::set_rva(uint32_t _rva) {
	this->rva = _rva;
}
void  export_table_item::set_ordinal(uint16_t _ordinal) {
	this->ordinal = _ordinal;
}
void  export_table_item::set_name_ordinal(uint16_t _ordinal) {
	this->name_ordinal = _ordinal;
}
void  export_table_item::set_has_name(bool b) {
	this->b_name = b;
}
void  export_table_item::set_forward(bool b) {
	this->b_forward = b;
}
void  export_table_item::set_forward_name(const std::string& forward_name) {
	this->b_forward = true;
	this->forward_name = forward_name;
}
void  export_table_item::set_func_name(const std::string& func_name) {
	this->b_name = true;
	this->func_name = func_name;
}
uint32_t export_table_item::get_rva() const {
	return rva;
}
uint16_t  export_table_item::get_ordinal() const {
	return ordinal;
}
uint16_t  export_table_item::get_name_ordinal() const {
	return name_ordinal;
}
bool  export_table_item::has_name() const {
	return b_name;
}
bool  export_table_item::is_forward() const {
	return b_forward;
}
std::string  export_table_item::get_forward_name() const {
	return forward_name;
}
std::string  export_table_item::get_func_name() const {
	return func_name;
}

export_table::export_table() {
	characteristics = 0;
	time_stamp = 0;
	major_version = 0;
	minor_version = 0;
	ordinal_base = 0;
	number_of_functions = 0;
	number_of_names = 0;
}
export_table::export_table(const export_table& exports) {
    this->operator=(exports);
}
export_table::~export_table() {};

export_table& export_table::operator=(const export_table& exports) {

    this->characteristics       = exports.characteristics;
    this->time_stamp            = exports.time_stamp;
    this->major_version         = exports.major_version;
    this->minor_version         = exports.minor_version;
    this->ordinal_base          = exports.ordinal_base;
    this->number_of_functions   = exports.number_of_functions;
    this->number_of_names       = exports.number_of_names;
    this->library_name          = exports.library_name;
    this->export_items          = exports.export_items;

    return *this;
}

void export_table::set_characteristics(uint32_t characteristics) {
	this->characteristics = characteristics;
}
void export_table::set_time_stamp(uint32_t time_stamp) {
	this->time_stamp = time_stamp;
}
void export_table::set_major_version(uint16_t major_version) {
	this->major_version = major_version;
}
void export_table::set_minor_version(uint16_t minor_version) {
	this->minor_version = minor_version;
}
void export_table::set_ordinal_base(uint32_t  ordinal_base) {
	this->ordinal_base = ordinal_base;
}
void export_table::set_number_of_functions(uint32_t  number_of_functions){
	this->number_of_functions = number_of_functions;
}
void export_table::set_number_of_names(uint32_t  number_of_names) {
	this->number_of_names = number_of_names;
}
void export_table::set_library_name(const std::string& library_name) {
	this->library_name = library_name;
}
void export_table::add_item(const export_table_item& item) {
	export_items.push_back(item);
}

uint32_t			export_table::get_characteristics() const {
	return this->characteristics;
}
uint32_t			export_table::get_time_stamp() const {
	return this->time_stamp;
}
uint16_t			export_table::get_major_version() const {
	return this->major_version;
}
uint16_t			export_table::get_minor_version() const {
	return this->minor_version;
}
uint32_t	export_table::get_ordinal_base() const {
	return this->ordinal_base;
}
uint32_t	export_table::get_number_of_functions() const {
	return this->number_of_functions;
}
uint32_t	export_table::get_number_of_names() const {
	return this->number_of_names;
}
std::string		export_table::get_library_name() const {
	return this->library_name;
}
const std::vector<export_table_item>& export_table::get_items() const {
    return this->export_items;
}
std::vector<export_table_item>& export_table::get_items() {
	return this->export_items;
}

bool export_table::get_exported_function(const std::string& func_name, export_table_item * &_item) {

	for (auto &item : export_items) {
		if (item.has_name() && item.get_func_name() == func_name) {
			_item = &item;
			return true;
		}
	}
	return false;
}
bool export_table::get_exported_function(uint16_t ordinal, export_table_item * &_item) {

	for (auto &item : export_items) {
		if (!item.has_name() && item.get_ordinal() == ordinal) {
			_item = &item;
			return true;
		}
	}
	return false;
}

directory_code get_export_table(const pe_image &image, export_table& exports) {
	exports.set_characteristics(0);
	exports.set_time_stamp(0);
	exports.set_major_version(0);
	exports.set_minor_version(0);
	exports.set_ordinal_base(0);
	exports.set_number_of_functions(0);
	exports.set_number_of_names(0);
	exports.set_library_name("");
	exports.get_items().clear();


	uint32_t virtual_address = image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_EXPORT);
	uint32_t virtual_size = image.get_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_EXPORT);


    if (virtual_address) {
        pe_image_io export_io(image);

        image_export_directory export_desc;

        if (export_io.set_image_offset(virtual_address).read(&export_desc, sizeof(export_desc)) != enma_io_success) {
            return directory_code::directory_code_currupted;
        }

        exports.set_characteristics(export_desc.characteristics);
        exports.set_time_stamp(export_desc.time_date_stamp);
        exports.set_major_version(export_desc.major_version);
        exports.set_minor_version(export_desc.minor_version);
        exports.set_ordinal_base(export_desc.base);
        exports.set_number_of_functions(export_desc.number_of_functions);
        exports.set_number_of_names(export_desc.number_of_names);

        if (!exports.get_number_of_functions()) { return directory_code::directory_code_success; }

        if (export_desc.name) {
            std::string lib_name;
            if (export_io.set_image_offset(export_desc.name).read_string(lib_name) != enma_io_success) {
                return directory_code::directory_code_currupted;
            }

            exports.set_library_name(lib_name);
        }

        for (uint32_t ordinal = 0; ordinal < export_desc.number_of_functions; ordinal++) {

            uint32_t func_rva;

            export_table_item func;

            if (export_io.set_image_offset(export_desc.address_of_functions + ordinal * sizeof(uint32_t)).read(
                &func_rva, sizeof(func_rva)) != enma_io_success) {
                return directory_code::directory_code_currupted;
            }

            func.set_rva(func_rva);

            if (!func_rva) { continue; }

            func.set_ordinal(uint16_t(export_desc.base + ordinal));

            for (uint32_t i = 0; i < export_desc.number_of_names; i++) {

                uint16_t ordinal2;

                if (export_io.set_image_offset(export_desc.address_of_name_ordinals + i * sizeof(uint16_t)).read(
                    &ordinal2, sizeof(ordinal2)) != enma_io_success) {
                    return directory_code::directory_code_currupted;
                }


                if (ordinal == ordinal2) {

                    uint32_t function_name_rva;

                    if (export_io.set_image_offset(export_desc.address_of_names + i * sizeof(uint32_t)).read(
                        &function_name_rva, sizeof(function_name_rva)) != enma_io_success) {
                        return directory_code::directory_code_currupted;
                    }

                    std::string func_name;

                    if (export_io.set_image_offset(function_name_rva).read_string(func_name) != enma_io_success) {
                        return directory_code::directory_code_currupted;
                    }

                    func.set_func_name(func_name);
                    func.set_has_name(true);
                    func.set_name_ordinal(ordinal2);

                    if (func_rva >= virtual_address + sizeof(image_export_directory) &&
                        func_rva < virtual_address + virtual_size) {

                        std::string forwarded_func_name;

                        if (export_io.set_image_offset(func_rva).read_string(forwarded_func_name) != enma_io_success) {
                            return directory_code::directory_code_currupted;
                        }

                        func.set_forward_name(std::string(forwarded_func_name));
                        func.set_forward(true);
                    }

                    break;
                }
            }

            exports.add_item(func);
        }

        return directory_code::directory_code_success;
    }

	return directory_code::directory_code_not_present;
}

bool build_export_table(pe_image &image, pe_section& section, export_table& exports) {//taken from pe bless

	if (!exports.get_items().size()) { return true; }

    pe_section_io export_io(section, image, enma_io_mode_allow_expand);
    export_io.align_up(0x10).seek_to_end();


	uint32_t needed_size_for_strings = uint32_t(exports.get_library_name().length() + 1);
	uint32_t number_of_names			= 0;
	uint32_t max_ordinal				= 0;
	uint32_t ordinal_base				= -1;


	if (exports.get_items().size()) {
		ordinal_base = exports.get_ordinal_base();
	}

	uint32_t needed_size_for_function_names = 0;
	uint32_t needed_size_for_function_forwards = 0;

	for (export_table_item& func : exports.get_items()) {
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
	uint32_t needed_size_for_function_name_ordinals = number_of_names * sizeof(uint16_t);
	uint32_t needed_size_for_function_name_rvas     = number_of_names * sizeof(uint32_t);
	uint32_t needed_size_for_function_addresses     = (max_ordinal - ordinal_base + 1) * sizeof(uint32_t);
    
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
    export_desc.name                    = directory_pos + sizeof(image_export_directory);

    if (export_io.write(&export_desc, sizeof(export_desc)) != enma_io_success) {
        return false;
    }

    if (export_io.write((void*)exports.get_library_name().c_str(), exports.get_library_name().length() + 1) != enma_io_success) {
        return false;
    }
    export_io.align_up(0x2);


	typedef std::map<std::string, uint16_t> funclist;
	funclist funcs;

	uint32_t last_ordinal = ordinal_base;
	for (export_table_item &func : exports.get_items()) {
		if (func.get_ordinal() > last_ordinal){

			uint32_t len = sizeof(uint32_t) * (func.get_ordinal() - last_ordinal - 1);
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
            
            current_pos_of_function_addresses += sizeof(uint32_t);

            if (export_io.set_section_offset(current_pos_of_function_forwards).write(
                (void*)func.get_forward_name().c_str(), func.get_forward_name().length() + 1) != enma_io_success) {

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

			current_pos_of_function_addresses += sizeof(function_rva);
		}
	}

	for (funclist::const_iterator it = funcs.begin(); it != funcs.end(); ++it){


        if (export_io.set_section_offset(current_pos_of_function_names_rvas).write(
            &current_pos_of_function_names, sizeof(current_pos_of_function_names)) != enma_io_success) {

            return false;
        }

		current_pos_of_function_names_rvas += sizeof(uint32_t);

        if (export_io.set_section_offset(current_pos_of_function_names).write(
           (void*) (*it).first.c_str(), (*it).first.length() + 1) != enma_io_success) {

            return false;
        }

		current_pos_of_function_names += static_cast<uint32_t>((*it).first.length() + 1);


        if (export_io.set_section_offset(current_pos_of_function_name_ordinals).write(
            (void*)&((*it).second), sizeof((*it).second)) != enma_io_success) {

            return false;
        }

		current_pos_of_function_name_ordinals += sizeof(uint16_t);
	}


	image.set_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_EXPORT, directory_pos);
	image.set_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_EXPORT, 
        sizeof(image_export_directory) +
        needed_size_for_function_name_ordinals + 
        needed_size_for_function_addresses +
        needed_size_for_strings +
        needed_size_for_function_name_rvas
    );

    return true;
}

directory_code get_placement_export_table(const pe_image &image, std::vector<directory_placement>& placement) {
   
    uint32_t virtual_address = image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_EXPORT);
    uint32_t virtual_size    = image.get_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_EXPORT);


    if (virtual_address) {
        pe_image_io export_io(image);

        image_export_directory export_desc;

        if (export_io.set_image_offset(virtual_address).read(&export_desc, sizeof(export_desc)) != enma_io_success) {
            return directory_code::directory_code_currupted;
        }

        if (!export_desc.number_of_functions) { return directory_code::directory_code_success; }

        if (export_desc.name) {
            std::string lib_name;

            if (export_io.set_image_offset(export_desc.name).read_string(lib_name) != enma_io_success) {
                return directory_code::directory_code_currupted;
            }

            placement.push_back({ export_desc.name, ALIGN_UP(lib_name.length() + 1,0x2),dp_id_export_names });
        }


        for (uint32_t ordinal = 0; ordinal < export_desc.number_of_functions; ordinal++) {

            uint32_t func_rva;

            if (export_io.set_image_offset(export_desc.address_of_functions + ordinal * sizeof(uint32_t)).read(
                &func_rva, sizeof(func_rva)) != enma_io_success) {
                return directory_code::directory_code_currupted;
            }

            if (!func_rva) { continue; }


            for (uint32_t i = 0; i < export_desc.number_of_names; i++) {

                uint16_t ordinal2;

                if (export_io.set_image_offset(export_desc.address_of_name_ordinals + i * sizeof(uint16_t)).read(
                    &ordinal2, sizeof(ordinal2)) != enma_io_success) {
                    return directory_code::directory_code_currupted;
                }

                if (ordinal == ordinal2) {

                    uint32_t function_name_rva;

                    if (export_io.set_image_offset(export_desc.address_of_names + i * sizeof(uint32_t)).read(
                        &function_name_rva, sizeof(function_name_rva)) != enma_io_success) {
                        return directory_code::directory_code_currupted;
                    }

                    std::string func_name;

                    if (export_io.set_image_offset(function_name_rva).read_string(func_name) != enma_io_success) {
                        return directory_code::directory_code_currupted;
                    }

                    placement.push_back({ function_name_rva,ALIGN_UP(func_name.length() + 1,0x2),dp_id_export_names });

                    if (func_rva >= virtual_address + sizeof(image_export_directory) &&
                        func_rva < virtual_address + virtual_size) {

                        std::string forwarded_func_name;

                        if (export_io.set_image_offset(func_rva).read_string(forwarded_func_name) != enma_io_success) {
                            return directory_code::directory_code_currupted;
                        }

                        placement.push_back({ func_rva,ALIGN_UP(forwarded_func_name.length() + 1,0x2),dp_id_export_names });
                    }

                    break;
                }
            }
        }

        if (export_desc.address_of_functions) {
            placement.push_back({ export_desc.address_of_functions,export_desc.number_of_functions * sizeof(uint32_t),
                dp_id_export_func_table
            });
        }

        if (export_desc.address_of_names) {
            placement.push_back({ export_desc.address_of_names,export_desc.number_of_names * sizeof(uint32_t),
                dp_id_export_name_table
            });
        }

        if (export_desc.address_of_name_ordinals) {
            placement.push_back({ export_desc.address_of_name_ordinals,export_desc.number_of_functions * sizeof(uint16_t),
                dp_id_export_ordinal_table
            });
        }

        placement.push_back({ virtual_address,ALIGN_UP(sizeof(image_export_directory),0x10),dp_id_export_desc });
        return directory_code::directory_code_success;
    }

    return directory_code::directory_code_not_present;
}


