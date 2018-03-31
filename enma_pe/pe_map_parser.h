#pragma once

class map_parser {
	e_map_compiller compiller;
	e_map_parse_mod parse_context;

	std::vector<map_segment> segments;
	std::vector<map_item_raw> items_raw;


	void map_parser::parse_line_bolrand(unsigned int line_idx,std::vector<std::string> &line);
	void map_parser::parse_line_visualstudio(unsigned int line_idx, std::vector<std::string> &line);

	bool map_parser::hexstring_to_value(std::string& hex_string, DWORD64& value);
	bool map_parser::address_string_to_values(std::string& address_string, DWORD64& section_num, DWORD64& offset);

	std::vector<std::string> map_parser::get_line_items(std::string& line);
	void					 map_parser::get_raw_map_file(std::string& map_text, raw_mapfile &mapfile);
public:
	map_parser::map_parser(std::string& filepath, pe_image& image, map_root& map, e_map_result &result);
	map_parser::~map_parser();
};