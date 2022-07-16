#include "stdafx.h"

using namespace enma;
using namespace pe_resource_internal;
using namespace hl;


pe_image_preview_icon::pe_image_preview_icon(pe_resource_directory& resources)
    : _resources(resources) {

    parse_resources();
}

bool pe_image_preview_icon::parse_resources() {

    auto resources_entry = hl::pe_image_resources_hl(_resources).get_directory(hl::kResourceIconGroup);

    if (!resources_entry || resources_entry->is_includes_data() ||
        resources_entry->get_resource_directory().get_entry_list().size() == 0) {

        return false;
    }

    pe_resource_directory_entry* group_lvl2 = 0;

    for (auto& entry : resources_entry->get_resource_directory().get_entry_list()) {

        if (!group_lvl2) {
            group_lvl2 = &entry;
        }
        else if (entry.get_id() < group_lvl2->get_id()) {
            group_lvl2 = &entry;
        }
    }

    if (!group_lvl2 || group_lvl2->is_includes_data() ||
        group_lvl2->get_resource_directory().get_entry_list().size() == 0) {

        return false;
    }

    for (auto& entry : group_lvl2->get_resource_directory().get_entry_list()) {
        _available_languages.push_back({ entry.get_id(), &entry });
    }
    
    return true;
}

bool pe_image_preview_icon::get_default_icon_group(pe_image_icon_group& icon, uint16_t& language_id) {

    for (auto& entry : _available_languages) {

        if (entry.first == 0x1000) {

            auto& data = entry.second->get_data_entry().get_data();
            language_id = entry.first;
            return icon.parse(data.data(), data.size());
        }
    }

    if (_available_languages.size()) {

        auto& data = _available_languages.front().second->get_data_entry().get_data();
        language_id = _available_languages.front().first;
        return icon.parse(data.data(), data.size());
    }

    return false;
}

bool pe_image_preview_icon::get_icon_group_by_language(uint16_t language_id, pe_image_icon_group& icon) {

    for (auto& entry : _available_languages) {

        if (entry.first == language_id) {

            auto& data = entry.second->get_data_entry().get_data();
            return icon.parse(data.data(), data.size());
        }
    }

    return false;
}