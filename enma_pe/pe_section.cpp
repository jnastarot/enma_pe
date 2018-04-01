#include "stdafx.h"
#include "pe_section.h"


pe_section::pe_section() {
    section_name.clear();
	virtual_size		= 0;
	virtual_address		= 0;
	pointer_to_raw		= 0;
	characteristics		= 0;

	section_data.clear();
}
pe_section::pe_section(const pe_section& section) {
    this->operator=(section);
}

pe_section::pe_section(const IMAGE_SECTION_HEADER& header) {
	this->section_name.reserve(8);
	this->section_name.resize(lstrlenA((char*)header.Name));
	memcpy((void*)this->section_name.data(), header.Name,8);

	this->virtual_size		= header.Misc.VirtualSize;
	this->virtual_address	= header.VirtualAddress;
	this->pointer_to_raw	= header.PointerToRawData;
	this->characteristics	= header.Characteristics;

	this->section_data.clear();
}
pe_section::pe_section(const IMAGE_SECTION_HEADER& header, const std::vector<BYTE>& data) {
	this->section_name.reserve(8);
	this->section_name.resize(lstrlenA((char*)header.Name));
	memcpy((void*)this->section_name.data(), header.Name, 8);

	this->virtual_size		= header.Misc.VirtualSize;
	this->virtual_address	= header.VirtualAddress;
	this->pointer_to_raw	= header.PointerToRawData;
	this->characteristics	= header.Characteristics;

	this->section_data = data;
}
pe_section::~pe_section() {

}

pe_section& pe_section::operator=(const pe_section& section) {
	this->section_name = section.section_name;
	this->virtual_size		= section.virtual_size;
	this->virtual_address	= section.virtual_address;
	this->pointer_to_raw	= section.pointer_to_raw;
	this->characteristics	= section.characteristics;
	this->section_data		= section.section_data;
	return *this;
}

pe_section& pe_section::set_section_name(const std::string& section_name) {
	this->section_name = section_name;
	return *this;
}
pe_section& pe_section::set_virtual_size(DWORD virtual_size) {
	this->virtual_size = virtual_size;
	return *this;
}
pe_section& pe_section::set_virtual_address(DWORD virtual_address) {
	this->virtual_address = virtual_address;
	return *this;
}
pe_section& pe_section::set_size_of_raw_data(DWORD size_of_raw_data) {
	section_data.resize(size_of_raw_data);
	return *this;
}
pe_section& pe_section::set_pointer_to_raw(DWORD pointer_to_raw) {
	this->pointer_to_raw = pointer_to_raw;
	return *this;
}
pe_section& pe_section::set_characteristics(DWORD characteristics) {
	this->characteristics = characteristics;
	return *this;
}

pe_section& pe_section::set_readable(bool flag) {
	if (flag) {
		this->characteristics |= IMAGE_SCN_MEM_READ;
	}
	else {
		this->characteristics = ~((~this->characteristics) | IMAGE_SCN_MEM_READ);
	}
	return *this;
}
pe_section& pe_section::set_writeable(bool flag) {
	if (flag) {
		this->characteristics |= IMAGE_SCN_MEM_WRITE;
	}
	else {
		this->characteristics = ~((~this->characteristics) | IMAGE_SCN_MEM_WRITE);
	}
	return *this;
}
pe_section& pe_section::set_executable(bool flag) {
	if (flag) {
		this->characteristics |= IMAGE_SCN_MEM_EXECUTE;
	}
	else {
		this->characteristics = ~((~this->characteristics) | IMAGE_SCN_MEM_EXECUTE);
	}
	return *this;
}

void pe_section::add_data(void * data, unsigned int data_size) {
	section_data.resize(section_data.size() + data_size);
	memcpy(section_data.data() + section_data.size() - data_size, data, data_size);
}

std::string pe_section::get_section_name() const {
	return section_name;
}
DWORD pe_section::get_virtual_size() const {
	return virtual_size;
}
DWORD pe_section::get_virtual_address() const {
	return virtual_address;
}
DWORD pe_section::get_size_of_raw_data() const {
	return section_data.size();
}
DWORD pe_section::get_pointer_to_raw() const {
	return pointer_to_raw;
}
DWORD pe_section::get_characteristics() const {
	return characteristics;
}

bool pe_section::is_readable() const {
	return (characteristics&IMAGE_SCN_MEM_READ) != 0;
}
bool pe_section::is_writeable() const {
	return (characteristics&IMAGE_SCN_MEM_WRITE) != 0;
}
bool pe_section::is_executable() const {
	return (characteristics&IMAGE_SCN_MEM_EXECUTE) != 0;
}

std::vector<BYTE>& pe_section::get_section_data() {
	return section_data;
}

const std::vector<BYTE>& pe_section::get_section_data() const {
    return section_data;
}


pe_section_io::pe_section_io(pe_section & _section,
    section_io_mode mode,
    section_io_addressing_type type) :
    section(&_section), section_offset(0),
    last_code(section_io_success), mode(mode), addressing_type(type) {
}

pe_section_io::pe_section_io(const pe_section_io& io_section) {
    this->operator=(io_section);
}

pe_section_io::~pe_section_io() {

}

pe_section_io& pe_section_io::operator=(const pe_section_io& io_section) {
    this->section = io_section.section;
    this->section_offset = io_section.section_offset;
    this->last_code = io_section.last_code;
    this->mode = io_section.mode;
    this->addressing_type = io_section.addressing_type;

    return *this;
}

template <class T> section_io_code pe_section_io::operator >> (T& data) {

    return section_io_code::section_io_success;
}
template <class T> section_io_code pe_section_io::operator<<(T& data) {

    return section_io_code::section_io_success;
}

section_io_code pe_section_io::read(std::vector<BYTE>& buffer, unsigned int size, int offset) {

    return section_io_code::section_io_success;
}

section_io_code pe_section_io::write(std::vector<BYTE>& buffer, unsigned int size, int offset) {

    return section_io_code::section_io_success;
}

pe_section_io& pe_section_io::align_up(unsigned int factor, bool offset_to_end) {
    this->section->get_section_data().resize(ALIGN_UP(this->section->get_size_of_raw_data(), factor));
    if (offset_to_end) {
        this->section_offset = this->section->get_size_of_raw_data();
    }
    add_size(ALIGN_UP(this->section->get_size_of_raw_data(), factor));

    return *this;
}

pe_section_io& pe_section_io::add_size(unsigned int size, bool offset_to_end) {
    if (size) {
        this->section->get_section_data().resize(this->section->get_size_of_raw_data() + size);
    }
    if (offset_to_end) {
        this->section_offset = this->section->get_size_of_raw_data();
    }

    return *this;
}

pe_section_io& pe_section_io::set_mode(section_io_mode mode) {
    this->mode = mode;

    return *this;
}
pe_section_io& pe_section_io::set_addressing_type(section_io_addressing_type type) {
    this->addressing_type = type;

    return *this;
}
pe_section_io& pe_section_io::set_section_offset(unsigned int offset) {
    this->section_offset = offset;

    return *this;
}

section_io_mode pe_section_io::get_mode() const {
    return this->mode;
}
section_io_code pe_section_io::get_last_code() const {
    return this->last_code;
}
section_io_addressing_type pe_section_io::get_addressing_type() const {
    return this->addressing_type;
}
unsigned int pe_section_io::get_section_offset() const {
    return this->section_offset;
}
pe_section* pe_section_io::get_section() {
    return this->section;
}
