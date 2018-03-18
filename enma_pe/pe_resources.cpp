#include "stdafx.h"
#include "pe_resources.h"



resource_data_entry::resource_data_entry()
	:codepage(0)
{}


resource_data_entry::resource_data_entry(void * data, unsigned int data_size, unsigned int codepage)
	: codepage(codepage) {
	this->data.resize(data_size);
	memcpy(this->data.data(), data, data_size);
}
resource_data_entry::~resource_data_entry(){}


void resource_data_entry::set_codepage(unsigned int codepage) {
	this->codepage = codepage;
}

void resource_data_entry::set_data(void * data, unsigned int data_size) {
	this->data.resize(data_size);
	memcpy(this->data.data(), data, data_size);
}

unsigned int resource_data_entry::get_codepage() const{
	return this->codepage;
}

std::vector<BYTE>&	resource_data_entry::get_data() {
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

void resource_directory_entry::set_id(unsigned int id) {
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

unsigned int resource_directory_entry::get_id() const{
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
resource_directory::resource_directory(IMAGE_RESOURCE_DIRECTORY * res_dir)
	: characteristics(res_dir->Characteristics),
	timestamp(res_dir->TimeDateStamp),
	major_version(res_dir->MajorVersion), minor_version(res_dir->MinorVersion),
	number_of_named_entries(res_dir->NumberOfNamedEntries), number_of_id_entries(res_dir->NumberOfIdEntries) {}


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


void resource_directory::set_characteristics(unsigned int characteristics) {
	this->characteristics = characteristics;
}
void resource_directory::set_timestamp(unsigned int timestamp) {
	this->timestamp = timestamp;
}
void resource_directory::set_number_of_named_entries(unsigned int number) {
	this->number_of_named_entries = number;
}
void resource_directory::set_number_of_id_entries(unsigned int number) {
	this->number_of_id_entries = number;
}
void resource_directory::set_major_version(unsigned short major_version) {
	this->major_version = major_version;
}
void resource_directory::set_minor_version(unsigned short minor_version) {
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

unsigned int resource_directory::get_characteristics() const {
	return this->characteristics;
}
unsigned int resource_directory::get_timestamp() const {
	return this->timestamp;
}
unsigned short resource_directory::get_major_version() const {
	return this->major_version;
}
unsigned short resource_directory::get_minor_version() const {
	return this->minor_version;
}
unsigned int resource_directory::get_number_of_named_entries() const {
	return this->number_of_named_entries;
}
unsigned int resource_directory::get_number_of_id_entries() const {
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
resource_directory::id_entry_finder::id_entry_finder(unsigned int id)
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
	:name_(name), named_(true)
{}
resource_directory::entry_finder::entry_finder(unsigned int id)
	: id_(id), named_(false)
{}
bool resource_directory::entry_finder::operator()(resource_directory_entry& entry) {
	if (named_) {
		return entry.is_named() && entry.get_name() == name_;
	}
	else {
		return !entry.is_named() && entry.get_id() == id_;
	}
}
bool resource_directory::entry_by_id(resource_directory_entry * &entry, unsigned int id) {
	std::vector<resource_directory_entry>::const_iterator i = std::find_if(entries.begin(), entries.end(), id_entry_finder(id));


	if (i == entries.end()) {
		return false;
	}

	entry = (resource_directory_entry *)&(*i);

	return true;
}
bool resource_directory::entry_by_name(resource_directory_entry * &entry, const std::wstring& name) {
	std::vector<resource_directory_entry>::const_iterator i = std::find_if(entries.begin(), entries.end(), name_entry_finder(name));


	if (i == entries.end()) {
		return false;
	}

	entry = (resource_directory_entry *)&(*i);


	return true;
}



void calculate_resource_data_space(resource_directory& root,//taken from pe bless
	unsigned int aligned_offset_from_section_start, unsigned int& needed_size_for_structures, unsigned int& needed_size_for_strings) {

	needed_size_for_structures += sizeof(IMAGE_RESOURCE_DIRECTORY);

	for (unsigned int entry_idx = 0; entry_idx < root.get_entry_list().size(); entry_idx++) {

		needed_size_for_structures += sizeof(IMAGE_RESOURCE_DIRECTORY_ENTRY);

		if (root.get_entry_list()[entry_idx].is_named()) {
			needed_size_for_strings += ((root.get_entry_list()[entry_idx].get_name().length() + 1) * 2 + sizeof(WORD));
		}

		if (!root.get_entry_list()[entry_idx].is_includes_data()) {
			calculate_resource_data_space(root.get_entry_list()[entry_idx].get_resource_directory(), aligned_offset_from_section_start, needed_size_for_structures, needed_size_for_strings);
		}
	}
}

void calculate_resource_data_space(resource_directory& root, unsigned int needed_size_for_structures, //taken from pe bless
	unsigned int needed_size_for_strings, unsigned int& needed_size_for_data, unsigned int& current_data_pos) {

	for (unsigned int entry_idx = 0; entry_idx < root.get_entry_list().size(); entry_idx++) {

		if (root.get_entry_list()[entry_idx].is_includes_data()) {

			unsigned int data_size = unsigned int(root.get_entry_list()[entry_idx].get_data_entry().get_data().size()
				+ sizeof(IMAGE_RESOURCE_DATA_ENTRY)
				+ ALIGN_UP(current_data_pos, sizeof(unsigned int)) - current_data_pos
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
	unsigned int res_rva, unsigned int offset_to_directory, std::set<unsigned int>& processed) {
	resource_directory ret;

	if (!processed.insert(offset_to_directory).second) { return ret; }


	IMAGE_RESOURCE_DIRECTORY directory;
	if (image.get_data_by_rva(res_rva + offset_to_directory, &directory, sizeof(directory))) {

		ret = resource_directory(&directory);

		for (unsigned long i = 0; i != unsigned long(directory.NumberOfIdEntries) + directory.NumberOfNamedEntries; i++) {
			//Read directory entries one by one
			IMAGE_RESOURCE_DIRECTORY_ENTRY dir_entry;
			if (image.get_data_by_rva(res_rva + sizeof(IMAGE_RESOURCE_DIRECTORY) + (i * sizeof(IMAGE_RESOURCE_DIRECTORY_ENTRY)) + offset_to_directory, &dir_entry, sizeof(dir_entry))) {

				//Create directory entry structure
				resource_directory_entry entry;

				//If directory is named
				if (dir_entry.NameIsString) {
					//get directory name length

					PIMAGE_RESOURCE_DIR_STRING_U directory_name = (PIMAGE_RESOURCE_DIR_STRING_U)&image.get_section_by_rva(res_rva + dir_entry.NameOffset)->get_section_data().data()[
						res_rva + dir_entry.NameOffset -
							image.get_section_by_rva(res_rva + dir_entry.NameOffset)->get_virtual_address()
					];

					std::wstring name;
					name.resize(directory_name->Length);
					memcpy((void*)name.data(), directory_name->NameString, directory_name->Length * sizeof(wchar_t));

					//Set entry UNICODE name
					entry.set_name(name);
				}
				else {
					entry.set_id(dir_entry.Id);
				}

				//If directory entry has another resource directory
				if (dir_entry.DataIsDirectory) {
					entry.add_resource_directory(process_resource_directory(image, res_rva, dir_entry.OffsetToDirectory, processed));
				}
				else {
					//If directory entry has data
					IMAGE_RESOURCE_DATA_ENTRY data_entry;
					if (image.get_data_by_rva(res_rva + dir_entry.OffsetToData, &data_entry, sizeof(data_entry))) {

						//Add data entry to directory entry
						entry.add_data_entry(
							resource_data_entry(
								&image.get_section_by_rva(data_entry.OffsetToData)->get_section_data().data()[
									data_entry.OffsetToData -
										image.get_section_by_rva(data_entry.OffsetToData)->get_virtual_address()
								],
								data_entry.Size,
										data_entry.CodePage
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
	unsigned int& current_structures_pos, unsigned int& current_data_pos, unsigned int& current_strings_pos, unsigned int offset_from_section_start){

	IMAGE_RESOURCE_DIRECTORY dir = { 0 };
	dir.Characteristics = root.get_characteristics();
	dir.MajorVersion	= root.get_major_version();
	dir.MinorVersion	= root.get_minor_version();
	dir.TimeDateStamp	= root.get_timestamp();

	std::sort(root.get_entry_list().begin(), root.get_entry_list().end(), entry_sorter());


	for (unsigned int entry_idx = 0; entry_idx < root.get_entry_list().size(); entry_idx++) {
		if (root.get_entry_list()[entry_idx].is_named()) {
			++dir.NumberOfNamedEntries;
		}
		else {
			++dir.NumberOfIdEntries;
		}
	}
	
	memcpy(&resource_section.get_section_data().data()[current_structures_pos], &dir, sizeof(dir));

	resource_section.add_data((BYTE*)&dir, sizeof(dir));

	BYTE* raw_data = resource_section.get_section_data().data();


	current_structures_pos += sizeof(dir);

	unsigned int this_current_structures_pos = current_structures_pos;

	current_structures_pos += sizeof(IMAGE_RESOURCE_DIRECTORY_ENTRY) * (dir.NumberOfNamedEntries + dir.NumberOfIdEntries);

	for (std::vector<resource_directory_entry>::iterator it = root.get_entry_list().begin(); it != root.get_entry_list().end(); ++it) {

		IMAGE_RESOURCE_DIRECTORY_ENTRY entry;

		if ((*it).is_named()){
			entry.Name = 0x80000000 | (current_strings_pos - offset_from_section_start);
			WORD unicode_length = (WORD)(*it).get_name().length();
			memcpy(&raw_data[current_strings_pos], &unicode_length, sizeof(unicode_length));
			current_strings_pos += sizeof(unicode_length);

			memcpy(&raw_data[current_strings_pos], (*it).get_name().c_str(), (*it).get_name().length() * sizeof(wchar_t) + sizeof(wchar_t));

			current_strings_pos += static_cast<unsigned long>((*it).get_name().length() * sizeof(wchar_t) + sizeof(wchar_t));
		}
		else{
			entry.Name = (*it).get_id();
		}

		if ((*it).is_includes_data()){
			current_data_pos = ALIGN_UP(current_data_pos, sizeof(unsigned int));
			IMAGE_RESOURCE_DATA_ENTRY data_entry = { 0 };
			data_entry.CodePage = (*it).get_data_entry().get_codepage();
			data_entry.Size = unsigned int((*it).get_data_entry().get_data().size());
			data_entry.OffsetToData = resource_section.get_virtual_address() + current_data_pos + sizeof(data_entry);

			entry.OffsetToData = current_data_pos - offset_from_section_start;

			memcpy(&raw_data[current_data_pos], &data_entry, sizeof(data_entry));
			current_data_pos += sizeof(data_entry);

			memcpy(&raw_data[current_data_pos], (*it).get_data_entry().get_data().data(), data_entry.Size);
			current_data_pos += data_entry.Size;

			memcpy(&raw_data[this_current_structures_pos], &entry, sizeof(entry));
			this_current_structures_pos += sizeof(entry);
		}
		else{
			entry.OffsetToData = 0x80000000 | (current_structures_pos - offset_from_section_start);

			memcpy(&raw_data[this_current_structures_pos], &entry, sizeof(entry));
			this_current_structures_pos += sizeof(entry);

			rebuild_resource_directory(resource_section, (*it).get_resource_directory(), current_structures_pos, current_data_pos, current_strings_pos, offset_from_section_start);
		}
	}
}

bool get_resources_table(const pe_image &image, resource_directory& resources) {
    resources.clear_resource_directory_entry_list();

	if (!image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_RESOURCE) || !image.get_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_RESOURCE)) {
		return false;
	}

	std::set<unsigned int> processed;

	resources = process_resource_directory(image, image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_RESOURCE), 0, processed);
	return false;
}
void build_resources_table(pe_image &image, pe_section& section, resource_directory& resources) {//taken from pe bless

	if (resources.get_entry_list().empty()) {
		image.set_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_RESOURCE, 0);
		image.set_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_RESOURCE, 0);
		return;
	}

	unsigned int aligned_offset_from_section_start = ALIGN_UP(section.get_size_of_raw_data(), sizeof(unsigned int));
	unsigned int needed_size_for_structures = aligned_offset_from_section_start - section.get_size_of_raw_data();
	unsigned int needed_size_for_strings = 0;
	unsigned int needed_size_for_data = 0;

	calculate_resource_data_space(resources, aligned_offset_from_section_start, needed_size_for_structures, needed_size_for_strings);

	{
		unsigned int current_data_pos = aligned_offset_from_section_start + needed_size_for_structures + needed_size_for_strings;
		calculate_resource_data_space(resources, needed_size_for_structures, needed_size_for_strings, needed_size_for_data, current_data_pos);
	}

	unsigned int needed_size = needed_size_for_structures + needed_size_for_strings + needed_size_for_data;

	if (section.get_size_of_raw_data() < needed_size + aligned_offset_from_section_start) {
		section.get_section_data().resize(section.get_size_of_raw_data() + needed_size + aligned_offset_from_section_start);
	}

	unsigned int current_structures_pos = aligned_offset_from_section_start;
	unsigned int current_strings_pos = current_structures_pos + needed_size_for_structures;
	unsigned int current_data_pos = current_strings_pos + needed_size_for_strings;
	rebuild_resource_directory(section, resources, current_structures_pos, current_data_pos, current_strings_pos, aligned_offset_from_section_start);


	image.set_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_RESOURCE, section.get_virtual_address() + aligned_offset_from_section_start);
	image.set_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_RESOURCE,	needed_size);
}
bool erase_resources_table(pe_image &image, std::vector<erased_zone>* zones) {
	if (!image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_RESOURCE) || !image.get_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_RESOURCE)) {
		return false;
	}

	pe_section * section = image.get_section_by_rva(image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_RESOURCE));
	if (section) {
		memset(section->get_section_data().data(), 0, image.get_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_RESOURCE));

		if (zones) {
			zones->push_back({
				image.get_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_RESOURCE) ,
				image.get_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_RESOURCE)
			});
		}


		image.set_directory_virtual_address(IMAGE_DIRECTORY_ENTRY_RESOURCE, 0);
		image.set_directory_virtual_size(IMAGE_DIRECTORY_ENTRY_RESOURCE, 0);
		return true;
	}

	return false;
}
 