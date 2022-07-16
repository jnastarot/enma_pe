#include "stdafx.h"

using namespace enma;
using namespace hl;

static pe_resource_directory_entry* find_resource_by_id(pe_resource_directory& resource_dir, uint16_t id) {

    pe_resource_directory_entry* found_entry = 0;

    if (resource_dir.entry_by_id(found_entry, id)) {
        return found_entry;
    }

    return 0;
}

static pe_resource_directory_entry* find_resource_by_id(pe_resource_directory_entry* resource_entry, uint16_t id) {

    if (!resource_entry || resource_entry->is_includes_data()) {
        return 0;
    }

    pe_resource_directory_entry* found_entry = 0;

    if (resource_entry->get_resource_directory().entry_by_id(found_entry, id)) {
        return found_entry;
    }

    return 0;
}

static pe_resource_directory_entry* find_resource_by_name(pe_resource_directory& resource_dir, const std::wstring& name) {

    pe_resource_directory_entry* found_entry = 0;

    if (resource_dir.entry_by_name(found_entry, name)) {
        return found_entry;
    }

    return 0;
}

static pe_resource_directory_entry* find_resource_by_name(pe_resource_directory_entry* resource_entry, const std::wstring& name) {

    if (!resource_entry || resource_entry->is_includes_data()) {
        return 0;
    }

    pe_resource_directory_entry* found_entry = 0;

    if (resource_entry->get_resource_directory().entry_by_name(found_entry, name)) {
        return found_entry;
    }

    return 0;
}

pe_resource_directory_entry* pe_image_resources_hl::get_directory(uint16_t type_id) {

    return find_resource_by_id(_resources, type_id);
}

pe_resource_directory_entry* pe_image_resources_hl::get_directory(const std::wstring& type_name) {

    return find_resource_by_name(_resources, type_name);
}

pe_resource_directory_entry* pe_image_resources_hl::get_directory(uint16_t type_id, uint16_t resource_id) {

    pe_resource_directory_entry* found_entry = get_directory(type_id);

    if (found_entry) {
        return find_resource_by_id(found_entry, resource_id);
    }

    return found_entry;
}

pe_resource_directory_entry* pe_image_resources_hl::get_directory(const std::wstring& type_name, uint16_t resource_id) {

    pe_resource_directory_entry* found_entry = get_directory(type_name);

    if (found_entry) {
        return find_resource_by_id(found_entry, resource_id);
    }

    return found_entry;
}

pe_resource_directory_entry* pe_image_resources_hl::get_directory(uint16_t type_id, const std::wstring& resource_name) {

    pe_resource_directory_entry* found_entry = get_directory(type_id);

    if (found_entry) {
        return find_resource_by_name(found_entry, resource_name);
    }

    return found_entry;
}

pe_resource_directory_entry* pe_image_resources_hl::get_directory(const std::wstring& type_name, const std::wstring& resource_name) {

    pe_resource_directory_entry* found_entry = get_directory(type_name);

    if (found_entry) {
        return find_resource_by_name(found_entry, resource_name);
    }

    return found_entry;
}

pe_resource_directory_entry* pe_image_resources_hl::get_directory(const std::wstring& type_name, uint16_t resource_id, uint16_t language_id) {

    pe_resource_directory_entry* found_entry = get_directory(type_name, resource_id);

    if (found_entry) {
        return find_resource_by_id(found_entry, language_id);
    }

    return found_entry;
}

pe_resource_directory_entry* pe_image_resources_hl::get_directory(uint16_t type_id, const std::wstring& resource_name, uint16_t language_id) {

    pe_resource_directory_entry* found_entry = get_directory(type_id, resource_name);

    if (found_entry) {
        return find_resource_by_id(found_entry, language_id);
    }

    return found_entry;
}

pe_resource_directory_entry* pe_image_resources_hl::get_directory(uint16_t type_id, uint16_t resource_id, uint16_t language_id) {

    pe_resource_directory_entry* found_entry = get_directory(type_id, resource_id);

    if (found_entry) {
        return find_resource_by_id(found_entry, language_id);
    }

    return found_entry;
}

pe_resource_directory_entry* pe_image_resources_hl::get_directory(const std::wstring& type_name, const std::wstring& resource_name, uint16_t language_id) {

    pe_resource_directory_entry* found_entry = get_directory(type_name, resource_name);

    if (found_entry) {
        return find_resource_by_id(found_entry, language_id);
    }

    return found_entry;
}
