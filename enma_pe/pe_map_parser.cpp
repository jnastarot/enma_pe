#include "stdafx.h"
#include "pe_map_parser.h"


map_parser::map_parser(std::string& filepath, pe_image& image, map_root& map, e_map_result &result) {
	map.timestamp = 0;
	map.dirs.clear();

	segments.clear();
	items_raw.clear();


	compiller	  = map_compiller_unknown;
	parse_context = map_parse_mod_none;


	raw_mapfile mapfile;
	std::string content;

    FILE* hfile = fopen(filepath.c_str(), "rb");

	if (hfile != nullptr) {

        fseek(hfile, 0, SEEK_END);
        uint32_t file_size = ftell(hfile);
        fseek(hfile, 0, SEEK_SET);

		content.reserve(file_size);
		content.resize(file_size);

		if (fread((void*)content.data(), file_size, 1, hfile)) {

			get_raw_map_file(content, mapfile);

			uint32_t line_idx = 0;
			for (auto &line : mapfile) {

				if (!line_idx) {
					if (line.size() == 1) {
						compiller = map_compiller_visualstudio;
					}
					else {
						compiller = map_compiller_bolrand;
					}
				}

				if (compiller == map_compiller_visualstudio) {
					parse_line_visualstudio(line_idx, line);
				}
				else if (compiller == map_compiller_bolrand) {
					parse_line_bolrand(line_idx, line);
				}


				line_idx++;
			}

			fclose(hfile);

			map_finalize_items(image, segments, items_raw, map);
			result = e_map_result::map_error_ok;
			return;
		}
        fclose(hfile);
		result = e_map_result::map_error_readfile;
		return;
	}
	result = e_map_result::map_error_openfile;
	return;
}

map_parser::~map_parser() {

}


std::vector<std::string> map_parser::get_line_items(std::string& line) {
	std::vector<std::string> items;
	std::istringstream str_stream(line);

	for (std::string word; str_stream >> word;) {
		items.push_back(word);
	}

	return items;
}

void map_parser::get_raw_map_file(std::string& map_text, raw_mapfile &mapfile) {
	mapfile.clear();

	std::istringstream i(map_text);

	for (std::string line; !i.eof() && std::getline(i, line);) {
		if (line.length() == 0) { continue; }
		if (line.substr(0, 1).compare("\x20") == 0) { line = line.substr(1); }
		if (line.substr(line.length() - 1).compare("\r") == 0) { line.pop_back(); }

		std::vector<std::string> linedata = get_line_items(line);
		if (linedata.size() == 0) { continue; }

		mapfile.push_back(linedata);
	}

}

bool map_parser::hexstring_to_value(std::string& hex_string, uint64_t& value) {
	bool was_init_first = false;
	value = 0;
	for (size_t i = 0; i < hex_string.length(); i++) {
		if (hex_string[i] >= '0' && hex_string[i] <= '9') {
			value *= 0x10;
			value += (hex_string[i] - '0');
			was_init_first = true;
		}
		else if (hex_string[i] >= 'a' && hex_string[i] <= 'f') {
			value *= 0x10;
			value += (0xA + (hex_string[i] - 'a'));
			was_init_first = true;
		}
		else if (hex_string[i] >= 'A' && hex_string[i] <= 'F') {
			value *= 0x10;
			value += (0xA + (hex_string[i] - 'A'));
			was_init_first = true;
		}
		else {
			return was_init_first;
		}
	}

	return was_init_first;
}

bool map_parser::address_string_to_values(std::string& address_string, uint64_t& section_num, uint64_t& offset) {

	if (hexstring_to_value(address_string.substr(0, 4), section_num) && hexstring_to_value(address_string.substr(5), offset)) {
		return true;
	}

	return false;
}

void map_parser::parse_line_bolrand(uint32_t line_idx, std::vector<std::string> &line) {

	if (this->parse_context == map_parse_mod_none) {
		if (line.size() == 5 && line[0] == "Program" && line[1] == "entry" && line[2] == "point" && line[3] == "at") {

			return;
		}
		else if ((line.size() >= 4 && line[0] == "Address" && line[1] == "Publics" && line[2] == "by" && line[3] == "Value") ||
			(line.size() >= 2 && line[0] == "Static" && line[1] == "symbols")
			) {
			this->parse_context = map_parse_mod_address;
			return;

		}
		else if (line.size() == 4 && line[0] == "Detailed" && line[1] == "map" && line[2] == "of" && line[3] == "segments") {
			this->parse_context = map_parse_mod_segment;
			return;
		}
	}
	else if (this->parse_context == map_parse_mod_segment) {
		if (line.size() == 7) {
			map_segment seg;
			if (address_string_to_values(line[0], seg.section_number, seg.offset) &&
				hexstring_to_value(line[1], seg.length)) {
				seg._class = line[2] == "C=CODE" ? segment_class_code : segment_class_data;

				seg.segment_name = line[5];
				
				segments.push_back(seg);
				return;
			}
			else {
				this->parse_context = map_parse_mod_none;
			}
		}
		else {
			this->parse_context = map_parse_mod_none;
			parse_line_bolrand(line_idx, line);
			return;
		}
	}
	else if (this->parse_context == map_parse_mod_address) {
		if (line.size() == 2) {
			map_item_raw item;
			if (address_string_to_values(line[0], item.section_number, item.offset)) {
				if (!item.section_number)return;

				//
				item._class = data_class_unknown;

				std::string libdata = line[line.size() - 1];

				std::vector<std::string> path;

				std::string path_step;
				uint32_t dot_counter = 0;
				uint32_t dont_count_dots = 0;
				uint32_t dots_total = 0;

				bool was_last_dot = false;
				for (uint32_t i = 0; i < libdata.length(); i++) {

					if (libdata[i] == '{' || libdata[i] == '<') {
						dont_count_dots++;
					}else if (libdata[i] == '}' || libdata[i] == '>') {
						dont_count_dots--;
					}


					if (dont_count_dots) {
						if (was_last_dot) {
							was_last_dot = false;
							if (dots_total < dot_counter) {
								dots_total = dot_counter;
							}
							dot_counter = 0;
							path.push_back(path_step);
							path_step.clear();
						}

						path_step += libdata[i];
					}
					else {
						if (libdata[i] == '.') {
							dot_counter++;
							if (!was_last_dot) { 
								was_last_dot = true; 
							}
						}
						else {
							if (was_last_dot) {
								was_last_dot = false;
								if (dots_total < dot_counter) {
									dots_total = dot_counter;
								}
								dot_counter = 0;
								path.push_back(path_step);
								path_step.clear();
							}

							path_step += libdata[i];			
						}
					}
				}

				bool is_section_mark = false;
				for (size_t i = 0; i < path_step.length(); i++) {
					if (!is_section_mark) {
						if (path_step[i] == '$') {
							is_section_mark = true;
							path_step.erase(path_step.begin() + i);
						}
						else {
							break;
						}
					}
					else {
						if (path_step[i] == '$') {
							is_section_mark = false;
						}

						path_step.erase(path_step.begin() + i);
					}
				}


				switch (dots_total) {
					case 1: {
						item._class = data_class_unknown;//func or data choose be later by matching with segment
						break;
					}
					case 2: {
						item._class = data_class_bolrand_object_vtbl;
						break;
					}
					case 3: {
						item._class = data_class_bolrand_object_desc;
						break;
					}
					default: {

					}
				}

				demangle(path_step, item.item_name);
				item.path = path;

				items_raw.push_back(item);
				return;
			}
		}
		else {
			this->parse_context = map_parse_mod_none;
			parse_line_bolrand(line_idx, line);
			return;
		}
	}

	this->parse_context = map_parse_mod_none;
}


void map_parser::parse_line_visualstudio(uint32_t line_idx, std::vector<std::string> &line) {

	if (this->parse_context == map_parse_mod_none){
		if(line.size() == 4 && line[0] == "entry" && line[1] == "point" && line[2] == "is") {

			return;
		} else if ( (line.size() >= 4 && line[0] == "Address" && line[1] == "Publics" && line[2] == "by" && line[3] == "Value") ||
			(line.size() >= 2 && line[0] == "Static" && line[1] == "symbols")
			) {
			this->parse_context = map_parse_mod_address;
			return;

		}else if (line.size() == 4 && line[0] == "Start" && line[1] == "Length" && line[2] == "Name" && line[3] == "Class") {
			this->parse_context = map_parse_mod_segment;
			return;
		}
	}
	else if(this->parse_context == map_parse_mod_segment){
		if (line.size() == 4) {
			map_segment seg;
			if (address_string_to_values(line[0],seg.section_number, seg.offset) &&
				hexstring_to_value(line[1], seg.length)) {
				seg.segment_name = line[2];
				seg._class = line[3] == "CODE" ? segment_class_code : segment_class_data;
				segments.push_back(seg);
				return;
			}
			else {
				this->parse_context = map_parse_mod_none;
			}
		}
		else {
			this->parse_context = map_parse_mod_none;
			parse_line_visualstudio(line_idx, line);
			return;
		}
	}
	else if (this->parse_context == map_parse_mod_address) {
		if (line.size() >= 4) {
			map_item_raw item;
			if (address_string_to_values(line[0], item.section_number, item.offset)) {
				if (!item.section_number)return;

				demangle(line[1], item.item_name);
				item._class = (line[3].compare("f") == 0) ? data_class_function : data_class_data;

				std::string libdata = line[line.size() - 1];
				uint32_t libsplit = libdata.find_first_of(":");

				std::vector<std::string> path;

				if (libsplit != -1) {
					path.push_back(libdata.substr(0, libsplit));
					path.push_back(libdata.substr(libsplit + 1));
				}
				else {
					path.push_back(libdata);
				}

				item.path = path;

				items_raw.push_back(item);
				return;
			}
		}
		else {
			this->parse_context = map_parse_mod_none;
			parse_line_visualstudio(line_idx, line);
			return;
		}
	}
	
	this->parse_context = map_parse_mod_none;
}





