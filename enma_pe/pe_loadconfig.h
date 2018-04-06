#pragma once


class load_config_table{
	uint32_t 		timestamp;
	uint16_t 		major_version;
	uint16_t 		minor_version;
	uint32_t 		global_flagsclear;
	uint32_t 		global_flagsset;
	uint32_t 		criticalsection_default_timeout;
    uint64_t 	decommit_freeblock_threshold;
    uint64_t 	decommit_totalfree_threshold;
    uint32_t 	    lock_prefix_table;      
    uint64_t    maximum_allocation_size;
    uint64_t 	virtual_memory_threshold;
	uint32_t 		process_heap_flags;
    uint64_t 	process_affinity_mask;
	uint16_t 		csd_version;
    uint32_t 	    editlist;			
    uint32_t 	    security_cookie;		
    uint32_t 	    se_handler_table;		
    uint32_t 	    guard_cf_check_function_pointer;
    uint32_t 	    guard_cf_function_table;
	uint32_t 		guard_flags;

	std::vector<uint32_t > se_handlers;
	std::vector<uint32_t > lock_prefixes_rva;
    std::vector<uint32_t > guard_cf_functions_rva;
public:
	load_config_table::load_config_table();
	load_config_table::~load_config_table();
public:
	void		load_config_table::set_timestamp(uint32_t  timestamp);
	void		load_config_table::set_major_version(uint16_t  major_version);
	void		load_config_table::set_minor_version(uint16_t  minor_version);
	void		load_config_table::set_global_flagsclear(uint32_t  global_flagsclear);
	void		load_config_table::set_global_flagsset(uint32_t  global_flagsset);
	void		load_config_table::set_criticalsection_default_timeout(uint32_t  criticalsection_default_timeout);
	void		load_config_table::set_decommit_freeblock_threshold(uint64_t  decommit_freeblock_threshold);
	void		load_config_table::set_decommit_totalfree_threshold(uint64_t  decommit_totalfree_threshold);
	void		load_config_table::set_lock_prefix_table(uint32_t  lock_prefix_table);	
	void		load_config_table::set_maximum_allocation_size(uint64_t  maximum_allocation_size);
	void		load_config_table::set_virtual_memory_threshold(uint64_t  virtual_memory_threshold);
	void		load_config_table::set_process_heap_flags(uint32_t  process_heap_flags);
	void		load_config_table::set_process_affinity_mask(uint64_t  process_affinity_mask);
	void		load_config_table::set_csd_version(uint16_t  csd_version);
	void		load_config_table::set_editlist(uint32_t  editlist);						
	void		load_config_table::set_security_cookie(uint32_t  security_cookie);			
	void		load_config_table::set_se_handler_table(uint32_t  se_handler_table);	
	void		load_config_table::set_guard_cf_check_function_pointer(uint32_t  guard_cf_check_function_pointer);
	void		load_config_table::set_guard_cf_function_table(uint32_t  guard_cf_function_table);	
	void		load_config_table::set_guard_flags(uint32_t  guard_flags);
public:
    bool        load_config_table::is_empty() const;

	uint32_t 		load_config_table::get_timestamp() const ;
	uint16_t 		load_config_table::get_major_version() const;
	uint16_t 		load_config_table::get_minor_version() const;
	uint32_t 		load_config_table::get_global_flagsclear() const;
	uint32_t 		load_config_table::get_global_flagsset() const;
	uint32_t 		load_config_table::get_criticalsection_default_timeout() const;
    uint64_t 	    load_config_table::get_decommit_freeblock_threshold() const;
    uint64_t 	    load_config_table::get_decommit_totalfree_threshold() const;
    uint32_t 	    load_config_table::get_lock_prefix_table() const;		
    uint64_t    	load_config_table::get_maximum_allocation_size() const;
    uint64_t 	    load_config_table::get_virtual_memory_threshold() const;
	uint32_t 		load_config_table::get_process_heap_flags() const;
    uint64_t 	    load_config_table::get_process_affinity_mask() const;
	uint16_t 		load_config_table::get_csd_version() const;
    uint32_t 	    load_config_table::get_editlist() const;				
    uint32_t 	    load_config_table::get_security_cookie() const;	
    uint32_t 	    load_config_table::get_se_handler_table() const;
    uint32_t 	    load_config_table::get_se_handler_count() const;
    uint32_t 	    load_config_table::get_guard_cf_check_function_pointer() const; 
    uint32_t 	    load_config_table::get_guard_cf_function_table() const;
    uint32_t 	    load_config_table::get_guard_cf_function_count() const;
	uint32_t 		load_config_table::get_guard_flags() const;

	std::vector<uint32_t >& load_config_table::get_se_handlers();
	std::vector<uint32_t >& load_config_table::get_lock_prefixes();
    std::vector<uint32_t >& load_config_table::get_guard_cf_functions();
};


bool get_load_config_table(_In_ const pe_image &image,	
    _Out_ load_config_table& load_config);
void build_load_config_table(_Inout_ pe_image &image, _Inout_ pe_section& section,
	_Inout_ load_config_table& load_config, _Inout_ relocation_table& relocs);

bool get_placement_load_config_table(_Inout_ pe_image &image, _Inout_ std::vector<directory_placement>& placement);