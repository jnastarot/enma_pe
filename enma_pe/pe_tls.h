#pragma once

class tls_table {
    uint32_t   start_address_raw_data;
    uint32_t   end_address_raw_data;
    uint32_t   address_of_index;
    uint32_t   address_of_callbacks;
    uint32_t   size_of_zero_fill;
    uint32_t	characteristics;

	struct tls_callback{
        uint32_t rva_callback;
		bool use_relocation;
	};

	std::vector<uint8_t> raw_data;
	std::vector<tls_table::tls_callback> callbacks;
public:
	tls_table::tls_table();
    tls_table::tls_table(const tls_table& tls);
	tls_table::~tls_table();

	tls_table& tls_table::operator=(const tls_table& tls);
public:
	void tls_table::set_start_address_raw_data(uint32_t   start_address_raw_data);
	void tls_table::set_end_address_raw_data(uint32_t   end_address_raw_data);
	void tls_table::set_address_of_index(uint32_t   address_of_index);
	void tls_table::set_address_of_callbacks(uint32_t   address_of_callbacks);
    void tls_table::set_size_of_zero_fill(uint32_t   size_of_zero_fill);
	void tls_table::set_characteristics(uint32_t   characteristics);
public:
    uint32_t tls_table::get_start_address_raw_data() const;
    uint32_t tls_table::get_end_address_raw_data() const;
    uint32_t tls_table::get_address_of_index() const;
    uint32_t tls_table::get_address_of_callbacks() const;
    uint32_t tls_table::get_size_of_zero_fill() const;
    uint32_t tls_table::get_characteristics() const;

	std::vector<uint8_t>&				  tls_table::get_raw_data();
	std::vector<tls_table::tls_callback>& tls_table::get_callbacks();
};

enum tls_table_build_id {
    tls_table_build_raw_data             = 1 << 1,
    tls_table_build_address_of_index     = 1 << 2,
    tls_table_build_callbacks            = 1 << 3,
};



bool get_tls_table(_In_ const pe_image &image,_Out_ tls_table& tls);


void build_internal_tls_data(_In_ const pe_image &image, _Inout_ pe_section& section,
    _Out_ tls_table& tls, _Out_ relocation_table& relocs,
    _In_ uint32_t build_items_ids/*tls_table_build_id*/);
void build_tls_table_only(_Inout_ pe_image &image, _Inout_ pe_section& section,
	_Inout_ tls_table& tls, _Out_ relocation_table& relocs);
void build_tls_full(_Inout_ pe_image &image, _Inout_ pe_section& section, 
    _Inout_ tls_table& tls, _Out_ relocation_table& relocs);

bool get_placement_tls_table(_Inout_ pe_image &image, _Inout_ std::vector<directory_placement>& placement);
