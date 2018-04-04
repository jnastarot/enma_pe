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
};

struct map_segment {
    e_map_segment_class _class;
    uint32_t section_number;
    uint32_t offset;
    uint32_t length;
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
    uint32_t section_number;
    uint32_t offset;
    uint32_t length;
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

void map_add_item(std::vector<std::string> &path, map_item& item, std::vector<map_dir>& map_dirs);
void map_finalize_items(pe_image& image, std::vector<map_segment>& segments, std::vector<map_item_raw>& items_raw, map_root& map);

void demangle(std::string name, std::string& demangled_name);


#include "pe_map_parser.h"
#include "pe_tds_parser.h"
#include "pe_string_finder.h"