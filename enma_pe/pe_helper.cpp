#include "stdafx.h"
#include "pe_helper.h"


void demangle(std::string name, std::string& demangled_name) {
    demangled_name.clear();

    char *p = (char*)name.c_str();
    char pre_buffer[500];
    size_t ch_idx = 0;
    char last[2] = { 0 };
    while (ch_idx < name.length()) {
        size_t l = strcspn(p, " \n\t()\"\'");

        if (last) {
            demangled_name += last;
        }

        last[0] = p[l];
        p[l] = 0;

        ZeroMemory(pre_buffer, sizeof(pre_buffer));
        if (p[0] && !UnDecorateSymbolName(p, pre_buffer, sizeof(pre_buffer), UNDNAME_COMPLETE)) {
            demangled_name += p;
        }
        else {
            demangled_name += pre_buffer;
        }
        p += l + 1;
        ch_idx += l + 1;
    }

    if (last) { demangled_name += last; }
}


void map_add_item(std::vector<std::string> &path, map_item& item, std::vector<map_dir>& map_dirs) {

    for (size_t i = 0; i < map_dirs.size(); i++) {
        if (path.size()) {
            if (map_dirs[i].dir_name == path[0]) {
                if (path.size() > 1) {
                    path.erase(path.begin());
                    map_add_item(path, item, map_dirs[i].dirs);
                }
                else {
                    if (item._class == data_class_function) {
                        map_dirs[i].code_items.push_back(item);
                    }
                    else {
                        map_dirs[i].data_items.push_back(item);
                    }
                }
                return;
            }
        }
        else {
            if (map_dirs[i].dir_name == "\\_(global)_/") {
                if (item._class == data_class_function) {
                    map_dirs[i].code_items.push_back(item);
                }
                else {
                    map_dirs[i].data_items.push_back(item);
                }
                return;
            }
        }
    }

    map_dir dir;
    if (path.size()) {
        dir.dir_name = path[0];
    }
    else {
        dir.dir_name = "\\_(global)_/";
    }
    map_dirs.push_back(dir);

    if (path.size() > 1) {
        path.erase(path.begin());
        map_add_item(path, item, map_dirs[map_dirs.size() - 1].dirs);
    }
    else {
        if (item._class == data_class_function) {
            map_dirs[map_dirs.size() - 1].code_items.push_back(item);
        }
        else {
            map_dirs[map_dirs.size() - 1].data_items.push_back(item);
        }
    }
}

#define is_value_in_bound(st,len,val) (st <= val && val < st+len)

void map_finalize_items(pe_image& image, std::vector<map_segment>& segments, std::vector<map_item_raw>& items_raw, map_root& map) {

    std::vector<std::vector<map_item_raw>> items_by_section;

    items_by_section.resize((size_t)segments[segments.size() - 1].section_number);

    for (size_t i = 0; i < items_raw.size(); i++) { //push items by sections
        if (items_raw[i].section_number) {
            items_by_section[(size_t)items_raw[i].section_number - 1].push_back(items_raw[i]);
        }
    }

    for (size_t item_sec = 0; item_sec < items_by_section.size(); item_sec++) { //sort items by offset

        std::sort(items_by_section[item_sec].begin(), items_by_section[item_sec].end(), [](const map_item_raw& a, const map_item_raw& b) {
            return a.offset < b.offset;
        });

    }

    for (size_t item_sec = 0; item_sec < items_by_section.size(); item_sec++) {
        for (size_t item_idx = 0; item_idx < items_by_section[item_sec].size(); item_idx++) { //calc lens
            if (items_by_section[item_sec].size() >  item_idx + 1) {
                if (items_by_section[item_sec][item_idx].offset == items_by_section[item_sec][item_idx + 1].offset) {
                    items_by_section[item_sec].erase(items_by_section[item_sec].begin() + item_idx + 1);
                    item_idx--;
                    continue;
                }

                items_by_section[item_sec][item_idx].length = items_by_section[item_sec][item_idx + 1].offset - items_by_section[item_sec][item_idx].offset;

                for (size_t seg_idx = 0; seg_idx < segments.size(); seg_idx++) {
                    if (segments[seg_idx].section_number == items_by_section[item_sec][item_idx].section_number) {
                        if (is_value_in_bound(
                            segments[seg_idx].offset, segments[seg_idx].length, //if in bound of segment
                            items_by_section[item_sec][item_idx].offset)) {

                            if ((items_by_section[item_sec][item_idx].offset + items_by_section[item_sec][item_idx].length) > //if length climbs beyond border
                                (segments[seg_idx].offset + segments[seg_idx].length)) {
                                items_by_section[item_sec][item_idx].length =
                                    (segments[seg_idx].offset + segments[seg_idx].length) - items_by_section[item_sec][item_idx].offset;
                            }
                        }
                    }
                }
            }
            else {
                for (size_t seg_idx = 0; seg_idx < segments.size(); seg_idx++) {
                    if (segments[seg_idx].section_number == items_by_section[item_sec][item_idx].section_number) {
                        if (is_value_in_bound(
                            segments[seg_idx].offset, segments[seg_idx].length, //if in bound of segment
                            items_by_section[item_sec][item_idx].offset)) {

                            if (items_by_section[item_sec][item_idx]._class == data_class_unknown) {
                                if (segments[seg_idx]._class == segment_class_code) {
                                    items_by_section[item_sec][item_idx]._class = data_class_function;
                                }
                                else {
                                    items_by_section[item_sec][item_idx]._class = data_class_data;
                                }
                            }

                            items_by_section[item_sec][item_idx].length =
                                (segments[seg_idx].offset + segments[seg_idx].length) - items_by_section[item_sec][item_idx].offset;
                        }
                    }
                }
            }
        }
    }

    for (size_t item_sec = 0; item_sec < items_by_section.size(); item_sec++) {
        for (size_t item_idx = 0; item_idx < items_by_section[item_sec].size(); item_idx++) { //push to rootmap

            items_by_section[item_sec][item_idx].offset += //offset to rva
                image.get_section_by_idx((size_t)items_by_section[item_sec][item_idx].section_number - 1)->get_virtual_address();

            map_add_item(items_by_section[item_sec][item_idx].path, items_by_section[item_sec][item_idx], map.dirs);
        }
    }
}
