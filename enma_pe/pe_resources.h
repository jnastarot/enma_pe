#pragma once


class resource_data_entry {
	uint32_t codepage;
	std::vector<uint8_t> data;
public:

	resource_data_entry::resource_data_entry();
	resource_data_entry::resource_data_entry(const void * data, uint32_t data_size, uint32_t codepage);
	resource_data_entry::~resource_data_entry();
public:
	void resource_data_entry::set_codepage(uint32_t codepage);
	void resource_data_entry::set_data(const void * data, uint32_t data_size);
public:
	uint32_t		resource_data_entry::get_codepage() const;
    const std::vector<uint8_t>&	resource_data_entry::get_data() const;
	std::vector<uint8_t>&	resource_data_entry::get_data();
};



class resource_directory;
class resource_directory_entry;
class resource_directory_entry {
	uint32_t id;
	std::wstring name;

	union includes {
		includes::includes();
		resource_data_entry* data;
		resource_directory * dir;
	}_ptr;

	bool includes_data;
	bool named;
public:
	resource_directory_entry::resource_directory_entry();
	resource_directory_entry::resource_directory_entry(const resource_directory_entry& other);
	resource_directory_entry::~resource_directory_entry();

	resource_directory_entry& resource_directory_entry::operator=(const resource_directory_entry& other);
	void resource_directory_entry::release();
public:
	void resource_directory_entry::set_name(const std::wstring& name);
	void resource_directory_entry::set_id(uint32_t id);

	void resource_directory_entry::add_data_entry(const resource_data_entry& entry);
	void resource_directory_entry::add_resource_directory(const resource_directory& dir);
public:
	bool                    resource_directory_entry::is_named() const;
	uint32_t			    resource_directory_entry::get_id() const;
	const std::wstring&     resource_directory_entry::get_name() const;
	bool                    resource_directory_entry::is_includes_data() const;

    const resource_directory&     resource_directory_entry::get_resource_directory() const;
    const resource_data_entry&    resource_directory_entry::get_data_entry() const;

	resource_directory&     resource_directory_entry::get_resource_directory();
	resource_data_entry&    resource_directory_entry::get_data_entry();
};


class resource_directory {
	uint32_t	characteristics;
	uint32_t	timestamp;
	uint16_t	major_version;
	uint16_t	minor_version;
	uint32_t	number_of_named_entries;
	uint32_t	number_of_id_entries;
	std::vector<resource_directory_entry> entries;
public:
	resource_directory::resource_directory();
    resource_directory::resource_directory(const resource_directory& resource_dir);
	resource_directory::resource_directory(const image_resource_directory& res_dir);
	resource_directory::~resource_directory();
	resource_directory& resource_directory::operator=(const resource_directory& resource_dir);

	bool resource_directory::entry_by_id(resource_directory_entry * &entry, uint32_t id);
	bool resource_directory::entry_by_name(resource_directory_entry * &entry, const std::wstring& name);

	void resource_directory::set_characteristics(uint32_t characteristics);
	void resource_directory::set_timestamp(uint32_t timestamp);
	void resource_directory::set_number_of_named_entries(uint32_t number);
	void resource_directory::set_number_of_id_entries(uint32_t number);
	void resource_directory::set_major_version(uint16_t major_version);
	void resource_directory::set_minor_version(uint16_t minor_version);

	void resource_directory::add_resource_directory_entry(const resource_directory_entry& entry);
	void resource_directory::clear_resource_directory_entry_list();
public:
	uint32_t	resource_directory::get_characteristics() const;
	uint32_t	resource_directory::get_timestamp() const;
	uint32_t	resource_directory::get_number_of_named_entries() const;
	uint32_t	resource_directory::get_number_of_id_entries() const;
	uint16_t	resource_directory::get_major_version() const;
	uint16_t	resource_directory::get_minor_version() const;
	std::vector<resource_directory_entry>&		resource_directory::get_entry_list();
public: 
	struct id_entry_finder{
	public:
		id_entry_finder(uint32_t id);
		bool operator()(resource_directory_entry& entry);
	private:
		uint32_t id_;
	};
	struct name_entry_finder{
	public:
		name_entry_finder(const std::wstring& name);
		bool operator()(resource_directory_entry& entry);

	private:
		std::wstring name_;
	};
	struct entry_finder{
	public:
		entry_finder(const std::wstring& name);
		entry_finder(uint32_t id);
		bool operator()(resource_directory_entry& entry);
	private:
		std::wstring name_;
		uint32_t id_;
		bool named_;
	};
};


directory_code get_resources_table(_In_ const pe_image &image,
	_Out_ resource_directory& resources);						
bool build_resources_table(_Inout_ pe_image &image, _Inout_ pe_section& section,
	_In_ resource_directory& resources);
directory_code get_placement_resources_table(_In_ const pe_image &image, _Inout_ std::vector<directory_placement>& placement);
