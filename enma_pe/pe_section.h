#pragma once

class pe_section{
	std::string section_name;
	uint32_t virtual_size;
	uint32_t virtual_address;
	uint32_t pointer_to_raw;
	uint32_t characteristics;

	std::vector<uint8_t> section_data;
public:
	pe_section::pe_section();
	pe_section::pe_section(const pe_section& section);
	pe_section::pe_section(const image_section_header& header);
	pe_section::pe_section(const image_section_header& header, const std::vector<uint8_t>& data);
	pe_section::~pe_section();
    
	pe_section& pe_section::operator=(const pe_section& section);

public:
	pe_section& pe_section::set_section_name(const std::string& name);
	pe_section& pe_section::set_virtual_size(uint32_t virtual_size);
	pe_section& pe_section::set_virtual_address(uint32_t virtual_address);
	pe_section& pe_section::set_size_of_raw_data(uint32_t size_of_raw_data);
	pe_section& pe_section::set_pointer_to_raw(uint32_t pointer_to_raw);
	pe_section& pe_section::set_characteristics(uint32_t characteristics);

	pe_section& pe_section::set_readable(bool flag);
	pe_section& pe_section::set_writeable(bool flag);
	pe_section& pe_section::set_executable(bool flag);

	void pe_section::add_data(const uint8_t * data,uint32_t data_size);
public:
	std::string pe_section::get_section_name() const;
	uint32_t pe_section::get_virtual_size() const;
	uint32_t pe_section::get_virtual_address() const;
	uint32_t pe_section::get_size_of_raw_data() const;
	uint32_t pe_section::get_pointer_to_raw() const;
	uint32_t pe_section::get_characteristics() const;

	bool pe_section::is_readable() const;
	bool pe_section::is_writeable() const;
	bool pe_section::is_executable() const;

    const std::vector<uint8_t>& pe_section::get_section_data() const;
    std::vector<uint8_t>& pe_section::get_section_data();
};


