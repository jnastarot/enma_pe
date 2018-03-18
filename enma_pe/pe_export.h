#pragma once

class export_table_item {
	DWORD rva;
	WORD ordinal;
	WORD name_ordinal;
	bool b_name;
	bool b_forward; 
	std::string forward_name;
	std::string name;
public:
	export_table_item::export_table_item();
	export_table_item::~export_table_item();

	export_table_item& export_table_item::operator=(const export_table_item& item);
	bool export_table_item::operator==(const export_table_item& item);
public:
	void  export_table_item::set_rva(DWORD rva);
	void  export_table_item::set_ordinal(WORD ordinal);
	void  export_table_item::set_name_ordinal(WORD ordinal);
	void  export_table_item::set_has_name(bool b);
	void  export_table_item::set_forward(bool b);
	void  export_table_item::set_forward_name(std::string forward_name);
	void  export_table_item::set_name(std::string name);
public:
	DWORD export_table_item::get_rva() const;
	WORD  export_table_item::get_ordinal() const;
	WORD  export_table_item::get_name_ordinal() const;
	bool  export_table_item::has_name() const;
	bool  export_table_item::is_forward() const;
	std::string  export_table_item::get_forward_name() const;
	std::string  export_table_item::get_name() const;

};

class export_table {
	DWORD   characteristics;
	DWORD   time_stamp;
	WORD    major_version;
	WORD    minor_version;
	unsigned int  ordinal_base;
	unsigned int  number_of_functions;
	unsigned int  number_of_names;

	std::string name;
	std::vector<export_table_item> export_items;
public:
	export_table::export_table();
	export_table::~export_table();
    export_table& export_table::operator=(const export_table& exports);
public:
	void			export_table::set_characteristics(DWORD characteristics);
	void			export_table::set_time_stamp(DWORD time_stamp);
	void			export_table::set_major_version(WORD major_version);
	void			export_table::set_minor_version(WORD minor_version);
	void			export_table::set_ordinal_base(unsigned int  ordinal_base);
	void			export_table::set_number_of_functions(unsigned int  number_of_functions);
	void			export_table::set_number_of_names(unsigned int  number_of_names);
	void			export_table::set_name(std::string name);
	void			export_table::add_item(export_table_item& item);

public:
	DWORD			export_table::get_characteristics() const;
	DWORD			export_table::get_time_stamp() const;
	WORD			export_table::get_major_version() const;
	WORD			export_table::get_minor_version() const;
	unsigned int	export_table::get_ordinal_base() const;
	unsigned int	export_table::get_number_of_functions() const;
	unsigned int	export_table::get_number_of_names() const;
	std::string		export_table::get_name() const;
	std::vector<export_table_item>& export_table::get_items();

	bool export_table::get_exported_function(std::string name, export_table_item * &item);
	bool export_table::get_exported_function(WORD ordinal, export_table_item * &item);
};


bool get_export_table(_In_ const pe_image &image,	
	_Out_ export_table& exports);
void build_export_table(_Inout_ pe_image &image, 
	_Inout_ pe_section& section, _In_ export_table& exports); 
bool erase_export_table(_Inout_ pe_image &image,
	_Inout_opt_ std::vector<erased_zone>* zones = 0);

 