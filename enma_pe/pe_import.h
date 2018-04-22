#pragma once

class imported_func {
    uint32_t iat_rva;

	uint16_t hint;
    std::string func_name;

	uint16_t ordinal;
	
    uint64_t iat_item;

	bool b_import_by_name;
public:
	imported_func::imported_func();
    imported_func::imported_func(const imported_func& func);
    imported_func::imported_func(uint32_t iat_rva, const std::string& func_name, uint16_t hint, uint64_t iat_item = 0);
    imported_func::imported_func(uint32_t iat_rva, uint16_t ordinal, uint64_t iat_item = 0);

	imported_func::~imported_func();

	imported_func& imported_func::operator=(const imported_func& func);
public:
    void imported_func::set_func_name(const std::string& func_name);
	void imported_func::set_hint(uint16_t hint);
	void imported_func::set_ordinal(uint16_t ordinal);
	void imported_func::set_iat_rva(uint32_t rva);
    void imported_func::set_iat_item(uint64_t iat_item);
    void imported_func::set_import_by_name(bool b);
public:
    std::string imported_func::get_func_name() const;
	uint16_t imported_func::get_hint() const;
	uint16_t imported_func::get_ordinal() const;
	uint32_t imported_func::get_iat_rva() const;
    uint64_t imported_func::get_iat_item()const;
    bool  imported_func::is_import_by_name() const;
};

class imported_library {
	std::string library_name;
	
    uint32_t timestamp;
	uint32_t iat_rva;
    uint32_t oft_rva;
    uint32_t library_name_rva;

	std::vector<imported_func> imported_items;
public:
	imported_library::imported_library();
    imported_library::imported_library(const imported_library& library);
	imported_library::~imported_library();

	imported_library& imported_library::operator=(const imported_library& library);
public:
	void imported_library::set_library_name(const std::string& library_name);
	void imported_library::set_timestamp(uint32_t timestamp);
	void imported_library::set_rva_iat(uint32_t rva);
    void imported_library::set_rva_oft(uint32_t rva);
    void imported_library::set_rva_library_name(uint32_t rva);
	void imported_library::add_item(const imported_func& item);
    void imported_library::clear();
public:
    size_t imported_library::size();
	std::string imported_library::get_library_name() const;
	uint32_t imported_library::get_timestamp() const;
	uint32_t imported_library::get_rva_iat() const;
    uint32_t imported_library::get_rva_oft() const;
    uint32_t imported_library::get_rva_library_name() const;

	std::vector<imported_func>& imported_library::get_items();
};

class import_table {
	std::vector<imported_library> libraries;
public:
	import_table::import_table();
    import_table::import_table(const import_table& imports);
	import_table::~import_table();

	import_table& import_table::operator=(const import_table& imports);
public:
	void import_table::add_library(const imported_library& lib);
    void import_table::clear();
public:
    size_t import_table::size();

	std::vector<imported_library>& import_table::get_libraries();
	bool import_table::get_imported_lib(const std::string& lib_name, imported_library * &lib);
	bool import_table::get_imported_func(const std::string& lib_name, const std::string& func_name, imported_library * &lib, imported_func * &func);
	bool import_table::get_imported_func(const std::string& lib_name, uint16_t ordinal, imported_library * &lib, imported_func * &func);
};


enum import_table_build_id {
    import_table_build_iat          = 1 << 1,
    import_table_build_oft          = 1 << 2,
    import_table_build_library_name = 1 << 3,
};


directory_code get_import_table(_In_ const pe_image &image,
    _Out_ import_table& imports,
    _In_opt_ const bound_import_table* bound_imports = 0);

bool build_internal_import_data(_Inout_ pe_image &image,
    _Inout_ pe_section& section, _Inout_ import_table& imports,
    _In_ uint32_t build_items_ids/*import_table_build_id*/,
    _In_opt_ const bound_import_table* bound_imports = 0);
bool build_import_table_only(_Inout_ pe_image &image,
	_Inout_ pe_section& section, _Inout_ import_table& imports);

bool build_import_table_full(_Inout_ pe_image &image,
    _Inout_ pe_section& section, _Inout_ import_table& imports,
    _In_opt_ const bound_import_table* bound_imports = 0);

directory_code get_placement_import_table(_In_ const pe_image &image,_Inout_ std::vector<directory_placement>& placement);
