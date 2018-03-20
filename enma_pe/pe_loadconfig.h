#pragma once


class load_config_table{
	DWORD		timestamp;
	WORD		major_version;
	WORD		minor_version;
	DWORD		global_flagsclear;
	DWORD		global_flagsset;
	DWORD		criticalsection_default_timeout;
    ULONGLONG	decommit_freeblock_threshold;
    ULONGLONG	decommit_totalfree_threshold;
    DWORD	    lock_prefix_table;      
    ULONGLONG   maximum_allocation_size;
    ULONGLONG	virtual_memory_threshold;
	DWORD		process_heap_flags;
    ULONGLONG	process_affinity_mask;
	WORD		csd_version;
    DWORD	    editlist;			
    DWORD	    security_cookie;		
    DWORD	    se_handler_table;		
    DWORD	    guard_cf_check_function_pointer;
    DWORD	    guard_cf_function_table;
	DWORD		guard_flags;

	std::vector<DWORD> se_handlers;
	std::vector<DWORD> lock_prefixes_rva;
    std::vector<DWORD> guard_cf_functions_rva;
public:
	load_config_table::load_config_table();
	load_config_table::~load_config_table();
public:
	void		load_config_table::set_timestamp(DWORD timestamp);
	void		load_config_table::set_major_version(WORD major_version);
	void		load_config_table::set_minor_version(WORD minor_version);
	void		load_config_table::set_global_flagsclear(DWORD global_flagsclear);
	void		load_config_table::set_global_flagsset(DWORD global_flagsset);
	void		load_config_table::set_criticalsection_default_timeout(DWORD criticalsection_default_timeout);
	void		load_config_table::set_decommit_freeblock_threshold(ULONGLONG decommit_freeblock_threshold);
	void		load_config_table::set_decommit_totalfree_threshold(ULONGLONG decommit_totalfree_threshold);
	void		load_config_table::set_lock_prefix_table(DWORD lock_prefix_table);	
	void		load_config_table::set_maximum_allocation_size(ULONGLONG maximum_allocation_size);
	void		load_config_table::set_virtual_memory_threshold(ULONGLONG virtual_memory_threshold);
	void		load_config_table::set_process_heap_flags(DWORD process_heap_flags);
	void		load_config_table::set_process_affinity_mask(ULONGLONG process_affinity_mask);
	void		load_config_table::set_csd_version(WORD csd_version);
	void		load_config_table::set_editlist(DWORD editlist);						
	void		load_config_table::set_security_cookie(DWORD security_cookie);			
	void		load_config_table::set_se_handler_table(DWORD se_handler_table);	
	void		load_config_table::set_guard_cf_check_function_pointer(DWORD guard_cf_check_function_pointer);
	void		load_config_table::set_guard_cf_function_table(DWORD guard_cf_function_table);	
	void		load_config_table::set_guard_flags(DWORD guard_flags);
public:
    bool        load_config_table::is_empty() const;

	DWORD		load_config_table::get_timestamp() const ;
	WORD		load_config_table::get_major_version() const;
	WORD		load_config_table::get_minor_version() const;
	DWORD		load_config_table::get_global_flagsclear() const;
	DWORD		load_config_table::get_global_flagsset() const;
	DWORD		load_config_table::get_criticalsection_default_timeout() const;
    ULONGLONG	    load_config_table::get_decommit_freeblock_threshold() const;
    ULONGLONG	    load_config_table::get_decommit_totalfree_threshold() const;
    DWORD	    load_config_table::get_lock_prefix_table() const;		
    ULONGLONG   	load_config_table::get_maximum_allocation_size() const;
    ULONGLONG	    load_config_table::get_virtual_memory_threshold() const;
	DWORD		load_config_table::get_process_heap_flags() const;
    ULONGLONG	    load_config_table::get_process_affinity_mask() const;
	WORD		load_config_table::get_csd_version() const;
    DWORD	    load_config_table::get_editlist() const;				
    DWORD	    load_config_table::get_security_cookie() const;	
    DWORD	    load_config_table::get_se_handler_table() const;
    DWORD	    load_config_table::get_se_handler_count() const;
    DWORD	    load_config_table::get_guard_cf_check_function_pointer() const; 
    DWORD	    load_config_table::get_guard_cf_function_table() const;
    DWORD	    load_config_table::get_guard_cf_function_count() const;
	DWORD		load_config_table::get_guard_flags() const;

	std::vector<DWORD>& load_config_table::get_se_handlers();
	std::vector<DWORD>& load_config_table::get_lock_prefixes();
    std::vector<DWORD>& load_config_table::get_guard_cf_functions();
};


bool get_load_config_table(_In_ const pe_image &image,	
    _Out_ load_config_table& load_config);
void build_load_config_table(_Inout_ pe_image &image, _Inout_ pe_section& section,
	_Inout_ load_config_table& load_config, _Inout_ relocation_table& relocs);
bool erase_load_config_table(_Inout_ pe_image &image,
	_Inout_opt_ std::vector<erased_zone>* zones = 0,
	_Inout_opt_ relocation_table* relocs = 0);