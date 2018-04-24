#include "stdafx.h"
#include "pe_map_msvc.h"


pe_map_msvc::pe_map_msvc()
{
}


pe_map_msvc::~pe_map_msvc()
{
}


void map_parser::parse_line_visualstudio(uint32_t line_idx, std::vector<std::string> &line) {

    if (this->parse_context == map_parse_mod_none) {
        if (line.size() == 4 && line[0] == "entry" && line[1] == "point" && line[2] == "is") {

            return;
        }
        else if ((line.size() >= 4 && line[0] == "Address" && line[1] == "Publics" && line[2] == "by" && line[3] == "Value") ||
            (line.size() >= 2 && line[0] == "Static" && line[1] == "symbols")
            ) {
            this->parse_context = map_parse_mod_address;
            return;

        }
        else if (line.size() == 4 && line[0] == "Start" && line[1] == "Length" && line[2] == "Name" && line[3] == "Class") {
            this->parse_context = map_parse_mod_segment;
            return;
        }
    }
    else if (this->parse_context == map_parse_mod_segment) {
        if (line.size() == 4) {
            map_segment seg;
            if (address_string_to_values(line[0], seg.section_number, seg.offset) &&
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