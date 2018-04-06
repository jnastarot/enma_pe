#include "stdafx.h"
#include "pe_resources.h"



resource_data_entry::resource_data_entry()
	:codepage(0)
{}


resource_data_entry::resource_data_entry(void * data, uint32_t data_size, uint32_t codepage)
	: codepage(codepage) {
	this->data.resize(data_size);
	memcpy(this->data.data(), data, data_size);
}
resource_data_entry::~resource_data_entry(){}


void resource_data_entry::set_codepage(uint32_t codepage) {
	this->codepage = codepage;
}

void resource_data_entry::set_data(void * data, uint32_t data_size) {
	this->data.resize(data_size);
	memcpy(this->data.data(), data, data_size);
}

uint32_t resource_data_entry::get_codepage() const{
	return this->codepage;
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

void resource_directory_entry::add_data_entry(resource_data_entry& entry) {
	release();
	_ptr.data = new resource_data_entry();
	_ptr.data->set_codepage(entry.get_codepage());
	_ptr.data->set_data(entry.get_data().data(), entry.get_data().size());
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
void resource_directory::add_resource_directory_entry(resource_directory_entry& entry) {
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
std::vector<resource_directory_entry>& resource_directory::get_entry_list() {
	return this->entries;
}



struct entry_sorter
{
public:
	bool operator()(resource_directory_entry& entry1, resource_directory_entry& entry2);
};
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
resource_directory::id_entry_finder::id_entry_finder(uint32_t id)
	:id_(id)
{}
bool resource_directory::id_entry_finder::operator()(resource_directory_entry& entry) {
	return !entry.is_named() && entry.get_id() == id_;
}
resource_directory::name_entry_finder::name_entry_finder(const std::wstring& name)
	:name_(name)
{}
bool resource_directory::name_entry_finder::operator()(resource_directory_entry& entry) {
	return entry.is_named() && entry.get_name() == name_;
}
resource_directory::entry_finder::entry_finder(const std::wstring& name)
	:name_(name), named_(true), id_(0)
{}
resource_directory::entry_finder::entry_finder(uint32_t id)
	: id_(id), named_(false), name_(L"")
{}
bool resource_directory::entry_finder::operator()(resource_directory_entry& entry) {
	if (named_) {
		return entry.is_named() && entry.get_name() == name_;
	}
	else {
		return !entry.is_named() && entry.get_id() == id_;
	}
}
bool resource_directory::entry_by_id(resource_directory_entry * &entry, uint32_t id) {
	auto& found_entry = std::find_if(entries.begin(), entries.end(), id_entry_finder(id));


	if (found_entry == entries.end()) {
		return false;
	}

	entry = (resource_directory_entry *)&(*found_entry);

	return true;
}
bool resource_directory::entry_by_name(resource_directory_entry * &entry, const std::wstring& name) {
	auto& found_entry = std::find_if(entries.begin(), entries.end(), name_entry_finder(name));


	if (found_entry == entries.end()) {
		return false;
	}

	entry = (resource_directory_entry *)&(*found_entry);


	return true;
}



void calculate_resource_data_space(resource_directory& root,//taken from pe bless
	uint32_t aligned_offset_from_section_start, uint32_t& needed_size_for_structures, uint32_t& needed_size_for_strings) {

	needed_size_for_structures += sizeof(image_resource_directory);

	for (size_t entry_idx = 0; entry_idx < root.get_entry_list().size(); entry_idx++) {

		needed_size_for_structures += sizeof(image_resource_directory_entry);

		if (root.get_entry_list()[entry_idx].is_named()) {
			needed_size_for_strings += ((root.get_entry_list()[entry_idx].get_name().length() + 1) * 2 + sizeof(uint16_t));
		}

		if (!root.get_entry_list()[entry_idx].is_includes_data()) {
			calculate_resource_data_space(root.get_entry_list()[entry_idx].get_resource_directory(), aligned_offset_from_section_start, needed_size_for_structures, needed_size_for_strings);
		}
	}
}

void calculate_resource_data_space(resource_directory& root, uint32_t needed_size_for_structures, //taken from pe bless
	uint32_t needed_size_for_strings, uint32_t& needed_size_for_data, uint32_t& current_data_pos) {

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

resource_directory process_resource_directory(const pe_image &image, //taken from pe bless
	uint32_t res_rva, uint32_t offset_to_directory, std::set<uint32_t>& processed) {
	resource_directory ret;

	if (!processed.insert(offset_to_directory).second) { return ret; }


    image_resource_directory directory;
	if (image.get_data_by_rva(res_rva + offset_to_directory, &directory, sizeof(directory))) {

		ret = resource_directory(directory);

		for (size_t i = 0; i != (directory.number_of_id_entries + directory.number_of_named_entries); i++) {
			//Read directory entries one by one
            image_resource_directory_entry dir_entry;
			if (image.get_data_by_rva(res_rva + sizeof(image_resource_directory) + (i * sizeof(image_resource_directory_entry)) + offset_to_directory, &dir_entry, sizeof(dir_entry))) {

				//Create directory entry structure
				resource_directory_entry entry;

				//If directory is named
				if (dir_entry.name_is_string) {
					//get directory name length
                    
                    image_resource_dir_string_u* directory_name = (image_resource_dir_string_u*)&image.get_section_by_rva(res_rva + dir_entry.name_offset)->get_section_data().data()[
						res_rva + dir_entry.name_offset -
							image.get_section_by_rva(res_rva + dir_entry.name_offset)->get_virtual_address()
					];

					std::wstring name;
					name.resize(directory_name->length);
					memcpy((void*)name.data(), directory_name->name_string, directory_name->length * sizeof(wchar_t));

					//Set entry UNICODE name
					entry.set_name(name);
				}
				else {
					entry.set_id(dir_entry.id);
				}

				//If directory entry has another resource directory
				if (dir_entry.data_is_directory) {
					entry.add_resource_directory(process_resource_directory(image, res_rva, dir_entry.offset_to_directory, processed));
				}
				else {
					//If directory entry has data
                    image_resource_data_entry data_entry;
					if (image.get_data_by_rva(res_rva + dir_entry.offset_to_data, &data_entry, sizeof(data_entry))) {

						//Add data entry to directory entry
						entry.add_data_entry(
							resource_data_entry(
								&image.get_section_by_rva(data_entry.offset_to_data)->get_section_data().data()[
									data_entry.offset_to_data -
										image.get_section_by_rva(data_entry.offset_to_data)->get_virtual_address()
								],
								data_entry.size,
										data_entry.code_page
										)
						);
					}
				}

				ret.add_resource_directory_entry(entry);
			}
		}
	}
	return ret;
}


void rebuild_resource_directory(pe_section& resource_section, resource_directory& root, //taken from pe bless
	uint32_t& current_structures_pos, uint32_t& current_data_pos, uint32_t& current_strings_pos, uint32_t offset_from_section_start){

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
	
	memcpy(&resource_section.get_section_data().data()[current_structures_pos], &dir, sizeof(dir));

	resource_section.add_data((uint8_t*)&dir, sizeof(dir));

	uint8_t* raw_data = resource_section.get_section_data().data();


	current_structures_pos += sizeof(dir);

	uint32_t this_current_structures_pos = current_structures_pos;

	current_structures_pos += sizeof(image_resource_directory_entry) * (dir.number_of_named_entries + dir.number_of_id_entries);

	for (auto& entry_item : root.get_entry_list()) {

        image_resource_directory_entry entry;

		if (entry_item.is_named()){
			entry.name = 0x80000000 | (current_strings_pos - offset_from_section_start);
			uint16_t unicode_length = (uint16_t)entry_item.get_name().length();
			memcpy(&raw_data[current_strings_pos], &unicode_length, sizeof(unicode_length));
			current_strings_pos += sizeof(unicode_length);

			memcpy(&raw_data[current_strings_pos], entry_item.get_name().c_str(), entry_item.get_name().length() * sizeof(wchar_t) + sizeof(wchar_t));

			current_strings_pos += uint32_t(entry_item.get_name().length() * sizeof(wchar_t) + sizeof(wchar_t));
		}
		else{
			entry.name = entry_item.get_id();
		}

		if (entry_item.is_includes_data()){
			current_data_pos = ALIGN_UP(current_data_pos, sizeof(uint32_t));
            image_resource_data_entry data_entry = { 0 };
			data_entry.code_page = entry_item.get_data_entry().get_codepage();
			data_entry.size = uint32_t(entry_item.get_data_entry().get_data().size());
			data_entry.offset_to_data = resource_section.get_virtual_address() + current_data_pos + sizeof(data_entry);

			entry.offset_to_data = current_data_pos - offset_from_section_start;

			memcpy(&raw_data[current_data_pos], &data_entry, sizeof(data_entry));
			current_data_pos += sizeof(data_entry);

			memcpy(&raw_data[current_data_pos], entry_item.get_data_entry().get_data().data(), data_entry.size);
			current_data_pos += data_entry.size;

			memcpy(&raw_data[this_current_structures_pos], &entry, sizeof(entry));
			this_current_structures_pos += sizeof(entry);
		}
		else{
			entry.offset_to_data = 0x80000000 | (current_structures_pos - offset_from_section_start);

			memcpy(&raw_data[this_current_structures_pos], &entry, sizeof(entry));
			this_current_structures_pos += sizeof(entry);

			rebuild_resource_directory(resource_section, entry_item.get_resource_directory(), current_structures_pos, current_data_pos, current_strings_pos, offset_from_section_start);
		}
	}
}

bool get_resources_table(const pe_image &image, resource_directory& resources) {
    resources.clear_resource_directory_entry_list();

	if (!image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_RESOURCE) || !image.get_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_RESOURCE)) {
		return false;
	}

	std::set<uint32_t> processed;

	resources = process_resource_directory(image, image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_RESOURCE), 0, processed);
	return false;
}
void build_resources_table(pe_image &image, pe_section& section, resource_directory& resources) {//taken from pe bless

	if (resources.get_entry_list().empty()) {
		image.set_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_RESOURCE, 0);
		image.set_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_RESOURCE, 0);
		return;
	}

	uint32_t aligned_offset_from_section_start = ALIGN_UP(section.get_size_of_raw_data(), sizeof(uint32_t));
	uint32_t needed_size_for_structures = aligned_offset_from_section_start - section.get_size_of_raw_data();
	uint32_t needed_size_for_strings = 0;
	uint32_t needed_size_for_data = 0;

	calculate_resource_data_space(resources, aligned_offset_from_section_start, needed_size_for_structures, needed_size_for_strings);

	{
		uint32_t current_data_pos = aligned_offset_from_section_start + needed_size_for_structures + needed_size_for_strings;
		calculate_resource_data_space(resources, needed_size_for_structures, needed_size_for_strings, needed_size_for_data, current_data_pos);
	}

	uint32_t needed_size = needed_size_for_structures + needed_size_for_strings + needed_size_for_data;

	if (section.get_size_of_raw_data() < needed_size + aligned_offset_from_section_start) {
		section.get_section_data().resize(section.get_size_of_raw_data() + needed_size + aligned_offset_from_section_start);
	}

	uint32_t current_structures_pos = aligned_offset_from_section_start;
	uint32_t current_strings_pos = current_structures_pos + needed_size_for_structures;
	uint32_t current_data_pos = current_strings_pos + needed_size_for_strings;
	rebuild_resource_directory(section, resources, current_structures_pos, current_data_pos, current_strings_pos, aligned_offset_from_section_start);


	image.set_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_RESOURCE, section.get_virtual_address() + aligned_offset_from_section_start);
	image.set_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_RESOURCE,	needed_size);
}

bool get_placement_resources_table(pe_image &image, std::vector<directory_placement>& placement) {

    uint32_t virtual_address = image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_BASERELOC);
    uint32_t virtual_size = image.get_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_BASERELOC);

	if (!virtual_address || !virtual_size) {
		return false;
	}

	pe_section * rsrc_section = image.get_section_by_rva(virtual_address);
	if (rsrc_section) {
        if (ALIGN_UP(rsrc_section->get_virtual_size(), image.get_section_align()) >= virtual_size) {

            placement.push_back({virtual_address ,virtual_size,dp_id_resources_desc });
            return true;
        }
	}

	return false;
}
 