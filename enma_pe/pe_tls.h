#pragma once

class tls_table {
	DWORD   start_address_raw_data;
	DWORD   end_address_of_raw_data;
	DWORD   address_of_index;            
	DWORD   address_of_callbacks;      
    DWORD   size_of_zero_fill;
	DWORD	characteristics;

	struct tls_callback{
		DWORD rva_callback;
		bool use_relocation;
	};

	std::vector<BYTE> raw_data;
	std::vector<tls_table::tls_callback> callbacks;
public:
	tls_table::tls_table();
	tls_table::~tls_table();
	tls_table& tls_table::operator=(const tls_table& tls);
public:
	void tls_table::set_start_address_raw_data(DWORD   start_address_raw_data);
	void tls_table::set_end_address_of_raw_data(DWORD   end_address_of_raw_data);
	void tls_table::set_address_of_index(DWORD   address_of_index);
	void tls_table::set_address_of_callbacks(DWORD   address_of_callbacks);
    void tls_table::set_size_of_zero_fill(DWORD   size_of_zero_fill);
	void tls_table::set_characteristics(DWORD   characteristics);
public:
	DWORD tls_table::get_start_address_raw_data() const;
	DWORD tls_table::get_end_address_of_raw_data() const;
	DWORD tls_table::get_address_of_index() const;
	DWORD tls_table::get_address_of_callbacks() const;
    DWORD tls_table::get_size_of_zero_fill() const;
	DWORD tls_table::get_characteristics() const;

	std::vector<BYTE>&				  tls_table::get_raw_data();
	std::vector<tls_table::tls_callback>& tls_table::get_callbacks();
};


bool get_tls_table(_In_ const pe_image &image,_Out_ tls_table& tls);
void build_tls_table(_Inout_ pe_image &image, _Inout_ pe_section& section,
	_Inout_ tls_table& tls, _Out_ relocation_table& relocs);
bool erase_tls_table(_Inout_ pe_image &image,
	_Inout_opt_ std::vector<erased_zone>* zones = 0,
	_Inout_opt_ relocation_table* relocs = 0);
