#pragma once
class pe_section{
	std::string section_name;
	DWORD virtual_size;
	DWORD virtual_address;
	DWORD pointer_to_raw;
	DWORD characteristics;

	std::vector<BYTE> section_data;
public:
	pe_section::pe_section();
	pe_section::pe_section(const pe_section& section);
	pe_section::pe_section(const IMAGE_SECTION_HEADER& header);
	pe_section::pe_section(const IMAGE_SECTION_HEADER& header, const std::vector<BYTE>& data);
	pe_section::~pe_section();

	pe_section& pe_section::operator=(const pe_section& section);
public:
	pe_section& pe_section::set_section_name(const std::string& name);
	pe_section& pe_section::set_virtual_size(DWORD virtual_size);
	pe_section& pe_section::set_virtual_address(DWORD virtual_address);
	pe_section& pe_section::set_size_of_raw_data(DWORD size_of_raw_data);
	pe_section& pe_section::set_pointer_to_raw(DWORD pointer_to_raw);
	pe_section& pe_section::set_characteristics(DWORD characteristics);

	pe_section& pe_section::set_readable(bool flag);
	pe_section& pe_section::set_writeable(bool flag);
	pe_section& pe_section::set_executable(bool flag);

	void pe_section::add_data(void * data,unsigned int data_size);
public:
	std::string pe_section::get_section_name() const;
	DWORD pe_section::get_virtual_size() const;
	DWORD pe_section::get_virtual_address() const;
	DWORD pe_section::get_size_of_raw_data() const;
	DWORD pe_section::get_pointer_to_raw() const;
	DWORD pe_section::get_characteristics() const;

	bool pe_section::is_readable() const;
	bool pe_section::is_writeable() const;
	bool pe_section::is_executable() const;

	std::vector<BYTE>& pe_section::get_section_data();
};

