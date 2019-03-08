#include "stdafx.h"
#include "pe_resources.h"



resource_data_entry::resource_data_entry()
	:codepage(0)
{}


resource_data_entry::resource_data_entry(const void * data, uint32_t data_size, uint32_t codepage)
	: codepage(codepage) {
	this->data.resize(data_size);
	memcpy(this->data.data(), data, data_size);
}
resource_data_entry::~resource_data_entry(){}


void resource_data_entry::set_codepage(uint32_t codepage) {
	this->codepage = codepage;
}

void resource_data_entry::set_data(const void * data, uint32_t data_size) {
	this->data.resize(data_size);
	memcpy(this->data.data(), data, data_size);
}

uint32_t resource_data_entry::get_codepage() const{
	return this->codepage;
}
const std::vector<uint8_t>&	resource_data_entry::get_data() const {
    return data;
}
std::vector<uint8_t>&	resource_data_entry::get_data() {
	return data;
}





resource_directory_entry::resource_directory_entry()
	: id(0), includes_data(false), named(false) {}


resource_directory_entry::resource_directory_entry(const resource_directory_entry& other)
	: id(other.id), name(other.name), includes_data(other.includes_data), named(other.named){

	if (other._ptr.data) {
		if (other.includes_data) {
			_ptr.data = new resource_data_entry(*other._ptr.data);
		}
		else {
			_ptr.dir = new resource_directory(*other._ptr.dir);
		}
	}
}
resource_directory_entry::~resource_directory_entry() {
	release();
}

resource_directory_entry::includes::includes()
	:data(0) {}


resource_directory_entry& resource_directory_entry::operator=(const resource_directory_entry& other) {

    if (&other != this) {
        release();

        id = other.id;
        name = other.name;
        includes_data = other.includes_data;
        named = other.named;

        if (other._ptr.data) {
            if (other.includes_data) {
                _ptr.data = new resource_data_entry(*other._ptr.data);
            }
            else {
                _ptr.dir = new resource_directory(*other._ptr.dir);
            }
        }
    }

	return *this;
}

void resource_directory_entry::release() {

	if (_ptr.data) {
		if (is_includes_data()) {
			delete _ptr.data;
		}
		else {
			delete _ptr.dir;
		}

		_ptr.data = 0;
	}
}


void resource_directory_entry::set_name(const std::wstring& name) {
	this->name = name;
	this->named = true;
	this->id = 0;
}

void resource_directory_entry::set_id(uint32_t id) {
	this->id = id;
	this->named = false;
	this->name.clear();
}

void resource_directory_entry::add_data_entry(const resource_data_entry& entry) {
	release();
	_ptr.data = new resource_data_entry();
	_ptr.data->set_codepage(entry.get_codepage());
	_ptr.data->set_data(entry.get_data().data(), uint32_t(entry.get_data().size()));
	includes_data = true;
}

void resource_directory_entry::add_resource_directory(const resource_directory& dir) {
	release();
	_ptr.dir = new resource_directory(dir);
	includes_data = false;
}

uint32_t resource_directory_entry::get_id() const{
	return this->id;
}

const std::wstring& resource_directory_entry::get_name() const {
	return this->name;
}

bool resource_directory_entry::is_named() const {
	return this->named;
}

bool resource_directory_entry::is_includes_data() const {
	return this->includes_data;
}


const resource_directory&     resource_directory_entry::get_resource_directory() const {
    return (const resource_directory&)(*this->_ptr.data);
}

const resource_data_entry&    resource_directory_entry::get_data_entry() const {
    return (const resource_data_entry&)(*this->_ptr.dir);
}

resource_data_entry& resource_directory_entry::get_data_entry() {
	return *this->_ptr.data;
}

resource_directory& resource_directory_entry::get_resource_directory() {
	return *this->_ptr.dir;
}

resource_directory::resource_directory()
	:characteristics(0),
	timestamp(0),
	major_version(0), minor_version(0),
	number_of_named_entries(0), number_of_id_entries(0)
{}

resource_directory::resource_directory(const resource_directory& resource_dir) {
    this->operator=(resource_dir);
}

resource_directory::resource_directory(const image_resource_directory& res_dir)
	: characteristics(res_dir.characteristics),
	timestamp(res_dir.time_date_stamp),
	major_version(res_dir.major_version), minor_version(res_dir.minor_version),
	number_of_named_entries(res_dir.number_of_named_entries), number_of_id_entries(res_dir.number_of_id_entries) {}


resource_directory::~resource_directory() {
	this->clear_resource_directory_entry_list();
}


resource_directory& resource_directory::operator=(const resource_directory& resource_dir) {

	characteristics			= resource_dir.characteristics;
	timestamp				= resource_dir.timestamp;
	major_version			= resource_dir.major_version;
	minor_version			= resource_dir.minor_version;
	number_of_named_entries = resource_dir.number_of_named_entries;
	number_of_id_entries	= resource_dir.number_of_id_entries;
	entries					= resource_dir.entries;

	return *this;
}


void resource_directory::set_characteristics(uint32_t characteristics) {
	this->characteristics = characteristics;
}
void resource_directory::set_timestamp(uint32_t timestamp) {
	this->timestamp = timestamp;
}
void resource_directory::set_number_of_named_entries(uint32_t number) {
	this->number_of_named_entries = number;
}
void resource_directory::set_number_of_id_entries(uint32_t number) {
	this->number_of_id_entries = number;
}
void resource_directory::set_major_version(uint16_t major_version) {
	this->major_version = major_version;
}
void resource_directory::set_minor_version(uint16_t minor_version) {
	this->minor_version = minor_version;
}
void resource_directory::add_resource_directory_entry(const resource_directory_entry& entry) {
	entries.push_back(entry);
	if (entry.is_named()) {
		++number_of_named_entries;
	}
	else {
		++number_of_id_entries;
	}
}
void resource_directory::clear_resource_directory_entry_list() {
	entries.clear();
	number_of_named_entries = 0;
	number_of_id_entries	= 0;
}

uint32_t resource_directory::get_characteristics() const {
	return this->characteristics;
}
uint32_t resource_directory::get_timestamp() const {
	return this->timestamp;
}
uint16_t resource_directory::get_major_version() const {
	return this->major_version;
}
uint16_t resource_directory::get_minor_version() const {
	return this->minor_version;
}
uint32_t resource_directory::get_number_of_named_entries() const {
	return this->number_of_named_entries;
}
uint32_t resource_directory::get_number_of_id_entries() const {
	return this->number_of_id_entries;
}
const std::vector<resource_directory_entry>& resource_directory::get_entry_list() const {
    return this->entries;
}
std::vector<resource_directory_entry>& resource_directory::get_entry_list() {
	return this->entries;
}

bool entry_sorter::operator()(resource_directory_entry& entry1, resource_directory_entry& entry2) {
	if (entry1.is_named() && entry2.is_named()) {
		return entry1.get_name() < entry2.get_name();
	}
	else if (!entry1.is_named() && !entry2.is_named()) {
		return entry1.get_id() < entry2.get_id();
	}
	else {
		return entry1.is_named();
	}
}

bool resource_directory::entry_by_id(const resource_directory_entry * &entry, uint32_t id) {

    for (auto& entry_item : entries) {

        if (!entry_item.is_named() && entry_item.get_id() == id) {
            entry = (resource_directory_entry *)&(entry_item);

            return true;
        }
    }

    return false;
}
bool resource_directory::entry_by_name(const resource_directory_entry * &entry, const std::wstring& name) {

    for (auto& entry_item : entries) {

        if (entry_item.is_named() && entry_item.get_name() == name) {
            entry = (resource_directory_entry *)&(entry_item);

            return true;
        }
    }

    return false;
}



void calculate_resource_data_space(const resource_directory& root,//taken from pe bless
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

void calculate_resource_data_space(const resource_directory& root, size_t needed_size_for_structures, //taken from pe bless
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

directory_code process_resource_directory(const pe_image &image, //taken from pe bless
	uint32_t res_rva, uint32_t offset_to_directory, std::set<uint32_t>& processed, resource_directory& rsrc_table) {

	if (!processed.insert(offset_to_directory).second) { return directory_code::directory_code_success; }


    image_resource_directory directory;
    pe_image_io rsrc_io(image);
    

	if (rsrc_io.set_image_offset(res_rva + offset_to_directory).read(&directory, sizeof(directory)) == enma_io_success) {

        rsrc_table = resource_directory(directory);
        rsrc_table.set_number_of_id_entries(0);  //because on add_resource_directory_entry count the number
        rsrc_table.set_number_of_named_entries(0);

		for (size_t i = 0; i != (directory.number_of_id_entries + directory.number_of_named_entries); i++) {
			//Read directory entries one by one
            image_resource_directory_entry dir_entry;
            

			if (rsrc_io.set_image_offset(
                uint32_t(res_rva + sizeof(image_resource_directory) + (i * sizeof(image_resource_directory_entry)) + offset_to_directory)
            ).read(&dir_entry, sizeof(image_resource_directory_entry)) == enma_io_success) {

				//Create directory entry structure
				resource_directory_entry entry;

				//If directory is named
				if (dir_entry.name_is_string) {
					//get directory name length
                    
                    image_resource_dir_string_u directory_name;
                    if (rsrc_io.set_image_offset(res_rva + dir_entry.name_offset).read(
                        &directory_name, sizeof(image_resource_dir_string_u)) == enma_io_success) {

                        std::wstring name;
                        name.resize(directory_name.length);
                        
                        if (rsrc_io.set_image_offset(res_rva + dir_entry.name_offset + (uint32_t)offsetof(image_resource_dir_string_u, name_string)).read(
                            (void*)name.data(), directory_name.length * WCHAR_SIZE) == enma_io_success) {

                            //Set entry UNICODE name
                            entry.set_name(name);
                        }
                        else {
                            return directory_code::directory_code_currupted;
                        }
                    }
                    else {
                        return directory_code::directory_code_currupted;
                    }
				}
				else {
					entry.set_id(dir_entry.id);
				}

				//If directory entry has another resource directory
				if (dir_entry.data_is_directory) {
                    resource_directory rsrc_entry;

                    directory_code code = process_resource_directory(image, res_rva, dir_entry.offset_to_directory, processed, rsrc_entry);

                    if (code == directory_code::directory_code_success) {
                        entry.add_resource_directory(rsrc_entry);
                    }
                    else {
                        return code;
                    }
				}
				else {
					//If directory entry has data
                    image_resource_data_entry data_entry;
                    
					if (rsrc_io.set_image_offset(res_rva + dir_entry.offset_to_data).read(&data_entry, sizeof(image_resource_data_entry)) == enma_io_success) {

                        std::vector<uint8_t> entry_data;

                        if (rsrc_io.set_image_offset(data_entry.offset_to_data).read(entry_data, data_entry.size) == enma_io_success) {

                            entry.add_data_entry(resource_data_entry(entry_data.data(), uint32_t(entry_data.size()), data_entry.code_page));
                        }
                        else {
                            return directory_code::directory_code_currupted;
                        }				
                    }
                    else {
                        return directory_code::directory_code_currupted;
                    }
				}

                rsrc_table.add_resource_directory_entry(entry);
            }
            else {
                return directory_code::directory_code_currupted;
            }
		}
	}
	return directory_code::directory_code_success;
}

bool rebuild_resource_directory(pe_image &image,pe_section& resource_section, resource_directory& root, //taken from pe bless
	uint32_t& current_structures_pos, uint32_t& current_data_pos, uint32_t& current_strings_pos, uint32_t offset_from_section_start){

    pe_section_io section_io(resource_section, image,enma_io_mode_allow_expand);

    image_resource_directory dir = { 0 };
	dir.characteristics = root.get_characteristics();
	dir.major_version	= root.get_major_version();
	dir.minor_version	= root.get_minor_version();
	dir.time_date_stamp	= root.get_timestamp();

	std::sort(root.get_entry_list().begin(), root.get_entry_list().end(), entry_sorter());


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

directory_code get_resources_table(const pe_image &image, resource_directory& resources) {
    resources.clear_resource_directory_entry_list();

	if (!image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_RESOURCE) || 
        !image.get_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_RESOURCE)) {

		return directory_code::directory_code_not_present;
	}

	std::set<uint32_t> processed;

	return process_resource_directory(image, image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_RESOURCE), 0, processed, resources);
}

bool build_resources_table(pe_image &image, pe_section& section, resource_directory& resources) {//taken from pe bless

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

directory_code get_placement_resources_table(const pe_image &image, pe_directory_placement& placement) {

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


        placement[virtual_address] = directory_placement(available_size, id_pe_resources, "");

        if (!down_oversize && !up_oversize) {
            return directory_code::directory_code_success;
        }

        return directory_code::directory_code_currupted;
    }
    else {

        return directory_code::directory_code_not_present;
    }
}
 