#pragma once


class tds_parser {
    std::vector<BYTE>tds_file;
    const uint8_t * p_names_section;

    std::vector<map_segment> segments;
    std::vector<map_item_raw> items_raw;

    std::string tds_parser::get_name_by_id(int32_t name_id);

    void tds_parser::parse_symbols(const int8_t * start, const int8_t * end);
    void tds_parser::parse_align_symbols(const int8_t * start, const int8_t * end, int32_t moduleIndex);
    void tds_parser::parse_global_symbols(const int8_t * start);

    void tds_parser::parse_item_path(map_item_raw& item, std::string& name);
    e_map_result tds_parser::parse(pe_image& image, map_root& map);
public:
    tds_parser::tds_parser(std::string& filepath, pe_image& image, map_root& map, e_map_result &result);
    tds_parser::tds_parser(std::vector<BYTE>& tds_file, pe_image& image, map_root& map, e_map_result &result);
    tds_parser::~tds_parser();
};