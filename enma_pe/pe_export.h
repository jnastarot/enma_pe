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
	export_table_item();
    export_table_item(const export_table_item& item);
	~export_table_item();

	export_table_item& operator=(const export_table_item& item);
	bool operator==(const export_table_item& item);
public:
	void  set_rva(uint32_t rva);
	void  set_ordinal(uint16_t ordinal);
	void  set_name_ordinal(uint16_t ordinal);
	void  set_has_name(bool b);
	void  set_forward(bool b);
	void  set_forward_name(const std::string& forward_name);
	void  set_func_name(const std::string& func_name);
public:
	uint32_t get_rva() const;
	uint16_t  get_ordinal() const;
	uint16_t  get_name_ordinal() const;
	bool  has_name() const;
	bool  is_forward() const;
	std::string  get_forward_name() const;
	std::string  get_func_name() const;

};

class export_table {
	uint32_t characteristics;
	uint32_t time_stamp;
	uint16_t major_version;
	uint16_t minor_version;
	uint32_t ordinal_base;
	uint32_t number_of_functions;
	uint32_t number_of_names;

	std::string library_name;
	std::vector<export_table_item> export_items;
public:
	export_table();
    export_table(const export_table& exports);
	~export_table();
    export_table& operator=(const export_table& exports);
public:
	void        set_characteristics(uint32_t characteristics);
	void        set_time_stamp(uint32_t time_stamp);
	void        set_major_version(uint16_t major_version);
	void        set_minor_version(uint16_t minor_version);
	void        set_ordinal_base(uint32_t ordinal_base);
	void        set_number_of_functions(uint32_t number_of_functions);
	void        set_number_of_names(uint32_t number_of_names);
	void        set_library_name(const std::string& library_name);
	void        add_item(const export_table_item& item);

public:
	uint32_t    get_characteristics() const;
	uint32_t    get_time_stamp() const;
	uint16_t    get_major_version() const;
	uint16_t    get_minor_version() const;
	uint32_t    get_ordinal_base() const;
	uint32_t    get_number_of_functions() const;
	uint32_t    get_number_of_names() const;
	std::string	get_library_name() const;
    const std::vector<export_table_item>& get_items() const;

	std::vector<export_table_item>& get_items();

	bool get_exported_function(const std::string& func_name, export_table_item * &item);
	bool get_exported_function(uint16_t ordinal, export_table_item * &item);
};


directory_code get_export_table(_In_ const pe_image &image,
	_Out_ export_table& exports);
bool build_export_table(_Inout_ pe_image &image, 
	_Inout_ pe_section& section, _In_ export_table& exports); 
directory_code get_placement_export_table(_In_ const pe_image &image, _Inout_ std::vector<directory_placement>& placement);