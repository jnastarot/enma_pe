#include "stdafx.h"

using namespace enma;

pe_resource_data_entry::pe_resource_data_entry(const void * data, uint32_t data_size, uint32_t codepage)
    : _codepage(codepage) {
    
    _data.resize(data_size);
    memcpy(_data.data(), data, data_size);
}

pe_resource_directory_entry::pe_resource_directory_entry(const pe_resource_directory_entry& other)
    : _id(other._id)
    , _name(other._name)
    , _includes_data(other._includes_data)
    , _named(other._named){

    if (other._ptr._data) {
        if (other._includes_data) {
            _ptr._data = new pe_resource_data_entry(*other._ptr._data);
        }
        else {
            _ptr._dir = new pe_resource_directory(*other._ptr._dir);
        }
    }
}
pe_resource_directory_entry::~pe_resource_directory_entry() {
    release();
}

pe_resource_directory_entry& pe_resource_directory_entry::operator=(const pe_resource_directory_entry& other) {

    if (&other != this) {
        release();

        _id = other._id;
        _name = other._name;
        _includes_data = other._includes_data;
        _named = other._named;

        if (other._ptr._data) {
            if (other._includes_data) {
                _ptr._data = new pe_resource_data_entry(*other._ptr._data);
            }
            else {
                _ptr._dir = new pe_resource_directory(*other._ptr._dir);
            }
        }
    }

    return *this;
}

void pe_resource_directory_entry::release() {

    if (_ptr._data) {
        if (is_includes_data()) {
            delete _ptr._data;
        }
        else {
            delete _ptr._dir;
        }

        _ptr._data = 0;
    }
}

void pe_resource_directory_entry::add_data_entry(const pe_resource_data_entry& entry) {

    release();
    
    _ptr._data = new pe_resource_data_entry();
    _ptr._data->set_codepage(entry.get_codepage());
    _ptr._data->set_data(entry.get_data().data(), uint32_t(entry.get_data().size()));
    _includes_data = true;
}

void pe_resource_directory_entry::add_resource_directory(const pe_resource_directory& dir) {

    release();
    
    _ptr._dir = new pe_resource_directory(dir);
    _includes_data = false;
}

pe_resource_directory::~pe_resource_directory() {
    this->clear_resource_directory_entry_list();
}

void pe_resource_directory::add_resource_directory_entry(const pe_resource_directory_entry& entry) {
    
    _entries.push_back(entry);
    
    if (entry.is_named()) {
        ++_number_of_named_entries;
    }
    else {
        ++_number_of_id_entries;
    }
}
void pe_resource_directory::clear_resource_directory_entry_list() {
    _entries.clear();
    _number_of_named_entries = 0;
    _number_of_id_entries    = 0;
}

bool pe_resource_directory::entry_by_id(pe_resource_directory_entry * &entry, uint32_t id) {

    for (auto& entry_item : _entries) {

        if (!entry_item.is_named() && entry_item.get_id() == id) {
            entry = (pe_resource_directory_entry *)&(entry_item);

            return true;
        }
    }

    return false;
}

bool pe_resource_directory::entry_by_id(const pe_resource_directory_entry*& entry, uint32_t id) {

    return entry_by_id((pe_resource_directory_entry*&)entry, id);
}

bool pe_resource_directory::entry_by_name(pe_resource_directory_entry * &entry, const std::wstring& name) {

    for (auto& entry_item : _entries) {

        if (entry_item.is_named() && entry_item.get_name() == name) {
            entry = (pe_resource_directory_entry *)&(entry_item);

            return true;
        }
    }

    return false;
}

bool pe_resource_directory::entry_by_name(const pe_resource_directory_entry*& entry, const std::wstring& name) {

    return entry_by_name((pe_resource_directory_entry*&)entry, name);
}


static void calculate_resource_data_space(const pe_resource_directory& root,
    uint32_t aligned_offset_from_section_start, size_t& needed_size_for_structures, size_t& needed_size_for_strings) {

    needed_size_for_structures += sizeof(image_resource_directory);

    for (size_t entry_idx = 0; entry_idx < root.get_entry_list().size(); entry_idx++) {

        needed_size_for_structures += sizeof(image_resource_directory_entry);

        if (root.get_entry_list()[entry_idx].is_named()) {
            needed_size_for_strings += (root.get_entry_list()[entry_idx].get_name().length() + 1) * 2 + INT16_SIZE;
        }

        if (!root.get_entry_list()[entry_idx].is_includes_data()) {
            calculate_resource_data_space(root.get_entry_list()[entry_idx].get_resource_directory(), aligned_offset_from_section_start, needed_size_for_structures, needed_size_for_strings);
        }
    }
}

static void calculate_resource_data_space(const pe_resource_directory& root, size_t needed_size_for_structures,
    size_t needed_size_for_strings, size_t& needed_size_for_data, size_t& current_data_pos) {

    for (size_t entry_idx = 0; entry_idx < root.get_entry_list().size(); entry_idx++) {

        if (root.get_entry_list()[entry_idx].is_includes_data()) {

            uint32_t data_size = uint32_t(root.get_entry_list()[entry_idx].get_data_entry().get_data().size()
                + sizeof(image_resource_data_entry)
                + ALIGN_UP(current_data_pos, sizeof(uint32_t)) - current_data_pos
                );

            needed_size_for_data += data_size;
            current_data_pos += data_size;
        }
        else {

            calculate_resource_data_space(root.get_entry_list()[entry_idx].get_resource_directory(),
                needed_size_for_structures, needed_size_for_strings, needed_size_for_data, current_data_pos
            );
        }
    }
}

static pe_directory_code process_resource_directory(const pe_image &image,
    uint32_t res_rva, uint32_t offset_to_directory, std::set<uint32_t>& processed, pe_resource_directory& rsrc_table) {

    if (!processed.insert(offset_to_directory).second) { 

        return pe_directory_code::pe_directory_code_success; 
    }

    image_resource_directory directory;
    pe_image_io rsrc_io(image);

    if (rsrc_io.set_image_offset(res_rva + offset_to_directory).read(&directory, sizeof(directory)) == enma_io_success) {

        rsrc_table = pe_resource_directory(directory);
        rsrc_table.set_number_of_id_entries(0);
        rsrc_table.set_number_of_named_entries(0);

        for (size_t i = 0; i != ((size_t)directory.number_of_id_entries + directory.number_of_named_entries); i++) {

            image_resource_directory_entry dir_entry;

            if (rsrc_io.set_image_offset(
                uint32_t(res_rva + sizeof(image_resource_directory) + (i * sizeof(image_resource_directory_entry)) + offset_to_directory)
            ).read(&dir_entry, sizeof(image_resource_directory_entry)) == enma_io_success) {

                pe_resource_directory_entry entry;

                if (dir_entry.name_is_string) {

                    image_resource_dir_string_u directory_name;
                    if (rsrc_io.set_image_offset(res_rva + dir_entry.name_offset).read(
                        &directory_name, sizeof(image_resource_dir_string_u)) == enma_io_success) {

                        std::wstring name;
                        name.resize(directory_name.length);
                        
                        if (rsrc_io.set_image_offset(res_rva + dir_entry.name_offset + (uint32_t)offsetof(image_resource_dir_string_u, name_string)).read(
                            (void*)name.data(), (size_t)directory_name.length * WCHAR_SIZE) == enma_io_success) {

                            entry.set_name(name);
                        }
                        else {
                            return pe_directory_code::pe_directory_code_currupted;
                        }
                    }
                    else {
                        return pe_directory_code::pe_directory_code_currupted;
                    }
                }
                else {
                    entry.set_id(dir_entry.id);
                }

                if (dir_entry.data_is_directory) {
                    pe_resource_directory rsrc_entry;

                    pe_directory_code code = process_resource_directory(image, res_rva, dir_entry.offset_to_directory, processed, rsrc_entry);

                    if (code == pe_directory_code::pe_directory_code_success) {
                        entry.add_resource_directory(rsrc_entry);
                    }
                    else {
                        return code;
                    }
                }
                else {

                    image_resource_data_entry data_entry;
                    
                    if (rsrc_io.set_image_offset(res_rva + dir_entry.offset_to_data).read(&data_entry, sizeof(image_resource_data_entry)) == enma_io_success) {

                        std::vector<uint8_t> entry_data;

                        if (rsrc_io.set_image_offset(data_entry.offset_to_data).read(entry_data, data_entry.size) == enma_io_success) {

                            entry.add_data_entry(pe_resource_data_entry(entry_data.data(), uint32_t(entry_data.size()), data_entry.code_page));
                        }
                        else {
                            return pe_directory_code::pe_directory_code_currupted;
                        }                
                    }
                    else {
                        return pe_directory_code::pe_directory_code_currupted;
                    }
                }

                rsrc_table.add_resource_directory_entry(entry);
            }
            else {
                return pe_directory_code::pe_directory_code_currupted;
            }
        }
    }
    return pe_directory_code::pe_directory_code_success;
}

static bool rebuild_resource_directory(pe_image &image,pe_section& resource_section, pe_resource_directory& root,
    uint32_t& current_structures_pos, uint32_t& current_data_pos, uint32_t& current_strings_pos, uint32_t offset_from_section_start){

    pe_section_io section_io(resource_section, image,enma_io_mode_allow_expand);

    image_resource_directory dir = { 0 };
    dir.characteristics = root.get_characteristics();
    dir.major_version    = root.get_major_version();
    dir.minor_version    = root.get_minor_version();
    dir.time_date_stamp    = root.get_timestamp();

    std::sort(root.get_entry_list().begin(), root.get_entry_list().end(), 
        [](pe_resource_directory_entry& entry1, pe_resource_directory_entry& entry2) {
            if (entry1.is_named() && entry2.is_named()) {
                return entry1.get_name() < entry2.get_name();
            }
            else if (!entry1.is_named() && !entry2.is_named()) {
                return entry1.get_id() < entry2.get_id();
            }
            else {
                return entry1.is_named();
            }
        });

    for (size_t entry_idx = 0; entry_idx < root.get_entry_list().size(); entry_idx++) {
        if (root.get_entry_list()[entry_idx].is_named()) {
            ++dir.number_of_named_entries;
        }
        else {
            ++dir.number_of_id_entries;
        }
    }

    section_io.set_section_offset(resource_section.get_virtual_address() + current_structures_pos).write(&dir, sizeof(dir));

    current_structures_pos += (uint32_t)sizeof(dir);

    uint32_t this_current_structures_pos = current_structures_pos;

    current_structures_pos += (uint32_t)sizeof(image_resource_directory_entry) * (dir.number_of_named_entries + dir.number_of_id_entries);

    for (auto& entry_item : root.get_entry_list()) {

        image_resource_directory_entry entry;

        if (entry_item.is_named()){

            entry.name = 0x80000000 | (current_strings_pos - offset_from_section_start);
            uint16_t unicode_length = (uint16_t)entry_item.get_name().length();

            if(section_io.set_section_offset(resource_section.get_virtual_address() + current_strings_pos).write(
                &unicode_length, sizeof(unicode_length)
            ) != enma_io_success) {
                return false;
            }

            current_strings_pos += INT16_SIZE;

            if (section_io.set_section_offset(resource_section.get_virtual_address() + current_strings_pos).write(
                (void*)entry_item.get_name().c_str(), uint32_t(entry_item.get_name().length() * sizeof(wchar_t) + sizeof(wchar_t))
            ) != enma_io_success) {
                return false;
            }

            current_strings_pos += uint32_t(entry_item.get_name().length() * sizeof(wchar_t) + sizeof(wchar_t));
        }
        else{
            entry.name = entry_item.get_id();
        }

        if (entry_item.is_includes_data()){

            current_data_pos = ALIGN_UP(current_data_pos, INT32_SIZE);
            
            image_resource_data_entry data_entry = { 0 };

            data_entry.code_page = entry_item.get_data_entry().get_codepage();
            data_entry.size = uint32_t(entry_item.get_data_entry().get_data().size());
            data_entry.offset_to_data = resource_section.get_virtual_address() + current_data_pos + (uint32_t)sizeof(data_entry);

            entry.offset_to_data = current_data_pos - offset_from_section_start;

            if (section_io.set_section_offset(resource_section.get_virtual_address() + current_data_pos).write(
                &data_entry, sizeof(data_entry)
            ) != enma_io_success) {
                return false;
            }

            current_data_pos += (uint32_t)sizeof(data_entry);

            if (section_io.set_section_offset(resource_section.get_virtual_address() + current_data_pos).write(
                entry_item.get_data_entry().get_data().data(), data_entry.size
            ) != enma_io_success) {
                return false;
            }

            current_data_pos += data_entry.size;

            if (section_io.set_section_offset(resource_section.get_virtual_address() + this_current_structures_pos).write(
                &entry, sizeof(entry)
            ) != enma_io_success) {
                return false;
            }

            this_current_structures_pos += (uint32_t)sizeof(entry);
        }
        else{
            entry.offset_to_data = 0x80000000 | (current_structures_pos - offset_from_section_start);

            if (section_io.set_section_offset(resource_section.get_virtual_address() + this_current_structures_pos).write(
                &entry, sizeof(entry)
            ) != enma_io_success) {
                return false;
            }

            this_current_structures_pos += (uint32_t)sizeof(entry);

            if (!rebuild_resource_directory(image, resource_section, entry_item.get_resource_directory(), 
                current_structures_pos, current_data_pos, current_strings_pos, offset_from_section_start)) {

                return false;
            }
        }
    }

    return true;
}

pe_directory_code enma::get_resources_directory(const pe_image &image, pe_resource_directory& resources) {
    resources.clear_resource_directory_entry_list();

    if (!image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_RESOURCE) || 
        !image.get_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_RESOURCE)) {

        return pe_directory_code::pe_directory_code_not_present;
    }

    std::set<uint32_t> processed;

    return process_resource_directory(image, image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_RESOURCE), 0, processed, resources);
}

bool enma::build_resources_directory(pe_image &image, pe_section& section, pe_resource_directory& resources) {

    if (resources.get_entry_list().empty()) {

        image.set_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_RESOURCE, 0);
        image.set_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_RESOURCE, 0);
        
        return true;
    }

    uint32_t aligned_offset_from_section_start = ALIGN_UP(section.get_size_of_raw_data(), INT32_SIZE);
    size_t needed_size_for_structures = aligned_offset_from_section_start - section.get_size_of_raw_data();
    size_t needed_size_for_strings = 0;
    size_t needed_size_for_data = 0;

    calculate_resource_data_space(resources, aligned_offset_from_section_start, needed_size_for_structures, needed_size_for_strings);

    {
        size_t current_data_pos = aligned_offset_from_section_start + needed_size_for_structures + needed_size_for_strings;
        calculate_resource_data_space(resources, needed_size_for_structures, needed_size_for_strings, needed_size_for_data, current_data_pos);
    }

    size_t needed_size = needed_size_for_structures + needed_size_for_strings + needed_size_for_data;

    if (section.get_size_of_raw_data() < uint32_t(needed_size + aligned_offset_from_section_start) ) {
        pe_section_io(section, image).add_size(needed_size + aligned_offset_from_section_start);
    }

    uint32_t current_structures_pos = aligned_offset_from_section_start;
    uint32_t current_strings_pos = uint32_t( current_structures_pos + needed_size_for_structures );
    uint32_t current_data_pos = uint32_t( current_strings_pos + needed_size_for_strings );

    image.set_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_RESOURCE, section.get_virtual_address() + aligned_offset_from_section_start);
    image.set_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_RESOURCE, uint32_t( needed_size ));

    return rebuild_resource_directory(image, section, resources, current_structures_pos, current_data_pos, current_strings_pos, aligned_offset_from_section_start);
}

pe_directory_code enma::get_placement_resources_directory(const pe_image &image, pe_placement& placement) {

    uint32_t virtual_address = image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_RESOURCE);
    uint32_t virtual_size    = image.get_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_RESOURCE);

    if (virtual_address && virtual_size) {
        pe_image_io rsrc_io(image);

        size_t _offset_real = 0;
        size_t available_size = 0;
        size_t down_oversize = 0;
        size_t up_oversize = 0;

        rsrc_io.view_image(
            virtual_address, virtual_size,
            _offset_real,
            available_size, down_oversize, up_oversize
        );

        placement[virtual_address] = pe_placement_entry(available_size, id_pe_placement::id_pe_resources, "");

        if (!down_oversize && !up_oversize) {
            return pe_directory_code::pe_directory_code_success;
        }
        else {
            return pe_directory_code::pe_directory_code_currupted;
        }
    }
    else {

        return pe_directory_code::pe_directory_code_not_present;
    }
}
 