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

void  export_table_item::set_rva(DWORD rva) {
	this->rva = rva;
}
void  export_table_item::set_ordinal(WORD ordinal) {
	this->ordinal = ordinal;
}
void  export_table_item::set_name_ordinal(WORD ordinal) {
	this->name_ordinal = ordinal;
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
DWORD export_table_item::get_rva() const {
	return rva;
}
WORD  export_table_item::get_ordinal() const {
	return ordinal;
}
WORD  export_table_item::get_name_ordinal() const {
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

void export_table::set_characteristics(DWORD characteristics) {
	this->characteristics = characteristics;
}
void export_table::set_time_stamp(DWORD time_stamp) {
	this->time_stamp = time_stamp;
}
void export_table::set_major_version(WORD major_version) {
	this->major_version = major_version;
}
void export_table::set_minor_version(WORD minor_version) {
	this->minor_version = minor_version;
}
void export_table::set_ordinal_base(unsigned int  ordinal_base) {
	this->ordinal_base = ordinal_base;
}
void export_table::set_number_of_functions(unsigned int  number_of_functions){
	this->number_of_functions = number_of_functions;
}
void export_table::set_number_of_names(unsigned int  number_of_names) {
	this->number_of_names = number_of_names;
}
void export_table::set_library_name(const std::string& library_name) {
	this->library_name = library_name;
}
void export_table::add_item(const export_table_item& item) {
	export_items.push_back(item);
}

DWORD			export_table::get_characteristics() const {
	return this->characteristics;
}
DWORD			export_table::get_time_stamp() const {
	return this->time_stamp;
}
WORD			export_table::get_major_version() const {
	return this->major_version;
}
WORD			export_table::get_minor_version() const {
	return this->minor_version;
}
unsigned int	export_table::get_ordinal_base() const {
	return this->ordinal_base;
}
unsigned int	export_table::get_number_of_functions() const {
	return this->number_of_functions;
}
unsigned int	export_table::get_number_of_names() const {
	return this->number_of_names;
}
std::string		export_table::get_library_name() const {
	return this->library_name;
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
bool export_table::get_exported_function(WORD ordinal, export_table_item * &_item) {

	for (auto &item : export_items) {
		if (!item.has_name() && item.get_ordinal() == ordinal) {
			_item = &item;
			return true;
		}
	}
	return false;
}

bool get_export_table(const pe_image &image, export_table& exports) {
	exports.set_characteristics(0);
	exports.set_time_stamp(0);
	exports.set_major_version(0);
	exports.set_minor_version(0);
	exports.set_ordinal_base(0);
	exports.set_number_of_functions(0);
	exports.set_number_of_names(0);
	exports.set_library_name("");
	exports.get_items().clear();


	DWORD virtual_address = image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_EXPORT);
	DWORD virtual_size = image.get_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_EXPORT);


	if (virtual_address) {

		pe_section * export_section = image.get_section_by_rva(virtual_address);

		if (export_section) {
			PIMAGE_EXPORT_DIRECTORY export_desc = (PIMAGE_EXPORT_DIRECTORY)&export_section->get_section_data().data()[virtual_address - export_section->get_virtual_address()];

			exports.set_characteristics(export_desc->Characteristics);
			exports.set_time_stamp(export_desc->TimeDateStamp);
			exports.set_major_version(export_desc->MajorVersion);
			exports.set_minor_version(export_desc->MinorVersion);
			exports.set_ordinal_base(export_desc->Base);
			exports.set_number_of_functions(export_desc->NumberOfFunctions);
			exports.set_number_of_names(export_desc->NumberOfNames);

			if (!exports.get_number_of_functions()) { return false; }

			if (export_desc->Name) {
				pe_section * name_export_section = image.get_section_by_rva(export_desc->Name);
				if (name_export_section) {
					exports.set_library_name(std::string((char*)(&export_section->get_section_data().data()[export_desc->Name - export_section->get_virtual_address()])));
				}
			}

			for (unsigned int ordinal = 0; ordinal < export_desc->NumberOfFunctions; ordinal++) {

				export_table_item func;

				func.set_rva(*(DWORD*)&image.get_section_by_rva((export_desc->AddressOfFunctions + ordinal * sizeof(DWORD)))->get_section_data().data()[
					(export_desc->AddressOfFunctions + ordinal * sizeof(DWORD))
						- image.get_section_by_rva((export_desc->AddressOfFunctions + ordinal * sizeof(DWORD)))->get_virtual_address()]
				);

				if (!func.get_rva()) { continue; }

				func.set_ordinal( WORD(export_desc->Base + ordinal));

				for (unsigned int i = 0; i < export_desc->NumberOfNames; i++) {

					WORD ordinal2 = *(WORD*)&image.get_section_by_rva((export_desc->AddressOfNameOrdinals + i * sizeof(WORD)))->get_section_data().data()[
						(export_desc->AddressOfNameOrdinals + i * sizeof(WORD))
							- image.get_section_by_rva((export_desc->AddressOfNameOrdinals + i * sizeof(WORD)))->get_virtual_address()];

					if (ordinal == ordinal2) {

						DWORD function_name_rva = *(DWORD*)&image.get_section_by_rva((export_desc->AddressOfNames + i * sizeof(DWORD)))->get_section_data().data()[
							(export_desc->AddressOfNames + i * sizeof(DWORD))
								- image.get_section_by_rva((export_desc->AddressOfNames + i * sizeof(DWORD)))->get_virtual_address()];


						const char* func_name = (char*)&image.get_section_by_rva(function_name_rva)->get_section_data().data()[
							function_name_rva
								- image.get_section_by_rva(function_name_rva)->get_virtual_address()];


						func.set_func_name(std::string(func_name));
						func.set_has_name(true);
						func.set_name_ordinal(ordinal2);

						if (func.get_rva() >= virtual_address + sizeof(IMAGE_EXPORT_DIRECTORY) &&
							func.get_rva() < virtual_address + virtual_size) {

							const char* forwarded_func_name = (char*)&image.get_section_by_rva(func.get_rva())->get_section_data().data()[
								func.get_rva() - image.get_section_by_rva(func.get_rva())->get_virtual_address()];

							func.set_forward_name(std::string(forwarded_func_name));
							func.set_forward(true);
						}

						break;
					}
				}

				exports.add_item(func);
			}

			return true;
		}
	}

	return false;
}

void build_export_table(pe_image &image, pe_section& section, export_table& exports) {//taken from pe bless

	if (!exports.get_items().size()) { return; }

    if (section.get_size_of_raw_data() & 0xF) {
        section.get_section_data().resize(
            section.get_section_data().size() + (0x10 - (section.get_section_data().size() & 0xF))
        );
    }

	DWORD needed_size_for_strings = (exports.get_library_name().length() + 1);
	DWORD number_of_names			= 0;
	DWORD max_ordinal				= 0;
	DWORD ordinal_base				= -1;


	if (exports.get_items().size()) {
		ordinal_base = exports.get_ordinal_base();
	}

	DWORD needed_size_for_function_names = 0;
	DWORD needed_size_for_function_forwards = 0;

	for (export_table_item& func : exports.get_items()) {
		max_ordinal = std::max<DWORD>(max_ordinal, func.get_ordinal());
		ordinal_base = std::min<DWORD>(ordinal_base, func.get_ordinal());

		if (func.has_name()) {
			++number_of_names;
			needed_size_for_function_names += DWORD(func.get_func_name().length() + 1);
		}
		if (func.is_forward()) {
			needed_size_for_function_forwards += DWORD(func.get_forward_name().length() + 1);
		}
	}

	needed_size_for_strings += needed_size_for_function_names;
	needed_size_for_strings += needed_size_for_function_forwards;
	DWORD needed_size_for_function_name_ordinals = number_of_names * sizeof(WORD);
	DWORD needed_size_for_function_name_rvas = number_of_names * sizeof(DWORD);
	DWORD needed_size_for_function_addresses = (max_ordinal - ordinal_base + 1) * sizeof(DWORD);


	DWORD directory_pos = section.get_size_of_raw_data();

	DWORD needed_size = sizeof(IMAGE_EXPORT_DIRECTORY);
	needed_size += needed_size_for_function_name_ordinals;
	needed_size += needed_size_for_function_addresses;
	needed_size += needed_size_for_strings;
	needed_size += needed_size_for_function_name_rvas;

	section.get_section_data().resize(needed_size + directory_pos);


	BYTE * raw_data = section.get_section_data().data();

	DWORD current_pos_of_function_names = DWORD(exports.get_library_name().length() + 1 + directory_pos + sizeof(IMAGE_EXPORT_DIRECTORY));
	DWORD current_pos_of_function_name_ordinals = current_pos_of_function_names + needed_size_for_function_names;
	DWORD current_pos_of_function_forwards = current_pos_of_function_name_ordinals + needed_size_for_function_name_ordinals;
	DWORD current_pos_of_function_addresses = current_pos_of_function_forwards + needed_size_for_function_forwards;
	DWORD current_pos_of_function_names_rvas = current_pos_of_function_addresses + needed_size_for_function_addresses;


	IMAGE_EXPORT_DIRECTORY dir = { 0 };
	dir.Characteristics		= exports.get_characteristics();
	dir.MajorVersion		= exports.get_major_version();
	dir.MinorVersion		= exports.get_minor_version();
	dir.TimeDateStamp		= exports.get_time_stamp();
	dir.NumberOfFunctions	= max_ordinal - ordinal_base + 1;
	dir.NumberOfNames		= number_of_names;
	dir.Base				= ordinal_base;
	dir.AddressOfFunctions  = section.get_virtual_address() + current_pos_of_function_addresses;
	dir.AddressOfNameOrdinals = section.get_virtual_address() + current_pos_of_function_name_ordinals;
	dir.AddressOfNames		= section.get_virtual_address() + current_pos_of_function_names_rvas;
	dir.Name				= section.get_virtual_address() + directory_pos + sizeof(IMAGE_EXPORT_DIRECTORY);
	memcpy(&raw_data[directory_pos], &dir, sizeof(dir));


	memcpy(&raw_data[directory_pos + sizeof(IMAGE_EXPORT_DIRECTORY)], exports.get_library_name().c_str(), exports.get_library_name().length() + 1);


	typedef std::map<std::string, WORD> funclist;
	funclist funcs;

	DWORD last_ordinal = ordinal_base;
	for (export_table_item &func : exports.get_items()) {
		if (func.get_ordinal() > last_ordinal){

			DWORD len = sizeof(DWORD) * (func.get_ordinal() - last_ordinal - 1);
			if (len){
				memset(&raw_data[current_pos_of_function_addresses], 0, len);
				current_pos_of_function_addresses += len;
			}

			last_ordinal = func.get_ordinal();
		}


		if (func.has_name()) {
			funcs.insert(std::make_pair(func.get_func_name(), static_cast<WORD>(func.get_ordinal() - ordinal_base)));
		}

		if (func.is_forward()) {
			DWORD function_rva = section.get_virtual_address() + current_pos_of_function_forwards;
			memcpy(&raw_data[current_pos_of_function_addresses], &function_rva, sizeof(function_rva));
			current_pos_of_function_addresses += sizeof(function_rva);

			memcpy(&raw_data[current_pos_of_function_forwards], func.get_forward_name().c_str(), func.get_forward_name().length() + 1);
			current_pos_of_function_forwards += static_cast<DWORD>(func.get_forward_name().length() + 1);
		}
		else{
			DWORD function_rva = func.get_rva();
			memcpy(&raw_data[current_pos_of_function_addresses], &function_rva, sizeof(function_rva));
			current_pos_of_function_addresses += sizeof(function_rva);
		}
	}

	for (funclist::const_iterator it = funcs.begin(); it != funcs.end(); ++it){

		DWORD function_name_rva = section.get_virtual_address() + current_pos_of_function_names;
		memcpy(&raw_data[current_pos_of_function_names_rvas], &function_name_rva, sizeof(function_name_rva));
		current_pos_of_function_names_rvas += sizeof(function_name_rva);

		memcpy(&raw_data[current_pos_of_function_names], (*it).first.c_str(), (*it).first.length() + 1);
		current_pos_of_function_names += static_cast<DWORD>((*it).first.length() + 1);

		WORD name_ordinal = (*it).second;
		memcpy(&raw_data[current_pos_of_function_name_ordinals], &name_ordinal, sizeof(name_ordinal));
		current_pos_of_function_name_ordinals += sizeof(name_ordinal);
	}


	image.set_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_EXPORT, section.get_virtual_address() + directory_pos);
	image.set_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_EXPORT, needed_size);
}

bool erase_export_table(pe_image &image, std::vector<erased_zone>* zones) {
   
    DWORD virtual_address = image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_EXPORT);
    DWORD virtual_size = image.get_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_EXPORT);


    if (virtual_address) {

        pe_section * export_section = image.get_section_by_rva(virtual_address);

        if (export_section) {
            PIMAGE_EXPORT_DIRECTORY export_desc = (PIMAGE_EXPORT_DIRECTORY)&export_section->get_section_data().data()[virtual_address - export_section->get_virtual_address()];

            if (!export_desc->NumberOfFunctions) { return false; }

            if (export_desc->Name) {
                pe_section * name_export_section = image.get_section_by_rva(export_desc->Name);
                if (name_export_section) {
                    ZeroMemory((char*)(&export_section->get_section_data().data()[export_desc->Name - export_section->get_virtual_address()]),
                        lstrlenA((char*)(&export_section->get_section_data().data()[export_desc->Name - export_section->get_virtual_address()])));
                }
            }


            for (unsigned int ordinal = 0; ordinal < export_desc->NumberOfFunctions; ordinal++) {

                DWORD func_rva = *(DWORD*)&image.get_section_by_rva((export_desc->AddressOfFunctions + ordinal * sizeof(DWORD)))->get_section_data().data()[
                    (export_desc->AddressOfFunctions + ordinal * sizeof(DWORD))
                        - image.get_section_by_rva((export_desc->AddressOfFunctions + ordinal * sizeof(DWORD)))->get_virtual_address()];

                if (!func_rva) { continue; }


                for (unsigned int i = 0; i < export_desc->NumberOfNames; i++) {

                    WORD ordinal2 = *(WORD*)&image.get_section_by_rva((export_desc->AddressOfNameOrdinals + i * sizeof(WORD)))->get_section_data().data()[
                        (export_desc->AddressOfNameOrdinals + i * sizeof(WORD))
                            - image.get_section_by_rva((export_desc->AddressOfNameOrdinals + i * sizeof(WORD)))->get_virtual_address()];

                    if (ordinal == ordinal2) {

                        DWORD function_name_rva = *(DWORD*)&image.get_section_by_rva((export_desc->AddressOfNames + i * sizeof(DWORD)))->get_section_data().data()[
                            (export_desc->AddressOfNames + i * sizeof(DWORD))
                                - image.get_section_by_rva((export_desc->AddressOfNames + i * sizeof(DWORD)))->get_virtual_address()];

                        char* func_name = (char*)&image.get_section_by_rva(function_name_rva)->get_section_data().data()[
                            function_name_rva
                                - image.get_section_by_rva(function_name_rva)->get_virtual_address()];

                        ZeroMemory(func_name, lstrlenA(func_name));
;

                        if (func_rva >= virtual_address + sizeof(IMAGE_EXPORT_DIRECTORY) &&
                            func_rva < virtual_address + virtual_size) {

                            char* forwarded_func_name = (char*)&image.get_section_by_rva(func_rva)->get_section_data().data()[
                                func_rva - image.get_section_by_rva(func_rva)->get_virtual_address()];

                            ZeroMemory(forwarded_func_name, lstrlenA(forwarded_func_name));
                        }

                        break;
                    }
                }
            }

            if (export_desc->AddressOfFunctions) {
                ZeroMemory(&image.get_section_by_rva((export_desc->AddressOfFunctions))->get_section_data().data()[
                    (export_desc->AddressOfFunctions)
                        - image.get_section_by_rva((export_desc->AddressOfFunctions))->get_virtual_address()],
                    export_desc->NumberOfFunctions * sizeof(DWORD));
            }

            if (export_desc->AddressOfNames) {
                ZeroMemory(&image.get_section_by_rva((export_desc->AddressOfNames))->get_section_data().data()[
                    (export_desc->AddressOfFunctions)
                        - image.get_section_by_rva((export_desc->AddressOfNames))->get_virtual_address()],
                    export_desc->NumberOfNames * sizeof(DWORD));
            }

            if (export_desc->AddressOfNameOrdinals) {
                ZeroMemory(&image.get_section_by_rva((export_desc->AddressOfNameOrdinals))->get_section_data().data()[
                    (export_desc->AddressOfFunctions)
                        - image.get_section_by_rva((export_desc->AddressOfNameOrdinals))->get_virtual_address()],
                    export_desc->NumberOfFunctions * sizeof(WORD));
            }

            ZeroMemory(&export_section->get_section_data().data()[virtual_address - export_section->get_virtual_address()],sizeof(IMAGE_EXPORT_DIRECTORY));
            return true;
        }
    }

    return false;
}


