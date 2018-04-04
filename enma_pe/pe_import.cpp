#include "stdafx.h"
#include "pe_import.h"


imported_func::imported_func() {
	hint				= 0;
	ordinal				= 0;
	b_import_by_name	= 0;
	iat_rva				= 0;
    func_name.clear();
}
imported_func::imported_func(const imported_func& func) {
    this->operator=(func);
}
imported_func::imported_func(uint32_t iat_rva, const std::string& func_name, uint16_t hint):
    iat_rva(iat_rva), func_name(func_name), hint(hint), ordinal(0), b_import_by_name(true){

}
imported_func::imported_func(uint32_t iat_rva, uint16_t ordinal):
    iat_rva(iat_rva), ordinal(ordinal), hint(0), b_import_by_name(false){
}

imported_func::~imported_func() {

}

imported_func& imported_func::operator=(const imported_func& func) {

	this->iat_rva = func.iat_rva;
	this->hint = func.hint;
	this->func_name = func.func_name;
	this->ordinal = func.ordinal;
	this->b_import_by_name = func.b_import_by_name;

    return *this;
}

void imported_func::set_hint(uint16_t hint) {
	this->hint = hint;
}
void imported_func::set_ordinal(uint16_t ordinal) {
	this->b_import_by_name = false;
	this->ordinal = ordinal;
}
void imported_func::set_func_name(const std::string& func_name) {
	this->b_import_by_name = true;
	this->func_name = func_name;
}
void imported_func::set_import_by_name(bool b) {
	this->b_import_by_name = b;
}
void imported_func::set_iat_rva(uint32_t rva) {
	this->iat_rva = rva;
}
uint16_t imported_func::get_hint() const {
	return this->hint;
}
uint16_t imported_func::get_ordinal() const {
	return this->ordinal;
}
std::string imported_func::get_func_name() const {
	return this->func_name;
}
bool  imported_func::is_import_by_name() const {
	return this->b_import_by_name;
}
uint32_t imported_func::get_iat_rva() const {
	return this->iat_rva;
}

imported_library::imported_library() {
	timestamp		= 0;
    rva_to_iat		= 0;
    rva_to_oft      = 0;
    library_name.clear();
	imported_items.clear();
}
imported_library::imported_library(const imported_library& library) {
    this->operator=(library);
}
imported_library::~imported_library() {
	imported_items.clear();
}
imported_library& imported_library::operator=(const imported_library& library) {

	this->timestamp      = library.timestamp;
	this->rva_to_iat     = library.rva_to_iat;
    this->rva_to_oft     = library.rva_to_oft;
	this->library_name   = library.library_name;
	this->imported_items = library.imported_items;

    return *this;
}


void imported_library::set_library_name(const std::string& library_name){
	this->library_name = library_name;
}
void imported_library::set_timestamp(uint32_t timestamp) {
	this->timestamp = timestamp;
}
void imported_library::set_rva_iat(uint32_t rva) {
	this->rva_to_iat = rva;
}
void imported_library::set_rva_oft(uint32_t rva) {
    this->rva_to_oft = rva;
}

void imported_library::add_item(const imported_func& item) {
	this->imported_items.push_back(item);
}

std::string imported_library::get_library_name() const {
	return this->library_name;
}
uint32_t imported_library::get_timestamp() const {
	return this->timestamp;
}
uint32_t imported_library::get_rva_iat() const {
	return this->rva_to_iat;
}
uint32_t imported_library::get_rva_oft() const {
    return this->rva_to_oft;
}

std::vector<imported_func>& imported_library::get_items() {
	return this->imported_items;
}


import_table::import_table() {

}
import_table::import_table(const import_table& imports) {
    this->operator=(imports);
}
import_table::~import_table() {

}

import_table& import_table::operator=(const import_table& imports) {

	this->libs = imports.libs;

    return *this;
}

void import_table::add_lib(const imported_library& lib) {
	this->libs.push_back(lib);
}

std::vector<imported_library>& import_table::get_libs() {
	return this->libs;
}

bool import_table::get_imported_lib(const std::string& lib_name, imported_library * &lib) {

	for (size_t i = 0; i < libs.size(); i++) {
		if (libs[i].get_library_name() == lib_name) {
			lib = &libs[i];
			return true;
		}
	}

	return false;
}
bool import_table::get_imported_func(const std::string& lib_name, const std::string& func_name, imported_library * &lib, imported_func * &func) {

	for (size_t i = 0; i < libs.size(); i++) {
		if (libs[i].get_library_name() == lib_name) {
			for (size_t j = 0; j < libs[i].get_items().size(); j++) {
				if (libs[i].get_items()[j].is_import_by_name() && libs[i].get_items()[j].get_func_name() == func_name) {
					lib = &libs[i];
					func = &libs[i].get_items()[j];
					return true;
				}
			}
		}
	}

	return false;
}
bool import_table::get_imported_func(const std::string& lib_name, uint16_t ordinal, imported_library * &lib, imported_func * &func) {

	for (uint32_t i = 0; i < libs.size(); i++) {
		if (libs[i].get_library_name() == lib_name) {
			for (uint32_t j = 0; j < libs[i].get_items().size(); j++) {
				if (!libs[i].get_items()[j].is_import_by_name() && libs[i].get_items()[j].get_ordinal() == ordinal) {
					lib = &libs[i];
					func = &libs[i].get_items()[j];
					return true;
				}
			}
		}
	}

	return false;
}


bool get_import_table(const pe_image &image, import_table& imports) {
	imports.get_libs().clear();
	
	uint32_t virtual_address = image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_IMPORT);
	uint32_t virtual_size = image.get_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_IMPORT);

	if (virtual_address && virtual_size) {
		pe_section * imp_section = image.get_section_by_rva(virtual_address);
		if (imp_section) {
			uint8_t  * raw_decs = &imp_section->get_section_data().data()[virtual_address - imp_section->get_virtual_address()];

			for (size_t imp_size = 0; imp_size < virtual_size; imp_size += sizeof(image_import_descriptor)) {
			    image_import_descriptor* imp_description = (image_import_descriptor*)(&raw_decs[imp_size]);
				if (imp_description->first_thunk && imp_description->name) {
					pe_section * imp_name_section = image.get_section_by_rva(imp_description->name);
					pe_section * imp_iat_section = image.get_section_by_rva(imp_description->first_thunk);
                    pe_section * imp_oft_section = image.get_section_by_rva(imp_description->original_first_thunk);

					imported_library import_lib;
                    import_lib.set_library_name(std::string(((char*)&imp_name_section->get_section_data().data()[
                        imp_description->name - imp_name_section->get_virtual_address()
                    ])));
                    import_lib.set_rva_iat(imp_description->first_thunk);
                    import_lib.set_rva_oft(imp_description->original_first_thunk);
                    import_lib.set_timestamp(imp_description->time_date_stamp);

					void* lib_iat = &imp_name_section->get_section_data().data()[(imp_description->first_thunk - imp_iat_section->get_virtual_address())];

                    if (imp_oft_section) {
                        lib_iat = &imp_oft_section->get_section_data().data()[(imp_description->original_first_thunk - imp_oft_section->get_virtual_address())];
                    }

                    uint32_t iat_ft_rva = imp_description->first_thunk;
       
					if (image.is_x32_image()) {
                        for (; *(uint32_t*)lib_iat; lib_iat = (void*)(&((uint32_t*)lib_iat)[1]) ) {

							if (*(uint32_t*)lib_iat&IMAGE_ORDINAL_FLAG32) {
                                import_lib.add_item(imported_func(iat_ft_rva,*(uint32_t*)lib_iat & 0xFFFF));
							}
							else {
								pe_section * imp_func = image.get_section_by_rva(*(uint32_t*)lib_iat);
								if (imp_func) {
                                    import_lib.add_item(imported_func(
                                        iat_ft_rva,
										(char*)&imp_func->get_section_data().data()[(*(uint32_t*)lib_iat - imp_func->get_virtual_address()) + sizeof(uint16_t)],
										*(uint16_t*)&imp_func->get_section_data().data()[(*(uint32_t*)lib_iat - imp_func->get_virtual_address())]
									));
								}
							}
                            iat_ft_rva += sizeof(uint32_t);
						}
					}
					else {
                        for (; *(uint64_t*)lib_iat; lib_iat = (void*)(&((uint64_t*)lib_iat)[1])) {

							if (*(uint64_t*)lib_iat&IMAGE_ORDINAL_FLAG64) {
                                import_lib.add_item(imported_func(iat_ft_rva, *(uint64_t*)lib_iat & 0xFFFF));
							}
							else {
								pe_section * imp_func = image.get_section_by_rva(*(uint32_t*)lib_iat);
								if (imp_func) {
                                    import_lib.add_item(imported_func(
                                        iat_ft_rva,
										(char*)&imp_func->get_section_data().data()[(*(uint32_t*)lib_iat - imp_func->get_virtual_address()) + sizeof(uint16_t)],
										*(uint16_t*)&imp_func->get_section_data().data()[(*(uint32_t*)lib_iat - imp_func->get_virtual_address())]
									));
								}
							}

                            iat_ft_rva += sizeof(uint64_t);
						}
					}

					imports.add_lib(import_lib);
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

void build_import_table(pe_image &image, pe_section& section, import_table& imports,
    bool use_original_table, bool rebuild_tables) {

    if (!imports.get_libs().size()) { return; }

    if (section.get_size_of_raw_data() & 0xF) {
        section.get_section_data().resize(
            section.get_section_data().size() + (0x10 - (section.get_section_data().size() & 0xF))
        );
    }

    size_t desc_table_size = sizeof(image_import_descriptor);
    size_t lib_names_size = 0;
    size_t func_names_size = 0;
    size_t thunk_size = 0;
    size_t original_thunk_size = 0;

    for (auto& lib : imports.get_libs()) {
        if (!lib.get_items().size()) { continue; }

        uint32_t current_lib_thunk_size = 0;

        for (auto& func : lib.get_items()) {
            if (func.is_import_by_name()) {
                func_names_size += func.get_func_name().length() + 1 + sizeof(uint16_t);//hint
                if ((func.get_func_name().length() + 1) & 1) { func_names_size += 1; }//+1 uint8_t  for aligned func name
            }

            if (image.is_x32_image()) { current_lib_thunk_size += sizeof(uint32_t); }
            else { current_lib_thunk_size += sizeof(uint64_t); }
        }

        if (image.is_x32_image()) { current_lib_thunk_size += sizeof(uint32_t); }
        else { current_lib_thunk_size += sizeof(uint64_t); }

        if (rebuild_tables || (!lib.get_rva_iat())) {
            thunk_size += current_lib_thunk_size;
        }

        if (use_original_table) {
            if (rebuild_tables || (!lib.get_rva_oft())) {
                original_thunk_size += current_lib_thunk_size;
            }
        }

        lib_names_size += lib.get_library_name().length() + 1;
        if ((lib.get_library_name().length() + 1) & 1) { func_names_size += 1; }//+1 uint8_t  for aligned lib name
        desc_table_size += sizeof(image_import_descriptor);
    }

    uint32_t import_desc_virtual_address = section.get_virtual_address() + section.get_size_of_raw_data() + thunk_size;
    uint32_t import_iat_virtual_address = section.get_virtual_address() + section.get_size_of_raw_data();
    uint32_t p_import_offset = section.get_size_of_raw_data();
    

    section.get_section_data().resize(section.get_size_of_raw_data() +
        desc_table_size +
        lib_names_size +
        func_names_size + 
        thunk_size +
        original_thunk_size);


    uint8_t  * import_thunks = &section.get_section_data().data()[p_import_offset];
    image_import_descriptor* import_descs = (image_import_descriptor*)&section.get_section_data().data()[p_import_offset + thunk_size];
    uint8_t  * import_original_thunks = &section.get_section_data().data()[p_import_offset + thunk_size + desc_table_size];
    uint8_t  * import_names = &section.get_section_data().data()[p_import_offset + thunk_size + desc_table_size + original_thunk_size];


    for (auto& lib : imports.get_libs()) {
        if (!lib.get_items().size()) { continue; }

        import_descs->characteristics = 0;
        import_descs->forwarder_chain = 0;
        import_descs->time_date_stamp = lib.get_timestamp();
        import_descs->name = section.get_virtual_address() + (import_names - section.get_section_data().data());
        
        bool need_build_iat_table = false;
        bool need_build_oft_table = false;
       

        if (rebuild_tables) {
            need_build_iat_table = true;
            import_descs->first_thunk = section.get_virtual_address() + (import_thunks - section.get_section_data().data());

            if (use_original_table) {
                need_build_oft_table = true;
                import_descs->original_first_thunk = section.get_virtual_address() + (import_original_thunks - section.get_section_data().data());
            }
            else {
                import_descs->original_first_thunk = 0;
            }
        }
        else {
            if (!lib.get_rva_iat()) {
                need_build_iat_table = true;
                import_descs->first_thunk = section.get_virtual_address() + (import_thunks - section.get_section_data().data());
            }
            else {
                import_descs->first_thunk = lib.get_rva_iat();
            }

            if (use_original_table) {
                if (!lib.get_rva_oft()) {
                    need_build_oft_table = true;
                    import_descs->original_first_thunk = section.get_virtual_address() + (import_original_thunks - section.get_section_data().data());
                }
                else {
                    import_descs->original_first_thunk = lib.get_rva_oft();
                }
            }
        }

        lstrcpyA((char*)import_names, lib.get_library_name().c_str());
        import_names += lib.get_library_name().length() + 1 + (((lib.get_library_name().length() + 1) & 1) ? 1 : 0);

        if (need_build_iat_table || need_build_oft_table) {

            for (auto& func : lib.get_items()) {
                if (func.is_import_by_name()) {
                    image_import_by_name* pimport_by_name = (image_import_by_name*)import_names;
                    uint32_t thunk_rva = (section.get_virtual_address() + (import_names - section.get_section_data().data()));

                    func.set_iat_rva(section.get_virtual_address() + (import_thunks - section.get_section_data().data()));
                    pimport_by_name->hint = func.get_hint();
                    lstrcpyA((char*)pimport_by_name->name, func.get_func_name().c_str());

                    import_names += sizeof(uint16_t) + func.get_func_name().length() + 1 + (((func.get_func_name().length() + 1) & 1) ? 1 : 0);

                    if (image.is_x32_image()) {
                        if (need_build_iat_table) {
                            *(uint32_t*)import_thunks = thunk_rva;          import_thunks += sizeof(uint32_t);
                        }
                        if (need_build_oft_table) {
                            *(uint32_t*)import_original_thunks = thunk_rva; import_original_thunks += sizeof(uint32_t);
                        }
                    }
                    else {
                        if (need_build_iat_table) {
                            *(uint64_t*)import_thunks = thunk_rva;          import_thunks += sizeof(uint64_t);
                        }
                        if (need_build_oft_table) {
                            *(uint64_t*)import_original_thunks = thunk_rva; import_original_thunks += sizeof(uint64_t);
                        }
                    }
                }
                else {
                    func.set_iat_rva(section.get_virtual_address() + (import_thunks - section.get_section_data().data()));

                    if (image.is_x32_image()) {
                        if (need_build_iat_table) {
                            *(uint32_t*)import_thunks = func.get_ordinal() | IMAGE_ORDINAL_FLAG32;          import_thunks += sizeof(uint32_t);
                        }
                        if (need_build_oft_table) {
                            *(uint32_t*)import_original_thunks = func.get_ordinal() | IMAGE_ORDINAL_FLAG32; import_original_thunks += sizeof(uint32_t);
                        }
                    }
                    else {
                        if (need_build_iat_table) {
                            *(uint64_t*)import_thunks = func.get_ordinal() | IMAGE_ORDINAL_FLAG64;          import_thunks += sizeof(uint64_t);
                        }
                        if (need_build_oft_table) {
                            *(uint64_t*)import_original_thunks = func.get_ordinal() | IMAGE_ORDINAL_FLAG64; import_original_thunks += sizeof(uint64_t);
                        }
                    }
                }
            }
            if (image.is_x32_image()) {
                if (need_build_iat_table) { import_thunks += sizeof(uint32_t); }
                if (need_build_oft_table) { import_original_thunks += sizeof(uint32_t); }
            }
            else {
                if (need_build_iat_table) { import_thunks += sizeof(uint64_t); }
                if (need_build_oft_table) { import_original_thunks += sizeof(uint64_t); }
            }
        }

        lib.set_rva_iat(import_descs->first_thunk);
        lib.set_rva_oft(import_descs->original_first_thunk);

        import_descs++;
    }
  

    image.set_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_IMPORT, import_desc_virtual_address);
    image.set_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_IMPORT, desc_table_size);
    image.set_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_IAT, import_iat_virtual_address);
    image.set_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_IAT, thunk_size);
}

bool erase_import_table(pe_image &image, std::vector<erased_zone>* zones) {
	uint32_t virtual_address = image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_IMPORT);
	uint32_t virtual_size = image.get_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_IMPORT);

	if (virtual_address && virtual_size) {
		
		pe_section * imp_section = image.get_section_by_rva(virtual_address);
		if (imp_section) {
            size_t imp_size;
			uint8_t  * raw_import = &imp_section->get_section_data().data()[virtual_address - imp_section->get_virtual_address()];
			for (imp_size = 0; imp_size < virtual_size; imp_size += sizeof(image_import_descriptor)) {
                image_import_descriptor* imp_description = (image_import_descriptor*)(&raw_import[imp_size]);
				if (imp_description->first_thunk && imp_description->name) {
					pe_section * imp_name_section = image.get_section_by_rva(imp_description->name);
					pe_section * imp_ft_section  = image.get_section_by_rva(imp_description->first_thunk);
					pe_section * imp_oft_section  = image.get_section_by_rva(imp_description->original_first_thunk);


					if (zones) {    
						zones->push_back({
							imp_description->name ,
							(uint32_t)strlen((char*)&imp_name_section->get_section_data().data()[
                                (imp_description->name - imp_name_section->get_virtual_address())
                            ]) + 1
						});
					}

					ZeroMemory((char*)&imp_name_section->get_section_data().data()[imp_description->name - imp_name_section->get_virtual_address()],
						strlen((char*)&imp_name_section->get_section_data().data()[(imp_description->name - imp_name_section->get_virtual_address())]));


                    size_t thunk_items_count = 0;
                    uint8_t * thunk_table = 0;
                                            
                    if (imp_description->original_first_thunk && imp_oft_section) {
                        thunk_table = &imp_oft_section->get_section_data().data()[
                            (imp_description->original_first_thunk - imp_oft_section->get_virtual_address())
                        ];
                    }
                    else {
                        thunk_table = &imp_ft_section->get_section_data().data()[
                            (imp_description->first_thunk - imp_ft_section->get_virtual_address())
                        ];
                    }


					if (image.is_x32_image()) {
                        for (thunk_items_count = 0; *(uint32_t*)thunk_table; thunk_table+=sizeof(uint32_t), thunk_items_count++) {
                            if ( !(*(uint32_t*)thunk_table&IMAGE_ORDINAL_FLAG32) ) {
                                pe_section * section_func_name = image.get_section_by_rva(*(uint32_t*)thunk_table);
                                if (section_func_name) {
                                    if (zones) {
                                        zones->push_back({
                                            *(uint32_t*)thunk_table ,
                                            (uint32_t)strlen((char*)&section_func_name->get_section_data().data()[
                                                (*(uint32_t*)thunk_table - section_func_name->get_virtual_address()) + sizeof(uint16_t)
                                            ]) + 2 + 2
                                        });
                                    }

                                    ZeroMemory(
                                        (char*)&section_func_name->get_section_data().data()[
                                            (*(uint32_t*)thunk_table - section_func_name->get_virtual_address())
                                        ],
                                        strlen((char*)&section_func_name->get_section_data().data()[
                                            (*(uint32_t*)thunk_table - section_func_name->get_virtual_address()) + sizeof(uint16_t)
                                        ]) + 2
                                    );
                                }
                            }
                        }

                        if (imp_description->original_first_thunk && imp_oft_section) {
                            if (zones) {
                                zones->push_back({ imp_description->original_first_thunk , (thunk_items_count+1)  * sizeof(uint32_t)});
                            }

                            ZeroMemory(&imp_oft_section->get_section_data().data()[
                                (imp_description->original_first_thunk - imp_oft_section->get_virtual_address())
                            ], (thunk_items_count + 1) * sizeof(uint32_t));
                        }

                        if (zones) {
                            zones->push_back({ imp_description->first_thunk , (thunk_items_count + 1) * sizeof(uint32_t) });
                        }

                        ZeroMemory(&imp_ft_section->get_section_data().data()[
                            (imp_description->first_thunk - imp_ft_section->get_virtual_address())
                        ], (thunk_items_count + 1) * sizeof(uint32_t));
					}
					else {
                        for (thunk_items_count = 0; *(uint64_t*)thunk_table; thunk_table += sizeof(uint64_t), thunk_items_count++) {
                            if (!(*(uint64_t*)thunk_table&IMAGE_ORDINAL_FLAG64)) {
                                pe_section * section_func_name = image.get_section_by_rva((uint32_t)*(uint64_t*)thunk_table);
                                if (section_func_name) {
                                    if (zones) {
                                        zones->push_back({
                                            (uint32_t)(*(uint64_t*)thunk_table) ,
                                            (uint32_t)strlen((char*)&section_func_name->get_section_data().data()[
                                                (*(uint64_t*)thunk_table - section_func_name->get_virtual_address()) + sizeof(uint16_t)
                                            ]) + 2 + 2
                                        });
                                    }

                                    ZeroMemory(
                                        (char*)&section_func_name->get_section_data().data()[
                                            (*(uint64_t*)thunk_table - section_func_name->get_virtual_address())
                                        ],
                                        strlen((char*)&section_func_name->get_section_data().data()[
                                            (*(uint64_t*)thunk_table - section_func_name->get_virtual_address()) + sizeof(uint16_t)
                                        ]) + 2
                                     );
                                }
                            }
                        }

                        if (imp_description->original_first_thunk && imp_oft_section) {
                            if (zones) {
                                zones->push_back({ imp_description->original_first_thunk , (thunk_items_count + 1) * sizeof(uint64_t) });
                            }

                            ZeroMemory(&imp_oft_section->get_section_data().data()[
                                (imp_description->original_first_thunk - imp_oft_section->get_virtual_address())
                            ], (thunk_items_count + 1) * sizeof(uint64_t));
                        }

                        if (zones) {
                            zones->push_back({ imp_description->first_thunk , (thunk_items_count + 1) * sizeof(uint64_t) });
                        }

                        ZeroMemory(&imp_ft_section->get_section_data().data()[
                            (imp_description->first_thunk - imp_ft_section->get_virtual_address())
                        ], (thunk_items_count + 1) * sizeof(uint64_t));
					}
                    
					ZeroMemory(imp_description, sizeof(image_import_descriptor));
				}
				else {
					if (zones) { zones->push_back({ virtual_address ,imp_size }); }
					image.set_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_IMPORT, 0);
					image.set_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_IMPORT, 0);
                    image.set_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_IAT, 0);
                    image.set_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_IAT, 0);
					return true;
				}
			}
			if (zones) { zones->push_back({ virtual_address ,imp_size }); }
			image.set_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_IMPORT, 0);
			image.set_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_IMPORT, 0);
            image.set_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_IAT, 0);
            image.set_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_IAT, 0);
			return true;
		}
	}
	return false;
}


