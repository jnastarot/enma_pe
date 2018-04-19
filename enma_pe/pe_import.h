#pragma once

class imported_func {
	uint16_t hint;
	uint16_t ordinal;
	std::string func_name;
	bool b_import_by_name;

	uint32_t iat_rva;
public:
	imported_func::imported_func();
    imported_func::imported_func(const imported_func& func);
    imported_func::imported_func(uint32_t iat_rva, const std::string& func_name, uint16_t hint);
    imported_func::imported_func(uint32_t iat_rva, uint16_t ordinal);

	imported_func::~imported_func();

	imported_func& imported_func::operator=(const imported_func& func);
public:
	void imported_func::set_hint(uint16_t hint);
	void imported_func::set_ordinal(uint16_t ordinal);
	void imported_func::set_func_name(const std::string& func_name);
	void imported_func::set_import_by_name(bool b);
	void imported_func::set_iat_rva(uint32_t rva);
public:
	uint16_t imported_func::get_hint() const;
	uint16_t imported_func::get_ordinal() const;
	std::string imported_func::get_func_name() const;
	bool  imported_func::is_import_by_name() const;
	uint32_t imported_func::get_iat_rva() const;
};

class imported_library {
	std::string library_name;
	uint32_t timestamp;
	uint32_t rva_to_iat;
    uint32_t rva_to_oft;

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
	void imported_library::add_item(const imported_func& item);
public:
	std::string imported_library::get_library_name() const;
	uint32_t imported_library::get_timestamp() const;
	uint32_t imported_library::get_rva_iat() const;
    uint32_t imported_library::get_rva_oft() const;

	std::vector<imported_func>& imported_library::get_items();
};

class import_table {
	std::vector<imported_library> libs;
public:
	import_table::import_table();
    import_table::import_table(const import_table& imports);
	import_table::~import_table();

	import_table& import_table::operator=(const import_table& imports);
public:
	void import_table::add_lib(const imported_library& lib);
public:
	std::vector<imported_library>& import_table::get_libs();
	bool import_table::get_imported_lib(const std::string& lib_name, imported_library * &lib);
	bool import_table::get_imported_func(const std::string& lib_name, const std::string& func_name, imported_library * &lib, imported_func * &func);
	bool import_table::get_imported_func(const std::string& lib_name, uint16_t ordinal, imported_library * &lib, imported_func * &func);
};


bool get_import_table(_In_ const pe_image &image,	
    _Out_ import_table& imports);
void build_import_table(_Inout_ pe_image &image,
	_Inout_ pe_section& section, _Inout_ import_table& imports, 
    _In_opt_ bool use_original_table = false,
    _In_opt_ bool rebuild_tables = true);

bool get_placement_import_table(_In_ const pe_image &image,_Inout_ std::vector<directory_placement>& placement);
