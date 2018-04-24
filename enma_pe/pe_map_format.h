#pragma once
enum e_map_segment_class {
    segment_class_unknown,
    segment_class_code,
    segment_class_data
};
enum e_map_data_class {
    data_class_unknown,
    data_class_function,
    data_class_data,
    data_class_bolrand_object_desc,//...
    data_class_bolrand_object_vtbl,//..
};
enum e_map_parseline_result {
    map_parseline_ok,
    map_parseline_fail
};
enum e_map_result {
    map_error_ok,
    map_error_openfile,
    map_error_no_math_map,
    map_error_readfile,
    map_error_parsefile,
};
enum e_map_parse_mod {
    map_parse_mod_none,
    map_parse_mod_segment,
    map_parse_mod_address,
};
enum e_map_compiller {
    map_compiller_unknown,
    map_compiller_bolrand,
    map_compiller_visualstudio,
    map_compiller_gcc,
};

struct map_segment {
    e_map_segment_class _class;
    size_t section_number;
    size_t offset;
    size_t length;
    std::string segment_name;
    map_segment::map_segment() {
        _class = segment_class_unknown;
        section_number = 0;
        offset = 0;
        length = 0;
    }
};

struct map_item {
    e_map_data_class _class;
    size_t section_number;
    size_t offset;
    size_t length;
    std::string item_name;
    void * custominfo;
    map_item::map_item() {
        _class = data_class_unknown;
        section_number = 0;
        offset = 0;
        length = 0;
        custominfo = 0;
    }
};

struct map_item_raw : map_item {
    std::vector<std::string> path;
};

struct map_dir {
    std::vector<map_dir> dirs;
    std::vector<map_item> code_items;
    std::vector<map_item> data_items;
    std::string dir_name;
    void * custominfo;
    map_dir::map_dir() {
        custominfo = 0;
    }
};

struct map_root {
    uint32_t timestamp;
    std::vector<map_dir>	 dirs;
};

typedef std::vector<std::vector<std::string>> raw_mapfile;

class map_parser {
    e_map_compiller compiller;
    uint32_t parse_context;
    std::vector<map_segment> segments;
	std::vector<map_item_raw> items_raw;


	void map_parser::parse_line_bolrand(uint32_t line_idx,std::vector<std::string> &line);
    void map_parser::parse_line_visualstudio(uint32_t line_idx, std::vector<std::string> &line);
    
    virtual void map_parser::parse_line(uint32_t line_idx, std::vector<std::string> &line);

    virtual bool map_parser::hexstring_to_value(std::string& hex_string, size_t& value);
    virtual bool map_parser::address_string_to_values(std::string& address_string, size_t& section_num, size_t& offset);

	std::vector<std::string> map_parser::get_line_items(std::string& line);
	void					 map_parser::get_raw_map_file(std::string& map_text, raw_mapfile &mapfile);
public:
	map_parser::map_parser(std::string& filepath, pe_image& image, map_root& map, e_map_result &result);
	map_parser::~map_parser();
};


void map_add_item(std::vector<std::string> &path, map_item& item, std::vector<map_dir>& map_dirs);
void map_finalize_items(pe_image& image, std::vector<map_segment>& segments, std::vector<map_item_raw>& items_raw, map_root& map);


#include "pe_map_bolrand.h"
#include "pe_map_msvc.h"
#include "pe_map_gcc.h"


e_map_result parse_map_file(
    std::string& filepath,map_root& map,
    pe_image* image = 0,
    e_map_compiller* compiller = 0);


