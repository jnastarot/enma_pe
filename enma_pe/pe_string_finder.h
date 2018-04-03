#pragma once

struct a_string_base {
    uint32_t rva;
    uint32_t size;
	std::string str;
};
struct w_string_base {
    uint32_t rva;
    uint32_t size;
	std::wstring str;
};


typedef std::vector<a_string_base> ansi_string_base_table;
typedef std::vector<w_string_base> wide_string_base_table;

struct string_base_table {
	ansi_string_base_table ansi_base;
	wide_string_base_table wide_base;
};

void get_strings_from_image(const pe_image& image, string_base_table& string_table,bool find_in_execute_sections = false);