#pragma once

class export_table_item {
	uint32_t rva;
	uint16_t ordinal;
	uint16_t name_ordinal;
	bool b_name;
	bool b_forward; 
	std::string forward_name;
	std::string func_name;
public:
	export_table_item::export_table_item();
    export_table_item::export_table_item(const export_table_item& item);
	export_table_item::~export_table_item();

	export_table_item& export_table_item::operator=(const export_table_item& item);
	bool export_table_item::operator==(const export_table_item& item);
public:
	void  export_table_item::set_rva(uint32_t rva);
	void  export_table_item::set_ordinal(uint16_t ordinal);
	void  export_table_item::set_name_ordinal(uint16_t ordinal);
	void  export_table_item::set_has_name(bool b);
	void  export_table_item::set_forward(bool b);
	void  export_table_item::set_forward_name(const std::string& forward_name);
	void  export_table_item::set_func_name(const std::string& func_name);
public:
	uint32_t export_table_item::get_rva() const;
	uint16_t  export_table_item::get_ordinal() const;
	uint16_t  export_table_item::get_name_ordinal() const;
	bool  export_table_item::has_name() const;
	bool  export_table_item::is_forward() const;
	std::string  export_table_item::get_forward_name() const;
	std::string  export_table_item::get_func_name() const;

};

class export_table {
	uint32_t   characteristics;
	uint32_t   time_stamp;
	uint16_t    major_version;
	uint16_t    minor_version;
	uint32_t ordinal_base;
	uint32_t number_of_functions;
	uint32_t number_of_names;

	std::string library_name;
	std::vector<export_table_item> export_items;
public:
	export_table::export_table();
    export_table::export_table(const export_table& exports);
	export_table::~export_table();
    export_table& export_table::operator=(const export_table& exports);
public:
	void        export_table::set_characteristics(uint32_t characteristics);
	void        export_table::set_time_stamp(uint32_t time_stamp);
	void        export_table::set_major_version(uint16_t major_version);
	void        export_table::set_minor_version(uint16_t minor_version);
	void        export_table::set_ordinal_base(uint32_t ordinal_base);
	void        export_table::set_number_of_functions(uint32_t number_of_functions);
	void        export_table::set_number_of_names(uint32_t number_of_names);
	void        export_table::set_library_name(const std::string& library_name);
	void        export_table::add_item(const export_table_item& item);

public:
	uint32_t    export_table::get_characteristics() const;
	uint32_t    export_table::get_time_stamp() const;
	uint16_t    export_table::get_major_version() const;
	uint16_t    export_table::get_minor_version() const;
	uint32_t    export_table::get_ordinal_base() const;
	uint32_t    export_table::get_number_of_functions() const;
	uint32_t    export_table::get_number_of_names() const;
	std::string	export_table::get_library_name() const;
	std::vector<export_table_item>& export_table::get_items();

	bool export_table::get_exported_function(const std::string& func_name, export_table_item * &item);
	bool export_table::get_exported_function(uint16_t ordinal, export_table_item * &item);
};


directory_code get_export_table(_In_ const pe_image &image,
	_Out_ export_table& exports);
bool build_export_table(_Inout_ pe_image &image, 
	_Inout_ pe_section& section, _In_ export_table& exports); 
directory_code get_placement_export_table(_Inout_ pe_image &image, _Inout_ std::vector<directory_placement>& placement);