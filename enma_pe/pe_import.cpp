#include "stdafx.h"
#include "pe_import.h"


imported_func::imported_func() {
	hint				= 0;
	ordinal				= 0;
	b_import_by_name	= 0;
	iat_rva				= 0;
	name.clear();
}

imported_func::imported_func(DWORD iat_rva, std::string name, WORD hint) {
    this->iat_rva = iat_rva;
    this->hint = hint;
    this->name = name;
    this->ordinal = 0;
    b_import_by_name = true;
}

imported_func::imported_func(DWORD iat_rva, WORD ordinal) {
    this->iat_rva = iat_rva;
    this->hint = 0;
    this->name.clear();

    this->ordinal = ordinal;

    b_import_by_name = false;
}

imported_func::~imported_func() {

}

imported_func& imported_func::operator=(const imported_func& func) {

	this->iat_rva = func.iat_rva;
	this->hint = func.hint;
	this->name = func.name;
	this->ordinal = func.ordinal;
	this->b_import_by_name = func.b_import_by_name;

    return *this;
}

void imported_func::set_hint(WORD hint) {
	this->hint = hint;
}
void imported_func::set_ordinal(WORD ordinal) {
	this->b_import_by_name = false;
	this->ordinal = ordinal;
}
void imported_func::set_name(std::string name) {
	this->b_import_by_name = true;
	this->name = name;
}
void imported_func::set_import_by_name(bool b) {
	this->b_import_by_name = b;
}
void imported_func::set_iat_rva(DWORD rva) {
	this->iat_rva = rva;
}
WORD imported_func::get_hint() const {
	return this->hint;
}
WORD imported_func::get_ordinal() const {
	return this->ordinal;
}
std::string imported_func::get_name() const {
	return this->name;
}
bool  imported_func::is_import_by_name() const {
	return this->b_import_by_name;
}
DWORD imported_func::get_iat_rva() const {
	return this->iat_rva;
}

imported_library::imported_library() {
	timestamp		= 0;
    rva_to_iat		= 0;
    rva_to_oft      = 0;
	name.clear();
	imported_items.clear();
}
imported_library::~imported_library() {
	imported_items.clear();
}
imported_library& imported_library::operator=(const imported_library& library) {

	this->timestamp      = library.timestamp;
	this->rva_to_iat     = library.rva_to_iat;
    this->rva_to_oft     = library.rva_to_oft;
	this->name           = library.name;
	this->imported_items = library.imported_items;

    return *this;
}


void imported_library::set_name(std::string name){
	this->name = name;
}
void imported_library::set_timestamp(DWORD timestamp) {
	this->timestamp = timestamp;
}
void imported_library::set_rva_iat(DWORD rva) {
	this->rva_to_iat = rva;
}
void imported_library::set_rva_oft(DWORD rva) {
    this->rva_to_oft = rva;
}

void imported_library::add_item(const imported_func& item) {
	this->imported_items.push_back(item);
}

std::string imported_library::get_name() const {
	return this->name;
}
DWORD imported_library::get_timestamp() const {
	return this->timestamp;
}
DWORD imported_library::get_rva_iat() const {
	return this->rva_to_iat;
}
DWORD imported_library::get_rva_oft() const {
    return this->rva_to_oft;
}

std::vector<imported_func>& imported_library::get_items() {
	return this->imported_items;
}


import_table::import_table() {

}
import_table::~import_table() {

}

import_table& import_table::operator=(const import_table& imports) {

	this->libs = imports.libs;

    return *this;
}

void import_table::add_lib(imported_library& lib) {
	this->libs.push_back(lib);
}

std::vector<imported_library>& import_table::get_libs() {
	return this->libs;
}

bool import_table::get_imported_lib(std::string lib_name, imported_library * &lib) {

	for (unsigned int i = 0; i < libs.size(); i++) {
		if (libs[i].get_name() == lib_name) {
			lib = &libs[i];
			return true;
		}
	}

	return false;
}
bool import_table::get_imported_func(std::string lib_name, std::string func_name, imported_library * &lib, imported_func * &func) {

	for (unsigned int i = 0; i < libs.size(); i++) {
		if (libs[i].get_name() == lib_name) {
			for (unsigned int j = 0; j < libs[i].get_items().size(); j++) {
				if (libs[i].get_items()[j].is_import_by_name() && libs[i].get_items()[j].get_name() == func_name) {
					lib = &libs[i];
					func = &libs[i].get_items()[j];
					return true;
				}
			}
		}
	}

	return false;
}
bool import_table::get_imported_func(std::string lib_name, WORD ordinal, imported_library * &lib, imported_func * &func) {

	for (unsigned int i = 0; i < libs.size(); i++) {
		if (libs[i].get_name() == lib_name) {
			for (unsigned int j = 0; j < libs[i].get_items().size(); j++) {
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
	
	DWORD virtual_address = image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_IMPORT);
	DWORD virtual_size = image.get_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_IMPORT);

	if (virtual_address && virtual_size) {
		pe_section * imp_section = image.get_section_by_rva(virtual_address);
		if (imp_section) {
			BYTE * raw_decs = &imp_section->get_section_data().data()[virtual_address - imp_section->get_virtual_address()];

			for (unsigned int imp_size = 0; imp_size < virtual_size; imp_size += sizeof(IMAGE_IMPORT_DESCRIPTOR)) {
				PIMAGE_IMPORT_DESCRIPTOR imp_description = (PIMAGE_IMPORT_DESCRIPTOR)(&raw_decs[imp_size]);
				if (imp_description->FirstThunk && imp_description->Name) {
					pe_section * imp_name_section = image.get_section_by_rva(imp_description->Name);
					pe_section * imp_iat_section = image.get_section_by_rva(imp_description->FirstThunk);
                    pe_section * imp_oft_section = image.get_section_by_rva(imp_description->OriginalFirstThunk);

					imported_library import_lib;
                    import_lib.set_name(std::string(((char*)&imp_name_section->get_section_data().data()[
                        imp_description->Name - imp_name_section->get_virtual_address()
                    ])));
                    import_lib.set_rva_iat(imp_description->FirstThunk);
                    import_lib.set_rva_oft(imp_description->OriginalFirstThunk);
                    import_lib.set_timestamp(imp_description->TimeDateStamp);

					void* lib_iat = &imp_name_section->get_section_data().data()[(imp_description->FirstThunk - imp_iat_section->get_virtual_address())];

                    if (imp_oft_section) {
                        lib_iat = &imp_oft_section->get_section_data().data()[(imp_description->OriginalFirstThunk - imp_oft_section->get_virtual_address())];
                    }

                    DWORD iat_ft_rva = imp_description->FirstThunk;
       
					if (image.is_x32_image()) {
                        for (; *(DWORD*)lib_iat; lib_iat = (void*)(&((DWORD*)lib_iat)[1]) ) {

							if (*(DWORD*)lib_iat&IMAGE_ORDINAL_FLAG32) {
                                import_lib.add_item(imported_func(iat_ft_rva,*(DWORD*)lib_iat & 0xFFFF));
							}
							else {
								pe_section * imp_func = image.get_section_by_rva(*(DWORD*)lib_iat);
								if (imp_func) {
                                    import_lib.add_item(imported_func(
                                        iat_ft_rva,
										(char*)&imp_func->get_section_data().data()[(*(DWORD*)lib_iat - imp_func->get_virtual_address()) + sizeof(WORD)],
										*(WORD*)&imp_func->get_section_data().data()[(*(DWORD*)lib_iat - imp_func->get_virtual_address())]
									));
								}
							}
                            iat_ft_rva += sizeof(DWORD);
						}
					}
					else {
                        for (; *(DWORD64*)lib_iat; lib_iat = (void*)(&((DWORD64*)lib_iat)[1])) {

							if (*(DWORD64*)lib_iat&IMAGE_ORDINAL_FLAG64) {
                                import_lib.add_item(imported_func(iat_ft_rva, *(DWORD64*)lib_iat & 0xFFFF));
							}
							else {
								pe_section * imp_func = image.get_section_by_rva(*(DWORD*)lib_iat);
								if (imp_func) {
                                    import_lib.add_item(imported_func(
                                        iat_ft_rva,
										(char*)&imp_func->get_section_data().data()[(*(DWORD*)lib_iat - imp_func->get_virtual_address()) + sizeof(WORD)],
										*(WORD*)&imp_func->get_section_data().data()[(*(DWORD*)lib_iat - imp_func->get_virtual_address())]
									));
								}
							}

                            iat_ft_rva += sizeof(DWORD64);
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

    unsigned int desc_table_size = sizeof(IMAGE_IMPORT_DESCRIPTOR);
    unsigned int lib_names_size = 0;
    unsigned int func_names_size = 0;
    unsigned int thunk_size = 0;
    unsigned int original_thunk_size = 0;

    for (auto& lib : imports.get_libs()) {
        if (!lib.get_items().size()) { continue; }

        unsigned int current_lib_thunk_size = 0;

        for (auto& func : lib.get_items()) {
            if (func.is_import_by_name()) {
                func_names_size += func.get_name().length() + 1 + sizeof(WORD);//hint
                if ((func.get_name().length() + 1) & 1) { func_names_size += 1; }//+1 byte for aligned func name
            }

            if (image.is_x32_image()) { current_lib_thunk_size += sizeof(DWORD); }
            else { current_lib_thunk_size += sizeof(DWORD64); }
        }

        if (image.is_x32_image()) { current_lib_thunk_size += sizeof(DWORD); }
        else { current_lib_thunk_size += sizeof(DWORD64); }

        if (rebuild_tables || (!lib.get_rva_iat() && !rebuild_tables)) {
            thunk_size += current_lib_thunk_size;
        }

        if (use_original_table) {
            if (rebuild_tables || (!lib.get_rva_oft() && !rebuild_tables)) {
                original_thunk_size += current_lib_thunk_size;
            }
        }

        lib_names_size += lib.get_name().length() + 1;
        if ((lib.get_name().length() + 1) & 1) { func_names_size += 1; }//+1 byte for aligned lib name
        desc_table_size += sizeof(IMAGE_IMPORT_DESCRIPTOR);
    }

    DWORD import_desc_virtual_address = section.get_virtual_address() + section.get_size_of_raw_data() + thunk_size;
    DWORD import_iat_virtual_address = section.get_virtual_address() + section.get_size_of_raw_data();
    DWORD p_import_offset = section.get_size_of_raw_data();
    

    section.get_section_data().resize(section.get_size_of_raw_data() +
        desc_table_size +
        lib_names_size +
        func_names_size + 
        thunk_size +
        original_thunk_size);


    BYTE * import_thunks = &section.get_section_data().data()[p_import_offset];
    PIMAGE_IMPORT_DESCRIPTOR import_descs = (PIMAGE_IMPORT_DESCRIPTOR)&section.get_section_data().data()[p_import_offset + thunk_size];
    BYTE * import_original_thunks = &section.get_section_data().data()[p_import_offset + thunk_size + desc_table_size];
    BYTE * import_names = &section.get_section_data().data()[p_import_offset + thunk_size + desc_table_size + original_thunk_size];


    for (auto& lib : imports.get_libs()) {
        if (!lib.get_items().size()) { continue; }

        import_descs->Characteristics = 0;
        import_descs->ForwarderChain = 0;
        import_descs->TimeDateStamp = lib.get_timestamp();
        import_descs->Name = section.get_virtual_address() + (import_names - section.get_section_data().data());
        
        bool need_build_iat_table = false;
        bool need_build_oft_table = false;
       

        if (rebuild_tables) {
            need_build_iat_table = true;
            import_descs->FirstThunk = section.get_virtual_address() + (import_thunks - section.get_section_data().data());

            if (use_original_table) {
                need_build_oft_table = true;
                import_descs->OriginalFirstThunk = section.get_virtual_address() + (import_original_thunks - section.get_section_data().data());
            }
            else {
                import_descs->OriginalFirstThunk = 0;
            }
        }
        else {
            if (!lib.get_rva_iat()) {
                need_build_iat_table = true;
                import_descs->FirstThunk = section.get_virtual_address() + (import_thunks - section.get_section_data().data());
            }
            else {
                import_descs->FirstThunk = lib.get_rva_iat();
            }

            if (use_original_table) {
                if (!lib.get_rva_oft()) {
                    need_build_oft_table = true;
                    import_descs->OriginalFirstThunk = section.get_virtual_address() + (import_original_thunks - section.get_section_data().data());
                }
                else {
                    import_descs->OriginalFirstThunk = lib.get_rva_oft();
                }
            }
        }

        lstrcpyA((char*)import_names, lib.get_name().c_str());
        import_names += lib.get_name().length() + 1 + (((lib.get_name().length() + 1) & 1) ? 1 : 0);

        if (need_build_iat_table || need_build_oft_table) {

            for (auto& func : lib.get_items()) {
                if (func.is_import_by_name()) {
                    PIMAGE_IMPORT_BY_NAME pimport_by_name = (PIMAGE_IMPORT_BY_NAME)import_names;
                    DWORD thunk_rva = (section.get_virtual_address() + (import_names - section.get_section_data().data()));

                    func.set_iat_rva(section.get_virtual_address() + (import_thunks - section.get_section_data().data()));
                    pimport_by_name->Hint = func.get_hint();
                    lstrcpyA(pimport_by_name->Name, func.get_name().c_str());

                    import_names += sizeof(WORD) + func.get_name().length() + 1 + (((func.get_name().length() + 1) & 1) ? 1 : 0);

                    if (image.is_x32_image()) {
                        if (need_build_iat_table) {
                            *(DWORD*)import_thunks = thunk_rva;          import_thunks += sizeof(DWORD);
                        }
                        if (need_build_oft_table) {
                            *(DWORD*)import_original_thunks = thunk_rva; import_original_thunks += sizeof(DWORD);
                        }
                    }
                    else {
                        if (need_build_iat_table) {
                            *(DWORD64*)import_thunks = thunk_rva;          import_thunks += sizeof(DWORD64);
                        }
                        if (need_build_oft_table) {
                            *(DWORD64*)import_original_thunks = thunk_rva; import_original_thunks += sizeof(DWORD64);
                        }
                    }
                }
                else {
                    func.set_iat_rva(section.get_virtual_address() + (import_thunks - section.get_section_data().data()));

                    if (image.is_x32_image()) {
                        if (need_build_iat_table) {
                            *(DWORD*)import_thunks = func.get_ordinal() | IMAGE_ORDINAL_FLAG32;          import_thunks += sizeof(DWORD);
                        }
                        if (need_build_oft_table) {
                            *(DWORD*)import_original_thunks = func.get_ordinal() | IMAGE_ORDINAL_FLAG32; import_original_thunks += sizeof(DWORD);
                        }
                    }
                    else {
                        if (need_build_iat_table) {
                            *(DWORD64*)import_thunks = func.get_ordinal() | IMAGE_ORDINAL_FLAG64;          import_thunks += sizeof(DWORD64);
                        }
                        if (need_build_oft_table) {
                            *(DWORD64*)import_original_thunks = func.get_ordinal() | IMAGE_ORDINAL_FLAG64; import_original_thunks += sizeof(DWORD64);
                        }
                    }
                }
            }
            if (image.is_x32_image()) {
                if (need_build_iat_table) { import_thunks += sizeof(DWORD); }
                if (need_build_oft_table) { import_original_thunks += sizeof(DWORD); }
            }
            else {
                if (need_build_iat_table) { import_thunks += sizeof(DWORD64); }
                if (need_build_oft_table) { import_original_thunks += sizeof(DWORD64); }
            }
        }

        lib.set_rva_iat(import_descs->FirstThunk);
        lib.set_rva_oft(import_descs->OriginalFirstThunk);

        import_descs++;
    }
  

    image.set_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_IMPORT, import_desc_virtual_address);
    image.set_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_IMPORT, desc_table_size);
    image.set_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_IAT, import_iat_virtual_address);
    image.set_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_IAT, thunk_size);
}

bool erase_import_table(pe_image &image, std::vector<erased_zone>* zones) {
	DWORD virtual_address = image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_IMPORT);
	DWORD virtual_size = image.get_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_IMPORT);

	if (virtual_address && virtual_size) {
		
		pe_section * imp_section = image.get_section_by_rva(virtual_address);
		if (imp_section) {
            unsigned int imp_size;
			BYTE * raw_import = &imp_section->get_section_data().data()[virtual_address - imp_section->get_virtual_address()];
			for (imp_size = 0; imp_size < virtual_size; imp_size += sizeof(IMAGE_IMPORT_DESCRIPTOR)) {
				PIMAGE_IMPORT_DESCRIPTOR imp_description = (PIMAGE_IMPORT_DESCRIPTOR)(&raw_import[imp_size]);
				if (imp_description->FirstThunk && imp_description->Name) {
					pe_section * imp_name_section = image.get_section_by_rva(imp_description->Name);
					pe_section * imp_ft_section  = image.get_section_by_rva(imp_description->FirstThunk);
					pe_section * imp_oft_section  = image.get_section_by_rva(imp_description->OriginalFirstThunk);


					if (zones) {    
						zones->push_back({
							imp_description->Name ,
							(unsigned int)lstrlenA((char*)&imp_name_section->get_section_data().data()[
                                (imp_description->Name - imp_name_section->get_virtual_address())
                            ]) + 1
						});
					}

					ZeroMemory((char*)&imp_name_section->get_section_data().data()[imp_description->Name - imp_name_section->get_virtual_address()],
						lstrlenA((char*)&imp_name_section->get_section_data().data()[(imp_description->Name - imp_name_section->get_virtual_address())]));


                    unsigned int thunk_items_count = 0;
                    BYTE * thunk_table = 0;
                                            
                    if (imp_description->OriginalFirstThunk && imp_oft_section) {
                        thunk_table = &imp_oft_section->get_section_data().data()[
                            (imp_description->OriginalFirstThunk - imp_oft_section->get_virtual_address())
                        ];
                    }
                    else {
                        thunk_table = &imp_ft_section->get_section_data().data()[
                            (imp_description->FirstThunk - imp_ft_section->get_virtual_address())
                        ];
                    }


					if (image.is_x32_image()) {
                        for (thunk_items_count = 0; *(DWORD*)thunk_table; thunk_table+=sizeof(DWORD), thunk_items_count++) {
                            if ( !(*(DWORD*)thunk_table&IMAGE_ORDINAL_FLAG32) ) {
                                pe_section * section_func_name = image.get_section_by_rva(*(DWORD*)thunk_table);
                                if (section_func_name) {
                                    if (zones) {
                                        zones->push_back({
                                            *(DWORD*)thunk_table ,
                                            (unsigned int)lstrlenA((char*)&section_func_name->get_section_data().data()[
                                                (*(DWORD*)thunk_table - section_func_name->get_virtual_address()) + sizeof(WORD)
                                            ]) + 2 + 2
                                        });
                                    }

                                    ZeroMemory(
                                        (char*)&section_func_name->get_section_data().data()[
                                            (*(DWORD*)thunk_table - section_func_name->get_virtual_address())
                                        ],
                                        lstrlenA((char*)&section_func_name->get_section_data().data()[
                                            (*(DWORD*)thunk_table - section_func_name->get_virtual_address()) + sizeof(WORD)
                                        ]) + 2
                                    );
                                }
                            }
                        }

                        if (imp_description->OriginalFirstThunk && imp_oft_section) {
                            if (zones) {
                                zones->push_back({ imp_description->OriginalFirstThunk , (thunk_items_count+1)  * sizeof(DWORD)});
                            }

                            ZeroMemory(&imp_oft_section->get_section_data().data()[
                                (imp_description->OriginalFirstThunk - imp_oft_section->get_virtual_address())
                            ], (thunk_items_count + 1) * sizeof(DWORD));
                        }

                        if (zones) {
                            zones->push_back({ imp_description->FirstThunk , (thunk_items_count + 1) * sizeof(DWORD) });
                        }

                        ZeroMemory(&imp_ft_section->get_section_data().data()[
                            (imp_description->FirstThunk - imp_ft_section->get_virtual_address())
                        ], (thunk_items_count + 1) * sizeof(DWORD));
					}
					else {
                        for (thunk_items_count = 0; *(DWORD64*)thunk_table; thunk_table += sizeof(DWORD64), thunk_items_count++) {
                            if (!(*(DWORD64*)thunk_table&IMAGE_ORDINAL_FLAG64)) {
                                pe_section * section_func_name = image.get_section_by_rva((DWORD)*(DWORD64*)thunk_table);
                                if (section_func_name) {
                                    if (zones) {
                                        zones->push_back({
                                            (DWORD)(*(DWORD64*)thunk_table) ,
                                            (unsigned int)lstrlenA((char*)&section_func_name->get_section_data().data()[
                                                (*(DWORD64*)thunk_table - section_func_name->get_virtual_address()) + sizeof(WORD)
                                            ]) + 2 + 2
                                        });
                                    }

                                    ZeroMemory(
                                        (char*)&section_func_name->get_section_data().data()[
                                            (*(DWORD64*)thunk_table - section_func_name->get_virtual_address())
                                        ],
                                        lstrlenA((char*)&section_func_name->get_section_data().data()[
                                            (*(DWORD64*)thunk_table - section_func_name->get_virtual_address()) + sizeof(WORD)
                                        ]) + 2
                                     );
                                }
                            }
                        }

                        if (imp_description->OriginalFirstThunk && imp_oft_section) {
                            if (zones) {
                                zones->push_back({ imp_description->OriginalFirstThunk , (thunk_items_count + 1) * sizeof(DWORD64) });
                            }

                            ZeroMemory(&imp_oft_section->get_section_data().data()[
                                (imp_description->OriginalFirstThunk - imp_oft_section->get_virtual_address())
                            ], (thunk_items_count + 1) * sizeof(DWORD64));
                        }

                        if (zones) {
                            zones->push_back({ imp_description->FirstThunk , (thunk_items_count + 1) * sizeof(DWORD64) });
                        }

                        ZeroMemory(&imp_ft_section->get_section_data().data()[
                            (imp_description->FirstThunk - imp_ft_section->get_virtual_address())
                        ], (thunk_items_count + 1) * sizeof(DWORD64));
					}
                    
					ZeroMemory(imp_description, sizeof(IMAGE_IMPORT_DESCRIPTOR));
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


