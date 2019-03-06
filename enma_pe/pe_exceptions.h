#pragma once

class exception_entry;

class exception_unwind_info {
    uint32_t unwind_info_rva;
    
    uint8_t version;
    uint8_t flags;
    uint8_t size_of_prolog;
    uint8_t count_of_codes;
    uint8_t frame_register;
    uint8_t frame_offset;

    uint32_t handler_rva;
    exception_entry * chained_item;

    std::vector<unwind_code> codes;

    void * custom_parameter;
public:
    exception_unwind_info();
    exception_unwind_info(uint32_t unwind_info_rva, uint8_t version,
    uint8_t flags,uint8_t size_of_prolog,uint8_t count_of_codes,uint8_t frame_register,uint8_t frame_offset);

    exception_unwind_info(const exception_unwind_info& item);
    ~exception_unwind_info();

    exception_unwind_info& operator=(const exception_unwind_info& item);
public:
    void add_unwind_code(const unwind_code& code);

    void set_version(uint8_t version);
    void set_flags(uint8_t flags);
    void set_size_of_prolog(uint8_t size_of_prolog);
    void set_count_of_codes(uint8_t count_of_codes);
    void set_frame_register(uint8_t frame_register);
    void set_frame_offset(uint8_t frame_offset);

    void set_chained_item(exception_entry * chained_item);
    void set_handler_rva(uint32_t rva);
    void set_unwind_info_rva(uint32_t rva);

    void set_custom_parameter(void * custom_parameter);
public: 

    uint8_t get_version() const;
    uint8_t get_flags() const;
    uint8_t get_size_of_prolog() const;
    uint8_t get_count_of_codes() const;
    uint8_t get_frame_register() const;
    uint8_t get_frame_offset() const;

    void * get_custom_parameter(void * custom_parameter);
    void * get_custom_parameter(void * custom_parameter) const;

    exception_entry * get_chained_item();
    const exception_entry * get_chained_item() const;
    uint32_t get_handler_rva() const;
    uint32_t get_unwind_info_rva() const;
};

class exception_entry {
    uint32_t address_begin;
    uint32_t address_end;
    uint32_t address_unwind_data;
public:
    exception_entry();
    exception_entry(const exception_entry& entry);
    exception_entry(uint32_t address_begin, uint32_t address_end, uint32_t address_unwind_data);
    ~exception_entry();

    exception_entry& operator=(const exception_entry& entry);
public:
    void set_begin_address(uint32_t rva_address);
    void set_end_address(uint32_t rva_address);
    void set_unwind_data_address(uint32_t rva_address);

public:
    uint32_t get_begin_address() const;
    uint32_t get_end_address() const;
    uint32_t get_unwind_data_address() const;
};

class exceptions_table {
    std::vector<exception_entry> exception_entries;
    std::vector<exception_unwind_info> unwind_entries;
public:
    exceptions_table();
    exceptions_table(const exceptions_table& exceptions);
    ~exceptions_table();

    exceptions_table& operator=(const exceptions_table& exceptions);
public:
    void add_exception_entry(uint32_t address_begin, uint32_t address_end, uint32_t address_unwind_data);
    void add_exception_entry(const exception_entry& entry);
    void add_unwind_entry(const exception_unwind_info& entry);
    void add_item(const runtime_function_entry& exc_entry);
    void clear();
public:
    size_t size() const;

    const std::vector<exception_unwind_info>& get_unwind_entries() const;
    std::vector<exception_unwind_info>& get_unwind_entries();

    const std::vector<exception_entry>& get_exception_entries() const;
    std::vector<exception_entry>& get_exception_entries();
};



directory_code get_exception_table(_In_ const pe_image &image, _Out_ exceptions_table& exceptions);
bool build_exceptions_table(_Inout_ pe_image &image, _Inout_ pe_section& section,
    _In_ const exceptions_table& exceptions);
directory_code get_placement_exceptions_table(_In_ const pe_image &image, _Inout_ pe_directory_placement& placement);