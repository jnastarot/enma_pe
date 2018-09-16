#include "stdafx.h"
#include "pe_import.h"


imported_func::imported_func() {
	hint				= 0;
	ordinal				= 0;
	b_import_by_name	= 0;
	iat_rva				= 0;
    iat_item            = 0;
    func_name.clear();
}
imported_func::imported_func(const imported_func& func) {
    this->operator=(func);
}

imported_func::imported_func(uint32_t iat_rva, const std::string& func_name, uint16_t hint, uint64_t iat_item):
    iat_rva(iat_rva),  hint(hint), func_name(func_name), ordinal(0), iat_item(iat_item), b_import_by_name(true){

}
imported_func::imported_func(uint32_t iat_rva, uint16_t ordinal, uint64_t iat_item):
    iat_rva(iat_rva), hint(0), func_name(""), ordinal(ordinal), iat_item(iat_item), b_import_by_name(false){
}

imported_func::~imported_func() {

}

imported_func& imported_func::operator=(const imported_func& func) {

	this->iat_rva = func.iat_rva;
	this->hint = func.hint;
	this->func_name = func.func_name;
	this->ordinal = func.ordinal;
	this->b_import_by_name = func.b_import_by_name;
    this->iat_item = func.iat_item;

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
void imported_func::set_iat_item(uint64_t iat_item) {
    this->iat_item = iat_item;
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
uint64_t imported_func::get_iat_item()const {
    return this->iat_item;
}

imported_library::imported_library() {
	timestamp		= 0;
    iat_rva		    = 0;
    oft_rva         = 0;
    library_name_rva = 0;
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
	this->iat_rva        = library.iat_rva;
    this->oft_rva        = library.oft_rva;
    this->library_name_rva = library.library_name_rva;
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
	this->iat_rva = rva;
}
void imported_library::set_rva_oft(uint32_t rva) {
    this->oft_rva = rva;
}
void imported_library::set_rva_library_name(uint32_t rva) {
    this->library_name_rva = rva;
}

void imported_library::add_item(const imported_func& item) {
	this->imported_items.push_back(item);
}
void imported_library::clear() {
    this->imported_items.clear();
}

size_t imported_library::size() {
    return this->imported_items.size();
}

std::string imported_library::get_library_name() const {
	return this->library_name;
}
uint32_t imported_library::get_timestamp() const {
	return this->timestamp;
}
uint32_t imported_library::get_rva_iat() const {
	return this->iat_rva;
}
uint32_t imported_library::get_rva_oft() const {
    return this->oft_rva;
}
uint32_t imported_library::get_rva_library_name() const {
    return this->library_name_rva;
}

const std::vector<imported_func>& imported_library::get_items() const {
    return this->imported_items;
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

	this->libraries = imports.libraries;

    return *this;
}

void import_table::add_library(const imported_library& lib) {
	this->libraries.push_back(lib);
}

void import_table::clear() {
    this->libraries.clear();
}

size_t import_table::size() {
    return this->libraries.size();
}

std::vector<imported_library>& import_table::get_libraries() {
	return this->libraries;
}

bool import_table::get_imported_lib(const std::string& lib_name, imported_library * &lib) {

    for (auto& library : libraries) {
        if (library.get_library_name() == lib_name) {
            lib = &library;
            return true;
        }
    }

	return false;
}
bool import_table::get_imported_func(const std::string& lib_name, const std::string& func_name, imported_library * &lib, imported_func * &_func) {

    for (auto& library : libraries) {
        if (library.get_library_name() == lib_name) {
            for (auto& func : library.get_items()) {
                if (func.is_import_by_name() && func.get_func_name() == func_name) {
                    lib = &library;
                    _func = &func;
                    return true;
                }
            }
        }
    }

	return false;
}
bool import_table::get_imported_func(const std::string& lib_name, uint16_t ordinal, imported_library * &lib, imported_func * &_func) {

    for (auto& library : libraries) {
        if (library.get_library_name() == lib_name) {
            for (auto& func : library.get_items()) {
                if (!func.is_import_by_name() && func.get_ordinal() == ordinal) {
                    lib = &library;
                    _func = &func;
                    return true;
                }
            }
        }
    }

	return false;
}

template<typename image_format>
directory_code _get_import_table(const pe_image &image, import_table& imports,const bound_import_table& bound_imports) {
    imports.clear();

    uint32_t virtual_address = image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_IMPORT);


    if (virtual_address) {
        pe_image_io import_io(image);
        import_io.set_image_offset(virtual_address);

        image_import_descriptor import_desc;

        if (import_io.read(&import_desc,sizeof(import_desc)) != enma_io_success) {
            return directory_code::directory_code_currupted;
        }

        if (import_desc.first_thunk) {

            do {
                imported_library library;

                std::string library_name;
                if (pe_image_io(image).set_image_offset(import_desc.name).read_string(library_name) != enma_io_success) {
                    return directory_code::directory_code_currupted;
                }

                library.set_library_name(library_name);
                library.set_rva_library_name(import_desc.name);
                library.set_rva_iat(import_desc.first_thunk);
                library.set_rva_oft(import_desc.original_first_thunk);
                library.set_timestamp(import_desc.time_date_stamp);

                bool is_bound_library = (import_desc.time_date_stamp &&
                    bound_imports.has_library(library_name, import_desc.time_date_stamp));

                pe_image_io iat_io(image);
                pe_image_io original_iat_io(image);
                iat_io.set_image_offset(import_desc.first_thunk);
                original_iat_io.set_image_offset(import_desc.original_first_thunk);


                typename image_format::ptr_size iat_item = 0;

                if (iat_io.read(&iat_item, sizeof(iat_item)) != enma_io_success) {
                    return directory_code::directory_code_currupted;
                }

                if (iat_item) {
                    do {
                        imported_func func;

                        if (import_desc.original_first_thunk) {
                            typename image_format::ptr_size original_iat_item = 0;
                            if (original_iat_io.read(&original_iat_item, sizeof(original_iat_item)) != enma_io_success) {
                                return directory_code::directory_code_currupted;
                            }

                            if (original_iat_item&image_format::ordinal_flag) {

                                library.add_item(imported_func(iat_io.get_image_offset() - (uint32_t)sizeof(typename image_format::ptr_size),
                                    uint16_t(original_iat_item^image_format::ordinal_flag), iat_item));
                            }
                            else {
                                pe_image_io import_func_name_io(image);
                                import_func_name_io.set_image_offset(uint32_t(original_iat_item));

                                uint16_t hint;
                                std::string func_name;

                                if (import_func_name_io.read(&hint,sizeof(hint)) != enma_io_success) {
                                    return directory_code::directory_code_currupted;
                                }

                                if (import_func_name_io.read_string(func_name) != enma_io_success) {
                                    return directory_code::directory_code_currupted;
                                }

                                library.add_item(imported_func(iat_io.get_image_offset() - (uint32_t)sizeof(typename image_format::ptr_size),
                                    func_name, hint, iat_item));
                            }
                        }
                        else {

                            if (is_bound_library) {
                                library.add_item(imported_func(iat_io.get_image_offset() - (uint32_t)sizeof(typename image_format::ptr_size), "",
                                    0, iat_item));
                            }
                            else {

                                if (iat_item&image_format::ordinal_flag) {
                                    library.add_item(imported_func(iat_io.get_image_offset() - (uint32_t)sizeof(typename image_format::ptr_size),
                                        uint16_t(iat_item^image_format::ordinal_flag), iat_item));
                                }
                                else {
                                    pe_image_io import_func_name_io(image);
                                    import_func_name_io.set_image_offset(uint32_t(iat_item));

                                    uint16_t hint;
                                    std::string func_name;

                                    if (import_func_name_io.read(&hint, sizeof(hint)) != enma_io_success) {
                                        return directory_code::directory_code_currupted;
                                    }

                                    if (import_func_name_io.read_string(func_name) != enma_io_success) {
                                        return directory_code::directory_code_currupted;
                                    }

                                    library.add_item(imported_func(iat_io.get_image_offset() - (uint32_t)sizeof(typename image_format::ptr_size),
                                        func_name, hint, iat_item));
                                }
                            }
                        }

                        if (iat_io.read(&iat_item, sizeof(iat_item)) != enma_io_success) {
                            return directory_code::directory_code_currupted;
                        }
                    } while (iat_item);
                }

                imports.add_library(library);

                if (import_io.read(&import_desc, sizeof(import_desc)) != enma_io_success) {
                    return directory_code::directory_code_currupted;
                }
            } while (import_desc.first_thunk);
        }

        return directory_code::directory_code_success;
    }


    return directory_code::directory_code_not_present;
}

template<typename image_format>
bool _build_internal_import_data(pe_image &image, pe_section& section, import_table& imports,
    uint32_t build_items_ids/*import_table_build_id*/,
    const bound_import_table* bound_imports = 0) {

    if (imports.size()) {
        pe_section_io import_io(section, image, enma_io_mode_allow_expand);
        import_io.align_up(0x10).seek_to_end();

        uint32_t names_size = 0;
        uint32_t thunk_size = 0;
        uint32_t original_thunk_size = 0;

        for (auto& library : imports.get_libraries()) {
            if (!library.size()) { continue; }

            uint32_t current_lib_thunk_size = 0;

            if ((build_items_ids & import_table_build_iat) || (build_items_ids & import_table_build_oft)) {

                for (auto& func : library.get_items()) {

                    if (func.is_import_by_name()) {
                        names_size += uint32_t(ALIGN_UP(func.get_func_name().length() + 1 + INT16_SIZE, 0x2));
                    }
                    current_lib_thunk_size += (uint32_t)sizeof(typename image_format::ptr_size);
                }
                current_lib_thunk_size += (uint32_t)sizeof(typename image_format::ptr_size);

                if (build_items_ids & import_table_build_iat) {
                    thunk_size += current_lib_thunk_size;
                }
                if (build_items_ids & import_table_build_oft) {
                    original_thunk_size += current_lib_thunk_size;
                }
            }

            if (build_items_ids & import_table_build_library_name) {
                names_size += uint32_t(ALIGN_UP(library.get_library_name().length() + 1, 0x2));
            }
        }

        uint32_t import_iat_virtual_address = import_io.get_section_offset();
        uint32_t import_iat_virtual_size    = thunk_size;

        pe_section_io import_thunks_io(section, image, enma_io_mode_allow_expand);
        pe_section_io import_original_thunks_io(section, image, enma_io_mode_allow_expand);
        pe_section_io import_names_io(section, image, enma_io_mode_allow_expand);

        import_thunks_io.set_section_offset(import_io.get_section_offset());
        import_original_thunks_io.set_section_offset(import_io.get_section_offset() + thunk_size);
        import_names_io.set_section_offset(import_io.get_section_offset() + thunk_size + original_thunk_size);

  
        for (auto& library : imports.get_libraries()) {

            bool is_bound_library = (library.get_timestamp() == UINT32_MAX && bound_imports &&
                bound_imports->has_library(library.get_library_name()));

            if (library.size() && 
                ((build_items_ids & import_table_build_iat) || (build_items_ids & import_table_build_oft))) {

                std::vector<typename image_format::ptr_size> thunk_table;
                std::vector<typename image_format::ptr_size> original_thunk_table;

                uint32_t func_iat_rva = import_thunks_io.get_section_offset();

                for (auto& func : library.get_items()) {
                    typename image_format::ptr_size thunk_item;

                    if (build_items_ids & import_table_build_iat) {
                        func.set_iat_rva(func_iat_rva);
                        func_iat_rva += (uint32_t)sizeof(typename image_format::ptr_size);
                    }

                    if (!is_bound_library || (build_items_ids & import_table_build_oft)) {

                        if (func.is_import_by_name()) {
                            thunk_item = import_names_io.get_section_offset();

                            uint16_t hint = func.get_hint();

                            if (import_names_io.write(&hint, sizeof(hint)) != enma_io_success) {
                                return false;
                            }

                            if (import_names_io.write(
                                (void*)func.get_func_name().c_str(), uint32_t(func.get_func_name().length() + 1)) != enma_io_success) {

                                return false;
                            }
                            import_names_io.align_up_offset(0x2);
                        }
                        else {
                            thunk_item = func.get_ordinal() | image_format::ordinal_flag;
                        }
                    }

                    if (is_bound_library) {
                        thunk_table.push_back(typename image_format::ptr_size(func.get_iat_item()));
                        original_thunk_table.push_back(thunk_item);
                    }
                    else {
                        thunk_table.push_back(thunk_item);
                        original_thunk_table.push_back(thunk_item);
                    }
                }


                if (build_items_ids & import_table_build_iat) {
                    library.set_rva_iat(import_thunks_io.get_section_offset());
                    typename image_format::ptr_size null_item = 0;

                    for (auto& item : thunk_table) {
                        if (import_thunks_io.write(&item, sizeof(item)) != enma_io_success) {
                            return false;
                        }
                    }
                    
                    if (import_thunks_io.write(&null_item, sizeof(null_item)) != enma_io_success) {
                        return false;
                    }
                }

                if (build_items_ids & import_table_build_oft) {
                    library.set_rva_oft(import_original_thunks_io.get_section_offset());
                    typename image_format::ptr_size null_item = 0;

                    for (auto& item : original_thunk_table) {
                        if (import_original_thunks_io.write(&item, sizeof(item)) != enma_io_success) {
                            return false;
                        }
                    }

                    if (import_original_thunks_io.write(&null_item, sizeof(null_item)) != enma_io_success) {
                        return false;
                    }
                }
            }

            if (build_items_ids & import_table_build_library_name) {
                library.set_rva_library_name(import_names_io.get_section_offset());

                if (import_names_io.write(
                    (void*)library.get_library_name().c_str(), uint32_t(library.get_library_name().length() + 1)) != enma_io_success) {

                    return false;
                }
                import_names_io.align_up_offset(0x2);
            }
        }
       
        image.set_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_IAT, import_iat_virtual_address);
        image.set_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_IAT, import_iat_virtual_size);
    }
    else {
        image.set_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_IAT, 0);
        image.set_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_IAT, 0);
    }

    return true;
}


template<typename image_format>
directory_code _get_placement_import_table(const pe_image &image, std::vector<directory_placement>& placement,
    const bound_import_table& bound_imports) {

    uint32_t virtual_address = image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_IMPORT);


    if (virtual_address) {
        pe_image_io import_io(image);
        import_io.set_image_offset(virtual_address);

        image_import_descriptor import_desc;

        if (import_io.read(&import_desc, sizeof(import_desc)) != enma_io_success) {
            return directory_code::directory_code_currupted;
        }
        uint32_t import_desc_table_size = sizeof(image_import_descriptor);

        if (import_desc.first_thunk) {

            do {

                std::string library_name;
                if (pe_image_io(image).set_image_offset(import_desc.name).read_string(library_name) != enma_io_success) {
                    return directory_code::directory_code_currupted;
                }

                placement.push_back({ import_desc.name ,ALIGN_UP(library_name.length()+1,0x2),dp_id_import_desc });

                bool is_bound_library = (import_desc.time_date_stamp == UINT32_MAX &&
                    bound_imports.has_library(library_name));

                pe_image_io iat_io(image);
                pe_image_io original_iat_io(image);
                iat_io.set_image_offset(import_desc.first_thunk);
                original_iat_io.set_image_offset(import_desc.original_first_thunk);

              
                typename image_format::ptr_size iat_item = 0;

                if (iat_io.read(&iat_item, sizeof(iat_item)) != enma_io_success) {
                    return directory_code::directory_code_currupted;
                }

                uint32_t iat_table_size = sizeof(typename image_format::ptr_size);

                if (iat_item) {
                    do {
                        imported_func func;

                        if (import_desc.original_first_thunk) {
                            typename image_format::ptr_size original_iat_item = 0;
                            if (original_iat_io.read(&original_iat_item, sizeof(original_iat_item)) != enma_io_success) {
                                return directory_code::directory_code_currupted;
                            }

                            if (!(original_iat_item&image_format::ordinal_flag)) {

                                pe_image_io import_func_name_io(image);
                                import_func_name_io.set_image_offset(uint32_t(original_iat_item + sizeof(uint16_t)));

                                std::string func_name;

                                if (import_func_name_io.read_string(func_name) != enma_io_success) {
                                    return directory_code::directory_code_currupted;
                                }

                                placement.push_back({ (uint32_t)original_iat_item ,
                                    ALIGN_UP(func_name.length() + 1 + sizeof(uint16_t),0x2),dp_id_import_names });
                            }
                        }
                        else {

                            if (!is_bound_library && !(iat_item&image_format::ordinal_flag)) {

                                pe_image_io import_func_name_io(image);
                                import_func_name_io.set_image_offset(uint32_t(iat_item + sizeof(uint16_t)));

                                std::string func_name;

                                if (import_func_name_io.read_string(func_name) == enma_io_success) {
                                    placement.push_back({ (uint32_t)(iat_item + sizeof(uint16_t)) ,
                                      ALIGN_UP(func_name.length() + 1 + sizeof(uint16_t),0x2),dp_id_import_names });
                                }
                            }
                        }

                        iat_table_size += (uint32_t)sizeof(typename image_format::ptr_size);

                        if (iat_io.read(&iat_item, sizeof(iat_item)) != enma_io_success) {
                            return directory_code::directory_code_currupted;
                        }
                    } while (iat_item);
                }

                if (import_desc.original_first_thunk) {
                    placement.push_back({ import_desc.original_first_thunk ,iat_table_size,dp_id_import_oft });
                }
                placement.push_back({ import_desc.first_thunk ,iat_table_size,dp_id_import_iat });


                import_desc_table_size += (uint32_t)sizeof(image_import_descriptor);

                if (import_io.read(&import_desc, sizeof(import_desc)) != enma_io_success) {
                    return directory_code::directory_code_currupted;
                }
            } while (import_desc.first_thunk);
        }

        placement.push_back({ virtual_address ,import_desc_table_size,dp_id_import_desc });

        return directory_code::directory_code_success;
    }


    return directory_code::directory_code_not_present;
}


directory_code get_import_table(const pe_image &image, import_table& imports, const bound_import_table& bound_imports) {

    if (image.is_x32_image()) {
        return _get_import_table<image_32>(image, imports, bound_imports);
    }
    else {
        return _get_import_table<image_64>(image, imports, bound_imports);
    }
}


bool build_internal_import_data(pe_image &image, pe_section& section, import_table& imports,
     uint32_t build_items_ids/*import_table_build_id*/,
     const bound_import_table* bound_imports) {

    if (image.is_x32_image()) {
        return _build_internal_import_data<image_32>(image, section, imports, build_items_ids, bound_imports);
    }
    else {
        return _build_internal_import_data<image_64>(image, section, imports, build_items_ids, bound_imports);
    }
}


bool build_import_table_only(pe_image &image, pe_section& section, import_table& imports) {

    if (imports.size()) {
        pe_section_io import_io(section, image);
        import_io.align_up(0x10).seek_to_end();

        image.set_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_IMPORT, import_io.get_section_offset());
        image.set_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_IMPORT, uint32_t(imports.size()*sizeof(image_import_descriptor)));

        for (auto& library : imports.get_libraries()) {
            image_import_descriptor import_desc;
            import_desc.original_first_thunk = library.get_rva_oft();        
            import_desc.time_date_stamp      = library.get_timestamp();
            import_desc.forwarder_chain      = 0;
            import_desc.name                 = library.get_rva_library_name();
            import_desc.first_thunk          = library.get_rva_iat();


            if (import_io.write(&import_desc, sizeof(import_desc)) != enma_io_success) {
                return false;
            }
        }
        image_import_descriptor null_desc;
        memset(&null_desc,0,sizeof(null_desc));
        if (import_io.write(&null_desc, sizeof(null_desc)) != enma_io_success) {
            return false;
        }
    }
    else {
        image.set_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_IMPORT, 0);
        image.set_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_IMPORT, 0);
    }

    return true;
}

bool build_import_table_full(pe_image &image,
    pe_section& section, import_table& imports,
    const bound_import_table* bound_imports) {

    return build_internal_import_data(image, section, imports,
        import_table_build_iat | import_table_build_oft | import_table_build_library_name, bound_imports) && 
        build_import_table_only(image,section,imports);
}

directory_code get_placement_import_table(const pe_image &image, std::vector<directory_placement>& placement,
    const bound_import_table& bound_imports) {

    if (image.is_x32_image()) {
        return _get_placement_import_table<image_32>(image, placement, bound_imports);
    }
    else {
        return _get_placement_import_table<image_64>(image, placement, bound_imports);
    }
}