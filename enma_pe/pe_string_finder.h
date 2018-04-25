#pragma once

#define MIN_STRING_SIZE 4
#define MAX_STRING_SIZE 1000

struct a_string_base {
    uint32_t rva;
	std::string str;
};
struct w_string_base {
    uint32_t rva;
	std::wstring str;
};

typedef std::vector<a_string_base> ansi_string_base_table;
typedef std::vector<w_string_base> wide_string_base_table;

struct string_base_table {
    std::string locale_name;
	ansi_string_base_table ansi_base;
	wide_string_base_table wide_base;
};

void get_strings_from_image_by_locale(_In_ const pe_image& image,_Out_ string_base_table& string_table,
    _In_ char * locale = 0,//by default is en-EN
    _In_ bool find_in_clean_image = true,//search without directories data
    _In_ bool find_in_execute_sections = false,//search without sections with executable flag
    _In_ uint32_t min_string_size = MIN_STRING_SIZE,
    _In_ uint32_t max_string_size = MAX_STRING_SIZE
    );
