#include "stdafx.h"


using namespace enma;
using namespace pe_resource_internal;
using namespace hl;


pe_image_preview_version_info::pe_image_preview_version_info(pe_resource_directory& resources)
    : _resources(resources) {

    parse_resources();
}

bool pe_image_preview_version_info::parse_resources() {

    auto resources_entry = hl::pe_image_resources_hl(_resources).get_directory(hl::kResourceversion, 1);

    if (!resources_entry || resources_entry->is_includes_data() ||
        resources_entry->get_resource_directory().get_entry_list().size() == 0) {

        return false;
    }

    for (auto& entry : resources_entry->get_resource_directory().get_entry_list()) {
        _available_languages.push_back({ entry.get_id(), &entry });
    }

    return true;
}

bool pe_image_preview_version_info::get_default_version(pe_image_version_info& ver, uint16_t& language_id) {

    for (auto& entry : _available_languages) {

        if (entry.first == 0x1000) {

            auto& data = entry.second->get_data_entry().get_data();
            language_id = entry.first;
            return ver.parse(data.data(), data.size());
        }
    }

    if (_available_languages.size()) {

        auto& data = _available_languages.front().second->get_data_entry().get_data();
        language_id = _available_languages.front().first;
        return ver.parse(data.data(), data.size());
    }

    return false;
}

bool pe_image_preview_version_info::get_version_by_language(uint16_t language_id, pe_image_version_info& ver) {

    for (auto& entry : _available_languages) {

        if (entry.first == language_id) {

            auto& data = entry.second->get_data_entry().get_data();
            return ver.parse(data.data(), data.size());
        }
    }

    return false;
}