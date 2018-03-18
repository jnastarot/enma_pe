#pragma once


class resource_data_entry {
	unsigned int codepage;
	std::vector<BYTE> data;
public:

	resource_data_entry::resource_data_entry();
	resource_data_entry::resource_data_entry(void * data, unsigned int data_size, unsigned int codepage);
	resource_data_entry::~resource_data_entry();
public:
	void resource_data_entry::set_codepage(unsigned int codepage);
	void resource_data_entry::set_data(void * data, unsigned int data_size);
public:
	unsigned int		resource_data_entry::get_codepage() const;
	std::vector<BYTE>&	resource_data_entry::get_data();
};



class resource_directory;
class resource_directory_entry;
class resource_directory_entry {
	unsigned int id;
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
	void resource_directory_entry::set_id(unsigned int id);

	void resource_directory_entry::add_data_entry(resource_data_entry& entry);
	void resource_directory_entry::add_resource_directory(const resource_directory& dir);
public:
	bool					resource_directory_entry::is_named() const;
	unsigned int			resource_directory_entry::get_id() const;
	const std::wstring&		resource_directory_entry::get_name() const;
	bool					resource_directory_entry::is_includes_data() const;
	resource_directory&		resource_directory_entry::get_resource_directory();
	resource_data_entry&	resource_directory_entry::get_data_entry();
};


class resource_directory {
	unsigned int	characteristics;
	unsigned int	timestamp;
	unsigned short	major_version;
	unsigned short	minor_version;
	unsigned int	number_of_named_entries;
	unsigned int	number_of_id_entries;
	std::vector<resource_directory_entry> entries;
public:
	resource_directory::resource_directory();
	resource_directory::resource_directory(IMAGE_RESOURCE_DIRECTORY * res_dir);
	resource_directory::~resource_directory();
	resource_directory& resource_directory::operator=(const resource_directory& resource_dir);

	bool resource_directory::entry_by_id(resource_directory_entry * &entry, unsigned int id);
	bool resource_directory::entry_by_name(resource_directory_entry * &entry, const std::wstring& name);

	void resource_directory::set_characteristics(unsigned int characteristics);
	void resource_directory::set_timestamp(unsigned int timestamp);
	void resource_directory::set_number_of_named_entries(unsigned int number);
	void resource_directory::set_number_of_id_entries(unsigned int number);
	void resource_directory::set_major_version(unsigned short major_version);
	void resource_directory::set_minor_version(unsigned short minor_version);

	void resource_directory::add_resource_directory_entry(resource_directory_entry& entry);
	void resource_directory::clear_resource_directory_entry_list();
public:
	unsigned int	resource_directory::get_characteristics() const;
	unsigned int	resource_directory::get_timestamp() const;
	unsigned int	resource_directory::get_number_of_named_entries() const;
	unsigned int	resource_directory::get_number_of_id_entries() const;
	unsigned short	resource_directory::get_major_version() const;
	unsigned short	resource_directory::get_minor_version() const;
	std::vector<resource_directory_entry>&		resource_directory::get_entry_list();
public: 
	struct id_entry_finder{
	public:
		id_entry_finder(unsigned int id);
		bool operator()(resource_directory_entry& entry);
	private:
		unsigned int id_;
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
		entry_finder(unsigned int id);
		bool operator()(resource_directory_entry& entry);
	private:
		std::wstring name_;
		unsigned int id_;
		bool named_;
	};
};


bool get_resources_table(_In_ const pe_image &image,
	_Out_ resource_directory& resources);						
void build_resources_table(_Inout_ pe_image &image, _Inout_ pe_section& section,
	_In_ resource_directory& resources);
bool erase_resources_table(_Inout_ pe_image &image,	
    _Inout_opt_ std::vector<erased_zone>* zones = 0);
