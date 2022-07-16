#include "stdafx.h"

using namespace enma;
using namespace pe_resource_internal;
using namespace hl;

pe_image_icon_group::pe_image_icon_group(const uint8_t* group_info, size_t group_info_size)
    : _valid_group(false)
    , _type(0) {

    parse(group_info, group_info_size);
}

bool pe_image_icon_group::parse(const uint8_t* group_info, size_t group_info_size) {
    
    _valid_group = false;
    _type = 0;
    _entries.clear();
    _group_data.clear();

    if (group_info_size < sizeof(group_icon_dir)) {
        return false;
    }

    group_icon_dir* icon_dir = (group_icon_dir*)group_info;

    _type = icon_dir->id_type;

    for (size_t idx = 0; idx < icon_dir->id_count; idx++) {

        if (group_info_size < (sizeof(group_icon_dir) + sizeof(group_icon_dir_entry) * idx)) {
            return false;
        }

        group_icon_dir_entry* entry = &icon_dir->id_entries[idx];

        _entries.push_back(*entry);
    }

    _valid_group = true;

    _group_data.insert(_group_data.begin(), group_info, group_info + group_info_size);

    return true;
}

std::vector<uint8_t> pe_image_icon_group::compile() {
    return std::vector<uint8_t>(); //not implemented
}

bool pe_image_icon_group::get_icon(pe_resource_directory& _resources, uint16_t used_language_id,
    std::vector<uint8_t>& icon_buf) {

    if (!_valid_group) {

        return false;
    }

    {
        size_t total_icon_size = sizeof(group_icon_dir) + sizeof(group_icon_entry) * _entries.size();

        for (auto& entry : _entries) {
            total_icon_size += entry.bytes_in_res;
        }

        icon_buf.resize(total_icon_size);
    }

    memcpy(&icon_buf[0], _group_data.data(), sizeof(group_icon_dir));

    for (size_t icon_offset = sizeof(group_icon_dir) + sizeof(group_icon_entry) * _entries.size(),
        header_offset = sizeof(group_icon_dir), 
        idx = 0;
        idx < _entries.size(); idx++, header_offset += sizeof(group_icon_entry)) {

        auto& dir_entry = _entries.begin();
        std::advance(dir_entry, idx);

        group_icon_entry icon_entry;
        icon_entry.width = dir_entry->width;
        icon_entry.height = dir_entry->height;
        icon_entry.color_count = dir_entry->color_count;
        icon_entry.reserved = dir_entry->reserved;
        icon_entry.planes = dir_entry->planes;
        icon_entry.bit_count = dir_entry->bit_count;
        icon_entry.bytes_in_res = dir_entry->bytes_in_res;
        icon_entry.offset = (uint32_t)icon_offset;

        memcpy(&icon_buf[header_offset], &icon_entry, sizeof(group_icon_entry));

        auto icon_resources_entry = 
            hl::pe_image_resources_hl(_resources).get_directory(hl::kResourceIcon, dir_entry->n_id, used_language_id);

        if (icon_resources_entry->is_includes_data()) {

            auto& data = icon_resources_entry->get_data_entry().get_data();

            if (data.size() != dir_entry->bytes_in_res) {

                return false;
            }

            memcpy(&icon_buf[icon_offset], data.data(), data.size());
            icon_offset += data.size();
        }
        else {
            return false;
        }
    }

    return true;
}