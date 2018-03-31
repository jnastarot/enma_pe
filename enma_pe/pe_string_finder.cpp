#include "stdafx.h"
#include "pe_string_finder.h"


bool is_ascii(char symbol) {//eng
	return ((unsigned int)symbol >= 0x20 && (unsigned int)symbol <= 0x7E);
}
bool is_ascii_mod(char symbol) { //cyrillic
	return ((unsigned int)symbol >= 0x20 && (unsigned int)symbol <= 0x3F) || ((unsigned int)symbol >= 0xC0 && (unsigned int)symbol <= 0xFF);
}

#define MIN_STRING_SIZE 4
#define MAX_STRING_SIZE 1000

void get_strings_from_image(const pe_image& image, string_base_table& string_table, bool find_in_execute_sections) {
	pe_image tide_image = image;

	erase_export_table(tide_image);
	erase_import_table(tide_image);
	erase_resources_table(tide_image);
	erase_exceptions_table(tide_image);
	erase_relocation_table(tide_image);
	erase_debug_table(tide_image);
	erase_tls_table(tide_image);
	erase_load_config_table(tide_image);
	erase_delay_import_table(tide_image);	
    erase_bound_import_table(tide_image);

	string_table.ansi_base.clear();
	string_table.wide_base.clear();



	for (unsigned int section_idx = 0; section_idx < tide_image.get_sections_number(); section_idx++) {
	
		pe_section * p_section = tide_image.get_section_by_idx(section_idx);

		if (p_section && (!p_section->is_executable() || find_in_execute_sections)) {

			BYTE * section_raw = p_section->get_section_data().data();
			unsigned int raw_size = p_section->get_size_of_raw_data();

			unsigned int top_ansi = 0;
			unsigned int top_wide = 0;
			unsigned int top_mod_ansi = 0;
			unsigned int top_mod_wide = 0;

			for (unsigned int raw_idx = 0; raw_idx < raw_size;) {
				
				if (is_ascii(section_raw[raw_idx])) {
					if (top_wide <= raw_idx && raw_idx + 1 < raw_size && (section_raw[raw_idx + 1] == 0)) {

						unsigned int i = 0;
						for (i = 0; 
							(raw_idx + i) < raw_size &&
							i / 2 < MAX_STRING_SIZE && is_ascii(section_raw[(raw_idx + i)]) &&
							section_raw[(raw_idx + i) + 1] == 0 && i / 2 + 1 < MAX_STRING_SIZE;
							i += 2
							) {}
						
						if (i/2 >= MIN_STRING_SIZE && section_raw[(raw_idx + i)] == 0 && section_raw[(raw_idx + i) + 1] == 0) {
							w_string_base str;
							str.size = i;
							str.rva = p_section->get_virtual_address() + raw_idx;
							str.str.resize(i);
							memcpy((void*)str.str.data(), &section_raw[raw_idx], i);
							string_table.wide_base.push_back(str);
							raw_idx += i;
							continue;
						}
						else {
							top_wide = (raw_idx + i);
						}
					}
					else {
						if (top_ansi <= raw_idx) {
							unsigned int i = 0;
							for (i = 0;
								(raw_idx + i) < raw_size && i < MAX_STRING_SIZE && is_ascii(section_raw[(raw_idx + i)]);
								i++
								) {
							}

							if (i >= MIN_STRING_SIZE && section_raw[(raw_idx + i)] == 0) {
								a_string_base str;
								str.size = i;
								str.rva = p_section->get_virtual_address() + raw_idx;
								str.str.resize(i);
								memcpy((void*)str.str.data(),&section_raw[raw_idx],i);
								string_table.ansi_base.push_back(str);
								raw_idx += i;
								continue;
							}
							else {
								top_ansi = (raw_idx + i);
							}
						}
					}
				}
				if (is_ascii_mod(section_raw[raw_idx])) {
					if (top_mod_wide <= raw_idx && raw_idx + 1 < raw_size && (section_raw[raw_idx + 1] == 0)) {

						unsigned int i = 0;
						for (i = 0;
							(raw_idx + i) < raw_size &&
							i / 2 < MAX_STRING_SIZE && is_ascii_mod(section_raw[(raw_idx + i)]) &&
							section_raw[(raw_idx + i) + 1] == 0 && i / 2 + 1 < MAX_STRING_SIZE;
							i += 2
							) {
						}

						if (i / 2 >= MIN_STRING_SIZE && section_raw[(raw_idx + i)] == 0 && section_raw[(raw_idx + i) + 1] == 0) {
							w_string_base str;
							str.size = i;
							str.rva = p_section->get_virtual_address() + raw_idx;
							str.str.resize(i);
							memcpy((void*)str.str.data(), &section_raw[raw_idx], i);
							string_table.wide_base.push_back(str);
							raw_idx += i;
							continue;
						}
						else {
							top_mod_wide = (raw_idx + i);
						}
					}
					else {
						if (top_mod_ansi <= raw_idx) {
							unsigned int i = 0;
							for (i = 0;
								(raw_idx + i) < raw_size && i < MAX_STRING_SIZE && is_ascii_mod(section_raw[(raw_idx + i)]);
								i++
								) {
							}

							if (i >= MIN_STRING_SIZE && section_raw[(raw_idx + i)] == 0) {
								a_string_base str;
								str.size = i;
								str.rva = p_section->get_virtual_address() + raw_idx;
								str.str.resize(i);
								memcpy((void*)str.str.data(), &section_raw[raw_idx], i);
								string_table.ansi_base.push_back(str);
								raw_idx += i;
								continue;
							}
							else {
								top_mod_ansi = (raw_idx + i);
							}
						}
					}
				}

				raw_idx++;
			}
		}
	}


	for (unsigned int i = 0; i < string_table.ansi_base.size(); i++) {
		bool b_erase_string = true;
		char last_ch = 0;

		for (unsigned int ch_idx = 0; ch_idx < string_table.ansi_base[i].str.size(); ch_idx++) {
			if (!ch_idx) {last_ch = string_table.ansi_base[i].str[ch_idx];}
			else {
				if (last_ch != string_table.ansi_base[i].str[ch_idx]) {
					b_erase_string = false;
					break;
				}
			}
		}

		if (b_erase_string) {
			string_table.ansi_base.erase(string_table.ansi_base.begin() + i);
			i--;
		}
	}

	for (unsigned int i = 0; i < string_table.wide_base.size(); i++) {
		bool b_erase_string = true;
		wchar_t last_ch = 0;
		for (unsigned int ch_idx = 0; ch_idx < string_table.wide_base[i].str.size(); ch_idx++) {
			if (!ch_idx) { last_ch = string_table.wide_base[i].str[ch_idx]; }
			else {
				if (last_ch != string_table.wide_base[i].str[ch_idx]) {
					b_erase_string = false;
					break;
				}
			}
		}

		if (b_erase_string) {
			string_table.wide_base.erase(string_table.wide_base.begin() + i);
			i--;
		}
	}
}

