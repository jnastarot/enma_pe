#pragma once

class imported_func {
	WORD hint;
	WORD ordinal;
	std::string name;
	bool b_import_by_name;

	DWORD iat_rva;
public:
	imported_func::imported_func();
    imported_func::imported_func(DWORD iat_rva, std::string name, WORD hint);
    imported_func::imported_func(DWORD iat_rva, WORD ordinal);

	imported_func::~imported_func();

	imported_func& imported_func::operator=(const imported_func& func);
public:
	void imported_func::set_hint(WORD hint);
	void imported_func::set_ordinal(WORD ordinal);
	void imported_func::set_name(std::string name);
	void imported_func::set_import_by_name(bool b);
	void imported_func::set_iat_rva(DWORD rva);
public:
	WORD imported_func::get_hint() const;
	WORD imported_func::get_ordinal() const;
	std::string imported_func::get_name() const;
	bool  imported_func::is_import_by_name() const;
	DWORD imported_func::get_iat_rva() const;
};

class imported_library {
	std::string name;
	DWORD timestamp;
	DWORD rva_to_iat;
    DWORD rva_to_oft;

	std::vector<imported_func> imported_items;
public:
	imported_library::imported_library();
	imported_library::~imported_library();

	imported_library& imported_library::operator=(const imported_library& library);
public:
	void imported_library::set_name(std::string name);
	void imported_library::set_timestamp(DWORD timestamp);
	void imported_library::set_rva_iat(DWORD rva);
    void imported_library::set_rva_oft(DWORD rva);
	void imported_library::add_item(const imported_func& item);
public:
	std::string imported_library::get_name() const;
	DWORD imported_library::get_timestamp() const;
	DWORD imported_library::get_rva_iat() const;
    DWORD imported_library::get_rva_oft() const;

	std::vector<imported_func>& imported_library::get_items();
};

class import_table {
	std::vector<imported_library> libs;
public:
	import_table::import_table();
	import_table::~import_table();

	import_table& import_table::operator=(const import_table& imports);
public:
	void import_table::add_lib(imported_library& lib);
public:
	std::vector<imported_library>& import_table::get_libs();
	bool import_table::get_imported_lib(std::string lib_name, imported_library * &lib);
	bool import_table::get_imported_func(std::string lib_name, std::string func_name, imported_library * &lib, imported_func * &func);
	bool import_table::get_imported_func(std::string lib_name, WORD ordinal, imported_library * &lib, imported_func * &func);
};


bool get_import_table(_In_ const pe_image &image,	
    _Out_ import_table& imports);
void build_import_table(_Inout_ pe_image &image,
	_Inout_ pe_section& section, _Inout_ import_table& imports, 
    _In_opt_ bool use_original_table = false,
    _In_opt_ bool rebuild_tables = true);
bool erase_import_table(_Inout_ pe_image &image, 
	_Inout_opt_ std::vector<erased_zone>* zones = 0);

